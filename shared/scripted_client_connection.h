#pragma once
#ifndef SCRIPTED_CLIENT_CONNECTION_H
#define SCRIPTED_CLIENT_CONNECTION_H

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/asio/read.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>

#include <string>
#include <vector>

#include "script1_0/input_buffer.h"
#include "script1_0/command_source.h"
#include "script1_0/command_parser.h"

namespace ng
{
	////////////////////////////////////////////////////////////
	/// Generic client connection used for games which communicate through scripts
	////////////////////////////////////////////////////////////
	template<class CommandParser>
	class ScriptedClientConnection
	{
	public:
		typedef boost::shared_ptr<ScriptedClientConnection> pointer;
		static pointer create(const std::string& host_name, const std::string& port);

		void init();
		void poll();
		void send_message_to_server(const std::string& update_message);

		// Accessors
		boost::asio::io_service& io_service();
		boost::asio::ip::tcp::socket& socket();

		void add_server_listener(typename CommandParser::pointer listener);
		void remove_server_listener(typename CommandParser::pointer listener);

		typedef boost::function<void (const std::string&)> EventCallback;
		void add_event_callback(EventCallback callback);
	private:
		ScriptedClientConnection(const std::string& host_name, const std::string& port);
		void set_connection_status(const std::string& message);

		void start_connecting_to_server();
		void keep_connecting_to_server();
		void handle_connection(const boost::system::error_code& ec);

		void start_reading_data();
		void handle_read_data(const boost::system::error_code& ec, size_t bytes_transferred);
		void process_input();

		void handle_sent_message(boost::shared_ptr<std::string> message, const boost::system::error_code& ec, size_t bytes_transferred);

		boost::asio::io_service io_service_;
		boost::asio::ip::tcp::socket socket_;
		std::string host_name_;
		std::string port_;
		bool connection_opened_;

		ng::InputBuffer input_buffer_;
		ng::CommandSource<CommandParser> command_source_;
		
		std::vector<EventCallback> event_callbacks_;
		void broadcast_event(const std::string& event);
	};

	template<class CommandParser>
	typename ScriptedClientConnection<CommandParser>::pointer ScriptedClientConnection<CommandParser>::create(const std::string& host_name, const std::string& port)
	{
		return ScriptedClientConnection::pointer(new ScriptedClientConnection(host_name, port));
	}

	template<class CommandParser>
	ScriptedClientConnection<CommandParser>::ScriptedClientConnection(const std::string& host_name, const std::string& port)
		: socket_(io_service_),
		host_name_(host_name),
		port_(port),
		connection_opened_(false)
	{
	}

	template<class CommandParser>
	void ScriptedClientConnection<CommandParser>::init()
	{
		start_connecting_to_server();
	}

	template<class CommandParser>
	void ScriptedClientConnection<CommandParser>::set_connection_status(const std::string& message)
	{
		broadcast_event(message);
	}

	template<class CommandParser>
	void ScriptedClientConnection<CommandParser>::start_connecting_to_server()
	{
		set_connection_status("connecting");
		keep_connecting_to_server();
	}

	template<class CommandParser>
	void ScriptedClientConnection<CommandParser>::keep_connecting_to_server()
	{
		using boost::asio::ip::tcp;

		boost::system::error_code ec;

		tcp::endpoint endpoint(
			boost::asio::ip::address::from_string(host_name_), 
			boost::lexical_cast<int>(port_)
			);

		socket_.async_connect(endpoint, 
			boost::bind(&ScriptedClientConnection<CommandParser>::handle_connection, this, boost::asio::placeholders::error)
			);
	}

	template<class CommandParser>
	void ScriptedClientConnection<CommandParser>::handle_connection(const boost::system::error_code& ec)
	{
		if(!ec)
		{
			start_reading_data();
			connection_opened_ = true;
			set_connection_status("connected");
		}
		else
		{
			keep_connecting_to_server();
		}
	}

	template<class CommandParser>
	void ScriptedClientConnection<CommandParser>::poll()
	{
		io_service_.poll();
	}

	template<class CommandParser>
	void ScriptedClientConnection<CommandParser>::start_reading_data()
	{
		socket_.async_read_some(boost::asio::buffer(input_buffer_.buffer(), input_buffer_.size()),
			boost::bind(&ScriptedClientConnection<CommandParser>::handle_read_data,this, boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred)
			);
	}

	template<class CommandParser>
	void ScriptedClientConnection<CommandParser>::handle_read_data(const boost::system::error_code& ec, size_t bytes_transferred)
	{
		if(!ec)
		{
			input_buffer_.pump_buffer(bytes_transferred);
			process_input();
			start_reading_data();
		}
		else
		{
			set_connection_status("read error");
			start_reading_data();
		}
	}

	template<class CommandParser>
	void ScriptedClientConnection<CommandParser>::process_input()
	{
		std::string current_line;
		while(input_buffer_.pop_token_from_queue(current_line))
		{
			command_source_.send_command(current_line);
		}
	}

	template<class CommandParser>
	void ScriptedClientConnection<CommandParser>::send_message_to_server(const std::string& update_message)
	{
		if(connection_opened_)
		{
			boost::shared_ptr<std::string> output_message(new std::string(update_message));

			socket_.async_write_some(boost::asio::buffer(output_message->data(), output_message->size()),
				boost::bind(&ScriptedClientConnection<CommandParser>::handle_sent_message, this, output_message,
				boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred)
				);
		}
	}

	template<class CommandParser>
	void ScriptedClientConnection<CommandParser>::add_server_listener(typename CommandParser::pointer listener)
	{
		command_source_.add_listener(listener);
	}

	template<class CommandParser>
	void ScriptedClientConnection<CommandParser>::remove_server_listener(typename CommandParser::pointer listener)
	{
		command_source_.remove_listener(listener);
	}

	template<class CommandParser>
	void ScriptedClientConnection<CommandParser>::add_event_callback(typename ScriptedClientConnection<CommandParser>::EventCallback callback)
	{
		event_callbacks_.push_back(callback);
	}

	template<class CommandParser>
	void ScriptedClientConnection<CommandParser>::broadcast_event(const std::string& event)
	{
		for(unsigned i = 0; i < event_callbacks_.size(); ++i)
		{
			event_callbacks_[i](event);
		}
	}

	template<class CommandParser>
	void ScriptedClientConnection<CommandParser>::handle_sent_message(boost::shared_ptr<std::string> message, const boost::system::error_code& ec, size_t /*bytes transferred*/)
	{
		if(!ec)
		{
			std::cout << "Successfully sent message: " << *message << std::endl;
		}
		else
		{
			set_connection_status("write error");
		}
	}

	template<class CommandParser>
	boost::asio::io_service& ScriptedClientConnection<CommandParser>::io_service()
	{
		return io_service_;
	}

	template<class CommandParser>
	boost::asio::ip::tcp::socket& ScriptedClientConnection<CommandParser>::socket()
	{
		return socket_;
	}
}

#endif
