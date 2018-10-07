
#include "webserver/castango/handler.h"
#include "webserver/castango/response.h"
#include <sstream>
#include <iostream>
#include "webserver/plugins/log.h"
#include "webserver/castango/middleware/middleware.h"
#include "url.h"
#include "middleware.h"
#include "plugin.h"

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


	auto log = dynamic_cast<my_log_plugin*>(plugin_manager->get_plugin(log_plugin_name));
	AuthMiddleWare* auth = dynamic_cast<AuthMiddleWare*>(middleware_manager->get_middleware(auth_middleware_name));

	int user_id = auth->get_auth_id();
	if (user_id != -1) {
		BaseHandler::send_response(new Response(string("is login: user_id = " + std::to_string(user_id))));
	}


	log->debug("aabbc");


	BaseHandler::recv_file(readsome, success, error);
}

