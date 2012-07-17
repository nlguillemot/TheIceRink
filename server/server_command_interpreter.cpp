#include "server_command_interpreter.h"
#include "ice_rink_server.h"

namespace circles
{
	ServerCommandInterpreter::ServerCommandInterpreter(IceRinkServer* server)
		: server_(server)
	{
	}

	ServerCommandInterpreter::pointer ServerCommandInterpreter::create(IceRinkServer* server)
	{
		return ServerCommandInterpreter::pointer(new ServerCommandInterpreter(server));
	}

	void ServerCommandInterpreter::add_player(const std::string& player_name, int red, int green, int blue)
	{
		server_->add_player(player_name,red,green,blue);
	}

	void ServerCommandInterpreter::remove_player(const std::string& player_name)
	{
		server_->remove_player(player_name);
	}

	void ServerCommandInterpreter::update_player_position(const std::string& player_name, float x, float y)
	{
		server_->update_player_position(player_name,x,y);
	}

	void ServerCommandInterpreter::update_player_acceleration(const std::string& player_name, float x, float y)
	{
		server_->update_player_acceleration(player_name,x,y);
	}
}
