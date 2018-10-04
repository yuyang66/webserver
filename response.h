#pragma once
#include <string>
#include <fstream>
#include <deque>
#include "Exception.h";

namespace yy_webserver {
	namespace castango {
		using std::string;
		using std::deque;

		class BaseResponse {
		public:
			// 返回写入数据的字节数
			virtual int write_buffer(unsigned char * buffer, int buffer_size) = 0;
			virtual bool is_finish() = 0;
		};

		
		template<typename T>
		void write_string(const string & s, T t) {
			copy(s.begin(), s.end(), t.end());
			t.push_back('\0');
		}

		class FileResponse :public BaseResponse {
		private:
			string filename_;
			string filepath_;
			deque<char> output_;
			std::ifstream file_data_;
			bool finish_ = false;
		public:
			FileResponse(const string & filename, const string & filepath) : filename_(filename), filepath_(filepath) {
				write_string("FILE", output_);
				write_string(filename_, output_);
				
				file_data_.open(filepath_, std::ios_base::in | std::ios_base::binary);
				if (not file_data_.is_open())
					throw Exception("file open file");
				file_data_.seekg(0, std::ios::end);

				write_string(std::to_string(file_data_.tellg()), output_);

				file_data_.seekg(0);
			}

			virtual int write_buffer(unsigned char * buffer, int buffer_size) {
				int i = 0;
				while (output_.size() != 0 and i < buffer_size) {
					buffer[i] = (unsigned char)output_.front();
					output_.pop_front();
					i++;
				}

				file_data_.read((char*)(buffer + i), buffer_size - i);

				if (file_data_.eof())
					finish_ = true;

				return file_data_.gcount() + i;
			}

			virtual bool is_finish() { return finish_; }
		};

		class Response : public BaseResponse {
			deque<char> output;

		public:
			explicit Response(const string & s) {
				write_string("STR", output);
				write_string(s, output);
			}

			virtual int write_buffer(unsigned char * buffer, int buffer_size) {
				int i = 0;
				while (output.size() != 0 and i < buffer_size) {
					buffer[i] = (unsigned char) output.front();
					output.pop_front();
					i++;
				}
				return i;
			}

			virtual bool is_finish() {
				return output.size() == 0;
			}
		};
	}
}