#pragma once


#include <exception>
#include <string>

namespace yy_webserver {
	class Exception : public std::exception 
	{
	private:
		std::string error_msg_;
	public:
		Exception() {};
		explicit Exception(const std::string & error_msg) : error_msg_(error_msg){}
		explicit Exception(std::string && error_msg) : error_msg_(error_msg){}

		virtual const std::string what() { return error_msg_; }

	};
}
