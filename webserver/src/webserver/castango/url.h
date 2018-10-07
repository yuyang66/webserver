#pragma once
#include <unordered_map>
#include <string>
#include "request.h"
#include "middleware/middleware.h"
#include "../plugins/plugin.h"
#include <deque>
#include <regex>
#include "../utils/exception.h"
#include <functional>

namespace yy_webserver {
	namespace castango {
		using std::string;
		using std::unordered_map;
		using std::deque;
		using std::regex;
		class UrlManager {
			unordered_map<string, std::function<void(const Request&, BasePluginManager*, BaseMiddleWareManager*)>> map_url_regex_handler_func_;
			deque<string> url_regex_order_;
		public:
			void add(const string& url_regex, const std::function<void(const Request&, BasePluginManager*, BaseMiddleWareManager*)>& func) {
				bool success = false;
				try {
					success = map_url_regex_handler_func_.insert({ url_regex , func }).second;
				}
				catch (...) {
					throw Exception("add url handler error");
				}
				if (not success) {
					throw Exception("repeated url regex");
				}
				url_regex_order_.push_back(url_regex);	
			}

			const std::function<void(const Request&, BasePluginManager*, BaseMiddleWareManager*)>* get_handler_func(const string& url) {
				for (auto url_regex : url_regex_order_) {
					std::smatch m;
					if (std::regex_search(url, regex(url_regex))) {
						return &map_url_regex_handler_func_[url_regex];
					}
				}
				return nullptr;
			}
		};
	}
}