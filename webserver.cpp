// webserver.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"


#include <iostream>
#include "socket.h"
#include "Exception.h"
#include <iostream>
#include "webserver.h"
#include "protocol_handler.h"

using namespace std;

int main(int argc, char* argv[])
{
	using yy_webserver::Server;
	using yy_webserver::TestHandler;


	Server<TestHandler> s;
	s.run();
	std::cin.get();
	return 0;
}
