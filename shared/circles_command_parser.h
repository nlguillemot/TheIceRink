#pragma once
#ifndef CIRCLES_COMMAND_PARSER_H
#define CIRCLES_COMMAND_PARSER_H

#include "script1_0/command_parser.h"
#include <boost/lexical_cast.hpp>
#include <iostream>

namespace circles
{
	template <class CirclesCommandInterpreter>
	class CirclesCommandParser : public ng::CommandParser<CirclesCommandInterpreter>
	{
	public:
		typedef boost::shared_ptr<CirclesCommandParser> pointer;
		static pointer create(typename CirclesCommandInterpreter::pointer interpreter);
		CirclesCommandParser(typename CirclesCommandInterpreter::pointer interpreter);
		
		void parse_line(const std::string& line);
	private:
		void process_line(const std::string& action, const std::vector<std::string>& args);
	};

	template <class CirclesCommandInterpreter>
	typename CirclesCommandParser<CirclesCommandInterpreter>::pointer CirclesCommandParser<CirclesCommandInterpreter>::create(typename CirclesCommandInterpreter::pointer interpreter)
	{
		return CirclesCommandParser::pointer(new CirclesCommandParser(interpreter));
	}

	template <class CirclesCommandInterpreter>
	CirclesCommandParser<CirclesCommandInterpreter>::CirclesCommandParser(typename CirclesCommandInterpreter::pointer interpreter)
		: CommandParser<CirclesCommandInterpreter>(interpreter)
	{
	}

	template <class CirclesCommandInterpreter>
	void CirclesCommandParser<CirclesCommandInterpreter>::parse_line(const std::string& line)
	{
		TokenList tokens = tokenize(line);
		std::string action = tokens[0];
		std::vector<std::string> args(tokens.begin()+1, tokens.end());
		process_line(action,args);
	}

	template <class CirclesCommandInterpreter>
	void CirclesCommandParser<CirclesCommandInterpreter>::process_line(const std::string& action, const std::vector<std::string>& args)
	{
		using boost::lexical_cast;

		try
		{
			if(action == "AddPlayer")
			{
				std::string player_name = lexical_cast<std::string>(args.at(0));
				int player_red = lexical_cast<int>(args.at(1));
				int player_green = lexical_cast<int>(args.at(2));
				int player_blue = lexical_cast<int>(args.at(3));
				interpreter_->add_player(player_name,player_red,player_green,player_blue);
			}
			else if(action == "RemovePlayer")
			{
				std::string player_name = lexical_cast<std::string>(args.at(0));
				interpreter_->remove_player(player_name);
			}
			else if(action == "UpdatePosition")
			{
				std::string player_name = lexical_cast<std::string>(args.at(0));
				float player_x = lexical_cast<float>(args.at(1));
				float player_y = lexical_cast<float>(args.at(2));
				interpreter_->update_player_position(player_name,player_x,player_y);
			}
			else if(action == "UpdateAcceleration")
			{
				std::string player_name = lexical_cast<std::string>(args.at(0));
				float player_x = lexical_cast<float>(args.at(1));
				float player_y = lexical_cast<float>(args.at(2));
				interpreter_->update_player_acceleration(player_name, player_x,player_y);
			}
			else
			{
				std::cerr << "Unhandled action: " << action << std::endl;
			}
		}
		catch(std::exception&)
		{
			std::cerr << "Failed to process command: " << action << std::endl;
		}
	}
}

#endif
