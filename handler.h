#include "response.h"
#include "request.h"

namespace yy_webserver {
	namespace castango {
		class Handler {
		public:
			virtual void operator () (const Request & requset, BaseResponse ** response) = 0;
		};
	}
}
