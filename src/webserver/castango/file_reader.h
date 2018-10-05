#pragma once
#include "response.h"

namespace yy_webserver {
	namespace castango {
		class FileReader {
			bool (*readsome_func_)(unsigned char * buffer, int length) ;
			BaseResponse* (*success_func_)();
			void (*error_func_)();

		public:
			

			FileReader() {};

			FileReader(bool(*readsome_func)(unsigned char * buffer, int length), BaseResponse* (*success_func)(), void(*error_func)()) :
				readsome_func_(readsome_func), success_func_(success_func), error_func_(error_func) {}

			bool readsome(unsigned char * buffer, int length) {
				return readsome_func_(buffer, length);
			}

			BaseResponse* success() {
				return success_func_();
			}

			void error() {
				error_func_();
			}
		};
	}
}