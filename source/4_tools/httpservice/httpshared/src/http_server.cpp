#include "httpshared/http_server.h"

namespace http
{
	// HTTP server implementation
	inline Server::Server()
		: new_task_queue(
			[] { return new ThreadPool(CPPHTTPLIB_THREAD_POOL_COUNT); }) {
#ifndef _WIN32
		signal(SIGPIPE, SIG_IGN);
#endif
	}

	inline rsl::unique_ptr<detail::MatcherBase>
		Server::make_matcher(rsl::string_view pattern) {
		if (pattern.find("/:") != rsl::string_view::npos()) {
			return rsl::make_unique<detail::PathParamsMatcher>(pattern);
		}
		else {
			return rsl::make_unique<detail::RegexMatcher>(pattern);
		}
	}

	inline Server& Server::Get(rsl::string_view pattern, Handler handler) {
		get_handlers_.emplace_back(make_matcher(pattern), rsl::move(handler));
		return *this;
	}

	inline Server& Server::Post(rsl::string_view pattern, Handler handler) {
		post_handlers_.emplace_back(make_matcher(pattern), rsl::move(handler));
		return *this;
	}

	inline Server& Server::Post(rsl::string_view pattern,
		HandlerWithContentReader handler) {
		post_handlers_for_content_reader_.emplace_back(make_matcher(pattern),
			rsl::move(handler));
		return *this;
	}

	inline Server& Server::Put(rsl::string_view pattern, Handler handler) {
		put_handlers_.emplace_back(make_matcher(pattern), rsl::move(handler));
		return *this;
	}

	inline Server& Server::Put(rsl::string_view pattern,
		HandlerWithContentReader handler) {
		put_handlers_for_content_reader_.emplace_back(make_matcher(pattern),
			rsl::move(handler));
		return *this;
	}

	inline Server& Server::Patch(rsl::string_view pattern, Handler handler) {
		patch_handlers_.emplace_back(make_matcher(pattern), rsl::move(handler));
		return *this;
	}

	inline Server& Server::Patch(rsl::string_view pattern,
		HandlerWithContentReader handler) {
		patch_handlers_for_content_reader_.emplace_back(make_matcher(pattern),
			rsl::move(handler));
		return *this;
	}

	inline Server& Server::Delete(rsl::string_view pattern, Handler handler) {
		delete_handlers_.emplace_back(make_matcher(pattern), rsl::move(handler));
		return *this;
	}

	inline Server& Server::Delete(rsl::string_view pattern,
		HandlerWithContentReader handler) {
		delete_handlers_for_content_reader_.emplace_back(make_matcher(pattern),
			rsl::move(handler));
		return *this;
	}

	inline Server& Server::Options(rsl::string_view pattern, Handler handler) {
		options_handlers_.emplace_back(make_matcher(pattern), rsl::move(handler));
		return *this;
	}

	inline bool Server::set_base_dir(rsl::string_view dir,
		rsl::string_view mount_point) {
		return set_mount_point(mount_point, dir);
	}

	inline bool Server::set_mount_point(rsl::string_view mount_point,
		rsl::string_view dir, Headers headers) {
		detail::FileStat stat(dir);
		if (stat.is_dir()) {
			rsl::string mnt = !mount_point.empty() ? mount_point : "/";
			if (!mnt.empty() && mnt[0] == '/') {
				base_dirs_.push_back({ mnt, dir, rsl::move(headers) });
				return true;
			}
		}
		return false;
	}

	inline bool Server::remove_mount_point(rsl::string_view mount_point) {
		for (auto it = base_dirs_.begin(); it != base_dirs_.end(); ++it) {
			if (it->mount_point == mount_point) {
				base_dirs_.erase(it);
				return true;
			}
		}
		return false;
	}

	inline Server&
		Server::set_file_extension_and_mimetype_mapping(rsl::string_view ext,
			rsl::string_view mime) {
		file_extension_and_mimetype_map_[ext] = mime;
		return *this;
	}

	inline Server& Server::set_default_file_mimetype(rsl::string_view mime) {
		default_file_mimetype_ = mime;
		return *this;
	}

	inline Server& Server::set_file_request_handler(Handler handler) {
		file_request_handler_ = rsl::move(handler);
		return *this;
	}

	inline Server& Server::set_error_handler_core(HandlerWithResponse handler,
		rsl::true_type) {
		error_handler_ = rsl::move(handler);
		return *this;
	}

	inline Server& Server::set_error_handler_core(Handler handler,
		rsl::false_type) {
		error_handler_ = [handler](const Request& req, Response& res) {
			handler(req, res);
			return HandlerResponse::Handled;
			};
		return *this;
	}

	inline Server& Server::set_exception_handler(ExceptionHandler handler) {
		exception_handler_ = rsl::move(handler);
		return *this;
	}

	inline Server& Server::set_pre_routing_handler(HandlerWithResponse handler) {
		pre_routing_handler_ = rsl::move(handler);
		return *this;
	}

	inline Server& Server::set_post_routing_handler(Handler handler) {
		post_routing_handler_ = rsl::move(handler);
		return *this;
	}

	inline Server& Server::set_logger(Logger logger) {
		logger_ = rsl::move(logger);
		return *this;
	}

	inline Server&
		Server::set_expect_100_continue_handler(Expect100ContinueHandler handler) {
		expect_100_continue_handler_ = rsl::move(handler);
		return *this;
	}

	inline Server& Server::set_address_family(int family) {
		address_family_ = family;
		return *this;
	}

	inline Server& Server::set_tcp_nodelay(bool on) {
		tcp_nodelay_ = on;
		return *this;
	}

	inline Server& Server::set_ipv6_v6only(bool on) {
		ipv6_v6only_ = on;
		return *this;
	}

	inline Server& Server::set_socket_options(SocketOptions socket_options) {
		socket_options_ = rsl::move(socket_options);
		return *this;
	}

	inline Server& Server::set_default_headers(Headers headers) {
		default_headers_ = rsl::move(headers);
		return *this;
	}

	inline Server& Server::set_header_writer(
		rsl::function<ssize_t(Stream&, Headers&)> const& writer) {
		header_writer_ = writer;
		return *this;
	}

	inline Server& Server::set_keep_alive_max_count(size_t count) {
		keep_alive_max_count_ = count;
		return *this;
	}

	inline Server& Server::set_keep_alive_timeout(time_t sec) {
		keep_alive_timeout_sec_ = sec;
		return *this;
	}

	inline Server& Server::set_read_timeout(time_t sec, time_t usec) {
		read_timeout_sec_ = sec;
		read_timeout_usec_ = usec;
		return *this;
	}

	inline Server& Server::set_write_timeout(time_t sec, time_t usec) {
		write_timeout_sec_ = sec;
		write_timeout_usec_ = usec;
		return *this;
	}

	inline Server& Server::set_idle_interval(time_t sec, time_t usec) {
		idle_interval_sec_ = sec;
		idle_interval_usec_ = usec;
		return *this;
	}

	inline Server& Server::set_payload_max_length(size_t length) {
		payload_max_length_ = length;
		return *this;
	}

	inline bool Server::bind_to_port(rsl::string_view host, int port,
		int socket_flags) {
		auto ret = bind_internal(host, port, socket_flags);
		if (ret == -1) { is_decommissioned = true; }
		return ret >= 0;
	}
	inline int Server::bind_to_any_port(rsl::string_view host, int socket_flags) {
		auto ret = bind_internal(host, 0, socket_flags);
		if (ret == -1) { is_decommissioned = true; }
		return ret;
	}

	inline bool Server::listen_after_bind() { return listen_internal(); }

	inline bool Server::listen(rsl::string_view host, int port,
		int socket_flags) {
		return bind_to_port(host, port, socket_flags) && listen_internal();
	}

	inline bool Server::is_running() const { return is_running_; }

	inline void Server::wait_until_ready() const {
		while (!is_running_ && !is_decommissioned) {
			rsl::this_thread::sleep_for(rsl::chrono::milliseconds{ 1 });
		}
	}

	inline void Server::stop() {
		if (is_running_) {
			assert(svr_sock_ != INVALID_SOCKET);
			rsl::atomic<socket_t> sock(svr_sock_.exchange(INVALID_SOCKET));
			detail::shutdown_socket(sock);
			detail::close_socket(sock);
		}
		is_decommissioned = false;
	}

	inline void Server::decommission() { is_decommissioned = true; }

	inline bool Server::parse_request_line(const char* s, Request& req) const {
		auto len = strlen(s);
		if (len < 2 || s[len - 2] != '\r' || s[len - 1] != '\n') { return false; }
		len -= 2;

		{
			size_t count = 0;

			detail::split(s, s + len, ' ', [&](const char* b, const char* e) {
				switch (count) {
				case 0: req.method = rsl::string(b, e); break;
				case 1: req.target = rsl::string(b, e); break;
				case 2: req.version = rsl::string(b, e); break;
				default: break;
				}
				count++;
				});

			if (count != 3) { return false; }
		}

		thread_local const rsl::set<rsl::string> methods{
				"GET",     "HEAD",    "POST",  "PUT",   "DELETE",
				"CONNECT", "OPTIONS", "TRACE", "PATCH", "PRI" };

		if (methods.find(req.method) == methods.end()) { return false; }

		if (req.version != "HTTP/1.1" && req.version != "HTTP/1.0") { return false; }

		{
			// Skip URL fragment
			for (size_t i = 0; i < req.target.size(); i++) {
				if (req.target[i] == '#') {
					req.target.erase(i);
					break;
				}
			}

			detail::divide(req.target, '?',
				[&](const char* lhs_data, rsl::size_t lhs_size,
					const char* rhs_data, rsl::size_t rhs_size) {
						req.path = detail::decode_url(
							rsl::string(lhs_data, lhs_size), false);
						detail::parse_query_text(rhs_data, rhs_size, req.params);
				});
		}

		return true;
	}

	inline bool Server::write_response(Stream& strm, bool close_connection,
		Request& req, Response& res) {
		// NOTE: `req.ranges` should be empty, otherwise it will be applied
		// incorrectly to the error content.
		req.ranges.clear();
		return write_response_core(strm, close_connection, req, res, false);
	}

	inline bool Server::write_response_with_content(Stream& strm,
		bool close_connection,
		const Request& req,
		Response& res) {
		return write_response_core(strm, close_connection, req, res, true);
	}

	inline bool Server::write_response_core(Stream& strm, bool close_connection,
		const Request& req, Response& res,
		bool need_apply_ranges) {
		assert(res.status != -1);

		if (400 <= res.status && error_handler_ &&
			error_handler_(req, res) == HandlerResponse::Handled) {
			need_apply_ranges = true;
		}

		rsl::string content_type;
		rsl::string boundary;
		if (need_apply_ranges) { apply_ranges(req, res, content_type, boundary); }

		// Prepare additional headers
		if (close_connection || req.get_header_value("Connection") == "close") {
			res.set_header("Connection", "close");
		}
		else {
			rsl::string s = "timeout=";
			s += rsl::to_string(keep_alive_timeout_sec_);
			s += ", max=";
			s += rsl::to_string(keep_alive_max_count_);
			res.set_header("Keep-Alive", s);
		}

		if ((!res.body.empty() || res.content_length_ > 0 || res.content_provider_) &&
			!res.has_header("Content-Type")) {
			res.set_header("Content-Type", "text/plain");
		}

		if (res.body.empty() && !res.content_length_ && !res.content_provider_ &&
			!res.has_header("Content-Length")) {
			res.set_header("Content-Length", "0");
		}

		if (req.method == "HEAD" && !res.has_header("Accept-Ranges")) {
			res.set_header("Accept-Ranges", "bytes");
		}

		if (post_routing_handler_) { post_routing_handler_(req, res); }

		// Response line and headers
		{
			detail::BufferStream bstrm;
			if (!detail::write_response_line(bstrm, res.status)) { return false; }
			if (!header_writer_(bstrm, res.headers)) { return false; }

			// Flush buffer
			auto& data = bstrm.get_buffer();
			detail::write_data(strm, data.data(), data.size());
		}

		// Body
		auto ret = true;
		if (req.method != "HEAD") {
			if (!res.body.empty()) {
				if (!detail::write_data(strm, res.body.data(), res.body.size())) {
					ret = false;
				}
			}
			else if (res.content_provider_) {
				if (write_content_with_provider(strm, req, res, boundary, content_type)) {
					res.content_provider_success_ = true;
				}
				else {
					ret = false;
				}
			}
		}

		// Log
		if (logger_) { logger_(req, res); }

		return ret;
	}

	inline bool
		Server::write_content_with_provider(Stream& strm, const Request& req,
			Response& res, rsl::string_view boundary,
			rsl::string_view content_type) {
		auto is_shutting_down = [this]() {
			return this->svr_sock_ == INVALID_SOCKET;
			};

		if (res.content_length_ > 0) {
			if (req.ranges.empty()) {
				return detail::write_content(strm, res.content_provider_, 0,
					res.content_length_, is_shutting_down);
			}
			else if (req.ranges.size() == 1) {
				auto offset_and_length = detail::get_range_offset_and_length(
					req.ranges[0], res.content_length_);

				return detail::write_content(strm, res.content_provider_,
					offset_and_length.first,
					offset_and_length.second, is_shutting_down);
			}
			else {
				return detail::write_multipart_ranges_data(
					strm, req, res, boundary, content_type, res.content_length_,
					is_shutting_down);
			}
		}
		else {
			if (res.is_chunked_content_provider_) {
				auto type = detail::encoding_type(req, res);

				rsl::unique_ptr<detail::compressor> compressor;
				if (type == detail::EncodingType::Gzip) {
#ifdef CPPHTTPLIB_ZLIB_SUPPORT
					compressor = rsl::make_unique<detail::gzip_compressor>();
#endif
				}
				else if (type == detail::EncodingType::Brotli) {
#ifdef CPPHTTPLIB_BROTLI_SUPPORT
					compressor = rsl::make_unique<detail::brotli_compressor>();
#endif
				}
				else if (type == detail::EncodingType::Zstd) {
#ifdef CPPHTTPLIB_ZSTD_SUPPORT
					compressor = rsl::make_unique<detail::zstd_compressor>();
#endif
				}
				else {
					compressor = rsl::make_unique<detail::nocompressor>();
				}
				assert(compressor != nullptr);

				return detail::write_content_chunked(strm, res.content_provider_,
					is_shutting_down, *compressor);
			}
			else {
				return detail::write_content_without_length(strm, res.content_provider_,
					is_shutting_down);
			}
		}
	}

	inline bool Server::read_content(Stream& strm, Request& req, Response& res) {
		MultipartFormDataMap::iterator cur;
		auto file_count = 0;
		if (read_content_core(
			strm, req, res,
			// Regular
			[&](const char* buf, size_t n) {
				if (req.body.size() + n > req.body.max_size()) { return false; }
				req.body.append(buf, n);
				return true;
			},
			// Multipart
			[&](const MultipartFormData& file) {
				if (file_count++ == CPPHTTPLIB_MULTIPART_FORM_DATA_FILE_MAX_COUNT) {
					return false;
				}
				cur = req.files.emplace(file.name, file);
				return true;
			},
			[&](const char* buf, size_t n) {
				auto& content = cur->second.content;
				if (content.size() + n > content.max_size()) { return false; }
				content.append(buf, n);
				return true;
			})) {
			const auto& content_type = req.get_header_value("Content-Type");
			if (!content_type.find("application/x-www-form-urlencoded")) {
				if (req.body.size() > CPPHTTPLIB_FORM_URL_ENCODED_PAYLOAD_MAX_LENGTH) {
					res.status = StatusCode::PayloadTooLarge_413; // NOTE: should be 414?
					return false;
				}
				detail::parse_query_text(req.body, req.params);
			}
			return true;
		}
		return false;
	}

	inline bool Server::read_content_with_content_receiver(
		Stream& strm, Request& req, Response& res, ContentReceiver receiver,
		MultipartContentHeader multipart_header,
		ContentReceiver multipart_receiver) {
		return read_content_core(strm, req, res, rsl::move(receiver),
			rsl::move(multipart_header),
			rsl::move(multipart_receiver));
	}

	inline bool
		Server::read_content_core(Stream& strm, Request& req, Response& res,
			ContentReceiver receiver,
			MultipartContentHeader multipart_header,
			ContentReceiver multipart_receiver) const {
		detail::MultipartFormDataParser multipart_form_data_parser;
		ContentReceiverWithProgress out;

		if (req.is_multipart_form_data()) {
			const auto& content_type = req.get_header_value("Content-Type");
			rsl::string boundary;
			if (!detail::parse_multipart_boundary(content_type, boundary)) {
				res.status = StatusCode::BadRequest_400;
				return false;
			}

			multipart_form_data_parser.set_boundary(rsl::move(boundary));
			out = [&](const char* buf, size_t n, uint64_t /*off*/, uint64_t /*len*/) {
				/* For debug
				size_t pos = 0;
				while (pos < n) {
					auto read_size = (rsl::min)<size_t>(1, n - pos);
					auto ret = multipart_form_data_parser.parse(
							buf + pos, read_size, multipart_receiver, multipart_header);
					if (!ret) { return false; }
					pos += read_size;
				}
				return true;
				*/
				return multipart_form_data_parser.parse(buf, n, multipart_receiver,
					multipart_header);
				};
		}
		else {
			out = [receiver](const char* buf, size_t n, uint64_t /*off*/,
				uint64_t /*len*/) { return receiver(buf, n); };
		}

		if (req.method == "DELETE" && !req.has_header("Content-Length")) {
			return true;
		}

		if (!detail::read_content(strm, req, payload_max_length_, res.status, nullptr,
			out, true)) {
			return false;
		}

		if (req.is_multipart_form_data()) {
			if (!multipart_form_data_parser.is_valid()) {
				res.status = StatusCode::BadRequest_400;
				return false;
			}
		}

		return true;
	}

	inline bool Server::handle_file_request(const Request& req, Response& res) {
		for (const auto& entry : base_dirs_) {
			// Prefix match
			if (!req.path.compare(0, entry.mount_point.size(), entry.mount_point)) {
				rsl::string sub_path = "/" + req.path.substr(entry.mount_point.size());
				if (detail::is_valid_path(sub_path)) {
					auto path = entry.base_dir + sub_path;
					if (path.back() == '/') { path += "index.html"; }

					detail::FileStat stat(path);

					if (stat.is_dir()) {
						res.set_redirect(sub_path + "/", StatusCode::MovedPermanently_301);
						return true;
					}

					if (stat.is_file()) {
						for (const auto& kv : entry.headers) {
							res.set_header(kv.first, kv.second);
						}

						auto mm = rsl::make_shared<detail::mmap>(path.c_str());
						if (!mm->is_open()) { return false; }

						res.set_content_provider(
							mm->size(),
							detail::find_content_type(path, file_extension_and_mimetype_map_,
								default_file_mimetype_),
							[mm](size_t offset, size_t length, DataSink& sink) -> bool {
								sink.write(mm->data() + offset, length);
								return true;
							});

						if (req.method != "HEAD" && file_request_handler_) {
							file_request_handler_(req, res);
						}

						return true;
					}
				}
			}
		}
		return false;
	}

	inline socket_t
		Server::create_server_socket(rsl::string_view host, int port,
			int socket_flags,
			SocketOptions socket_options) const {
		return detail::create_socket(
			host, rsl::string(), port, address_family_, socket_flags, tcp_nodelay_,
			ipv6_v6only_, rsl::move(socket_options),
			[](socket_t sock, struct addrinfo& ai, bool& /*quit*/) -> bool {
				if (::bind(sock, ai.ai_addr, static_cast<socklen_t>(ai.ai_addrlen))) {
					return false;
				}
				if (::listen(sock, CPPHTTPLIB_LISTEN_BACKLOG)) { return false; }
				return true;
			});
	}

	inline int Server::bind_internal(rsl::string_view host, int port,
		int socket_flags) {
		if (is_decommissioned) { return -1; }

		if (!is_valid()) { return -1; }

		svr_sock_ = create_server_socket(host, port, socket_flags, socket_options_);
		if (svr_sock_ == INVALID_SOCKET) { return -1; }

		if (port == 0) {
			struct sockaddr_storage addr;
			socklen_t addr_len = sizeof(addr);
			if (getsockname(svr_sock_, reinterpret_cast<struct sockaddr*>(&addr),
				&addr_len) == -1) {
				return -1;
			}
			if (addr.ss_family == AF_INET) {
				return ntohs(reinterpret_cast<struct sockaddr_in*>(&addr)->sin_port);
			}
			else if (addr.ss_family == AF_INET6) {
				return ntohs(reinterpret_cast<struct sockaddr_in6*>(&addr)->sin6_port);
			}
			else {
				return -1;
			}
		}
		else {
			return port;
		}
	}

	inline bool Server::listen_internal() {
		if (is_decommissioned) { return false; }

		auto ret = true;
		is_running_ = true;
		auto se = detail::scope_exit([&]() { is_running_ = false; });

		{
			rsl::unique_ptr<TaskQueue> task_queue(new_task_queue());

			while (svr_sock_ != INVALID_SOCKET) {
#ifndef _WIN32
				if (idle_interval_sec_ > 0 || idle_interval_usec_ > 0) {
#endif
					auto val = detail::select_read(svr_sock_, idle_interval_sec_,
						idle_interval_usec_);
					if (val == 0) { // Timeout
						task_queue->on_idle();
						continue;
					}
#ifndef _WIN32
				}
#endif

#if defined _WIN32
				// sockets connected via WASAccept inherit flags NO_HANDLE_INHERIT,
				// OVERLAPPED
				socket_t sock = WSAAccept(svr_sock_, nullptr, nullptr, nullptr, 0);
#elif defined SOCK_CLOEXEC
				socket_t sock = accept4(svr_sock_, nullptr, nullptr, SOCK_CLOEXEC);
#else
				socket_t sock = accept(svr_sock_, nullptr, nullptr);
#endif

				if (sock == INVALID_SOCKET) {
					if (errno == EMFILE) {
						// The per-process limit of open file descriptors has been reached.
						// Try to accept new connections after a short sleep.
						rsl::this_thread::sleep_for(rsl::chrono::microseconds{ 1 });
						continue;
					}
					else if (errno == EINTR || errno == EAGAIN) {
						continue;
					}
					if (svr_sock_ != INVALID_SOCKET) {
						detail::close_socket(svr_sock_);
						ret = false;
					}
					else {
						; // The server socket was closed by user.
					}
					break;
				}

				detail::set_socket_opt_time(sock, SOL_SOCKET, SO_RCVTIMEO,
					read_timeout_sec_, read_timeout_usec_);
				detail::set_socket_opt_time(sock, SOL_SOCKET, SO_SNDTIMEO,
					write_timeout_sec_, write_timeout_usec_);

				if (!task_queue->enqueue(
					[this, sock]() { process_and_close_socket(sock); })) {
					detail::shutdown_socket(sock);
					detail::close_socket(sock);
				}
			}

			task_queue->shutdown();
		}

		is_decommissioned = !ret;
		return ret;
	}

	inline bool Server::routing(Request& req, Response& res, Stream& strm) {
		if (pre_routing_handler_ &&
			pre_routing_handler_(req, res) == HandlerResponse::Handled) {
			return true;
		}

		// File handler
		if ((req.method == "GET" || req.method == "HEAD") &&
			handle_file_request(req, res)) {
			return true;
		}

		if (detail::expect_content(req)) {
			// Content reader handler
			{
				ContentReader reader(
					[&](ContentReceiver receiver) {
						return read_content_with_content_receiver(
							strm, req, res, rsl::move(receiver), nullptr, nullptr);
					},
					[&](MultipartContentHeader header, ContentReceiver receiver) {
						return read_content_with_content_receiver(strm, req, res, nullptr,
							rsl::move(header),
							rsl::move(receiver));
					});

				if (req.method == "POST") {
					if (dispatch_request_for_content_reader(
						req, res, rsl::move(reader),
						post_handlers_for_content_reader_)) {
						return true;
					}
				}
				else if (req.method == "PUT") {
					if (dispatch_request_for_content_reader(
						req, res, rsl::move(reader),
						put_handlers_for_content_reader_)) {
						return true;
					}
				}
				else if (req.method == "PATCH") {
					if (dispatch_request_for_content_reader(
						req, res, rsl::move(reader),
						patch_handlers_for_content_reader_)) {
						return true;
					}
				}
				else if (req.method == "DELETE") {
					if (dispatch_request_for_content_reader(
						req, res, rsl::move(reader),
						delete_handlers_for_content_reader_)) {
						return true;
					}
				}
			}

			// Read content into `req.body`
			if (!read_content(strm, req, res)) { return false; }
		}

		// Regular handler
		if (req.method == "GET" || req.method == "HEAD") {
			return dispatch_request(req, res, get_handlers_);
		}
		else if (req.method == "POST") {
			return dispatch_request(req, res, post_handlers_);
		}
		else if (req.method == "PUT") {
			return dispatch_request(req, res, put_handlers_);
		}
		else if (req.method == "DELETE") {
			return dispatch_request(req, res, delete_handlers_);
		}
		else if (req.method == "OPTIONS") {
			return dispatch_request(req, res, options_handlers_);
		}
		else if (req.method == "PATCH") {
			return dispatch_request(req, res, patch_handlers_);
		}

		res.status = StatusCode::BadRequest_400;
		return false;
	}

	inline bool Server::dispatch_request(Request& req, Response& res,
		const Handlers& handlers) const {
		for (const auto& x : handlers) {
			const auto& matcher = x.first;
			const auto& handler = x.second;

			if (matcher->match(req)) {
				handler(req, res);
				return true;
			}
		}
		return false;
	}

	inline void Server::apply_ranges(const Request& req, Response& res,
		rsl::string& content_type,
		rsl::string& boundary) const {
		if (req.ranges.size() > 1 && res.status == StatusCode::PartialContent_206) {
			auto it = res.headers.find("Content-Type");
			if (it != res.headers.end()) {
				content_type = it->second;
				res.headers.erase(it);
			}

			boundary = detail::make_multipart_data_boundary();

			res.set_header("Content-Type",
				"multipart/byteranges; boundary=" + boundary);
		}

		auto type = detail::encoding_type(req, res);

		if (res.body.empty()) {
			if (res.content_length_ > 0) {
				size_t length = 0;
				if (req.ranges.empty() || res.status != StatusCode::PartialContent_206) {
					length = res.content_length_;
				}
				else if (req.ranges.size() == 1) {
					auto offset_and_length = detail::get_range_offset_and_length(
						req.ranges[0], res.content_length_);

					length = offset_and_length.second;

					auto content_range = detail::make_content_range_header_field(
						offset_and_length, res.content_length_);
					res.set_header("Content-Range", content_range);
				}
				else {
					length = detail::get_multipart_ranges_data_length(
						req, boundary, content_type, res.content_length_);
				}
				res.set_header("Content-Length", rsl::to_string(length));
			}
			else {
				if (res.content_provider_) {
					if (res.is_chunked_content_provider_) {
						res.set_header("Transfer-Encoding", "chunked");
						if (type == detail::EncodingType::Gzip) {
							res.set_header("Content-Encoding", "gzip");
						}
						else if (type == detail::EncodingType::Brotli) {
							res.set_header("Content-Encoding", "br");
						}
						else if (type == detail::EncodingType::Zstd) {
							res.set_header("Content-Encoding", "zstd");
						}
					}
				}
			}
		}
		else {
			if (req.ranges.empty() || res.status != StatusCode::PartialContent_206) {
				;
			}
			else if (req.ranges.size() == 1) {
				auto offset_and_length =
					detail::get_range_offset_and_length(req.ranges[0], res.body.size());
				auto offset = offset_and_length.first;
				auto length = offset_and_length.second;

				auto content_range = detail::make_content_range_header_field(
					offset_and_length, res.body.size());
				res.set_header("Content-Range", content_range);

				assert(offset + length <= res.body.size());
				res.body = res.body.substr(offset, length);
			}
			else {
				rsl::string data;
				detail::make_multipart_ranges_data(req, res, boundary, content_type,
					res.body.size(), data);
				res.body.swap(data);
			}

			if (type != detail::EncodingType::None) {
				rsl::unique_ptr<detail::compressor> compressor;
				rsl::string content_encoding;

				if (type == detail::EncodingType::Gzip) {
#ifdef CPPHTTPLIB_ZLIB_SUPPORT
					compressor = rsl::make_unique<detail::gzip_compressor>();
					content_encoding = "gzip";
#endif
				}
				else if (type == detail::EncodingType::Brotli) {
#ifdef CPPHTTPLIB_BROTLI_SUPPORT
					compressor = rsl::make_unique<detail::brotli_compressor>();
					content_encoding = "br";
#endif
				}
				else if (type == detail::EncodingType::Zstd) {
#ifdef CPPHTTPLIB_ZSTD_SUPPORT
					compressor = rsl::make_unique<detail::zstd_compressor>();
					content_encoding = "zstd";
#endif
				}

				if (compressor) {
					rsl::string compressed;
					if (compressor->compress(res.body.data(), res.body.size(), true,
						[&](const char* data, size_t data_len) {
							compressed.append(data, data_len);
							return true;
						})) {
						res.body.swap(compressed);
						res.set_header("Content-Encoding", content_encoding);
					}
				}
			}

			auto length = rsl::to_string(res.body.size());
			res.set_header("Content-Length", length);
		}
	}

	inline bool Server::dispatch_request_for_content_reader(
		Request& req, Response& res, ContentReader content_reader,
		const HandlersForContentReader& handlers) const {
		for (const auto& x : handlers) {
			const auto& matcher = x.first;
			const auto& handler = x.second;

			if (matcher->match(req)) {
				handler(req, res, content_reader);
				return true;
			}
		}
		return false;
	}

	inline bool
		Server::process_request(Stream& strm, rsl::string_view remote_addr,
			int remote_port, rsl::string_view local_addr,
			int local_port, bool close_connection,
			bool& connection_closed,
			const rsl::function<void(Request&)>& setup_request) {
		rsl::array<char, 2048> buf{};

		detail::stream_line_reader line_reader(strm, buf.data(), buf.size());

		// Connection has been closed on client
		if (!line_reader.getline()) { return false; }

		Request req;

		Response res;
		res.version = "HTTP/1.1";
		res.headers = default_headers_;

		// Request line and headers
		if (!parse_request_line(line_reader.ptr(), req) ||
			!detail::read_headers(strm, req.headers)) {
			res.status = StatusCode::BadRequest_400;
			return write_response(strm, close_connection, req, res);
		}

		// Check if the request URI doesn't exceed the limit
		if (req.target.size() > CPPHTTPLIB_REQUEST_URI_MAX_LENGTH) {
			Headers dummy;
			detail::read_headers(strm, dummy);
			res.status = StatusCode::UriTooLong_414;
			return write_response(strm, close_connection, req, res);
		}

		if (req.get_header_value("Connection") == "close") {
			connection_closed = true;
		}

		if (req.version == "HTTP/1.0" &&
			req.get_header_value("Connection") != "Keep-Alive") {
			connection_closed = true;
		}

		req.remote_addr = remote_addr;
		req.remote_port = remote_port;
		req.set_header("REMOTE_ADDR", req.remote_addr);
		req.set_header("REMOTE_PORT", rsl::to_string(req.remote_port));

		req.local_addr = local_addr;
		req.local_port = local_port;
		req.set_header("LOCAL_ADDR", req.local_addr);
		req.set_header("LOCAL_PORT", rsl::to_string(req.local_port));

		if (req.has_header("Range")) {
			const auto& range_header_value = req.get_header_value("Range");
			if (!detail::parse_range_header(range_header_value, req.ranges)) {
				res.status = StatusCode::RangeNotSatisfiable_416;
				return write_response(strm, close_connection, req, res);
			}
		}

		if (setup_request) { setup_request(req); }

		if (req.get_header_value("Expect") == "100-continue") {
			int status = StatusCode::Continue_100;
			if (expect_100_continue_handler_) {
				status = expect_100_continue_handler_(req, res);
			}
			switch (status) {
			case StatusCode::Continue_100:
			case StatusCode::ExpectationFailed_417:
				detail::write_response_line(strm, status);
				strm.write("\r\n");
				break;
			default:
				connection_closed = true;
				return write_response(strm, true, req, res);
			}
		}

		// Setup `is_connection_closed` method
		auto sock = strm.socket();
		req.is_connection_closed = [sock]() {
			return !detail::is_socket_alive(sock);
			};

		// Routing
		auto routed = false;
#ifdef CPPHTTPLIB_NO_EXCEPTIONS
		routed = routing(req, res, strm);
#else
		try {
			routed = routing(req, res, strm);
		}
		catch (rsl::exception& e) {
			if (exception_handler_) {
				auto ep = rsl::current_exception();
				exception_handler_(req, res, ep);
				routed = true;
			}
			else {
				res.status = StatusCode::InternalServerError_500;
				rsl::string val;
				auto s = e.what();
				for (size_t i = 0; s[i]; i++) {
					switch (s[i]) {
					case '\r': val += "\\r"; break;
					case '\n': val += "\\n"; break;
					default: val += s[i]; break;
					}
				}
				res.set_header("EXCEPTION_WHAT", val);
			}
		}
		catch (...) {
			if (exception_handler_) {
				auto ep = rsl::current_exception();
				exception_handler_(req, res, ep);
				routed = true;
			}
			else {
				res.status = StatusCode::InternalServerError_500;
				res.set_header("EXCEPTION_WHAT", "UNKNOWN");
			}
		}
#endif
		if (routed) {
			if (res.status == -1) {
				res.status = req.ranges.empty() ? StatusCode::OK_200
					: StatusCode::PartialContent_206;
			}

			// Serve file content by using a content provider
			if (!res.file_content_path_.empty()) {
				const auto& path = res.file_content_path_;
				auto mm = rsl::make_shared<detail::mmap>(path.c_str());
				if (!mm->is_open()) {
					res.body.clear();
					res.content_length_ = 0;
					res.content_provider_ = nullptr;
					res.status = StatusCode::NotFound_404;
					return write_response(strm, close_connection, req, res);
				}

				auto content_type = res.file_content_content_type_;
				if (content_type.empty()) {
					content_type = detail::find_content_type(
						path, file_extension_and_mimetype_map_, default_file_mimetype_);
				}

				res.set_content_provider(
					mm->size(), content_type,
					[mm](size_t offset, size_t length, DataSink& sink) -> bool {
						sink.write(mm->data() + offset, length);
						return true;
					});
			}

			if (detail::range_error(req, res)) {
				res.body.clear();
				res.content_length_ = 0;
				res.content_provider_ = nullptr;
				res.status = StatusCode::RangeNotSatisfiable_416;
				return write_response(strm, close_connection, req, res);
			}

			return write_response_with_content(strm, close_connection, req, res);
		}
		else {
			if (res.status == -1) { res.status = StatusCode::NotFound_404; }

			return write_response(strm, close_connection, req, res);
		}
	}

	inline bool Server::is_valid() const { return true; }

	inline bool Server::process_and_close_socket(socket_t sock) {
		rsl::string remote_addr;
		int remote_port = 0;
		detail::get_remote_ip_and_port(sock, remote_addr, remote_port);

		rsl::string local_addr;
		int local_port = 0;
		detail::get_local_ip_and_port(sock, local_addr, local_port);

		auto ret = detail::process_server_socket(
			svr_sock_, sock, keep_alive_max_count_, keep_alive_timeout_sec_,
			read_timeout_sec_, read_timeout_usec_, write_timeout_sec_,
			write_timeout_usec_,
			[&](Stream& strm, bool close_connection, bool& connection_closed) {
				return process_request(strm, remote_addr, remote_port, local_addr,
					local_port, close_connection, connection_closed,
					nullptr);
			});

		detail::shutdown_socket(sock);
		detail::close_socket(sock);
		return ret;
	}

}