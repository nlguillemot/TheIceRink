#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/String.hpp>

#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "player.h"
#include "circles_command_parser.h"
#include "circles_command_interpreter.h"
#include "script1_0/scripted_client_connection.h"

namespace circles
{
	class ServerConnection;

	class MainApp
	{
	public:
		void exec();

		// Accessors for app data
		Player::pointer session_player() const;
		const sf::String& connection_info() const;
		void set_connection_info(const std::string& info);
		void handle_server_callback(const std::string& info);

		void add_player(const std::string& player_name, int red, int green, int blue);
		void remove_player(const std::string& player_name);
		void update_player_position(const std::string& player_name, float x, float y);
		void update_player_acceleration(const std::string& player_name, float x, float y);
	private:
		void init();
		void run();
		void exit();

		void init_graphics();
		void init_connection();

		void send_self_connection();

		void update_players();
		void send_updated_position(boost::system::error_code);
		void draw_players();

		boost::shared_ptr<sf::RenderWindow> app_;
		Player::pointer session_player_;

		typedef std::map<std::string, Player::pointer> PlayerList;
		PlayerList active_players_;

		sf::String version_info_;
		sf::String connection_info_;

		typedef CirclesCommandParser<CirclesCommandInterpreter> AppCommandParser;

		typedef ng::ScriptedClientConnection<CirclesCommandParser<CirclesCommandInterpreter>> AppClientConnection;
		AppClientConnection::pointer client_connection_;

		boost::shared_ptr<boost::asio::deadline_timer> player_update_timer_;
	};
}
