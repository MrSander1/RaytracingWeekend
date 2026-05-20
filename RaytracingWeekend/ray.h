#ifndef RAY_H
#define RAY_H

#include <SFML/Graphics.hpp>

class ray {
public: 
	ray() {}
	
	ray(const sf::Vector3<double> origin, const sf::Vector3<double> direction) : orig(origin), dir(direction) {}

	const sf::Vector3<double> origin() const { return orig; }
	const sf::Vector3<double> direction() const { return dir; }

	sf::Vector3<double> at(double t) const {
		return orig + (t * dir);
	}
private: 
	sf::Vector3<double> orig;
	sf::Vector3<double> dir;
};

#endif