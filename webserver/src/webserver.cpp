// webserver.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。

#include "webserver.h"


void runserver() {
	my_server server;
	configure_url(server.protocol_handler);
	server.run();
}
