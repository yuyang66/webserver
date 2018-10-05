// webserver.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"


#include <iostream>
#include "socket.h"
#include "Exception.h"
#include <iostream>
#include "webserver.h"
#include "protocol_handler.h"
#include "plugin.h"
#include "log.h"
#include "middleware.h"

using namespace std;
using yy_webserver::Server;
using yy_webserver::castango::ProtocolHandler;
using yy_webserver::PluginManager;
using yy_webserver::DailyLog;
using yy_webserver::castango::MiddleWareManager;

using yy_webserver::castango::BaseMiddleWareManager;
using yy_webserver::castango::Request;
using yy_webserver::BasePluginManager;

extern void AB(const Request& req, BasePluginManager* plugin_manager, BaseMiddleWareManager* middleware_manager);

int main(int argc, char* argv[])
{
	

	extern char log1_dir[];
	extern char log2_dir[];
	extern char log3_dir[];
	extern char plugin1_name[];
	extern char plugin2_name[];
	extern char plugin3_name[];

	

	using my_plugin_manager = PluginManager <DailyLog<log1_dir, plugin1_name>, DailyLog<log2_dir, plugin2_name>, DailyLog<log3_dir, plugin3_name>>;
	using my_middleware_manager = MiddleWareManager<>;

	Server<ProtocolHandler<my_plugin_manager, my_middleware_manager>> s;

	s.protocol_handler.add_url("AB", AB);

	s.run();
	std::cin.get();
	return 0;
}
