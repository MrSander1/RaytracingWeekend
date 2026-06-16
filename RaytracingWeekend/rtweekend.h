#ifndef rtweekend_h
#define rtweekend_h


#include <SFML/Graphics.hpp>

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <memory>

using std::make_shared;
using std::shared_ptr;

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

inline double degrees_to_radians(double degrees) {
	return degrees * pi / 180.0;
}

inline double random_double() {
	// Returns a random real in [0,1).
	return std::rand() / (RAND_MAX + 1.0);
}
inline double random_double(double min, double max) {
	// Returns a random real in [min,max).
	return min + (max - min) * random_double();
}

inline sf::Vector3<double> random() {
	return sf::Vector3<double> {random_double(), random_double(), random_double()};
}

inline sf::Vector3<double> random(double min, double max) {
	return sf::Vector3<double> {random_double(min, max), random_double(min, max), random_double(min, max)};
}

inline sf::Vector3<double> random_unit_vector() {
	while (true) {
		auto p = random(-1.0, 1.0);
		auto lensq = p.lengthSquared();
		if (1e-160 < lensq && lensq <= 1) {
			return p / sqrt(lensq);
		}
	}
}

inline sf::Vector3<double> random_on_hemisphere(const sf::Vector3<double>& normal) {
	sf::Vector3<double> on_unit_sphere = random_unit_vector();
	if (on_unit_sphere.dot(normal) > 0.0) { // In the same hemisphere as the normal
		return on_unit_sphere;
	}
	else {
		return -on_unit_sphere;
	}
}

inline sf::Vector3<double> reflect(const sf::Vector3<double>& v, const sf::Vector3<double>& n) {
	return v - (2.0 * v.dot(n) * n);
}

inline sf::Vector3<double> refract(const sf::Vector3<double>& uv, const sf::Vector3<double>& n, double etai_over_etat) {
	auto cos_theta = std::fmin(-uv.dot(n), 1.0);
	sf::Vector3<double> r_out_perp = etai_over_etat * (uv + cos_theta * n);
	sf::Vector3<double> r_out_parallel = -std::sqrt(std::fabs(1.0 - r_out_perp.lengthSquared())) * n;
	return r_out_perp + r_out_parallel;
}

inline bool near_zero(const sf::Vector3<double>& vec) {
	// Return true if the vector is close to zero in all dimensions.
	auto s = 1e-8;
	return (std::fabs(vec.x < s) && std::fabs(vec.y < s) && std::fabs(vec.z < s));
}

inline sf::Vector3<double> random_in_unit_disk() {
	while (true) {
		auto p = sf::Vector3<double>(random_double(-1.0, 1.0), random_double(-1.0, 1.0), 0.0);
		if (p.lengthSquared() < 1) {
			return p;
		}
	}
}



#include "ray.h"
#include "interval.h"

#endif