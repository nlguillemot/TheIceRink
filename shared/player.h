#pragma once
#ifndef PLAYER_H
#define PLAYER_H

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Shape.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/String.hpp>

namespace sf
{
	class RenderWindow;
}

#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>

#include <string>

#include "configuration.h"

namespace circles
{
	class Player
	{
	public:
		typedef boost::shared_ptr<Player> pointer;
		
		static pointer create(const std::string& player_name, int red, int green, int blue, int radius);
		static pointer create(const circles::Configuration& config);

		Player(const std::string& player_name, int red, int green, int blue, int radius);
		Player(const circles::Configuration& config);

		sf::Color color() const;
		void set_color(const sf::Color& c);		int red() const;
		int green() const;
		int blue() const;


		sf::Shape shape() const;
		int radius() const;
		sf::String name() const;

		sf::Vector2f position() const;
		void set_position(const sf::Vector2f& p);
		sf::Vector2f velocity() const;
		void set_velocity(const sf::Vector2f& v);
		sf::Vector2f acceleration() const;
		void set_acceleration(const sf::Vector2f& a);

		void update(float dt);
		void send_updated_position();
		void draw(sf::RenderWindow& window);
		void handle_input(sf::Key::Code code, bool pressed);

		typedef boost::function<void (const std::string&)> EventCallback;
		void add_event_callback(EventCallback listener);
	private:
		void init(const std::string& player_name, int red, int green, int blue, int radius);

		// Player graphical representation properties
		static const int kOutlineThickness = 5;
		static const int kNameOffset = 50;
		// Player physics properties
		static const int kPlayerMaximumSpeed = 200;
		static const int kPlayerMovementForce = 50;
		static const int kPlayerMass = 2;

		sf::String name_;
		sf::Color color_;
		sf::Shape circle_;
		int radius_;
		sf::Vector2f velocity_;
		sf::Vector2f acceleration_;

		std::vector<EventCallback> event_callbacks_;
		void broadcast_message(const std::string& message);
	};
}

#endif
