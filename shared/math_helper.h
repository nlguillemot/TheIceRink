#pragma once

#include <SFML/System/Vector2.hpp>

namespace circles
{
	template<class T>
	T clamp(T value, T minimum, T maximum)
	{
		if(value < minimum)
		{
			return minimum;
		}
		else if(value > maximum)
		{
			return maximum;
		}
		else
		{
			return value;
		}
	}

	template<class T>
	sf::Vector2<T> clamp_Vector2(sf::Vector2<T> vector, T minimum, T maximum)
	{
		float magnitude = sqrt(vector.x * vector.x + vector.y * vector.y);
		sf::Vector2<T> normalized_vector = vector/magnitude;

		if(magnitude < minimum)
		{
			return normalized_vector * minimum;
		}
		else if(magnitude > maximum)
		{
			return normalized_vector * maximum;
		}
		else
		{
			return vector;
		}
	}

}
