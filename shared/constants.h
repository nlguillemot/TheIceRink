#pragma once

namespace circles
{
	struct Constants
	{
		// Window properties
		static const int kWindowWidth = 640;
		static const int kWindowHeight = 480;
		static const int kWindowDebugTextSize = 14;

		// Player graphical properties
		static const int kPlayerRadius = 10;

		// Networking properties
		static const int kPlayerPositionUpdateDelay = 2; // seconds
	};
}
