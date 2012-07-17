#include "player.h"

#include <SFML/Window/Input.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include <sstream>

#include "constants.h"
#include "math_helper.h"


namespace circles
{
	Player::Player(const std::string& player_name, int red, int green, int blue, int radius)
	{
		init(player_name,red,green,blue,radius);
	}

	Player::Player(const circles::Configuration& config)
	{
		init(config.character_name(),config.red(),config.green(),config.blue(),config.radius());
	}

	void Player::init(const std::string& player_name, int red, int green, int blue, int radius)
	{
		sf::Vector2f starting_position((float)Constants::kWindowWidth/2 - radius,(float)Constants::kWindowHeight/2 - radius);
		color_ = sf::Color((sf::Uint8)red, (sf::Uint8)green, (sf::Uint8)blue);
	
		circle_ = sf::Shape::Circle(0,0, (float)radius, color_, (float)kOutlineThickness, sf::Color(0,0,0));
		circle_.SetPosition(starting_position);

		radius_ = radius;
		name_ = sf::String(player_name);
		name_.SetColor(sf::Color::Black);
	}

	Player::pointer Player::create(const std::string& player_name, int red, int green, int blue, int radius)
	{
		return Player::pointer(new Player(player_name,red,green,blue,radius));
	}
		
	Player::pointer Player::create(const circles::Configuration& config)
	{
		return Player::pointer(new Player(config));
	}

	sf::Color Player::color() const
	{
		return color_;
	}

	void Player::set_color(const sf::Color& c)
	{
		circle_.SetColor(c);
		color_ = c;
	}
	
	int Player::red() const
	{
		return color_.r;
	}

	int Player::green() const
	{
		return color_.g;
	}

	int Player::blue() const	
	{
		return color_.b;
	}

	sf::Shape Player::shape() const
	{
		return circle_;
	}

	int Player::radius() const
	{
		return radius_;
	}

	sf::String Player::name() const
	{
		return name_;
	}

	sf::Vector2f Player::position() const
	{
		return circle_.GetPosition();
	}

	void Player::set_position(const sf::Vector2f& p)
	{
		circle_.SetPosition(p.x, p.y);
		if(position().x < 0)
		{
			circle_.SetPosition(0, position().y);
			set_velocity(sf::Vector2f(0,velocity().y));
		}
		if(position().x > Constants::kWindowWidth)
		{
			circle_.SetPosition((float)Constants::kWindowWidth, position().y);
			set_velocity(sf::Vector2f(0,velocity().y));
		}
		if(position().y < 0)
		{
			circle_.SetPosition(position().x,0);
			set_velocity(sf::Vector2f(velocity().x,0));
		}
		if(position().y > Constants::kWindowHeight)
		{
			circle_.SetPosition(position().x, (float)Constants::kWindowHeight);
			set_velocity(sf::Vector2f(velocity().x,0));
		}

		name_.SetPosition(position().x - name_.GetRect().GetWidth()/2, position().y - (float)kNameOffset);
	}

	sf::Vector2f Player::velocity() const
	{
		return velocity_;
	}

	void Player::set_velocity(const sf::Vector2f& v)
	{
		velocity_ = v;
		clamp_Vector2<float>(velocity_, 0, (float)kPlayerMaximumSpeed);
	}

	sf::Vector2f Player::acceleration() const
	{
		return acceleration_;
	}

	void Player::set_acceleration(const sf::Vector2f& a)
	{
		acceleration_ = a;
	}

	void Player::update(float dt)
	{
		set_velocity( velocity() + acceleration() * dt );

		set_position( position() + velocity() * dt );
	}

	void Player::send_updated_position()
	{
		std::stringstream command;
		command << "UpdatePosition " << std::string(name_.GetText()) << " " << position().x << " " << position().y << std::endl;
		broadcast_message(command.str());
	}

	void Player::draw(sf::RenderWindow& window)
	{
		window.Draw(name_);
		window.Draw(circle_);
	}

	void Player::handle_input(sf::Key::Code code, bool pressed)
	{
		using namespace sf::Key;
		sf::Vector2f resultant_acceleration;

		sf::Vector2f old_acceleration = acceleration();

		switch(code)
		{
		case Up:
			if(pressed)
				resultant_acceleration.y = (float)-kPlayerMovementForce/kPlayerMass;
			else
				resultant_acceleration.y = 0;
			break;
		case Down:
			if(pressed)
				resultant_acceleration.y = (float)kPlayerMovementForce/kPlayerMass;
			else
				resultant_acceleration.y = 0;
			break;
		case Left:
			if(pressed)
				resultant_acceleration.x = (float)-kPlayerMovementForce/kPlayerMass;
			else
				resultant_acceleration.x = 0;
			break;
		case Right:
			if(pressed)
				resultant_acceleration.x = (float)kPlayerMovementForce/kPlayerMass;
			else
				resultant_acceleration.x = 0;
			break;
		default:
			break;
		}

		set_acceleration(resultant_acceleration);
		
		if(old_acceleration != acceleration())
		{
			std::stringstream command;
			command << "UpdateAcceleration " << std::string(name_.GetText()) << " " << acceleration().x << " " << acceleration().y << std::endl;
			broadcast_message(command.str());
		}
	}

	void Player::add_event_callback(EventCallback listener)
	{
		event_callbacks_.push_back(listener);
	}

	void Player::broadcast_message(const std::string& message)
	{
		for(unsigned i = 0; i < event_callbacks_.size(); ++i)
		{
			event_callbacks_[i](message);
		}
	}
}
