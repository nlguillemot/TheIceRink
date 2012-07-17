#pragma once
#ifndef CLIENT_COMMAND_INTERPRETER_H
#define CLIENT_COMMAND_INTERPRETER_H

#include "command_parser.h"
#include "circles_command_interpreter.h"

namespace circles
{
	class MainApp;
////////////////////////////////////////////////////////////
/// Game client specific reactions to the commands received from the server
////////////////////////////////////////////////////////////
	class ClientCommandInterpreter : public CirclesCommandInterpreter
	{
	public:
		ClientCommandInterpreter(MainApp* app);
		static pointer create(MainApp* app);
		
		void add_player(const std::string& player_name, int red, int green, int blue);
		void remove_player(const std::string& player_name);
		void update_player_position(const std::string& player_name, float x, float y);
		void update_player_acceleration(const std::string& player_name, float x, float y);
	private:
		MainApp* app_;
	};
}
#endif
