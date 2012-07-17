#include "network_helper.h"
#include "server_connection.h"

#include <sstream>

namespace circles
{
	NetworkHelper::NetworkHelper(ServerConnection& server_connection)
		: server_connection_(server_connection)
	{
	}

	void NetworkHelper::send_add_player(const std::string& player_name, int red, int green, int blue)
	{
		std::stringstream message_stream;
		message_stream << "Player AddToGame " << player_name << " " << red << " " << green << " " << blue << std::endl;
		server_connection_.send_message_to_server(message_stream.str());
	}

	void NetworkHelper::send_remove_player(const std::string& player_name)
	{
		std::stringstream message_stream;
		message_stream << "Player RemoveFromGame " << player_name << std::endl;
		server_connection_.send_message_to_server(message_stream.str());
	}
	
	void NetworkHelper::send_update_player_position(const std::string& player_name, int x, int y)
	{
		std::stringstream message_stream;
		message_stream << "Player UpdatePosition " << player_name << " " << x << " " << y << std::endl;
		server_connection_.send_message_to_server(message_stream.str());
	}

	void NetworkHelper::send_update_player_force(const std::string& player_name, int x, int y)
	{
		std::stringstream message_stream;
		message_stream << "Player UpdateForce " << player_name << " " << x << " " << y << std::endl;
		server_connection_.send_message_to_server(message_stream.str());
	}
}
