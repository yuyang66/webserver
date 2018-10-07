#pragma once

#include "webserver/castango/middleware/middleware.h"
#include <string>
#include "webserver/castango/request.h"
#include "webserver/plugins/plugin.h"
#include "webserver/plugins/db.h"
#include <map>
#include "plugin.h"
#include <iostream>

using yy_webserver::castango::Request;
using yy_webserver::BasePluginManager;
using yy_webserver::castango::BaseMiddleWare;
using std::string;
using std::map;

extern char auth_middleware_name[];


class AuthMiddleWare : public BaseMiddleWare {
	int auth_id;
public:
	AuthMiddleWare() {
		name_ = auth_middleware_name;
	}

	virtual void run(const Request& request, BasePluginManager* plugin_manager) {
		const map<string, string>& args = request.get_args();
		auto id_iter = args.find("id");
		auto token_iter = args.find("token");
		if (id_iter == args.end() or token_iter == args.end()) {
			auth_id = -1;
			return;
		}
		
		my_db_plugin* db = dynamic_cast<my_db_plugin*>(plugin_manager->get_plugin(db_plugin_name));
		
		int result = 0;
		string sql = "select * from user where id = " + id_iter->second + " and token = \"" + token_iter->second + "\"";
		db->exec(
			sql.c_str(), 
			[](void *result, int argc, char **argv, char **azColName) -> int {
				*((int*)result) += 1;
				return 0;
			}, (void*)&result, 0
		);
		if (result != 0) {
			auth_id = std::stoi(id_iter->second);
		}
		else {
			auth_id = -1;
		}
	}

	int get_auth_id() { return auth_id; };
};

