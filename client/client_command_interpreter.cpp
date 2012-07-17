#include "client_command_interpreter.h"

#include "main_app.h"

namespace circles
{
	ClientCommandInterpreter::ClientCommandInterpreter(MainApp* app)
		: app_(app)
	{
	}

	ClientCommandInterpreter::pointer ClientCommandInterpreter::create(MainApp* app)
	{
		return ClientCommandInterpreter::pointer(new ClientCommandInterpreter(app));
	}

	void ClientCommandInterpreter::add_player(const std::string& player_name, int red, int green, int blue)
	{
		app_->add_player(player_name, red, green, blue);
	}

	void ClientCommandInterpreter::remove_player(const std::string& player_name)
	{
		app_->remove_player(player_name);
	}

	void ClientCommandInterpreter::update_player_position(const std::string& player_name, float x, float y)
	{
		app_->update_player_position(player_name,x,y);
	}

	void ClientCommandInterpreter::update_player_acceleration(const std::string& player_name, float x, float y)
	{
		app_->update_player_acceleration(player_name,x,y);
	}
}
