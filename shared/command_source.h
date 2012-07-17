#pragma once
#ifndef COMMAND_SOURCE_H
#define COMMAND_SOURCE_H

#include <vector>
#include <boost/bind.hpp>

namespace ng
{
	////////////////////////////////////////////////////////////
	/// Used to broadcast commands to a list of command parsers
	////////////////////////////////////////////////////////////
	template<class CommandParser>
	class CommandSource
	{
	public:
		void add_listener(typename CommandParser::pointer listener);
		void remove_listener(typename CommandParser::pointer listener);
		void send_command(const std::string& line);
	private:
		typedef std::vector<typename CommandParser::pointer> ListenerList;
		ListenerList command_listeners_;
	};

	template<class CommandParser>
	void CommandSource<CommandParser>::add_listener(typename CommandParser::pointer listener)
	{
		command_listeners_.push_back(listener);
	}

	template<class CommandParser>
	void CommandSource<CommandParser>::remove_listener(typename CommandParser::pointer listener)
	{
		ListenerList::iterator it
			= std::find(command_listeners_.begin(), command_listeners_.end(),
			listener);
		assert(it != command_listeners_.end());
		command_listeners_.erase(it);
	}

	template<class CommandParser>
	void CommandSource<CommandParser>::send_command(const std::string& line)
	{
		for(ListenerList::iterator it = command_listeners_.begin(); it != command_listeners_.end(); ++it)
		{
			(*it)->parse_line(line);
		}
	}
}

#endif
