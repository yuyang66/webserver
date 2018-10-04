
#include <string>
#include <map>
#include "exception.h"


namespace yy_webserver {
	namespace castango {
		using std::string;
		using std::map;

		class UploadFile {
		private:
			string filename_;
			unsigned long filesize_;
		public:
			UploadFile(const string & filename, unsigned long filesize) : filename_(filename), filesize_(filesize) {};
		};


		class Request {
		public:
			enum class method {GET, POST, POSTFILE};
		private:
			method method_;
			int args_num_;
			int file_num_;
			string url_;
			map<string, string> args_;
			map<string, UploadFile> files_;
		
		public:
			void set_method(method m) { method_ = m; }
			void set_method(const string & m) {
				if (m == "GET")
					method_ = method::GET;
				else if (m == "POST")
					method_ = method::POST;
				else if (m == "POSTFILE")
					method_ = method::POSTFILE;
			}

			void set_args_num(int args_num) { args_num_ = args_num; }
			void set_file_num(int file_num) { file_num_ = file_num; }
			void set_url(const string & url) { url_ = url; }

			void add_args(const string & key, const string & value) {
				if (not args_.insert({ key, value }).second)
					throw  Exception("args repeat");
			}

			void add_file(const string & filename, const string & prop) {
				unsigned long filesize;

				try {
					filesize = std::stoul(prop);
				}
				catch (...) {
					throw Exception("file args error");
				}

				if(not files_.insert({ filename, UploadFile(filename, filesize) }).second)
					throw  Exception("filename repeat");
			}

			method get_method() const { return method_;  }
			int get_args_num() const { return args_num_; }
			int get_file_num() const { return file_num_; }
			const string& get_url() const { return url_; }
			const map<string, string> & get_args() const { return args_; }
			const map<string, UploadFile> & get_files() const { return files_; }
		};
	}
}