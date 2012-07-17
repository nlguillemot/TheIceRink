#pragma once
#ifndef ICE_RINK_SERVER_H
#define ICE_RINK_SERVER_H

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/error.hpp>

#include <vector>

#include "server_command_interpreter.h"
#include "circles_command_parser.h"
#include "player.h"
#include "script1_0/scripted_server_connection.h"

namespace circles
{
	class IceRinkServer
	{
	public:
		typedef CirclesCommandParser<CirclesCommandInterpreter> AppCommandParser;
		typedef ng::ScriptedServerConnection<CirclesCommandParser<CirclesCommandInterpreter>> AppServerConnection;
		typedef std::vector<AppServerConnection::pointer> ServerConnectionList;
		typedef std::map<std::string, Player::pointer> PlayerList;
		
		IceRinkServer(boost::asio::io_service& io_service, int port);

		void add_player(const std::string& player_name, int red, int green, int blue);
		void remove_player(const std::string& player_name);
		void update_player_position(const std::string& player_name, float x, float y);
		void update_player_acceleration(const std::string& player_name, float x, float y);

		void on_connection_error(AppServerConnection::pointer broken_connection, const boost::system::error_code e);
	private:
		void start_accept();
		void handle_accept(AppServerConnection::pointer new_connection, const boost::system::error_code& ec);
		void send_current_player_list(AppServerConnection::pointer new_connection);

		boost::asio::ip::tcp::acceptor acceptor_;
		ServerConnectionList connections_;

		PlayerList active_players_;
		typedef std::map<AppServerConnection::pointer, Player::pointer> ConnectionPlayerMap;
		ConnectionPlayerMap connection_player_map_;
	};
}

#endif
