#include "main_app.h"

#include <iostream>

int main()
{
	try
	{
		circles::MainApp app;
		app.exec();
	}
	catch(std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
}
