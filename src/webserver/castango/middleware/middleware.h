#pragma once
#pragma once

#include <map>
#include <string>
#include "../../utils/exception.h"
#include "../../plugins/plugin.h"
#include "../request.h"
#include <deque>

namespace yy_webserver {
	namespace castango {
		using std::string;


		class BaseMiddleWare {
		public:
			string name_;
			virtual ~BaseMiddleWare() {};
			virtual void run(const Request& request, BasePluginManager* plugin_manager) = 0;
		};

		class BaseMiddleWareManager {
			BaseMiddleWareManager(const BaseMiddleWareManager &) = delete;
			BaseMiddleWareManager& operator = (const BaseMiddleWareManager &) = delete;
		protected:
			std::map<string, BaseMiddleWare *> middlewares_;
			std::deque<BaseMiddleWare *> order_middlewares_;
		public:
			BaseMiddleWareManager() {};
			BaseMiddleWare* get_middleware(const string& name) {
				if (middlewares_.count(name) == 0)
					throw Exception("middleware not exist");
				return middlewares_[name];
			}
			const std::deque<BaseMiddleWare *>& get_middlewares() const {
				return order_middlewares_;
			}

			virtual ~BaseMiddleWareManager() {};
		};


		template <typename... MiddleWares>
		class MiddleWareManager {};


		template <>
		class MiddleWareManager<> : public BaseMiddleWareManager {
			MiddleWareManager(const MiddleWareManager &) = delete;
			MiddleWareManager& operator = (const MiddleWareManager &) = delete;
		public:
			MiddleWareManager() {};
			virtual ~MiddleWareManager() {};
		};

		template <typename FirstMiddleWare, typename... OtherMiddleWares>
		class MiddleWareManager<FirstMiddleWare, OtherMiddleWares...> : public MiddleWareManager<OtherMiddleWares...> {
			MiddleWareManager(const MiddleWareManager &) = delete;
			MiddleWareManager& operator = (const MiddleWareManager &) = delete;

			BaseMiddleWare* middleware_ = nullptr;

			void delete_middleware() {
				delete middleware_;
			}
		public:
			using MiddleWareManager<OtherMiddleWares...>::middlewares_;
			using MiddleWareManager<OtherMiddleWares...>::get_middleware;

			using MiddleWareManager<OtherMiddleWares...>::order_middlewares_;
			using MiddleWareManager<OtherMiddleWares...>::get_middlewares;

			MiddleWareManager() : MiddleWareManager<OtherMiddleWares...>(){
				try {
					middleware_ = new FirstMiddleWare();
					if (not middlewares_.insert({ middleware_->name_, middleware_ }).second) {
						throw Exception("repeat middleware");
					}
					order_middlewares_.push_front(middleware_);
				}
				catch (...) {
					delete_middleware();
				}
			}

			virtual ~MiddleWareManager() {
				delete_middleware();
			}
		};
	}
}