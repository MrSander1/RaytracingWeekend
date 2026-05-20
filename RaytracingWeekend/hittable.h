#ifndef hittable_h
#define hittable_h

#include "rtweekend.h"

class hit_record {
public: 
	sf::Vector3<double> p;
	sf::Vector3<double> normal;
	double t;
	bool front_face;

	void set_face_normal(const ray& r, const sf::Vector3<double> outward_normal) {
		front_face = r.direction().dot(outward_normal) < 0;
		normal = front_face ? outward_normal : -outward_normal;
	}
	
};

class hittable {
public:
	virtual ~hittable() = default;

	virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const = 0;
};

#endif