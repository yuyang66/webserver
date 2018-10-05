#pragma once

#include <map>
#include <string>
#include "../utils/exception.h"

namespace yy_webserver {
	using std::string;

	class BasePlugin {
	public:
		string name_;
		virtual void abstract_symbol() = 0;
		virtual ~BasePlugin() {};
	};

	class BasePluginManager {
		BasePluginManager(const BasePluginManager &) = delete;
		BasePluginManager& operator = (const BasePluginManager &) = delete;
	protected:
		std::map<string, BasePlugin *> plugins_;
	public:
		BasePluginManager() {}
		BasePlugin* get_plugin(const string name) {
			if (plugins_.count(name) == 0)
				throw Exception("plugin not exist");
			return plugins_[name];
		}
		virtual ~BasePluginManager() {};
	};


	template <typename... Plugins>
	class PluginManager {};


	template <>
	class PluginManager<> : public BasePluginManager{

		PluginManager(const PluginManager &) = delete;
		PluginManager& operator = (const PluginManager &) = delete;
	public:
		PluginManager() {}
		virtual ~PluginManager() {};
	};

	template <typename FirstPlugin, typename... OtherPlugins>
	class PluginManager<FirstPlugin, OtherPlugins...> : public PluginManager<OtherPlugins...> {
		PluginManager(const PluginManager &) = delete;
		PluginManager& operator = (const PluginManager &) = delete;

		BasePlugin* plugin_ = nullptr;

		void delete_plugin() {
			delete plugin_;
		}
	protected:
		using PluginManager<OtherPlugins...>::plugins_;
	public:
		using PluginManager<OtherPlugins...>::get_plugin;

		PluginManager() {
			try {
				plugin_ = new FirstPlugin();
				if (not plugins_.insert({ plugin_->name_, plugin_ }).second) {
					throw Exception("repeat plugin");
				}
			}
			catch (...) {
				delete_plugin();
			}
		}

		virtual ~PluginManager() {
			delete_plugin();
		}
	};
}


//	template <typename... Plugins>
//	class PluginManager {
//		PluginManager(const PluginManager &) = delete;
//		PluginManager& operator = (const PluginManager &) = delete;
//
//	protected:
//		std::map<string, BasePlugin *> plugins_;
//
//	public:
//		PluginManager(){
//			try {
//				add_plugins<Plugins...>();
//			}
//			catch (...) {
//				delete_plugins();
//			}
//		}
//
//
//		void add_plugins() {}
//
//		template <typename OnePlugin, typename... OtherPlugins>
//		void add_plugins() {
//			OnePlugin* one_plugin = new OnePlugin();
//
//			if (not plugins_.insert({ one_plugin->name_, one_plugin }).second) {
//				throw Exception("repeat plugin");
//			}
//			add_plugins<OtherPlugins...>();
//		}
//
//		BasePlugin* get_plugin(const string name) {
//			return plugins_[name];
//		}
//
//		void delete_plugins() {
//			for (auto iter : plugins_) {
//				delete iter.second;
//			}
//		}
//
//		~PluginManager() {
//			delete_plugins();
//		}
//	};
//}