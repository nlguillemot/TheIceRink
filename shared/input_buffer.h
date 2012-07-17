#pragma once
#ifndef INPUT_BUFFER_H
#define INPUT_BUFFER_H

#include <string>
#include <algorithm>
#include <boost/array.hpp>
#include <string>

namespace ng
{
	////////////////////////////////////////////////////////////
	/// Tokenizes input into individual lines in order to be parsed python-style
	////////////////////////////////////////////////////////////
	class InputBuffer
	{
	public:	
		inline void pump_buffer(size_t bytes_transferred);
		// returns true if successfully tokenized a line and stored it in "token", false otherwise
		inline bool pop_token_from_queue(std::string& token);
		// Accessors
		inline int size() const;
		inline char* buffer();
	private:
		std::string process_queue_;
		boost::array<char, 128> buffer_;
	};

	int InputBuffer::size() const
	{
		return buffer_.size();
	}

	char* InputBuffer::buffer()
	{
		return buffer_.data();
	}

	void InputBuffer::pump_buffer(size_t bytes_transferred)
	{
		std::stringstream stream;
		stream.write(buffer_.data(), bytes_transferred);
		process_queue_ += stream.str();
	}

	// returns true if successfully tokenized a line and stored it in "token", false otherwise
	bool InputBuffer::pop_token_from_queue(std::string& token)
	{
		std::string::iterator next_newline_it = std::find(
			process_queue_.begin(),process_queue_.end(), '\n'
			);

		if(next_newline_it == process_queue_.end())
		{
			return false;
		}
		else
		{
			int index_of_newline_ch = next_newline_it - process_queue_.begin();
			token = process_queue_.substr(0, index_of_newline_ch);
			process_queue_.erase(process_queue_.begin(), next_newline_it+1);
			return true;
		}
	}
}

#endif
