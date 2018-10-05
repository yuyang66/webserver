#pragma once

#include <iostream>
#include "settings.h"
#include "handler.h"
#include "request.h"
#include "response.h"
#include <deque>
#include <algorithm>
#include "file_reader.h"
#include <memory>

namespace yy_webserver {
	class BaseParser {
	public:
		enum class outer_status { Error, Reading, ReadingWriting, Writing ,Finish };
		virtual outer_status parser_some(const unsigned char * read_buffer, int read_length, unsigned char * write_buffer, 
			int & write_length, int buffer_size) = 0;
		virtual ~BaseParser() {};
	};

	namespace castango {
		class Parser : public BaseParser {
		private:
			enum class inner_status {GET_METHOD, GET_URL, GET_ARGS_NUM, GET_FILE_NUM, GET_ARGS, GET_FILE_ARGS ,GET_FILE, RESPONSE};
			inner_status status_ = inner_status::GET_METHOD;
			BaseHandler * handler_;
			Request request_;
			BaseResponse * response_ = nullptr;
			FileReader file_reader_;


			std::deque<unsigned char> buffer;

			bool pop_string(string & s) {
				auto pos = find(buffer.cbegin(), buffer.cend(), '\0');
				if (pos == buffer.end()) 
					return false;
				
				try {
					for_each(buffer.cbegin(), pos, [&s](unsigned char c) { s += char(c); });
					buffer.erase(buffer.begin(), pos + 1);
					return true;
				}
				catch (...) {
					throw Exception("string error");
				}

			}

			bool pop_args(string & key, string & value) {
				string next_string;
				if (not pop_string(next_string))
					return false;
				auto split = find(next_string.begin(), next_string.end(), '=');
				if(split == next_string.end())
					throw Exception("no args");
				key = next_string.substr(0, split - next_string.begin());
				value = next_string.substr(split - next_string.begin() + 1);
				return true;
			}

			void parser_get_method() {
				string method;
				bool success;
				try {
					success = pop_string(method);
				}
				catch (...) {
					throw outer_status::Error;
				}

				if (not success) {
					if (buffer.size() > 8)
						throw outer_status::Error;
					else
						throw outer_status::Reading;
				}

				transform(method.begin(), method.end(), method.begin(), ::toupper);
				
				if (method == "GET" || method == "POST" || method == "POSTFILE") {
					status_ = inner_status::GET_URL;
					request_.set_method(method);
				}
				else {
					throw outer_status::Error;
				}
			}

			void parser_get_url() {
				string url;
				bool success;
				try {
					success = pop_string(url);
				}
				catch (...) {
					throw outer_status::Error;
				}

				if (not success) 
					throw outer_status::Reading;

				request_.set_url(url);
				status_ = inner_status::GET_ARGS_NUM;
			}

			void parser_get_args_num() {
				string args_num;
				bool success;
				try {
					success = pop_string(args_num);
				}
				catch (...) {
					throw outer_status::Error;
				}

				if (not success)
					throw outer_status::Reading;

				try {
					request_.set_args_num(stoi(args_num));
				}
				catch (...) {
					throw outer_status::Error;
				}

				if (request_.get_method() != Request::method::POSTFILE) {
					request_.set_file_num(0);
					status_ = inner_status::GET_ARGS;	
				}
				else {
					status_ = inner_status::GET_FILE_NUM;
				}
			}

			void parser_get_file_num() {
				string file_num;
				bool success;
				try {
					success = pop_string(file_num);
				}
				catch (...) {
					throw outer_status::Error;
				}

				if (not success)
					throw outer_status::Reading;

				try {
					request_.set_file_num(stoi(file_num));
				}
				catch (...) {
					throw outer_status::Error;
				}

				status_ = inner_status::GET_ARGS;
			}

			void parser_get_args() {
				if (request_.get_args().size() >= (unsigned int)request_.get_args_num()) {
					status_ = inner_status::GET_FILE_ARGS;
					return;
				}

				string key, value;
				bool success;
				try {
					success = pop_args(key, value);
				}
				catch (...) {
					throw outer_status::Error;
				}

				if (not success)
					throw outer_status::Reading;

				try {
					request_.add_args(key, value);
				}
				catch (...) {
					throw outer_status::Error;
				}
			}

			void parser_get_file_args() {
				if (request_.get_files().size() >= (unsigned int)request_.get_file_num()) {
					try {
						(*handler_)(request_, &response_);
						status_ = inner_status::RESPONSE;
						return;
					}
					catch (FileReader& file_reader) {
						file_reader_ = file_reader;
						status_ = inner_status::GET_FILE;
						return;
					}
					catch (...) {
						throw outer_status::Error;
					}
				}

				string key, value;
				bool success;
				try {
					success = pop_args(key, value);
				}
				catch (...) {
					throw outer_status::Error;
				}

				if (not success)
					throw outer_status::Reading;

				try {
					request_.add_file(key, value);
				}
				catch (...) {
					throw outer_status::Error;
				}
			}

			void parser_get_file(int buffer_size) {
				unsigned char * file_read_buffer = new unsigned char[buffer_size];
				std::unique_ptr<unsigned char[]> u_ptr_file_read_buffer(file_read_buffer);
				int read_size = min((unsigned int)buffer_size, buffer.size());
				for (auto i = 0; i < read_size; i++) {
					file_read_buffer[i] = buffer.front();
					buffer.pop_front();
				}

				bool finish;
				try {
					finish = file_reader_.readsome(file_read_buffer, read_size);

					if (finish) {
						response_ = file_reader_.success();
						status_ = inner_status::RESPONSE;
						return;
					}

				}
				catch (...) {
					throw outer_status::Error;
				}

				if (buffer.size() == 0) 
					throw outer_status::Reading;

			}

			void parser_response(unsigned char * write_buffer, int & write_length, int buffer_size) {
				try {
					if (response_->is_finish())
						throw outer_status::Finish;

					write_length = response_->write_buffer(write_buffer, buffer_size);
					throw outer_status::Writing;
				}
				catch (outer_status & res) {
					throw res;
				}
				catch (...) {
					throw outer_status::Error;
				}
			}


		public:
			Parser(BaseHandler* handler) : handler_(handler) {};



			virtual outer_status parser_some(const unsigned char * read_buffer, int read_length, unsigned char * write_buffer, 
				int & write_length, int buffer_size) noexcept{
				for (int i = 0; i < read_length; i++) {
					buffer.push_back(read_buffer[i]);
				}

				while (true) {
					try {
						switch (status_) {
						case inner_status::GET_METHOD: parser_get_method(); break;
						case inner_status::GET_URL: parser_get_url(); break;
						case inner_status::GET_ARGS_NUM: parser_get_args_num(); break;
						case inner_status::GET_FILE_NUM: parser_get_file_num(); break;
						case inner_status::GET_ARGS: parser_get_args(); break;
						case inner_status::GET_FILE_ARGS: parser_get_file_args(); break;
						case inner_status::GET_FILE: parser_get_file(buffer_size); break;
						case inner_status::RESPONSE: parser_response(write_buffer, write_length, buffer_size); break;
						}
					}
					catch (outer_status res) {
						return res;
					}
					catch (...) {
						return outer_status::Error;
					}
				}
			}

			virtual ~Parser() {
				if (status_ == inner_status::GET_FILE) {
					try {
						file_reader_.error();
					}
					catch (...) {}
				}

				delete response_;
			}
		};
	}
}
