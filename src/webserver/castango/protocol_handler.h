#pragma once
#include "parser.h"
#include "handler.h"
#include "../utils/exception.h"

namespace yy_webserver {
	namespace castango {
		template <typename TPluginManager, typename TMiddleWareManager>
		class ProtocolHandler {
			Handler<TMiddleWareManager> handler_;
			TPluginManager plugin_manager_;
		public:
			ProtocolHandler() : handler_(&plugin_manager_) {}


			BaseParser * new_parser() {
				return new Parser(&handler_);
			}

			void add_url(const string& url_regex, const std::function<void(const Request&, BasePluginManager*, BaseMiddleWareManager*)>& func) {
				handler_.add_url(url_regex, func);
			}
		};
	}
}

