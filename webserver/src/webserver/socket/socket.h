#pragma once


#include "../utils/exception.h"
#include "../settings.h"

#include <WinSock2.h>
#include <iostream>
#include <sstream>
#include <cstring>
#include <queue>
#include <memory>

#pragma comment(lib, "ws2_32.lib")


namespace yy_webserver {
	using std::string;
	using std::queue;
	namespace tcp {
		class SocketConnectServer;

		class Socket {
		private:
			static int socket_num;
			Socket & operator = (const Socket & other) = delete;
			Socket(const Socket & other) = delete;

			static void init() {
				WSADATA wsaData;
				if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
					throw StaticLoadingError();
			}
			static void end() {
				WSACleanup();
			}

		protected:
			SOCKET sock_;

		public:
			class StaticLoadingError : public Exception 
			{
			public:
				StaticLoadingError() : Exception("loading static socket lib error") {}
			};

			Socket(){
				if (0 == socket_num)
					init();
				socket_num++;
				sock_ = socket(AF_INET, SOCK_STREAM, 0);

				if (DEBUG)
					std::cout << "socket add num : " << socket_num << std::endl;
			}

			~Socket(){ 
				closesocket(sock_);
				socket_num--;
				if (0 == socket_num)
					end();

				if (DEBUG)
					std::cout << "socket del num : " << socket_num << std::endl;

			}
		};


		// 用于服务端监听的套接字
		class SocketServer : public Socket {
			
		private:
			int port_;
			unsigned long addr_;

		public:
			class SocketListenError : public Exception
			{
				using Exception::Exception;

			public:
				virtual const string what() {
					return "socket listen error : " + Exception::what();
				}
			};

			SocketServer(int port = 8080, unsigned long addr = 0) : port_(port), addr_(addr) {
				SOCKADDR_IN addrSrv;
				addrSrv.sin_family = AF_INET;
				addrSrv.sin_port = htons(port);
				addrSrv.sin_addr.S_un.S_addr = addr;
				
				if (SOCKET_ERROR == bind(sock_, (LPSOCKADDR)&addrSrv, sizeof(SOCKADDR_IN)))
					throw SocketListenError("socket bind error");
			}

			void socket_listen() {
				// listen()
				if (listen(sock_, 10) == SOCKET_ERROR)
					throw SocketListenError(std::to_string(WSAGetLastError()));
			}

			SocketConnectServer * socket_accept(int socket_buffer_size = 512);
		};

		// 用于和另一段传递消息的套接字， 抽象基类
		class SocketConnect : public Socket {
		private:
			unsigned int buffer_size_;
			queue<unsigned char> buffer_;
			
			


			void recv_new_data() {
				

				char* buffer = new char[buffer_size_];
				int res = recv(sock_, buffer, buffer_size_, 0);

				for (int i = 0; i < res; i++) 
					buffer_.push(buffer[i]);
				
				delete[] buffer;

				if (res == 0)
					throw SocketConnectError("src socket closed");
				if (res == SOCKET_ERROR)
					throw SocketConnectError("recv data error");
			}




		public:
			class SocketConnectError : public Exception
			{
				using Exception::Exception;

			public:
				virtual const string what() {
					return "socket connect error : " + Exception::what();
				}
			};

			SocketConnect(unsigned int buffer_size = 512) : buffer_size_(buffer_size){}

			void set_buffer_size(int buffer_size) { buffer_size_ = buffer_size; }

			
			SocketConnect & send_bytes(const unsigned char * s, unsigned long length) {
				for (unsigned int i = 0; i < length; i += buffer_size_) {
					if (send(sock_, (const char *) s + i, min(buffer_size_, length - i), 0) == SOCKET_ERROR)
						throw SocketConnectError("send data error");
				}
				return *this;
			}

			SocketConnect & operator << (const char * s) {
				return send_bytes((const unsigned char *) s, strlen(s) + 1);
			}

			SocketConnect & operator << (const string & s) {
				return *this << s.c_str();
			}

			SocketConnect & operator << (const string && s) {
				return *this << s.c_str();
			}

			


			int recv_bytes(unsigned char * s, unsigned long max_length) {
				if (buffer_.size() == 0)
					recv_new_data();

				unsigned int i;
				for (i = 0; i < max_length && buffer_.size() != 0; i++) {
					s[i] = (unsigned char) buffer_.front();
					buffer_.pop();
				}
				return i;
			}




			SocketConnect & operator >> (string & s) {
				s = "";
				while (true) {
					while (buffer_.size() != 0) {
						char c = buffer_.front();
						buffer_.pop();

						if (c == '\0')
							return *this;
						s += c;
					}
					recv_new_data();
				}
			}

			virtual void abstract_symbol() = 0;
		};

		// 服务端用于和另一段传递消息的套接字， 只能使用SocketServer::socket_accept 创建
		class SocketConnectServer : public SocketConnect {
		private:
			SocketConnectServer(SOCKET sock, unsigned int buffer_size = 512): SocketConnect(buffer_size) {
				closesocket(sock_);
				sock_ = sock;
			}


		public:
			virtual void abstract_symbol() {};
			friend SocketConnectServer * SocketServer::socket_accept(int);
		};

		class SocketConnectClient : public SocketConnect {
		public:
			SocketConnectClient(int port, unsigned long addr) {
				if (sock_ == SOCKET_ERROR)
					throw SocketConnectError("create socket error");
				SOCKADDR_IN addrSrv;
				addrSrv.sin_family = AF_INET;
				addrSrv.sin_port = htons(port);
				addrSrv.sin_addr.S_un.S_addr = addr;

				if (connect(sock_, (struct sockaddr*)&addrSrv, sizeof(addrSrv)) == INVALID_SOCKET)
					throw SocketConnectError("connect server error :" + std::to_string(WSAGetLastError()));
			}

			virtual void abstract_symbol() {};
		};
	}
}



