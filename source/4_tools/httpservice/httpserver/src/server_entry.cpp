#include "rex_engine/engine/entrypoint.h"

#include "httplib.h"

#include "rex_engine/diagnostics/log.h"

#include "rex_std/thread.h"

DEFINE_LOG_CATEGORY(LogHttpServer);

class HttpServer
{
public:
	HttpServer()
#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
		: m_server(SERVER_CERT_FILE, SERVER_PRIVATE_KEY_FILE)
#else
		: m_server()
#endif
	{
		REX_ASSERT_X(m_server.is_valid(), "server has an error...");

		m_server.Get("/", [=](const httplib::Request& /*req*/, httplib::Response& res) {
			res.set_redirect("/hi");
			});

		m_server.Get("/hi", [](const httplib::Request& /*req*/, httplib::Response& res) {
			res.set_content("Hello World!\n", "text/plain");
			});

		m_server.Get("/slow", [](const httplib::Request& /*req*/, httplib::Response& res) {
			rsl::this_thread::sleep_for(rsl::chrono::seconds(2));
			res.set_content("Slow...\n", "text/plain");
			});

		m_server.Get("/dump", [this](const httplib::Request& req, httplib::Response& res) {
			res.set_content(dump_headers(req.headers), "text/plain");
			});

		m_server.Get("/stop",
			[&](const httplib::Request& /*req*/, httplib::Response& /*res*/) { svr.stop(); });

		m_server.Get("/shutdown", [](const httplib::Request& req, httplib::Response& res) {
			res.set_content("Should shutdown machine", "text/plain");
			});

		m_server.set_error_handler([](const httplib::Request& /*req*/, httplib::Response& res) {
			const char* fmt = "<p>Error Status: <span style='color:red;'>%d</span></p>";
			char buf[BUFSIZ];
			snprintf(buf, sizeof(buf), fmt, res.status);
			res.set_content(buf, "text/html");
			});

		m_server.set_logger([this](const httplib::Request& req, const httplib::Response& res) {
			printf("%s", log(req, res).c_str());
			});

		m_server.listen("localhost", 8080);
	}

private:
	rsl::string dump_headers(const httplib::Headers& headers) {
		rsl::string s;
		char buf[BUFSIZ];

		for (auto it = headers.begin(); it != headers.end(); ++it) {
			const auto& x = *it;
			snprintf(buf, sizeof(buf), "%s: %s\n", x.first.c_str(), x.second.c_str());
			s += buf;
		}

		return s;
	}

	rsl::string log(const httplib::Request& req, const httplib::Response& res) {
		rsl::string s;
		char buf[BUFSIZ];

		s += "================================\n";

		snprintf(buf, sizeof(buf), "%s %s %s", req.method.c_str(),
			req.version.c_str(), req.path.c_str());
		s += buf;

		rsl::string query;
		for (auto it = req.params.begin(); it != req.params.end(); ++it) {
			const auto& x = *it;
			snprintf(buf, sizeof(buf), "%c%s=%s",
				(it == req.params.begin()) ? '?' : '&', x.first.c_str(),
				x.second.c_str());
			query += buf;
		}
		snprintf(buf, sizeof(buf), "%s\n", query.c_str());
		s += buf;

		s += dump_headers(req.headers);

		s += "--------------------------------\n";

		snprintf(buf, sizeof(buf), "%d %s\n", res.status, res.version.c_str());
		s += buf;
		s += dump_headers(res.headers);
		s += "\n";

		if (!res.body.empty()) { s += res.body; }

		s += "\n";

		return s;
	}

private:
	httplib::Server m_server;
};

rsl::unique_ptr<HttpServer> g_http_server;

bool init(const rex::ApplicationCreationParams& appCreationParams)
{
	g_http_server = rsl::make_unique<HttpServer>();
	return true;
}
void update()
{
}
void shutdown()
{}

//-------------------------------------------------------------------------
rex::ApplicationCreationParams app_entry(rex::PlatformCreationParams& platformParams)
{
	rex::ApplicationCreationParams app_params(platformParams);

	app_params.gui_params.window_width = 1280;
	app_params.gui_params.window_height = 720;

	app_params.engine_params.app_init_func = init;
	app_params.engine_params.app_update_func = update;
	app_params.engine_params.app_shutdown_func = shutdown;
	app_params.engine_params.app_name.assign("Http Server");
	app_params.is_gui_app = false;

	return app_params;
}