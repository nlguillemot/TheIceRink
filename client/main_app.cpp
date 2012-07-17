#include "main_app.h"

#include <boost/bind.hpp>
#include <algorithm>

#include "configuration.h"
#include "constants.h"
#include "circles_command_parser.h"
#include "client_command_interpreter.h"
#include "script1_0/scripted_client_connection.h"

namespace circles
{
	void MainApp::init()
	{
		init_graphics();
		init_connection();
	}

	void MainApp::init_graphics()
	{
		circles::Configuration session_configuration("config.txt");
		const std::string window_title = std::string("The Ice Rink at ") + session_configuration.host_name() + ":" + session_configuration.port();

		sf::VideoMode video_mode(circles::Constants::kWindowWidth,circles::Constants::kWindowHeight);
		app_.reset(new sf::RenderWindow(video_mode, window_title));
		app_->EnableKeyRepeat(false);

		session_player_.reset( new circles::Player(session_configuration) );
		active_players_[(std::string)session_player_->name().GetText()] = session_player_;

		version_info_ = sf::String("multiplayer pre-alpha",sf::Font::GetDefaultFont(),(float)Constants::kWindowDebugTextSize);
		version_info_.SetPosition((float)Constants::kWindowWidth - version_info_.GetRect().GetWidth(),
			(float)Constants::kWindowHeight - version_info_.GetRect().GetHeight()
			);
		version_info_.SetColor(sf::Color::Black);
		
		connection_info_ = sf::String("",sf::Font::GetDefaultFont(),(float)Constants::kWindowDebugTextSize);
		connection_info_.SetColor(sf::Color::Black);
		set_connection_info("network info");
	}

	void MainApp::init_connection()
	{
		circles::Configuration session_configuration("config.txt");
		client_connection_ = AppClientConnection::create(session_configuration.host_name(), session_configuration.port());
		
		AppCommandParser::pointer command_parser = AppCommandParser::create(ClientCommandInterpreter::create(this));
		client_connection_->add_server_listener(command_parser);
		client_connection_->add_event_callback( boost::bind(&MainApp::set_connection_info,this,_1) );
		client_connection_->add_event_callback( boost::bind(&MainApp::handle_server_callback, this, _1) );
		client_connection_->init();

		session_player_->add_event_callback(boost::bind(&AppClientConnection::send_message_to_server, client_connection_, _1));
		
		player_update_timer_.reset(new boost::asio::deadline_timer(client_connection_->io_service(), boost::posix_time::seconds(Constants::kPlayerPositionUpdateDelay)));
		player_update_timer_->async_wait( boost::bind(&MainApp::send_updated_position, this, boost::asio::placeholders::error) );
	}

	void MainApp::exit()
	{
	}

	void MainApp::exec()
	{
		init();
		run();
		exit();
	}

	Player::pointer MainApp::session_player() const
	{
		return session_player_;
	}

	const sf::String& MainApp::connection_info() const
	{
		return connection_info_;
	}

	void MainApp::set_connection_info(const std::string& info)
	{
		connection_info_.SetText(info);
		connection_info_.SetPosition(0, (float)Constants::kWindowHeight - connection_info_.GetRect().GetHeight());
	}

	void MainApp::handle_server_callback(const std::string& info)
	{
		if(info == "connected")
		{
			send_self_connection();
		}
	}

	void MainApp::send_self_connection()
	{
		std::stringstream message;
		message << "AddPlayer " << (std::string)session_player_->name().GetText() << " " << (int)session_player_->red() << " " << (int)session_player_->green() << " " << (int)session_player_->blue() << std::endl;
		client_connection_->send_message_to_server(message.str());
	}

	void MainApp::add_player(const std::string& player_name, int red, int green, int blue)
	{
		if(active_players_.find(player_name) == active_players_.end())
		{
			active_players_[player_name] = Player::create(player_name,red,green,blue,Constants::kPlayerRadius);
		}
	}

	void MainApp::remove_player(const std::string& player_name)
	{
		PlayerList::iterator it = active_players_.find(player_name);
		if(it != active_players_.end())
		{
			active_players_.erase(it);
		}
	}

	void MainApp::update_player_position(const std::string& player_name, float x, float y)
	{
		PlayerList::iterator it = active_players_.find(player_name);
		if(it != active_players_.end())
		{
			(it->second)->set_position(sf::Vector2f(x,y));
		}
	}

	void MainApp::update_player_acceleration(const std::string& player_name, float x, float y)
	{
		PlayerList::iterator it = active_players_.find(player_name);
		if(it != active_players_.end())
		{
			(it->second)->set_acceleration(sf::Vector2f(x,y));
		}
	}

	void MainApp::run()
	{
		while(app_->IsOpened())
		{
			sf::Event e;
			while(app_->GetEvent(e))
			{
				switch(e.Type)
				{
				case sf::Event::Closed:
					app_->Close();
					break;
				case sf::Event::KeyPressed:
					session_player_->handle_input(e.Key.Code, true);
					break;
				case sf::Event::KeyReleased:
					session_player_->handle_input(e.Key.Code, false);
					break;
				default:
					break;
				}
			}

			update_players();

			app_->Clear(sf::Color::White);

			draw_players();

			app_->Draw(connection_info_);
			app_->Draw(version_info_);

			app_->Display();

			client_connection_->poll();
		}
	}

	void MainApp::update_players()
	{
		float frametime = app_->GetFrameTime();
		for(PlayerList::iterator it = active_players_.begin(); it != active_players_.end(); ++it)
		{
			(it->second)->update(frametime);
		}
	}

	void MainApp::send_updated_position(boost::system::error_code)
	{
		session_player_->send_updated_position();
		player_update_timer_->expires_at(player_update_timer_->expires_at() + boost::posix_time::seconds(Constants::kPlayerPositionUpdateDelay));
		player_update_timer_->async_wait(boost::bind(&MainApp::send_updated_position, this,
			boost::asio::placeholders::error));	
	}

	void MainApp::draw_players()
	{
		for(PlayerList::iterator it = active_players_.begin(); it != active_players_.end(); ++it)
		{
			(it->second)->draw(*app_);
		}
	}
}
