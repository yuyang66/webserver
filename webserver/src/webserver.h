#pragma once

#include "plugin.h"
#include "middleware.h"
#include "webserver/webserver.h"
#include "webserver/castango/protocol_handler.h"
#include "webserver/plugins/plugin.h"
#include "webserver/castango/middleware/middleware.h"

using namespace std;
using yy_webserver::Server;
using yy_webserver::castango::ProtocolHandler;
using yy_webserver::PluginManager;
using yy_webserver::castango::MiddleWareManager;

using yy_webserver::castango::BaseMiddleWareManager;
using yy_webserver::castango::Request;
using yy_webserver::BasePluginManager;




using my_plugin_manager = PluginManager <my_log_plugin, my_db_plugin>;
using my_middleware_manager = MiddleWareManager<AuthMiddleWare>;

using my_handler = ProtocolHandler<my_plugin_manager, my_middleware_manager>;
using my_server = Server<my_handler>;


void runserver();
void configure_url(my_handler&);

