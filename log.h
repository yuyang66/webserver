#pragma once


#include "exception.h"
#include "settings.h"
#include "auto_lock.h"

#include <string>
#include <fstream>
#include <mutex>
#include <time.h>
#include "plugin.h"

namespace yy_webserver {
	using std::string;
	using std::ofstream;
	using std::ifstream;
	class Log {
	private:
		ofstream outFile;
		std::mutex file_lock;

	public:
		explicit Log(const string & filename) {
			outFile.open(filename, std::ios::app);
			if (!outFile)
				throw Exception(filename + "open file");
		}

		~Log() {
			outFile.close();
		}

		Log& operator << (const string & s) {
			std::unique_lock < std::mutex > lck(file_lock);
			outFile << s << std::endl;
			return *this;
		}
		Log& operator << (const char * s) {
			std::unique_lock < std::mutex > lck(file_lock);
			outFile << s << std::endl;
			return *this;
		}
	};

	
	template <const char * basedir, const char * name>
	class DailyLog : public BasePlugin {
		string get_time()
		{
			time_t timep;
			time(&timep);
			tm t;
			char tmp[64];
			localtime_s(&t, &timep);
			strftime(tmp, sizeof(tmp), "%Y_%m_%d", &t);

			return tmp;
		}
		string basedir_;
		std::mutex file_lock;

	public:
		DailyLog() {
			name_ = name;
			basedir_ = basedir;
		};

		void debug(const string & s) {
			std::unique_lock < std::mutex > lck(file_lock);
			Log log(basedir + get_time() + '_' + "debug.log");
			log << s;
		}
		void info(const string & s) {
			std::unique_lock < std::mutex > lck(file_lock);
			Log log(basedir + get_time() + '_' + "info.log");
			log << s;
		}
		void error(const string & s) {
			std::unique_lock < std::mutex > lck(file_lock);
			Log log(basedir + get_time() + '_' + "error.log");
			log << s;
		}

		virtual void abstract_symbol() {};
	};

}





