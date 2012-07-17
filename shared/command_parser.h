#pragma once
#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

namespace ng
{
	////////////////////////////////////////////////////////////
	/// Parses lines of script and turns them into commands sent to an interpreter
	////////////////////////////////////////////////////////////
	template<class CommandInterpreter>
	class CommandParser
	{
	public:
		CommandParser(typename CommandInterpreter::pointer interpreter);
		typedef boost::shared_ptr<CommandParser> pointer;
		static pointer create(typename CommandInterpreter::pointer interpreter);

		virtual void parse_line(const std::string& line)=0;
	protected:
		typedef std::vector<std::string> TokenList;
		TokenList tokenize(const std::string& line);

		typename CommandInterpreter::pointer interpreter_;
	};

	template<class CommandInterpreter>
	typename CommandParser<CommandInterpreter>::pointer CommandParser<CommandInterpreter>::create(typename CommandInterpreter::pointer interpreter)
	{
		return CommandParser::pointer(new CommandParser(interpreter));
	}

	template<class CommandInterpreter>
	CommandParser<CommandInterpreter>::CommandParser(typename CommandInterpreter::pointer interpreter)
		: interpreter_(interpreter)
	{
	}

	template <class CommandInterpreter>
	typename CommandParser<CommandInterpreter>::TokenList CommandParser<CommandInterpreter>::tokenize(const std::string& line)
	{
		TokenList tokens;
		std::stringstream stream;
		stream << line;
		std::string temp;
		while(!stream.eof())
		{
			stream >> temp;
			tokens.push_back(temp);
		}
		return tokens;
	}
}
#endif
