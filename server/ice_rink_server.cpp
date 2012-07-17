#include "ice_rink_server.h"
#include "script1_0/scripted_server_connection.h"
#include "constants.h"

#include <boost/bind.hpp>
#include <boost/asio/placeholders.hpp>
#include <iostream>

namespace circles
{
	IceRinkServer::IceRinkServer(boost::asio::io_service& io_service, int port)
		: acceptor_(io_service,boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
	{
		start_accept();
	}

	void IceRinkServer::start_accept()
	{
		AppServerConnection::pointer new_connection =
			AppServerConnection::create(acceptor_.get_io_service());

		acceptor_.async_accept(new_connection->socket(),
			boost::bind(&IceRinkServer::handle_accept, this, new_connection,
			boost::asio::placeholders::error));
	}

	void IceRinkServer::handle_accept(AppServerConnection::pointer new_connection, const boost::system::error_code& ec)
	{
		if(!ec)
		{
			std::cout << "Accepted new connection" << std::endl;
			connections_.push_back(new_connection);
			AppCommandParser::pointer new_parser = AppCommandParser::create(ServerCommandInterpreter::create(this));
			new_connection->add_server_listener(new_parser);
			new_connection->start();
			new_connection->add_error_callback( boost::bind(&IceRinkServer::on_connection_error, this, new_connection, _1) );
			connection_player_map_[new_connection] = Player::pointer();
			send_current_player_list(new_connection);
		}
		start_accept();
	}

	void IceRinkServer::send_current_player_list(AppServerConnection::pointer new_connection)
	{
		for(PlayerList::iterator it = active_players_.begin(); it != active_players_.end(); ++it)
		{
			std::stringstream message;
			Player::pointer player = it->second;
			message << "AddPlayer " << (std::string)player->name().GetText() << " " << player->red() << " " << player->green() << " " << player->blue() << std::endl;
			new_connection->send_message_to_client(message.str());
			message.clear();
			message << "UpdatePosition " << (std::string)player->name().GetText() << " " << player->position().x << " " << player->position().y << std::endl;
			new_connection->send_message_to_client(message.str());
			message.clear();
			message << "UpdateAcceleration " << (std::string)player->name().GetText() << " " << player->position().x << " " << player->position().y << std::endl;
		}
	}

	void IceRinkServer::add_player(const std::string& player_name, int red, int green, int blue)
	{
		if(active_players_.find(player_name) == active_players_.end())
		{
			active_players_[player_name] = Player::create(player_name,red,green,blue,Constants::kPlayerRadius);
		}
		
		for(ConnectionPlayerMap::iterator it = connection_player_map_.begin(); it != connection_player_map_.end(); ++it)
		{
			if(!it->second)
			{
				it->second = active_players_[player_name];
				break;
			}
		}

		std::stringstream message;
		message << "AddPlayer " << player_name << " " << red << " " << green << " " << blue << std::endl;
		for(ServerConnectionList::iterator it = connections_.begin(); it != connections_.end(); ++it)
		{
			(*it)->send_message_to_client(message.str()); 
		}
	}

	void IceRinkServer::remove_player(const std::string& player_name)
	{
		PlayerList::iterator it = active_players_.find(player_name);
		if(it != active_players_.end())
		{
			for(ConnectionPlayerMap::iterator con = connection_player_map_.begin(); con != connection_player_map_.end(); ++con)
			{
				if(con->second == it->second)
				{
					connection_player_map_.erase(con);
					break;
				}
			}
			active_players_.erase(it);
		}
		
		std::stringstream message;
		message << "RemovePlayer " << player_name << std::endl;
		for(ServerConnectionList::iterator it = connections_.begin(); it != connections_.end(); ++it)
		{
			(*it)->send_message_to_client(message.str()); 
		}
	}

	void IceRinkServer::update_player_position(const std::string& player_name, float x, float y)
	{
		std::stringstream message;
		message << "UpdatePosition " << player_name << " " << x << " " << y << std::endl;
		for(ServerConnectionList::iterator it = connections_.begin(); it != connections_.end(); ++it)
		{
			(*it)->send_message_to_client(message.str()); 
		}
	}

	void IceRinkServer::update_player_acceleration(const std::string& player_name, float x, float y)
	{
		std::stringstream message;
		message << "UpdateAcceleration " << player_name << " " << x << " " << y << std::endl;
		for(ServerConnectionList::iterator it = connections_.begin(); it != connections_.end(); ++it)
		{
			(*it)->send_message_to_client(message.str()); 
		}
	}

	void IceRinkServer::on_connection_error(AppServerConnection::pointer broken_connection,const boost::system::error_code e)
	{
		ConnectionPlayerMap::iterator it = connection_player_map_.find(broken_connection);
		if(it != connection_player_map_.end())
		{
			remove_player((std::string)connection_player_map_[broken_connection]->name().GetText());
		}

		ServerConnectionList::iterator con = std::find(connections_.begin(), connections_.end(), broken_connection);
		if(con != connections_.end())
		{
			connections_.erase(con);
		}
	}
}
