#pragma once


#include "exception.h"
#include "settings.h"
#include "auto_lock.h"

#include <string>
#include <fstream>
#include <mutex>


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
		}
		Log& operator << (const char * s) {
			std::unique_lock < std::mutex > lck(file_lock);
			outFile << s << std::endl;
		}

	};
}





