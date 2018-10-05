#include "socket.h"

namespace yy_webserver {
	namespace tcp {
		int Socket::socket_num = 0;

		SocketConnectServer * SocketServer::socket_accept(int socket_buffer_size) {
			// �ͻ�����Ϣ
			SOCKADDR_IN addrClient;
			int len = sizeof(SOCKADDR);

			SOCKET sockConn = accept(sock_, (SOCKADDR *)&addrClient, &len);
			if (sockConn == SOCKET_ERROR)
				throw SocketListenError("connect fail");

			return new SocketConnectServer(sockConn, socket_buffer_size);
		}
	}
}