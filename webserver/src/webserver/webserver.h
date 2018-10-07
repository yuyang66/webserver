#pragma once
#include "socket/socket.h"

#include <windows.h>
#include <memory.h>
#include "castango/parser.h"
#include <queue>
#include <mutex>
#include <condition_variable>
#include "utils/auto_lock.h"
#include <iostream>
#include <thread>

#include <future>
#include <iostream>
#include "settings.h"

namespace yy_webserver {
	using namespace tcp;
	template <typename TProtocolHandler, int buffer_size = 512> 
	class Server {
	private:
		
		SocketServer socket_;

		Server(const Server & other) = delete;
		Server & operator = (const Server & other) = delete;

		
		void response_thread(std::queue<unsigned char> * response, std::mutex * mutex_lock, std::condition_variable * cond,const bool * response_end,
			std::unique_ptr<SocketConnect> * u_ptr_socket_connect) noexcept
		{
			unsigned char * buffer = new unsigned char[buffer_size];
			int num, i;
			while (true) {
				{
					std::unique_lock<std::mutex> lck(*mutex_lock);

					if (0 == response->size()) {
						if (*response_end) break;
						else cond->wait(lck);
					}
					num = min(buffer_size, response->size());
					for (i = 0; i < num; i++) {
						buffer[i] = response->front();
						response->pop();
					}
				}
				try {
					(*u_ptr_socket_connect)->send_bytes(buffer, num);
				}
				catch (Exception& e) {
					std::cout << e.what();
					break;
				}
			}
			delete[] buffer;
		}

		void do_handler(std::queue<unsigned char> & response, std::mutex & mutex_lock, std::condition_variable & cond,
			std::unique_ptr<SocketConnect> & u_ptr_socket_connect)
		{
			
			std::unique_ptr<BaseParser> u_ptr_parser(protocol_handler.new_parser());
			
			unsigned char * read_buffer = new  unsigned char[buffer_size];
			unsigned char * write_buffer = new  unsigned char[buffer_size];
			int write_bytes;
			int read_bytes;

			BaseParser::outer_status res;

			try {
				read_bytes = u_ptr_socket_connect->recv_bytes(read_buffer, buffer_size);

				while (true) {
					res = u_ptr_parser->parser_some( read_buffer, read_bytes, write_buffer, write_bytes, buffer_size);
					read_bytes = 0;
					if (res == BaseParser::outer_status::Error or res == BaseParser::outer_status::Finish)
						break;
					if (res == BaseParser::outer_status::Writing or res == BaseParser::outer_status::ReadingWriting) {
						std::unique_lock < std::mutex > lck(mutex_lock);
						for(int i = 0; i < write_bytes; i++)
							response.push(write_buffer[i]);
						cond.notify_all();
					}
					if (res == BaseParser::outer_status::Reading or res == BaseParser::outer_status::ReadingWriting) 
						read_bytes = u_ptr_socket_connect->recv_bytes(read_buffer, buffer_size);
				}
			}
			catch (Exception& e) {
				std::cout << e.what();
			}

			
			delete[] read_buffer;
			delete[] write_buffer;
		}

		void prepare_handler(SocketConnect * socket_connect) {
			std::unique_ptr<SocketConnect> u_ptr_socket_connect(socket_connect);
			
			std::queue<unsigned char> response;
			bool response_end = false;
			std::mutex mutex_lock;
			std::condition_variable cond;
			
			auto res_t = std::async(std::launch::async, &Server::response_thread, this, &response, &mutex_lock, &cond, &response_end, &u_ptr_socket_connect);


			do_handler(response, mutex_lock, cond, u_ptr_socket_connect);

			{
				std::unique_lock < std::mutex > lck(mutex_lock);
				response_end = true;
				cond.notify_all();
			}

			while (res_t.wait_for(std::chrono::seconds(1)) != std::future_status::ready);
		}


	public:
		TProtocolHandler protocol_handler;

		explicit Server(int port = 8080, int addr = 0) : socket_(port, addr) {
			socket_.socket_listen();
		};

		void run() {
			while (1) {
				SocketConnectServer* socket_connect;
				socket_connect = socket_.socket_accept(buffer_size);
				std::async(std::launch::async, &Server::prepare_handler, this, socket_connect);

			}
		}
	};
}
