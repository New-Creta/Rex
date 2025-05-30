#include "rex_engine/engine/entrypoint.h"

#include "httplib.h"

class HttpClient
{
public:
  HttpClient()
  {
#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
		httplib::SSLClient cli("localhost", 8080);
		// httplib::SSLClient cli("google.com");
		// httplib::SSLClient cli("www.youtube.com");
		cli.set_ca_cert_path(CA_CERT_FILE);
		cli.enable_server_certificate_verification(true);
#else
		httplib::Client cli("localhost", 8080);
#endif

		if (auto res = cli.Get("/hi")) {
			std::cout << res->status << "\n";
			std::cout << res->get_header_value("Content-Type") << "\n";
			std::cout << res->body << "\n";
		}
		else {
			std::cout << "error code: " << res.error() << "\n";
#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
			auto result = cli.get_openssl_verify_result();
			if (result) {
				std::cout << "verify error: " << X509_verify_cert_error_string(result) << "\n";
			}
#endif
		}
  }
};

rsl::unique_ptr<HttpClient> g_http_client;

bool init(const rex::ApplicationCreationParams& appCreationParams)
{
  g_http_client = rsl::make_unique<HttpClient>();
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
  app_params.engine_params.app_name.assign("Http Client");
  app_params.is_gui_app = false;

  return app_params;
}