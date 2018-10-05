#pragma once
#include "response.h"
#include "request.h"
#include <sstream>
#include <map>
#include "file_reader.h"
#include "middleware/middleware.h"
#include "../plugins/plugin.h"
#include "../utils/exception.h"
#include "url.h"


namespace yy_webserver {
	namespace castango {
		using std::string;
		class BaseHandler {
		public:
			static void recv_file(bool(*readsome_func)(unsigned char * buffer, int length), BaseResponse* (*success_func)(), void(*error_func)()) {
				throw FileReader(readsome_func, success_func, error_func);
			}

			static void send_response(BaseResponse* response) {
				throw response;
			}

			virtual void operator () (const Request & request, BaseResponse ** response) = 0;
			virtual ~BaseHandler() {};
		};


		template<typename TMiddleWareManager>
		class Handler : public BaseHandler {
			BasePluginManager * plugin_manager_;
			UrlManager url_manager_;

		public:
			Handler(BasePluginManager * plugin_manager) : plugin_manager_(plugin_manager) {}

			virtual void operator () (const Request & request, BaseResponse ** response) {
				try {
					TMiddleWareManager middleware_manager;
					BaseMiddleWareManager* base_middleware_manager = &middleware_manager;

					for (BaseMiddleWare* middleware : base_middleware_manager->get_middlewares()) {
						middleware->run(request, plugin_manager_);
					}
					dispatch(request, base_middleware_manager);
				}
				catch (BaseResponse* res) {
					*response = res;
					return;
				}
				catch (FileReader& res) {
					throw FileReader(res);
				}
				catch (Exception & e) {
					throw e;
				}
				catch (...) {
					throw Exception("Error");
				}
				*response = new Response("no response");
				return;
			}

			void dispatch(const Request & request, BaseMiddleWareManager* middleware_manager) {
				const std::function<void(const Request&, BasePluginManager*, BaseMiddleWareManager*)> * handler_func = nullptr;
				handler_func = url_manager_.get_handler_func(request.get_url());
				if (handler_func == nullptr) {
					throw new Response("404");
				}
				(*handler_func)(request, plugin_manager_, middleware_manager);
			}

			void add_url(const string& url_regex, const std::function<void(const Request&, BasePluginManager*, BaseMiddleWareManager*)>& func) {
				url_manager_.add(url_regex, func);
			}

		};
	}
}
