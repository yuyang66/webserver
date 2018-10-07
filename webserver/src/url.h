#pragma once
#include "webserver/castango/request.h"
#include "webserver/plugins/plugin.h"
#include "webserver/castango/middleware/middleware.h"

using yy_webserver::castango::BaseMiddleWareManager;
using yy_webserver::castango::Request;
using yy_webserver::BasePluginManager;

void AB(const Request& req, BasePluginManager* plugin_manager, BaseMiddleWareManager* middleware_manager);
