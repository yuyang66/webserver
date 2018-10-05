#include "pch.h"
#include "handler.h"
#include "response.h"
#include <sstream>
#include <iostream>
#include "log.h"

using yy_webserver::castango::Request;
using yy_webserver::BasePluginManager;
using yy_webserver::castango::BaseMiddleWareManager;
using yy_webserver::castango::Response;
using yy_webserver::castango::BaseHandler;
using yy_webserver::DailyLog;

static int count = 0;
static std::ostringstream ss;


bool readsome(unsigned char * bufferr, int length) {
	count += length;
	ss << count << ' ';
	return count > 2000;
}


yy_webserver::castango::BaseResponse * success() {
	return new yy_webserver::castango::Response(ss.str());
}


void error() {
	std::cout << "error";
}



void AB(const Request& req, BasePluginManager* plugin_manager, BaseMiddleWareManager* middleware_manager) {
	extern char log1_dir[];
	extern char log2_dir[];
	extern char log3_dir[];
	extern char plugin1_name[];
	extern char plugin2_name[];
	extern char plugin3_name[];

	auto log = dynamic_cast<DailyLog<log1_dir, plugin1_name>*>( plugin_manager->get_plugin(plugin1_name));
	log->debug("aabbc");


	BaseHandler::recv_file(readsome, success, error);
}

