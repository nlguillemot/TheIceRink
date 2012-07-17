#include "configuration.h"

#include <fstream>
#include <iostream>
#include <boost/lexical_cast.hpp>

#include "math_helper.h"
#include "constants.h"

namespace circles
{
	Configuration::Configuration(const std::string& filename)
	{
		std::ifstream configuration_input_file(filename);

		for(;;)
		{
			std::string current_line;
			std::getline(configuration_input_file, current_line);

			if(!current_line.empty())
			{
				process_line(current_line);
			}

			if(configuration_input_file.eof())
			{
				break;
			}
		}
	}

	void Configuration::process_line(const std::string& line)
	{
		size_t colon_position_in_entry = line.find_first_of(':');
		std::string entry_tag = line.substr(0, colon_position_in_entry);
		std::string entry_info = line.substr(colon_position_in_entry+1);

		try
		{
			if(entry_tag == "name")
			{
				character_name_ = entry_info;
			}
			else if(entry_tag == "host")
			{
				host_name_ = entry_info;
			}
			else if(entry_tag == "port")
			{
				port_ = entry_info;
			}
			else if(entry_tag == "red")
			{
				red_ = boost::lexical_cast<int>(entry_info);
			}
			else if(entry_tag == "green")
			{
				green_ = boost::lexical_cast<int>(entry_info);
			}
			else if(entry_tag == "blue")
			{
				blue_ = boost::lexical_cast<int>(entry_info);
			}
		}
		catch(std::exception& e)
		{
			std::cerr << e.what() << std::endl;
		}
	}

	std::string Configuration::character_name() const
	{
		return character_name_;
	}

	std::string Configuration::host_name() const
	{
		return host_name_;
	}

	std::string Configuration::port() const
	{
		return port_;
	}

	int Configuration::red() const
	{
		return red_;
	}

	int Configuration::green() const
	{
		return green_;
	}

	int Configuration::blue() const
	{
		return blue_;
	}

	int Configuration::radius() const
	{
		return Constants::kPlayerRadius;
	}
}
