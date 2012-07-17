#include <iostream>

#include <boost/asio/io_service.hpp>

#include "ice_rink_server.h"

int main()
{
	try
	{
		boost::asio::io_service io;
		circles::IceRinkServer server(io,13);
		io.run();
	}
	catch(std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
}
