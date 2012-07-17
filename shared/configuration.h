#pragma once

#include <string>

namespace circles
{
	class Configuration
	{
	public:
		Configuration(const std::string& filename);

		std::string character_name() const;
		std::string host_name() const;
		std::string port() const;
		int red() const;
		int green() const;
		int blue() const;
		int radius() const;
	private:
		void process_line(const std::string& line);

		std::string character_name_;
		std::string host_name_;
		std::string port_;
		int red_;
		int green_;
		int blue_;
	};
}