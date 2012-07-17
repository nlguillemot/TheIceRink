#pragma once
#ifndef SCRIPTED_SERVER_CONNECTION_H
#define SCRIPTED_SERVER_CONNECTION_H

#include <boost/asio/ip/tcp.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <string>

#include "script1_0/input_buffer.h"
#include "script1_0/command_source.h"

namespace ng
{
	////////////////////////////////////////////////////////////
	/// Generic server connection used for games which communicate through scripts
	////////////////////////////////////////////////////////////
	template<class CommandParser>
	class ScriptedServerConnection : public boost::enable_shared_from_this<ScriptedServerConnection<CommandParser>>
	{
	public:
		typedef boost::shared_ptr<ScriptedServerConnection<CommandParser>> pointer;
		typedef boost::function<void (const boost::system::error_code)> ConnectionErrorCallback;
		static pointer create(boost::asio::io_service& io_service);
		~ScriptedServerConnection();

		boost::asio::ip::tcp::socket& socket();

		void send_message_to_client(const std::string& update_message);

		void start();
		void add_server_listener(typename CommandParser::pointer listener);
		void remove_server_listener(typename CommandParser::pointer listener);

		void add_error_callback(ConnectionErrorCallback callback);
	private:
		ScriptedServerConnection(boost::asio::io_service& io_service);

		void start_reading();
		void handle_read_data(const boost::system::error_code& ec, size_t bytes_transferred);
		void process_input();
		// returns true if successfully tokenized a line and stored it in "token", false otherwise
		bool create_token_from_queue(std::string& token);

		void handle_sent_message(boost::shared_ptr<std::string> message, const boost::system::error_code& ec, size_t bytes_transferred);
		void broadcast_error(const boost::system::error_code ec);
		boost::asio::ip::tcp::socket socket_;
		
		ng::CommandSource<CommandParser> command_source_;
		ng::InputBuffer input_buffer_;

		std::vector<ConnectionErrorCallback> error_callbacks_;
	};

	template<class CommandParser>
	typename ScriptedServerConnection<CommandParser>::pointer ScriptedServerConnection<CommandParser>::create(boost::asio::io_service& io_service)
	{
		return ScriptedServerConnection::pointer(new ScriptedServerConnection<CommandParser>(io_service));
	}

	template<class CommandParser>
	boost::asio::ip::tcp::socket& ScriptedServerConnection<CommandParser>::socket()
	{
		return socket_;
	}

	template<class CommandParser>
	void ScriptedServerConnection<CommandParser>::send_message_to_client(const std::string& update_message)
	{
		boost::shared_ptr<std::string> output_message(new std::string(update_message));

		socket_.async_write_some(boost::asio::buffer(output_message->data(), output_message->size()),
			boost::bind(&ScriptedServerConnection<CommandParser>::handle_sent_message, this, output_message,
			boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred)
			);
	}

	template<class CommandParser>
	void ScriptedServerConnection<CommandParser>::handle_sent_message(boost::shared_ptr<std::string> message, const boost::system::error_code& ec, size_t bytes_transferred)
	{
		if(ec)
		{
			std::cerr << "Write error" << std::endl;
			broadcast_error(ec);
		}
	}

	template<class CommandParser>
	void ScriptedServerConnection<CommandParser>::broadcast_error(const boost::system::error_code ec)
	{
		for(std::vector<ConnectionErrorCallback>::iterator it = error_callbacks_.begin(); it != error_callbacks_.end(); ++it)
		{
			(*it)(ec);
		}
	}

	template<class CommandParser>
	void ScriptedServerConnection<CommandParser>::start()
	{
		start_reading();
	}
	
	template<class CommandParser>
	void ScriptedServerConnection<CommandParser>::add_server_listener(typename CommandParser::pointer listener)
	{
		command_source_.add_listener(listener);
	}
	
	template<class CommandParser>
	void ScriptedServerConnection<CommandParser>::remove_server_listener(typename CommandParser::pointer listener)
	{
		command_source_.remove_listener(listener);
	}

	template<class CommandParser>
	void ScriptedServerConnection<CommandParser>::add_error_callback(ConnectionErrorCallback callback)
	{
		error_callbacks_.push_back(callback);
	}

	template<class CommandParser>
	ScriptedServerConnection<CommandParser>::ScriptedServerConnection(boost::asio::io_service& io_service)
		:	socket_(io_service)
	{
	}

	template<class CommandParser>
	ScriptedServerConnection<CommandParser>::~ScriptedServerConnection()
	{
		std::cout << "Connection closed" << std::endl;
	}

	template<class CommandParser>
	void ScriptedServerConnection<CommandParser>::start_reading()
	{
		socket_.async_read_some(boost::asio::buffer(input_buffer_.buffer(), input_buffer_.size()),
			boost::bind(&ScriptedServerConnection<CommandParser>::handle_read_data, this, boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred)
			);
	}

	template<class CommandParser>
	void ScriptedServerConnection<CommandParser>::handle_read_data(const boost::system::error_code& ec, size_t bytes_transferred)
	{
		if(!ec)
		{
			input_buffer_.pump_buffer(bytes_transferred);
			process_input();
			start_reading();
		}
		else
		{
			std::cerr << "read error" << std::endl;
			broadcast_error(ec);
		}
	}

	template<class CommandParser>
	void ScriptedServerConnection<CommandParser>::process_input()
	{
		std::string current_line;
		while(input_buffer_.pop_token_from_queue(current_line))
		{
			command_source_.send_command(current_line);
			std::cout << "Processing: " << current_line << std::endl;
		}
	}
}

#endif
