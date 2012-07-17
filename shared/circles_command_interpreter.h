#ifndef CIRCLES_COMMAND_INTERPRETER_H
#define CIRCLES_COMMAND_INTERPRETER_H

#include <string>
#include <boost/shared_ptr.hpp>

namespace circles
{
////////////////////////////////////////////////////////////
/// Defines the set of commands which can be sent by the parser
////////////////////////////////////////////////////////////
	class CirclesCommandInterpreter
	{
	public:
		typedef boost::shared_ptr<CirclesCommandInterpreter> pointer;
		
		virtual void add_player(const std::string& player_name, int red, int green, int blue)=0;
		virtual void remove_player(const std::string& player_name)=0;
		virtual void update_player_position(const std::string& player_name, float x, float y)=0;
		virtual void update_player_acceleration(const std::string& player_name, float x, float y)=0;
	};
}

#endif
