#pragma once
#include "webserver/plugins/log.h"
#include "webserver/plugins/db.h"

using yy_webserver::Sqlite3DB;
using yy_webserver::DailyLog;

extern char log_dir[];
extern char log_plugin_name[];

extern char db_name[];
extern char db_plugin_name[];

using my_db_plugin = Sqlite3DB<db_name, db_plugin_name>;
using my_log_plugin = DailyLog<log_dir, log_plugin_name>;