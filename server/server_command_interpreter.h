#pragma once
#ifndef SERVER_COMMAND_INTERPRETER_H
#define SERVER_COMMAND_INTERPRETER_H

#include "circles_command_interpreter.h"

namespace circles
{
	class IceRinkServer;

	class ServerCommandInterpreter : public CirclesCommandInterpreter
	{
	public:
		ServerCommandInterpreter(IceRinkServer* server);
		static pointer create(IceRinkServer* server);

		void add_player(const std::string& player_name, int red, int green, int blue);
		void remove_player(const std::string& player_name);
		void update_player_position(const std::string& player_name, float x, float y);
		void update_player_acceleration(const std::string& player_name, float x, float y);
		void get_player_list();
	private:
		IceRinkServer* server_;
	};
}
#endif
