#include "httpshared/http_client.h"

#include "httpshared/http_utils.h"

#include "rex_engine/diagnostics/assert.h"

namespace http
{
	// HTTP client implementation
	ClientBase::ClientBase(const ClientDesc& desc)
		: m_host(escape_abstract_namespace_unix_domain(desc.host))
		, m_port(desc.port)
		, m_host_and_port(adjust_host_string(m_host) + ":" + rsl::to_string(m_port))
		, m_client_cert_path(desc.cert_path)
		, m_client_key_path(desc.key_path)
	{}

	ClientBase::~ClientBase() {
		// Wait until all the requests in flight are handled.
		size_t retry_count = 10;
		while (retry_count-- > 0) {
			{
				rsl::unique_lock<rsl::mutex> guard(m_socket_mutex);
				if (m_socket_requests_in_flight == 0) { break; }
			}
			rsl::this_thread::sleep_for(rsl::chrono::milliseconds{ 1 });
		}

		rsl::unique_lock<rsl::mutex> guard(m_socket_mutex);
		shutdown_socket(m_socket);
		close_socket(m_socket);
	}

	bool ClientBase::is_valid() const { return true; }

	Result ClientBase::get(const GetRequestDesc& desc)
	{
		rsl::string path = desc.path;
		if (!desc.params.empty())
		{
			path = append_query_params(desc.path, desc.params);
		}

		Request req;
		req.method						= HttpMethod::Get;
		req.path							= path;
		req.headers						= desc.headers;
		req.response_handler	= rsl::move(desc.response_handler);
		req.progress					= rsl::move(desc.progress);
		req.content_receiver	= 
			[content_receiver = desc.content_receiver](const char* data, size_t data_length, uint64_t /*offset*/, uint64_t /*total_length*/) 
			{
					return content_receiver(data, data_length);
			};
		
		if (m_max_timeout_msec > 0) 
		{
			req.start_time			= rsl::chrono::steady_clock::now();
		}

		return send_(rsl::move(req));
	}
	Result ClientBase::head(const HeadRequestDesc& desc)
	{
		Request req;
		req.method = HttpMethod::Head;
		req.headers = desc.headers;
		req.path.assign(desc.path);
		if (m_max_timeout_msec > 0) 
		{
			req.start_time = rsl::chrono::steady_clock::now();
		}

		return send_(rsl::move(req));
	}
	Result ClientBase::post(const PostRequestDesc& desc)
	{
		REX_ASSERT_X(!desc.params.empty() && !desc.items.empty(), "you can't have both items and parameters in a post request");

		PostRequestDesc new_desc = desc;

		// If the user provided parameters, these should be transformed into the body
		if (!desc.params.empty())
		{
			new_desc.body = params_to_query_str(desc.params);
		}

		rsl::string boundary;
		if (!desc.items.empty())
		{
			boundary = desc.boundary;
			if (boundary.empty())
			{
				boundary = make_multipart_data_boundary();
			}
			else if (!is_multipart_boundary_chars_valid(boundary)) 
			{
				return Result{ nullptr, Error::UnsupportedMultipartBoundaryChars };
			}
			const auto& content_type = serialize_multipart_formdata_get_content_type(boundary);
			new_desc.body = serialize_multipart_formdata(desc.items, boundary);
		}

		ContentProviderWithoutLength content_provider_without_length{};
		if (!desc.provider_items.empty())
		{
			content_provider_without_length = get_multipart_content_provider(boundary, desc.items, desc.provider_items);
		}

		// send with content provider
		Request req;
		req.method = method;
		req.headers = headers;
		req.path = path;
		req.progress = progress;
		if (m_max_timeout_msec > 0) {
			req.start_time = std::chrono::steady_clock::now();
		}

		auto error = Error::Success;

		if (!content_type.empty()) { req.set_header("Content-Type", content_type); }

#ifdef CPPHTTPLIB_ZLIB_SUPPORT
		if (m_compress) { req.set_header("Content-Encoding", "gzip"); }
#endif

#ifdef CPPHTTPLIB_ZLIB_SUPPORT
		if (m_compress && !content_provider_without_length) {
			// TODO: Brotli support
			detail::gzip_compressor compressor;

			if (content_provider) {
				auto ok = true;
				size_t offset = 0;
				DataSink data_sink;

				data_sink.write = [&](const char* data, size_t data_len) -> bool {
					if (ok) {
						auto last = offset + data_len == content_length;

						auto ret = compressor.compress(
							data, data_len, last,
							[&](const char* compressed_data, size_t compressed_data_len) {
								req.body.append(compressed_data, compressed_data_len);
								return true;
							});

						if (ret) {
							offset += data_len;
						}
						else {
							ok = false;
						}
					}
					return ok;
					};

				while (ok && offset < content_length) {
					if (!content_provider(offset, content_length - offset, data_sink)) {
						error = Error::Canceled;
						return nullptr;
					}
				}
			}
			else {
				if (!compressor.compress(body, content_length, true,
					[&](const char* data, size_t data_len) {
						req.body.append(data, data_len);
						return true;
					})) {
					error = Error::Compression;
					return nullptr;
				}
			}
		}
		else
#endif
		{
			if (content_provider) {
				req.content_length_ = content_length;
				req.content_provider_ = std::move(content_provider);
				req.is_chunked_content_provider_ = false;
			}
			else if (content_provider_without_length) {
				req.content_length_ = 0;
				req.content_provider_ = detail::ContentProviderAdapter(
					std::move(content_provider_without_length));
				req.is_chunked_content_provider_ = true;
				req.set_header("Transfer-Encoding", "chunked");
			}
			else {
				req.body.assign(body, content_length);
			}
		}

		auto res = detail::make_unique<Response>();
		send(req, *res, error) ? std::move(res) : nullptr;

		return Result{ std::move(res), error, std::move(req.headers) };
	}
	Result ClientBase::put(const PutRequestDesc& desc)
	{
		REX_ASSERT_X(!desc.params.empty() && !desc.items.empty(), "you can't have both items and parameters in a post request");

		if (!desc.params.empty())
		{
			desc.body = params_to_query_str(desc.params);
		}

		if (!desc.items.empty())
		{
			rsl::string boundary = desc.boundary;
			if (boundary.empty())
			{
				boundary = make_multipart_data_boundary();
			}
			else if (!is_multipart_boundary_chars_valid(boundary))
			{
				return Result{ nullptr, Error::UnsupportedMultipartBoundaryChars };
			}
			const auto& content_type = serialize_multipart_formdata_get_content_type(boundary);
			desc.body = serialize_multipart_formdata(desc.items, boundary);
		}

		get_multipart_content_provider(boundary, items, provider_items);

		Request req;
		req.method = method;
		req.headers = headers;
		req.path = path;
		req.progress = progress;
		if (m_max_timeout_msec > 0) {
			req.start_time = std::chrono::steady_clock::now();
		}

		auto error = Error::Success;

		if (!content_type.empty()) { req.set_header("Content-Type", content_type); }

#ifdef CPPHTTPLIB_ZLIB_SUPPORT
		if (m_compress) { req.set_header("Content-Encoding", "gzip"); }
#endif

#ifdef CPPHTTPLIB_ZLIB_SUPPORT
		if (m_compress && !content_provider_without_length) {
			// TODO: Brotli support
			detail::gzip_compressor compressor;

			if (content_provider) {
				auto ok = true;
				size_t offset = 0;
				DataSink data_sink;

				data_sink.write = [&](const char* data, size_t data_len) -> bool {
					if (ok) {
						auto last = offset + data_len == content_length;

						auto ret = compressor.compress(
							data, data_len, last,
							[&](const char* compressed_data, size_t compressed_data_len) {
								req.body.append(compressed_data, compressed_data_len);
								return true;
							});

						if (ret) {
							offset += data_len;
						}
						else {
							ok = false;
						}
					}
					return ok;
					};

				while (ok && offset < content_length) {
					if (!content_provider(offset, content_length - offset, data_sink)) {
						error = Error::Canceled;
						return nullptr;
					}
				}
			}
			else {
				if (!compressor.compress(body, content_length, true,
					[&](const char* data, size_t data_len) {
						req.body.append(data, data_len);
						return true;
					})) {
					error = Error::Compression;
					return nullptr;
				}
			}
		}
		else
#endif
		{
			if (content_provider) {
				req.content_length_ = content_length;
				req.content_provider_ = std::move(content_provider);
				req.is_chunked_content_provider_ = false;
			}
			else if (content_provider_without_length) {
				req.content_length_ = 0;
				req.content_provider_ = detail::ContentProviderAdapter(
					std::move(content_provider_without_length));
				req.is_chunked_content_provider_ = true;
				req.set_header("Transfer-Encoding", "chunked");
			}
			else {
				req.body.assign(body, content_length);
			}
		}

		auto res = detail::make_unique<Response>();
		send(req, *res, error) ? std::move(res) : nullptr;

		return Result{ std::move(res), error, std::move(req.headers) };
	}
	Result ClientBase::patch(const PatchRequestDesc& desc)
	{
		Request req;
		req.method = method;
		req.headers = headers;
		req.path = path;
		req.progress = progress;
		if (m_max_timeout_msec > 0) {
			req.start_time = std::chrono::steady_clock::now();
		}

		auto error = Error::Success;

		if (!content_type.empty()) { req.set_header("Content-Type", content_type); }

#ifdef CPPHTTPLIB_ZLIB_SUPPORT
		if (m_compress) { req.set_header("Content-Encoding", "gzip"); }
#endif

#ifdef CPPHTTPLIB_ZLIB_SUPPORT
		if (m_compress && !content_provider_without_length) {
			// TODO: Brotli support
			detail::gzip_compressor compressor;

			if (content_provider) {
				auto ok = true;
				size_t offset = 0;
				DataSink data_sink;

				data_sink.write = [&](const char* data, size_t data_len) -> bool {
					if (ok) {
						auto last = offset + data_len == content_length;

						auto ret = compressor.compress(
							data, data_len, last,
							[&](const char* compressed_data, size_t compressed_data_len) {
								req.body.append(compressed_data, compressed_data_len);
								return true;
							});

						if (ret) {
							offset += data_len;
						}
						else {
							ok = false;
						}
					}
					return ok;
					};

				while (ok && offset < content_length) {
					if (!content_provider(offset, content_length - offset, data_sink)) {
						error = Error::Canceled;
						return nullptr;
					}
				}
			}
			else {
				if (!compressor.compress(body, content_length, true,
					[&](const char* data, size_t data_len) {
						req.body.append(data, data_len);
						return true;
					})) {
					error = Error::Compression;
					return nullptr;
				}
			}
		}
		else
#endif
		{
			if (content_provider) {
				req.content_length_ = content_length;
				req.content_provider_ = std::move(content_provider);
				req.is_chunked_content_provider_ = false;
			}
			else if (content_provider_without_length) {
				req.content_length_ = 0;
				req.content_provider_ = detail::ContentProviderAdapter(
					std::move(content_provider_without_length));
				req.is_chunked_content_provider_ = true;
				req.set_header("Transfer-Encoding", "chunked");
			}
			else {
				req.body.assign(body, content_length);
			}
		}

		auto res = detail::make_unique<Response>();
		send(req, *res, error) ? std::move(res) : nullptr;

		return Result{ std::move(res), error, std::move(req.headers) };
	}
	Result ClientBase::del(const DeleteRequestDesc& desc)
	{
		Request req;
		req.method = "DELETE";
		req.headers = headers;
		req.path = path;
		req.progress = progress;
		if (m_max_timeout_msec > 0) {
			req.start_time = std::chrono::steady_clock::now();
		}

		if (!content_type.empty()) { req.set_header("Content-Type", content_type); }
		req.body.assign(body, content_length);

		return send_(std::move(req));
	}
	Result ClientBase::options(const OptionsRequestDesc& desc)
	{
		Request req;
		req.method = "OPTIONS";
		req.headers = headers;
		req.path = path;
		if (m_max_timeout_msec > 0) {
			req.start_time = std::chrono::steady_clock::now();
		}

		return send_(std::move(req));
	}

	void ClientBase::copy_settings(const ClientBase& rhs) {
		m_client_cert_path = rhs.m_client_cert_path;
		m_client_key_path = rhs.m_client_key_path;
		m_connection_timeout_sec = rhs.m_connection_timeout_sec;
		m_read_timeout_sec = rhs.m_read_timeout_sec;
		m_read_timeout_usec = rhs.m_read_timeout_usec;
		m_write_timeout_sec = rhs.m_write_timeout_sec;
		m_write_timeout_usec = rhs.m_write_timeout_usec;
		m_max_timeout_msec = rhs.m_max_timeout_msec;
		m_basic_auth_username = rhs.m_basic_auth_username;
		m_basic_auth_password = rhs.m_basic_auth_password;
		m_bearer_token_auth_token = rhs.m_bearer_token_auth_token;
#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
		digest_auth_username_ = rhs.digest_auth_username_;
		digest_auth_password_ = rhs.digest_auth_password_;
#endif
		m_keep_alive = rhs.m_keep_alive;
		m_follow_location = rhs.m_follow_location;
		m_url_encode = rhs.m_url_encode;
		m_address_family = rhs.m_address_family;
		m_tcp_nodelay = rhs.m_tcp_nodelay;
		m_ipv6_v6only = rhs.m_ipv6_v6only;
		m_socketoptions = rhs.m_socketoptions;
		m_compress = rhs.m_compress;
		m_dem_compress = rhs.m_dem_compress;
		m_interface = rhs.m_interface;
		m_proxy_host = rhs.m_proxy_host;
		m_proxy_port = rhs.m_proxy_port;
		m_proxy_basic_auth_username = rhs.m_proxy_basic_auth_username;
		m_proxy_basic_auth_password = rhs.m_proxy_basic_auth_password;
		m_proxy_bearer_token_auth_token = rhs.m_proxy_bearer_token_auth_token;
#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
		proxy_digest_auth_username_ = rhs.proxy_digest_auth_username_;
		proxy_digest_auth_password_ = rhs.proxy_digest_auth_password_;
#endif
#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
		ca_cert_file_path_ = rhs.ca_cert_file_path_;
		ca_cert_dir_path_ = rhs.ca_cert_dir_path_;
		ca_cert_store_ = rhs.ca_cert_store_;
#endif
#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
		server_certificate_verification_ = rhs.server_certificate_verification_;
		server_hostname_verification_ = rhs.server_hostname_verification_;
		server_certificate_verifier_ = rhs.server_certificate_verifier_;
#endif
		//logger_ = rhs.logger_;
	}

	socket_t ClientBase::create_client_socket(Error& error) const {
		if (!m_proxy_host.empty() && m_proxy_port != -1) 
		{
			return create_client_socket(
				m_proxy_host, rsl::string(), m_proxy_port, m_address_family, m_tcp_nodelay,
				m_ipv6_v6only, m_socketoptions, m_connection_timeout_sec,
				m_connection_timeout_usec, m_read_timeout_sec, m_read_timeout_usec,
				m_write_timeout_sec, m_write_timeout_usec, m_interface, error);
		}

		// Check is custom IP specified for m_host
		rsl::string ip;
		auto it = m_addr_map.find(m_host);
		if (it != m_addr_map.end()) { ip = it->value; }

		return create_client_socket(
			m_host, ip, m_port, m_address_family, m_tcp_nodelay, m_ipv6_v6only,
			m_socketoptions, m_connection_timeout_sec, m_connection_timeout_usec,
			m_read_timeout_sec, m_read_timeout_usec, m_write_timeout_sec,
			m_write_timeout_usec, m_interface, error);
	}

	bool ClientBase::create_and_connect_socket(Socket& socket,
		Error& error) {
		auto sock = create_client_socket(error);
		if (sock == INVALID_SOCKET) { return false; }
		socket.sock = sock;
		return true;
	}

	void ClientBase::shutdown_ssl(Socket& /*socket*/,
		bool /*shutdown_gracefully*/) {
		// If there are any requests in flight from threads other than us, then it's
		// a thread-unsafe race because individual ssl* objects are not thread-safe.
		REX_ASSERT_X(m_socket_requests_in_flight == 0 ||
			m_socket_requests_are_from_thread == rsl::this_thread::get_id(),
			"There are still requests in flight from other threads, this is not expected");
	}

	void ClientBase::shutdown_socket(Socket& socket) const {
		if (socket.sock == INVALID_SOCKET) { return; }
		http::shutdown_socket(socket.sock);
	}

	void ClientBase::close_socket(Socket& socket) {
		// If there are requests in flight in another thread, usually closing
		// the socket will be fine and they will simply receive an error when
		// using the closed socket, but it is still a bug since rarely the OS
		// may reassign the socket id to be used for a new socket, and then
		// suddenly they will be operating on a live socket that is different
		// than the one they intended!
		REX_ASSERT_X(m_socket_requests_in_flight == 0 ||
			m_socket_requests_are_from_thread == rsl::this_thread::get_id(),
			"There are still requests in flight from other threads, this is not expected");

		// It is also a bug if this happens while SSL is still active
#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
		assert(socket.ssl == nullptr);
#endif
		if (socket.sock == INVALID_SOCKET) { return; }
		close_socket(socket.sock);
		socket.sock = INVALID_SOCKET;
	}

	bool ClientBase::read_response_line(Stream& strm, const Request& req,
		Response& res) const {
		rsl::array<char, 2048> buf{};

		stream_line_reader line_reader(strm, buf.data(), buf.size());

		if (!line_reader.getline()) { return false; }

#ifdef CPPHTTPLIB_ALLOW_LF_AS_LINE_TERMINATOR
		thread_local const std::regex re("(HTTP/1\\.[01]) (\\d{3})(?: (.*?))?\r?\n");
#else
		thread_local const std::regex re("(HTTP/1\\.[01]) (\\d{3})(?: (.*?))?\r\n");
#endif

		rsl::cmatch m;
		if (!rsl::regex_match(line_reader.ptr(), m, re)) {
			return req.method == "CONNECT";
		}
		res.version = rsl::string(m[1]);
		res.status = rsl::stoi(rsl::string(m[2]));
		res.reason = rsl::string(m[3]);

		// Ignore '100 Continue'
		while (res.status == StatusCode::Continue_100) {
			if (!line_reader.getline()) { return false; } // CRLF
			if (!line_reader.getline()) { return false; } // next response line

			if (!rsl::regex_match(line_reader.ptr(), m, re)) { return false; }
			res.version = rsl::string(m[1]);
			res.status = rsl::stoi(rsl::string(m[2])).value();
			res.reason = rsl::string(m[3]);
		}

		return true;
	}

	bool ClientBase::send(Request& req, Response& res, Error& error) {
		rsl::unique_lock<rsl::recursive_mutex> request_mutex_guard(request_mutex_);
		auto ret = send_(req, res, error);
		if (error == Error::SSLPeerCouldBeClosed_) {
			assert(!ret);
			ret = send_(req, res, error);
		}
		return ret;
	}

	bool ClientBase::send_(Request& req, Response& res, Error& error) {
		{
			rsl::unique_lock<rsl::mutex> guard(m_socket_mutex);

			// Set this to false immediately - if it ever gets set to true by the end of
			// the request, we know another thread instructed us to close the socket.
			m_socket_should_be_closed_when_request_is_done = false;

			auto is_alive = false;
			if (m_socket.is_open()) {
				is_alive = is_m_socketalive(m_socket.sock);

#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
				if (is_alive && is_ssl()) {
					if (is_ssl_peer_could_be_closed(m_socket.ssl, m_socket.sock)) {
						is_alive = false;
					}
				}
#endif

				if (!is_alive) {
					// Attempt to avoid sigpipe by shutting down non-gracefully if it seems
					// like the other side has already closed the connection Also, there
					// cannot be any requests in flight from other threads since we locked
					// request_mutex_, so safe to close everything immediately
					const bool shutdown_gracefully = false;
					shutdown_ssl(m_socket, shutdown_gracefully);
					shutdown_socket(m_socket);
					close_socket(m_socket);
				}
			}

			if (!is_alive) {
				if (!create_and_connect_socket(m_socket, error)) { return false; }

#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
				// TODO: refactoring
				if (is_ssl()) {
					auto& scli = static_cast<SSLClient&>(*this);
					if (!m_proxy_host.empty() && m_proxy_port != -1) {
						auto success = false;
						if (!scli.connect_with_proxy(m_socket, req.start_time, res, success,
							error)) {
							return success;
						}
					}

					if (!scli.initialize_ssl(m_socket, error)) { return false; }
				}
#endif
			}

			// Mark the current socket as being in use so that it cannot be closed by
			// anyone else while this request is ongoing, even though we will be
			// releasing the mutex.
			if (m_socket_requests_in_flight > 1) {
				REX_ASSERT_X(m_socket_requests_are_from_thread == rsl::this_thread::get_id(), "There are still socket requests in flight");
			}
			m_socket_requests_in_flight += 1;
			m_socket_requests_are_from_thread = rsl::this_thread::get_id();
		}

		for (const auto& header : m_default_headers) {
			if (req.headers.find(header.first) == req.headers.end()) {
				req.headers.insert(header);
			}
		}

		auto ret = false;
		auto close_connection = !m_keep_alive;

		auto se = scope_exit([&]() {
			// Briefly lock mutex in order to mark that a request is no longer ongoing
			rsl::unique_lock<rsl::mutex> guard(m_socket_mutex);
			m_socket_requests_in_flight -= 1;
			if (m_socket_requests_in_flight <= 0) {
				REX_ASSERT_X(m_socket_requests_in_flight == 0, "There are still socket requests in flight");
				m_socket_requests_are_from_thread = rsl::thread::id(-1);
			}

			if (m_socket_should_be_closed_when_request_is_done || close_connection ||
				!ret) {
				shutdown_ssl(m_socket, true);
				shutdown_socket(m_socket);
				close_socket(m_socket);
			}
			});

		ret = process_socket(m_socket, req.start_time, [&](Stream& strm) {
			return handle_request(strm, req, res, close_connection, error);
			});

		if (!ret) {
			if (error == Error::Success) { error = Error::Unknown; }
		}

		return ret;
	}

	Result ClientBase::send(const Request& req) {
		auto req2 = req;
		return send_(rsl::move(req2));
	}

	Result ClientBase::send_(Request&& req) {
		auto res = make_unique<Response>();
		auto error = Error::Success;
		auto ret = send(req, *res, error);
		return Result{ ret ? rsl::move(res) : nullptr, error, rsl::move(req.headers) };
	}

	bool ClientBase::handle_request(Stream& strm, Request& req,
		Response& res, bool close_connection,
		Error& error) {
		if (req.path.empty()) {
			error = Error::Connection;
			return false;
		}

		auto req_save = req;

		bool ret;

		if (!is_ssl() && !m_proxy_host.empty() && m_proxy_port != -1) {
			auto req2 = req;
			req2.path = "http://" + host_and_port_ + req.path;
			ret = process_request(strm, req2, res, close_connection, error);
			req = req2;
			req.path = req_save.path;
		}
		else {
			ret = process_request(strm, req, res, close_connection, error);
		}

		if (!ret) { return false; }

		if (res.get_header_value("Connection") == "close" ||
			(res.version == "HTTP/1.0" && res.reason != "Connection established")) {
			// TODO this requires a not-entirely-obvious chain of calls to be correct
			// for this to be safe.

			// This is safe to call because handle_request is only called by send_
			// which locks the request mutex during the process. It would be a bug
			// to call it from a different thread since it's a thread-safety issue
			// to do these things to the socket if another thread is using the socket.
			rsl::unique_lock<rsl::mutex> guard(m_socket_mutex);
			shutdown_ssl(m_socket, true);
			shutdown_socket(m_socket);
			close_socket(m_socket);
		}

		if (300 < res.status && res.status < 400 && m_follow_location) {
			req = req_save;
			ret = redirect(req, res, error);
		}

#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
		if ((res.status == StatusCode::Unauthorized_401 ||
			res.status == StatusCode::ProxyAuthenticationRequired_407) &&
			req.authorization_count_ < 5) {
			auto is_proxy = res.status == StatusCode::ProxyAuthenticationRequired_407;
			const auto& username =
				is_proxy ? proxy_digest_auth_username_ : digest_auth_username_;
			const auto& password =
				is_proxy ? proxy_digest_auth_password_ : digest_auth_password_;

			if (!username.empty() && !password.empty()) {
				rsl::map<rsl::string, rsl::string> auth;
				if (parse_www_authenticate(res, auth, is_proxy)) {
					Request new_req = req;
					new_req.authorization_count_ += 1;
					new_req.headers.erase(is_proxy ? "Proxy-Authorization"
						: "Authorization");
					new_req.headers.insert(make_digest_authentication_header(
						req, auth, new_req.authorization_count_, random_string(10),
						username, password, is_proxy));

					Response new_res;

					ret = send(new_req, new_res, error);
					if (ret) { res = new_res; }
				}
			}
		}
#endif

		return ret;
	}

	bool ClientBase::redirect(Request& req, Response& res, Error& error) {
		if (req.redirect_count_ == 0) {
			error = Error::ExceedRedirectCount;
			return false;
		}

		auto location = res.get_header_value("location");
		if (location.empty()) { return false; }

		thread_local const std::regex re(
			R"((?:(https?):)?(?://(?:\[([a-fA-F\d:]+)\]|([^:/?#]+))(?::(\d+))?)?([^?#]*)(\?[^#]*)?(?:#.*)?)");

		rsl::smatch m;
		if (!rsl::regex_match(location, m, re)) { return false; }

		auto scheme = is_ssl() ? "https" : "http";

		auto next_scheme = m[1].str();
		auto next_host = m[2].str();
		if (next_host.empty()) { next_host = m[3].str(); }
		auto port_str = m[4].str();
		auto next_path = m[5].str();
		auto next_query = m[6].str();

		auto next_port = m_port;
		if (!port_str.empty()) {
			next_port = rsl::stoi(port_str);
		}
		else if (!next_scheme.empty()) {
			next_port = next_scheme == "https" ? 443 : 80;
		}

		if (next_scheme.empty()) { next_scheme = scheme; }
		if (next_host.empty()) { next_host = m_host; }
		if (next_path.empty()) { next_path = "/"; }

		auto path = decode_url(next_path, true) + next_query;

		if (next_scheme == scheme && next_host == m_host && next_port == m_port) {
			return redirect(*this, req, res, path, location, error);
		}
		else {
			if (next_scheme == "https") {
#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
				SSLClient cli(next_host, next_port);
				cli.copy_settings(*this);
				if (ca_cert_store_) { cli.set_ca_cert_store(ca_cert_store_); }
				return redirect(cli, req, res, path, location, error);
#else
				return false;
#endif
			}
			else {
				ClientBase cli(next_host, next_port);
				cli.copy_settings(*this);
				return redirect(cli, req, res, path, location, error);
			}
		}
	}

	bool ClientBase::write_content_with_provider(Stream& strm,
		const Request& req,
		Error& error) const {
		auto is_shutting_down = []() { return false; };

		if (req.is_chunked_content_provider_) {
			// TODO: Brotli support
			rsl::unique_ptr<compressor> compressor;
#ifdef CPPHTTPLIB_ZLIB_SUPPORT
			if (m_compress) {
				compressor = make_unique<gzip_compressor>();
			}
			else
#endif
			{
				compressor = make_unique<nocompressor>();
			}

			return write_content_chunked(strm, req.content_provider_,
				is_shutting_down, *compressor, error);
		}
		else {
			return write_content(strm, req.content_provider_, 0,
				req.content_length_, is_shutting_down, error);
		}
	}

	bool ClientBase::write_request(Stream& strm, Request& req,
		bool close_connection, Error& error) {
		// Prepare additional headers
		if (close_connection) {
			if (!req.has_header("Connection")) {
				req.set_header("Connection", "close");
			}
		}

		if (!req.has_header("Host")) {
			if (is_ssl()) {
				if (m_port == 443) {
					req.set_header("Host", m_host);
				}
				else {
					req.set_header("Host", host_and_port_);
				}
			}
			else {
				if (m_port == 80) {
					req.set_header("Host", m_host);
				}
				else {
					req.set_header("Host", host_and_port_);
				}
			}
		}

		if (!req.has_header("Accept")) { req.set_header("Accept", "*/*"); }

		if (!req.content_receiver) {
			if (!req.has_header("Accept-Encoding")) {
				rsl::string accept_encoding;
#ifdef CPPHTTPLIB_BROTLI_SUPPORT
				accept_encoding = "br";
#endif
#ifdef CPPHTTPLIB_ZLIB_SUPPORT
				if (!accept_encoding.empty()) { accept_encoding += ", "; }
				accept_encoding += "gzip, deflate";
#endif
#ifdef CPPHTTPLIB_ZSTD_SUPPORT
				if (!accept_encoding.empty()) { accept_encoding += ", "; }
				accept_encoding += "zstd";
#endif
				req.set_header("Accept-Encoding", accept_encoding);
			}

#ifndef CPPHTTPLIB_NO_DEFAULT_USER_AGENT
			if (!req.has_header("User-Agent")) {
				auto agent = rsl::string("cpp-httplib/") + CPPHTTPLIB_VERSION;
				req.set_header("User-Agent", agent);
			}
#endif
		};

		if (req.body.empty()) {
			if (req.content_provider_) {
				if (!req.is_chunked_content_provider_) {
					if (!req.has_header("Content-Length")) {
						auto length = rsl::to_string(req.content_length_);
						req.set_header("Content-Length", length);
					}
				}
			}
			else {
				if (req.method == "POST" || req.method == "PUT" ||
					req.method == "PATCH") {
					req.set_header("Content-Length", "0");
				}
			}
		}
		else {
			if (!req.has_header("Content-Type")) {
				req.set_header("Content-Type", "text/plain");
			}

			if (!req.has_header("Content-Length")) {
				auto length = rsl::to_string(req.body.size());
				req.set_header("Content-Length", length);
			}
		}

		if (!m_basic_auth_password.empty() || !m_basic_auth_username.empty()) {
			if (!req.has_header("Authorization")) {
				req.headers.insert(make_basic_authentication_header(
					m_basic_auth_username, m_basic_auth_password, false));
			}
		}

		if (!m_proxy_basic_auth_username.empty() &&
			!m_proxy_basic_auth_password.empty()) {
			if (!req.has_header("Proxy-Authorization")) {
				req.headers.insert(make_basic_authentication_header(
					m_proxy_basic_auth_username, m_proxy_basic_auth_password, true));
			}
		}

		if (!m_bearer_token_auth_token.empty()) {
			if (!req.has_header("Authorization")) {
				req.headers.insert(make_bearer_token_authentication_header(
					m_bearer_token_auth_token, false));
			}
		}

		if (!m_proxy_bearer_token_auth_token.empty()) {
			if (!req.has_header("Proxy-Authorization")) {
				req.headers.insert(make_bearer_token_authentication_header(
					m_proxy_bearer_token_auth_token, true));
			}
		}

		// Request line and headers
		{
			BufferStream bstrm;

			const auto& path_with_query =
				req.params.empty() ? req.path
				: append_query_params(req.path, req.params);

			const auto& path =
				m_url_encode ? encode_url(path_with_query) : path_with_query;

			write_request_line(bstrm, req.method, path);

			m_header_writer(bstrm, req.headers);

			// Flush buffer
			auto& data = bstrm.get_buffer();
			if (!write_data(strm, data.data(), data.size())) {
				error = Error::Write;
				return false;
			}
		}

		// Body
		if (req.body.empty()) {
			return write_content_with_provider(strm, req, error);
		}

		if (!write_data(strm, req.body.data(), req.body.size())) {
			error = Error::Write;
			return false;
		}

		return true;
	}

	rsl::unique_ptr<Response> ClientBase::send_with_content_provider(
		Request& req, const char* body, size_t content_length,
		ContentProvider content_provider,
		ContentProviderWithoutLength content_provider_without_length,
		rsl::string_view content_type, Error& error) {
		if (!content_type.empty()) { req.set_header("Content-Type", content_type); }

#ifdef CPPHTTPLIB_ZLIB_SUPPORT
		if (m_compress) { req.set_header("Content-Encoding", "gzip"); }
#endif

#ifdef CPPHTTPLIB_ZLIB_SUPPORT
		if (m_compress && !content_provider_without_length) {
			// TODO: Brotli support
			gzip_compressor compressor;

			if (content_provider) {
				auto ok = true;
				size_t offset = 0;
				DataSink data_sink;

				data_sink.write = [&](const char* data, size_t data_len) -> bool {
					if (ok) {
						auto last = offset + data_len == content_length;

						auto ret = compressor.compress(
							data, data_len, last,
							[&](const char* compressed_data, size_t compressed_data_len) {
								req.body.append(compressed_data, compressed_data_len);
								return true;
							});

						if (ret) {
							offset += data_len;
						}
						else {
							ok = false;
						}
					}
					return ok;
					};

				while (ok && offset < content_length) {
					if (!content_provider(offset, content_length - offset, data_sink)) {
						error = Error::Canceled;
						return nullptr;
					}
				}
			}
			else {
				if (!compressor.compress(body, content_length, true,
					[&](const char* data, size_t data_len) {
						req.body.append(data, data_len);
						return true;
					})) {
					error = Error::Compression;
					return nullptr;
				}
			}
		}
		else
#endif
		{
			if (content_provider) {
				req.content_length_ = content_length;
				req.content_provider_ = rsl::move(content_provider);
				req.is_chunked_content_provider_ = false;
			}
			else if (content_provider_without_length) {
				req.content_length_ = 0;
				req.content_provider_ = ContentProviderAdapter(
					rsl::move(content_provider_without_length));
				req.is_chunked_content_provider_ = true;
				req.set_header("Transfer-Encoding", "chunked");
			}
			else {
				req.body.assign(body, content_length);
			}
		}

		auto res = rsl::make_unique<Response>();
		return send(req, *res, error) ? rsl::move(res) : nullptr;
	}

	Result ClientBase::send_with_content_provider(
		rsl::string_view method, rsl::string_view path, const Headers& headers,
		const char* body, size_t content_length, ContentProvider content_provider,
		ContentProviderWithoutLength content_provider_without_length,
		rsl::string_view content_type, Progress progress) {
		Request req;
		req.method.assign(method);
		req.headers = headers;
		req.path.assign(path);
		req.progress = progress;
		if (m_max_timeout_msec > 0) {
			req.start_time = rsl::chrono::steady_clock::now();
		}

		auto error = Error::Success;

		auto res = send_with_content_provider(
			req, body, content_length, rsl::move(content_provider),
			rsl::move(content_provider_without_length), content_type, error);

		return Result{ rsl::move(res), error, rsl::move(req.headers) };
	}

	rsl::string
		ClientBase::adjust_host_string(rsl::string_view host) const {
		if (host.find(':') != rsl::string::npos()) { return "[" + host + "]"; }
		return rsl::string(host);
	}

	bool ClientBase::process_request(Stream& strm, Request& req,
		Response& res, bool close_connection,
		Error& error) {
		// Send request
		if (!write_request(strm, req, close_connection, error)) { return false; }

#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
		if (is_ssl()) {
			auto is_proxy_enabled = !m_proxy_host.empty() && m_proxy_port != -1;
			if (!is_proxy_enabled) {
				if (is_ssl_peer_could_be_closed(m_socket.ssl, m_socket.sock)) {
					error = Error::SSLPeerCouldBeClosed_;
					return false;
				}
			}
		}
#endif

		// Receive response and headers
		if (!read_response_line(strm, req, res) ||
			!read_headers(strm, res.headers)) {
			error = Error::Read;
			return false;
		}

		// Body
		if ((res.status != StatusCode::NoContent_204) && req.method != "HEAD" &&
			req.method != "CONNECT") {
			auto redirect = 300 < res.status && res.status < 400 &&
				res.status != StatusCode::NotModified_304 &&
				m_follow_location;

			if (req.response_handler && !redirect) {
				if (!req.response_handler(res)) {
					error = Error::Canceled;
					return false;
				}
			}

			auto out =
				req.content_receiver
				? static_cast<ContentReceiverWithProgress>(
					[&](const char* buf, size_t n, uint64_t off, uint64_t len) {
						if (redirect) { return true; }
						auto ret = req.content_receiver(buf, n, off, len);
						if (!ret) { error = Error::Canceled; }
						return ret;
					})
				: static_cast<ContentReceiverWithProgress>(
					[&](const char* buf, size_t n, uint64_t /*off*/,
						uint64_t /*len*/) {
							REX_ASSERT_X(res.body.size() + n <= res.body.max_size(), "out of bounds access");
							res.body.append(buf, n);
							return true;
					});

			auto progress = [&](uint64_t current, uint64_t total) {
				if (!req.progress || redirect) { return true; }
				auto ret = req.progress(current, total);
				if (!ret) { error = Error::Canceled; }
				return ret;
				};

			if (res.has_header("Content-Length")) {
				if (!req.content_receiver) {
					auto len = res.get_header_value_u64("Content-Length");
					if (len > res.body.max_size()) {
						error = Error::Read;
						return false;
					}
					res.body.reserve(static_cast<size_t>(len));
				}
			}

			if (res.status != StatusCode::NotModified_304) {
				int dummy_status;
				if (!read_content(strm, res, (rsl::numeric_limits<size_t>::max)(),
					dummy_status, rsl::move(progress),
					rsl::move(out), m_dem_compress)) {
					if (error != Error::Canceled) { error = Error::Read; }
					return false;
				}
			}
		}

		// Log
		//if (logger_) { logger_(req, res); }

		return true;
	}

	ContentProviderWithoutLength ClientBase::get_multipart_content_provider(
		rsl::string_view boundary, const MultipartFormDataItems& items,
		const MultipartFormDataProviderItems& provider_items) const {
		size_t cur_item = 0;
		size_t cur_start = 0;
		// cur_item and cur_start are copied to within the rsl::function and maintain
		// state between successive calls
		return [&, cur_item, cur_start](size_t offset,
			DataSink& sink) mutable -> bool {
				if (!offset && !items.empty()) {
					sink.os << serialize_multipart_formdata(items, boundary, false);
					return true;
				}
				else if (cur_item < provider_items.size()) {
					if (!cur_start) {
						const auto& begin = serialize_multipart_formdata_item_begin(
							provider_items[cur_item], boundary);
						offset += begin.size();
						cur_start = offset;
						sink.os << begin;
					}

					DataSink cur_sink;
					auto has_data = true;
					cur_sink.write = sink.write;
					cur_sink.done = [&]() { has_data = false; };

					if (!provider_items[cur_item].provider(offset - cur_start, cur_sink)) {
						return false;
					}

					if (!has_data) {
						sink.os << serialize_multipart_formdata_item_end();
						cur_item++;
						cur_start = 0;
					}
					return true;
				}
				else {
					sink.os << serialize_multipart_formdata_finish(boundary);
					sink.done();
					return true;
				}
			};
	}

	bool ClientBase::process_socket(
		const Socket& socket,
		rsl::chrono::time_point<rsl::chrono::steady_clock> start_time,
		rsl::function<bool(Stream& strm)> callback) {
		return process_client_socket(
			socket.sock, m_read_timeout_sec, m_read_timeout_usec, m_write_timeout_sec,
			m_write_timeout_usec, m_max_timeout_msec, start_time, rsl::move(callback));
	}

	bool ClientBase::is_ssl() const { return false; }

	void ClientBase::stop() {
		rsl::unique_lock<rsl::mutex> guard(m_socket_mutex);

		// If there is anything ongoing right now, the ONLY thread-safe thing we can
		// do is to shutdown_socket, so that threads using this socket suddenly
		// discover they can't read/write any more and error out. Everything else
		// (closing the socket, shutting ssl down) is unsafe because these actions are
		// not thread-safe.
		if (m_socket_requests_in_flight > 0) {
			shutdown_socket(m_socket);

			// Aside from that, we set a flag for the socket to be closed when we're
			// done.
			m_socket_should_be_closed_when_request_is_done = true;
			return;
		}

		// Otherwise, still holding the mutex, we can shut everything down ourselves
		shutdown_ssl(m_socket, true);
		shutdown_socket(m_socket);
		close_socket(m_socket);
	}

	rsl::string_view ClientBase::host() const { return m_host; }

	int ClientBase::port() const { return m_port; }

	size_t ClientBase::is_socket_open() const {
		rsl::unique_lock<rsl::mutex> guard(m_socket_mutex);
		return m_socket.is_open();
	}

	socket_t ClientBase::socket() const { return m_socket.sock; }

	void ClientBase::set_connection_timeout(time_t sec, time_t usec) {
		m_connection_timeout_sec = sec;
		m_connection_timeout_usec = usec;
	}

	void ClientBase::set_read_timeout(time_t sec, time_t usec) {
		m_read_timeout_sec = sec;
		m_read_timeout_usec = usec;
	}

	void ClientBase::set_write_timeout(time_t sec, time_t usec) {
		m_write_timeout_sec = sec;
		m_write_timeout_usec = usec;
	}

	void ClientBase::set_max_timeout(time_t msec) {
		m_max_timeout_msec = msec;
	}

	void ClientBase::set_basic_auth(rsl::string_view username,
		rsl::string_view password) {
		m_basic_auth_username = username;
		m_basic_auth_password = password;
	}

	void ClientBase::set_bearer_token_auth(rsl::string_view token) {
		m_bearer_token_auth_token = token;
	}

#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
	void ClientBase::set_digest_auth(rsl::string_view username,
		rsl::string_view password) {
		digest_auth_username_ = username;
		digest_auth_password_ = password;
	}
#endif

	void ClientBase::set_keep_alive(bool on) { m_keep_alive = on; }

	void ClientBase::set_follow_location(bool on) { m_follow_location = on; }

	void ClientBase::set_url_encode(bool on) { m_url_encode = on; }

	void
		ClientBase::set_hostname_addr_map(rsl::map<rsl::string, rsl::string> addr_map) {
		m_addr_map = rsl::move(addr_map);
	}

	void ClientBase::set_default_headers(Headers headers) {
		m_default_headers = rsl::move(headers);
	}

	void ClientBase::set_header_writer(
		rsl::function<s32(Stream&, Headers&)> const& writer) {
		m_header_writer = writer;
	}

	void ClientBase::set_address_family(int family) {
		m_address_family = family;
	}

	void ClientBase::set_tcp_nodelay(bool on) { m_tcp_nodelay = on; }

	void ClientBase::set_ipv6_v6only(bool on) { m_ipv6_v6only = on; }

	void ClientBase::set_socket_options(SocketOptions m_socketoptions) {
		m_socketoptions = rsl::move(m_socketoptions);
	}

	void ClientBase::set_compress(bool on) { m_compress = on; }

	void ClientBase::set_decompress(bool on) { m_decompress = on; }

	void ClientBase::set_interface(rsl::string_view intf) {
		m_interface.assign(intf);
	}

	void ClientBase::set_proxy(rsl::string_view host, int port) {
		m_proxy_host.assign(host);
		m_proxy_port = port;
	}

	void ClientBase::set_proxy_basic_auth(rsl::string_view username,
		rsl::string_view password) {
		m_proxy_basic_auth_username.assign(username);
		m_proxy_basic_auth_password.assign(password);
	}

	void ClientBase::set_proxy_bearer_token_auth(rsl::string_view token) {
		m_proxy_bearer_token_auth_token = token;
	}

	void ClientBase::send_with_content_provider(const SendRequestDesc& desc)
	{
		Request req;

		// Prepare the request struct
		req.method = desc.method;
		req.headers = desc.headers;
		req.path = desc.path;
		req.progress = desc.progress;

		if (m_max_timeout_msec > 0)
		{
			req.start_time = rsl::chrono::steady_clock::now();
		}

		REX_ASSERT_X(desc.content_provider != nullptr ^ !desc.body.empty(), "You cannot have a body if you're using a content provider");

		// Prepare the request data
		if (desc.content_provider)
		{
			req.content_provider = desc.content_provider;

			if (!desc.content_provider->knowns_content_length())
			{
				req.set_header("Transfer-Encoding", "chunked");
			}
		}
		else
		{
			req.body = desc.body;
	}

#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
	void ClientBase::set_proxy_digest_auth(rsl::string_view username,
		rsl::string_view password) {
		proxy_digest_auth_username_ = username;
		proxy_digest_auth_password_ = password;
	}

	void ClientBase::set_ca_cert_path(rsl::string_view ca_cert_file_path,
		rsl::string_view ca_cert_dir_path) {
		ca_cert_file_path_ = ca_cert_file_path;
		ca_cert_dir_path_ = ca_cert_dir_path;
	}

	void ClientBase::set_ca_cert_store(X509_STORE* ca_cert_store) {
		if (ca_cert_store && ca_cert_store != ca_cert_store_) {
			ca_cert_store_ = ca_cert_store;
		}
	}

	X509_STORE* ClientBase::create_ca_cert_store(const char* ca_cert,
		rsl::size_t size) const {
		auto mem = BIO_new_mem_buf(ca_cert, static_cast<int>(size));
		auto se = scope_exit([&] { BIO_free_all(mem); });
		if (!mem) { return nullptr; }

		auto inf = PEM_X509_INFO_read_bio(mem, nullptr, nullptr, nullptr);
		if (!inf) { return nullptr; }

		auto cts = X509_STORE_new();
		if (cts) {
			for (auto i = 0; i < static_cast<int>(sk_X509_INFO_num(inf)); i++) {
				auto itmp = sk_X509_INFO_value(inf, i);
				if (!itmp) { continue; }

				if (itmp->x509) { X509_STORE_add_cert(cts, itmp->x509); }
				if (itmp->crl) { X509_STORE_add_crl(cts, itmp->crl); }
			}
		}

		sk_X509_INFO_pop_free(inf, X509_INFO_free);
		return cts;
	}

	void ClientBase::enable_server_certificate_verification(bool enabled) {
		server_certificate_verification_ = enabled;
	}

	void ClientBase::enable_server_hostname_verification(bool enabled) {
		server_hostname_verification_ = enabled;
	}

	void ClientBase::set_server_certificate_verifier(
		rsl::function<SSLVerifierResponse(SSL* ssl)> verifier) {
		server_certificate_verifier_ = verifier;
	}
#endif

	//void ClientBase::set_logger(Logger logger) {
	//	logger_ = rsl::move(logger);
	//}

//	Client::Client(rsl::string_view scheme_host_port)
//		: Client(scheme_host_port, rsl::string(), rsl::string()) {}
//
//	Client::Client(rsl::string_view scheme_host_port,
//		rsl::string_view client_cert_path,
//		rsl::string_view client_key_path) {
//		const static std::regex re(
//			R"((?:([a-z]+):\/\/)?(?:\[([a-fA-F\d:]+)\]|([^:/?#]+))(?::(\d+))?)");
//
//		rsl::smatch m;
//		if (rsl::regex_match(scheme_host_port, m, re)) {
//			auto scheme = m[1].str();
//
//#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
//			if (!scheme.empty() && (scheme != "http" && scheme != "https")) {
//#else
//			if (!scheme.empty() && scheme != "http") {
//#endif
//#ifndef CPPHTTPLIB_NO_EXCEPTIONS
//				rsl::string msg = "'" + scheme + "' scheme is not supported.";
//				throw rsl::invalid_argument(msg);
//#endif
//				return;
//			}
//
//			auto is_ssl = scheme == "https";
//
//			auto host = m[2].str();
//			if (host.empty()) { host = m[3].str(); }
//
//			auto port_str = m[4].str();
//			auto port = !port_str.empty() ? rsl::stoi(port_str) : (is_ssl ? 443 : 80);
//
//			if (is_ssl) {
//#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
//				cli_ = make_unique<SSLClient>(host, port, client_cert_path,
//					client_key_path);
//				is_ssl_ = is_ssl;
//#endif
//			}
//			else {
//				cli_ = make_unique<ClientBase>(host, port, client_cert_path,
//					client_key_path);
//			}
//		}
//		else {
//			// NOTE: Update TEST(UniversalClientImplTest, Ipv6LiteralAddress)
//			// if port param below changes.
//			cli_ = make_unique<ClientBase>(scheme_host_port, 80,
//				client_cert_path, client_key_path);
//		}
//	} // namespace detail

#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
	void Client::set_ca_cert_store(X509_STORE * ca_cert_store) {
		if (is_ssl_) {
			static_cast<SSLClient&>(*cli_).set_ca_cert_store(ca_cert_store);
		}
		else {
			cli_->set_ca_cert_store(ca_cert_store);
		}
	}

	void Client::load_ca_cert_store(const char* ca_cert, rsl::size_t size) {
		set_ca_cert_store(cli_->create_ca_cert_store(ca_cert, size));
	}

	long Client::get_openssl_verify_result() const {
		if (is_ssl_) {
			return static_cast<SSLClient&>(*cli_).get_openssl_verify_result();
		}
		return -1; // NOTE: -1 doesn't match any of X509_V_ERR_???
	}

	SSL_CTX* Client::ssl_context() const {
		if (is_ssl_) { return static_cast<SSLClient&>(*cli_).ssl_context(); }
		return nullptr;
	}
#endif


#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
	// SSL HTTP client implementation
	SSLClient::SSLClient(rsl::string_view host)
		: SSLClient(host, 443, rsl::string(), rsl::string()) {}

	SSLClient::SSLClient(rsl::string_view host, int port)
		: SSLClient(host, port, rsl::string(), rsl::string()) {}

	SSLClient::SSLClient(rsl::string_view host, int port,
		rsl::string_view client_cert_path,
		rsl::string_view client_key_path,
		rsl::string_view private_key_password)
		: ClientBase(host, port, client_cert_path, client_key_path) {
		ctx_ = SSL_CTX_new(TLS_client_method());

		SSL_CTX_set_min_proto_version(ctx_, TLS1_2_VERSION);

		split(&m_host[0], &m_host[m_host.size()], '.',
			[&](const char* b, const char* e) {
				host_components_.emplace_back(b, e);
			});

		if (!client_cert_path.empty() && !client_key_path.empty()) {
			if (!private_key_password.empty()) {
				SSL_CTX_set_default_passwd_cb_userdata(
					ctx_, reinterpret_cast<void*>(
						const_cast<char*>(private_key_password.c_str())));
			}

			if (SSL_CTX_use_certificate_file(ctx_, client_cert_path.c_str(),
				SSL_FILETYPE_PEM) != 1 ||
				SSL_CTX_use_PrivateKey_file(ctx_, client_key_path.c_str(),
					SSL_FILETYPE_PEM) != 1) {
				SSL_CTX_free(ctx_);
				ctx_ = nullptr;
			}
		}
	}

	SSLClient::SSLClient(rsl::string_view host, int port,
		X509 * client_cert, EVP_PKEY * client_key,
		rsl::string_view private_key_password)
		: ClientBase(host, port) {
		ctx_ = SSL_CTX_new(TLS_client_method());

		split(&m_host[0], &m_host[m_host.size()], '.',
			[&](const char* b, const char* e) {
				host_components_.emplace_back(b, e);
			});

		if (client_cert != nullptr && client_key != nullptr) {
			if (!private_key_password.empty()) {
				SSL_CTX_set_default_passwd_cb_userdata(
					ctx_, reinterpret_cast<void*>(
						const_cast<char*>(private_key_password.c_str())));
			}

			if (SSL_CTX_use_certificate(ctx_, client_cert) != 1 ||
				SSL_CTX_use_PrivateKey(ctx_, client_key) != 1) {
				SSL_CTX_free(ctx_);
				ctx_ = nullptr;
			}
		}
	}

	SSLClient::~SSLClient() {
		if (ctx_) { SSL_CTX_free(ctx_); }
		// Make sure to shut down SSL since shutdown_ssl will resolve to the
		// base function rather than the derived function once we get to the
		// base class destructor, and won't free the SSL (causing a leak).
		shutdown_ssl_impl(m_socket, true);
	}

	bool SSLClient::is_valid() const { return ctx_; }

	void SSLClient::set_ca_cert_store(X509_STORE * ca_cert_store) {
		if (ca_cert_store) {
			if (ctx_) {
				if (SSL_CTX_get_cert_store(ctx_) != ca_cert_store) {
					// Free memory allocated for old cert and use new store `ca_cert_store`
					SSL_CTX_set_cert_store(ctx_, ca_cert_store);
				}
			}
			else {
				X509_STORE_free(ca_cert_store);
			}
		}
	}

	void SSLClient::load_ca_cert_store(const char* ca_cert,
		rsl::size_t size) {
		set_ca_cert_store(ClientBase::create_ca_cert_store(ca_cert, size));
	}

	long SSLClient::get_openssl_verify_result() const {
		return verify_result_;
	}

	SSL_CTX* SSLClient::ssl_context() const { return ctx_; }

	bool SSLClient::create_and_connect_socket(Socket & socket, Error & error) {
		return is_valid() && ClientBase::create_and_connect_socket(socket, error);
	}

	// Assumes that m_socket_mutex is locked and that there are no requests in flight
	bool SSLClient::connect_with_proxy(
		Socket & socket,
		rsl::chrono::time_point<rsl::chrono::steady_clock> start_time,
		Response & res, bool& success, Error & error) {
		success = true;
		Response proxy_res;
		if (!process_client_socket(
			socket.sock, m_read_timeout_sec, m_read_timeout_usec,
			m_write_timeout_sec, m_write_timeout_usec, m_max_timeout_msec,
			start_time, [&](Stream& strm) {
				Request req2;
				req2.method = "CONNECT";
				req2.path = host_and_port_;
				if (m_max_timeout_msec > 0) {
					req2.start_time = rsl::chrono::steady_clock::now();
				}
				return process_request(strm, req2, proxy_res, false, error);
			})) {
			// Thread-safe to close everything because we are assuming there are no
			// requests in flight
			shutdown_ssl(socket, true);
			shutdown_socket(socket);
			close_socket(socket);
			success = false;
			return false;
		}

		if (proxy_res.status == StatusCode::ProxyAuthenticationRequired_407) {
			if (!proxy_digest_auth_username_.empty() &&
				!proxy_digest_auth_password_.empty()) {
				rsl::map<rsl::string, rsl::string> auth;
				if (parse_www_authenticate(proxy_res, auth, true)) {
					proxy_res = Response();
					if (!process_client_socket(
						socket.sock, m_read_timeout_sec, m_read_timeout_usec,
						m_write_timeout_sec, m_write_timeout_usec, m_max_timeout_msec,
						start_time, [&](Stream& strm) {
							Request req3;
							req3.method = "CONNECT";
							req3.path = host_and_port_;
							req3.headers.insert(make_digest_authentication_header(
								req3, auth, 1, random_string(10),
								proxy_digest_auth_username_, proxy_digest_auth_password_,
								true));
							if (m_max_timeout_msec > 0) {
								req3.start_time = rsl::chrono::steady_clock::now();
							}
							return process_request(strm, req3, proxy_res, false, error);
						})) {
						// Thread-safe to close everything because we are assuming there are
						// no requests in flight
						shutdown_ssl(socket, true);
						shutdown_socket(socket);
						close_socket(socket);
						success = false;
						return false;
					}
				}
			}
		}

		// If status code is not 200, proxy request is failed.
		// Set error to ProxyConnection and return proxy response
		// as the response of the request
		if (proxy_res.status != StatusCode::OK_200) {
			error = Error::ProxyConnection;
			res = rsl::move(proxy_res);
			// Thread-safe to close everything because we are assuming there are
			// no requests in flight
			shutdown_ssl(socket, true);
			shutdown_socket(socket);
			close_socket(socket);
			return false;
		}

		return true;
	}

	bool SSLClient::load_certs() {
		auto ret = true;

		rsl::call_once(initialize_cert_, [&]() {
			rsl::unique_lock<rsl::mutex> guard(ctx_mutex_);
			if (!ca_cert_file_path_.empty()) {
				if (!SSL_CTX_load_verify_locations(ctx_, ca_cert_file_path_.c_str(),
					nullptr)) {
					ret = false;
				}
			}
			else if (!ca_cert_dir_path_.empty()) {
				if (!SSL_CTX_load_verify_locations(ctx_, nullptr,
					ca_cert_dir_path_.c_str())) {
					ret = false;
				}
			}
			else {
				auto loaded = false;
#ifdef _WIN32
				loaded =
					load_system_certs_on_windows(SSL_CTX_get_cert_store(ctx_));
#elif defined(CPPHTTPLIB_USE_CERTS_FROM_MACOSX_KEYCHAIN) && defined(__APPLE__)
#if TARGET_OS_OSX
				loaded = load_system_certs_on_macos(SSL_CTX_get_cert_store(ctx_));
#endif // TARGET_OS_OSX
#endif // _WIN32
				if (!loaded) { SSL_CTX_set_default_verify_paths(ctx_); }
			}
			});

		return ret;
	}

	bool SSLClient::initialize_ssl(Socket & socket, Error & error) {
		auto ssl = ssl_new(
			socket.sock, ctx_, ctx_mutex_,
			[&](SSL* ssl2) {
				if (server_certificate_verification_) {
					if (!load_certs()) {
						error = Error::SSLLoadingCerts;
						return false;
					}
					SSL_set_verify(ssl2, SSL_VERIFY_NONE, nullptr);
				}

				if (!ssl_connect_or_accept_nonblocking(
					socket.sock, ssl2, SSL_connect, m_connection_timeout_sec,
					m_connection_timeout_usec)) {
					error = Error::SSLConnection;
					return false;
				}

				if (server_certificate_verification_) {
					auto verification_status = SSLVerifierResponse::NoDecisionMade;

					if (server_certificate_verifier_) {
						verification_status = server_certificate_verifier_(ssl2);
					}

					if (verification_status == SSLVerifierResponse::CertificateRejected) {
						error = Error::SSLServerVerification;
						return false;
					}

					if (verification_status == SSLVerifierResponse::NoDecisionMade) {
						verify_result_ = SSL_get_verify_result(ssl2);

						if (verify_result_ != X509_V_OK) {
							error = Error::SSLServerVerification;
							return false;
						}

						auto server_cert = SSL_get1_peer_certificate(ssl2);
						auto se = scope_exit([&] { X509_free(server_cert); });

						if (server_cert == nullptr) {
							error = Error::SSLServerVerification;
							return false;
						}

						if (server_hostname_verification_) {
							if (!verify_host(server_cert)) {
								error = Error::SSLServerHostnameVerification;
								return false;
							}
						}
					}
				}

				return true;
			},
			[&](SSL* ssl2) {
#if defined(OPENSSL_IS_BORINGSSL)
				SSL_set_tlsext_host_name(ssl2, m_host.c_str());
#else
				// NOTE: Direct call instead of using the OpenSSL macro to suppress
				// -Wold-style-cast warning
				SSL_ctrl(ssl2, SSL_CTRL_SET_TLSEXT_HOSTNAME, TLSEXT_NAMETYPE_host_name,
					static_cast<void*>(const_cast<char*>(m_host.c_str())));
#endif
				return true;
			});

		if (ssl) {
			socket.ssl = ssl;
			return true;
		}

		shutdown_socket(socket);
		close_socket(socket);
		return false;
	}

	void SSLClient::shutdown_ssl(Socket & socket, bool shutdown_gracefully) {
		shutdown_ssl_impl(socket, shutdown_gracefully);
	}

	void SSLClient::shutdown_ssl_impl(Socket & socket,
		bool shutdown_gracefully) {
		if (socket.sock == INVALID_SOCKET) {
			assert(socket.ssl == nullptr);
			return;
		}
		if (socket.ssl) {
			ssl_delete(ctx_mutex_, socket.ssl, socket.sock,
				shutdown_gracefully);
			socket.ssl = nullptr;
		}
		assert(socket.ssl == nullptr);
	}

	bool SSLClient::process_socket(
		const Socket & socket,
		rsl::chrono::time_point<rsl::chrono::steady_clock> start_time,
		rsl::function<bool(Stream & strm)> callback) {
		assert(socket.ssl);
		return process_client_m_socketssl(
			socket.ssl, socket.sock, m_read_timeout_sec, m_read_timeout_usec,
			m_write_timeout_sec, m_write_timeout_usec, m_max_timeout_msec, start_time,
			rsl::move(callback));
	}

	bool SSLClient::is_ssl() const { return true; }

	bool SSLClient::verify_host(X509 * server_cert) const {
		/* Quote from RFC2818 section 3.1 "Server Identity"

			 If a subjectAltName extension of type dNSName is present, that MUST
			 be used as the identity. Otherwise, the (most specific) Common Name
			 field in the Subject field of the certificate MUST be used. Although
			 the use of the Common Name is existing practice, it is deprecated and
			 Certification Authorities are encouraged to use the dNSName instead.

			 Matching is performed using the matching rules specified by
			 [RFC2459].  If more than one identity of a given type is present in
			 the certificate (e.g., more than one dNSName name, a match in any one
			 of the set is considered acceptable.) Names may contain the wildcard
			 character * which is considered to match any single domain name
			 component or component fragment. E.g., *.a.com matches foo.a.com but
			 not bar.foo.a.com. f*.com matches foo.com but not bar.com.

			 In some cases, the URI is specified as an IP address rather than a
			 hostname. In this case, the iPAddress subjectAltName must be present
			 in the certificate and must exactly match the IP in the URI.

		*/
		return verify_host_with_subject_alt_name(server_cert) ||
			verify_host_with_common_name(server_cert);
	}

	bool
		SSLClient::verify_host_with_subject_alt_name(X509 * server_cert) const {
		auto ret = false;

		auto type = GEN_DNS;

		struct in6_addr addr6 = {};
		struct in_addr addr = {};
		size_t addr_len = 0;

#ifndef __MINGW32__
		if (inet_pton(AF_INET6, m_host.c_str(), &addr6)) {
			type = GEN_IPADD;
			addr_len = sizeof(struct in6_addr);
		}
		else if (inet_pton(AF_INET, m_host.c_str(), &addr)) {
			type = GEN_IPADD;
			addr_len = sizeof(struct in_addr);
		}
#endif

		auto alt_names = static_cast<const struct stack_st_GENERAL_NAME*>(
			X509_get_ext_d2i(server_cert, NID_subject_alt_name, nullptr, nullptr));

		if (alt_names) {
			auto dsn_matched = false;
			auto ip_matched = false;

			auto count = sk_GENERAL_NAME_num(alt_names);

			for (decltype(count) i = 0; i < count && !dsn_matched; i++) {
				auto val = sk_GENERAL_NAME_value(alt_names, i);
				if (val->type == type) {
					auto name =
						reinterpret_cast<const char*>(ASN1_STRING_get0_data(val->d.ia5));
					auto name_len = static_cast<size_t>(ASN1_STRING_length(val->d.ia5));

					switch (type) {
					case GEN_DNS: dsn_matched = check_host_name(name, name_len); break;

					case GEN_IPADD:
						if (!memcmp(&addr6, name, addr_len) ||
							!memcmp(&addr, name, addr_len)) {
							ip_matched = true;
						}
						break;
					}
				}
			}

			if (dsn_matched || ip_matched) { ret = true; }
		}

		GENERAL_NAMES_free(const_cast<STACK_OF(GENERAL_NAME)*>(
			reinterpret_cast<const STACK_OF(GENERAL_NAME)*>(alt_names)));
		return ret;
	}

	bool SSLClient::verify_host_with_common_name(X509 * server_cert) const {
		const auto subject_name = X509_get_subject_name(server_cert);

		if (subject_name != nullptr) {
			char name[BUFSIZ];
			auto name_len = X509_NAME_get_text_by_NID(subject_name, NID_commonName,
				name, sizeof(name));

			if (name_len != -1) {
				return check_host_name(name, static_cast<size_t>(name_len));
			}
		}

		return false;
	}

	bool SSLClient::check_host_name(const char* pattern,
		size_t pattern_len) const {
		if (m_host.size() == pattern_len && m_host == pattern) { return true; }

		// Wildcard match
		// https://bugs.launchpad.net/ubuntu/+source/firefox-3.0/+bug/376484
		rsl::vector<rsl::string> pattern_components;
		split(&pattern[0], &pattern[pattern_len], '.',
			[&](const char* b, const char* e) {
				pattern_components.emplace_back(b, e);
			});

		if (host_components_.size() != pattern_components.size()) { return false; }

		auto itr = pattern_components.begin();
		for (const auto& h : host_components_) {
			auto& p = *itr;
			if (p != h && p != "*") {
				auto partial_match = (p.size() > 0 && p[p.size() - 1] == '*' &&
					!p.compare(0, p.size() - 1, h));
				if (!partial_match) { return false; }
			}
			++itr;
		}

		return true;
	}
#endif
}