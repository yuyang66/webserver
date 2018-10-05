#pragma once

#include "../utils/exception.h"
#include "../settings.h"
#include "../utils/auto_lock.h"

#include <string>
#include <fstream>
#include <mutex>
#include <time.h>
#include "plugin.h"

#pragma comment(lib, "sqlite3.lib")

#include "../extern/sqlite3.h"

namespace yy_webserver {
	using std::string;
	template <const char * filename, const char * plugin_name>
	class Sqlite3DB : public BasePlugin {
	public:
		sqlite3 *db;
		std::mutex db_lock;
	public:
		Sqlite3DB() {
			name_ = plugin_name;
			if (sqlite3_open(filename, &db)) {
				throw Exception(string("open sqlite3: ") + filename + "error" + sqlite3_errmsg(db));
			}
		};

		int exec(const char * sql, int (*callback)(void *, int, char **, char **), void* data = 0, char ** errormsg = 0) {
			std::unique_lock < std::mutex > lck(db_lock);
			return sqlite3_exec(db, sql, callback, data, errormsg);
		}

		int exec(const string& sql, int(*callback)(void *, int, char **, char **), void* data = 0, char ** errormsg = 0) {
			return exec(sql.c_str(), callback, data, errormsg);
		}

		virtual void abstract_symbol() {};

		~Sqlite3DB() {
			sqlite3_close(db);
		}
	};
}