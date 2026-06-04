#ifndef sphere_h
#define sphere_h

#include "hittable.h"


class sphere : public hittable {
public: 
	sphere(const sf::Vector3<double>& center, double radius, shared_ptr<material> mat) : center(center), radius(std::fmax(0, radius)), mat(mat) {

	}

	bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
		sf::Vector3<double> oc = center - r.origin();
		auto a = r.direction().lengthSquared();
		auto h = r.direction().dot(oc);
		auto c = oc.lengthSquared() - (radius * radius);
		auto discriminant = h * h - a * c;

		if (discriminant < 0) {
			return false;
		}

		auto sqrtd = std::sqrt(discriminant);

		// Find the nearest root that lies in the acceptable range
		auto root = (h - sqrtd) / a;
		if (!ray_t.surrounds(root)) {
			root = (h + sqrtd) / a;
			if (!ray_t.surrounds(root)) {
				return false;
			}
		}

		rec.t = root;
		rec.p = r.at(rec.t);
		sf::Vector3<double> outward_normal = (rec.p - center) / radius;
		rec.set_face_normal(r, outward_normal);
		rec.mat = mat;

		return true;
	}

private:
	sf::Vector3<double> center;
	double radius;
	shared_ptr<material> mat;
};

#endif