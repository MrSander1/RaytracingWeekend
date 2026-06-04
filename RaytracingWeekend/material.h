#ifndef MATERIAL_H
#define MATERIAL_H

#include "hittable.h"

class material {
public:
	virtual ~material() = default;

	virtual bool scatter(
		const ray& r_in, const hit_record& rec, sf::Vector3<double>& attenuation, ray& scattered
	) const {
		return false;
	}
};

class lambertian : public material {
public:
	lambertian(const sf::Vector3<double>& albedo) : albedo(albedo) {}

	bool scatter(const ray& r_in, const hit_record& rec, sf::Vector3<double>& attenuation, ray& scattered)
		const override {
			auto scatter_direction = rec.normal + random_unit_vector();

			// Catch degenerate scatter direction
			if (near_zero(scatter_direction)) {
				scatter_direction = rec.normal;
			}

			scattered = ray(rec.p, scatter_direction);
			attenuation = albedo;
			return true;
	}

private: 
	sf::Vector3<double> albedo;
};

class metal : public material {
public:
	metal (const sf::Vector3<double>& albedo, double fuzz) : albedo(albedo), fuzz(fuzz < 1 ? fuzz: 1) {}

	bool scatter(const ray& r_in, const hit_record& rec, sf::Vector3<double>& attenuation, ray& scattered)
		const override {
		sf::Vector3<double> reflected = reflect(r_in.direction(), rec.normal);
		reflected = reflected.normalized() + (fuzz * random_unit_vector());
		scattered = ray(rec.p, reflected);
		attenuation = albedo;
		return (scattered.direction().dot(rec.normal) > 0);
	}

private: 
	sf::Vector3<double> albedo;
	double fuzz;
};


#endif