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

class dielectric : public material {
public:
	dielectric(double refraction_index) : refraction_index(refraction_index) {}

	bool scatter(const ray& r_in, const hit_record& rec, sf::Vector3<double>& attenuation, ray& scattered)
	const override {
		attenuation = sf::Vector3<double>(1.0, 1.0, 1.0);
		double ri = rec.front_face ? (1.0 / refraction_index) : refraction_index;

		sf::Vector3<double> unit_direction = r_in.direction().normalized();
		double cos_theta = std::fmin(-unit_direction.dot(rec.normal), 1.0);
		double sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);

		bool cannot_refract = ri * sin_theta > 1.0;
		sf::Vector3<double> direction;

		if (cannot_refract) {
			direction = reflect(unit_direction, rec.normal);
		}

		else {
			direction = refract(unit_direction, rec.normal, ri);
		}

		scattered = ray(rec.p, direction);
		return true;
	}

private: 
	// Refractive index in vacuum or air, or the ratio of the material's refractive index over
    // the refractive index of the enclosing media
	double refraction_index;

	static double reflectance(double cosine, double refraction_index) {
		// Use Schlick's approximation for reflectance.
		auto r0 = (1 - refraction_index) / (1 + refraction_index);
		r0 = r0 * r0;
		return r0 + (1 - r0) * std::pow((1 - cosine), 5);
	}
};


#endif