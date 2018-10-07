
#include "url.h"
#include "webserver.h"


void configure_url(my_handler& handler) {
	handler.add_url("AB", AB);
}