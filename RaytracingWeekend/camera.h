#ifndef CAMERA_H
#define CAMERA_H 

#include "hittable.h"
#include "material.h"

class camera {
public:
	// should be empty

	double aspect_ratio = 1.0; // Ratio of image width over height
	int image_width = 100; // Rendered image width in pixel count
	int samples_per_pixel = 10; // Count of random samples for each pixel
	int max_depth = 10; // Maximum number of ray bounces into scene

	double vfov = 90;  // Vertical view angle (field of view)
	sf::Vector3<double> lookfrom = sf::Vector3<double>(0.0, 0.0, 0.0); // Point camera is looking from
	sf::Vector3<double> lookat = sf::Vector3<double>(0.0, 0.0, -1.0); // Point camera is looking at
	sf::Vector3<double> vup = sf::Vector3<double>(0.0, 1.0, 0.0); // Camera-relative "up" direction

	double defocus_angle = 0; // Variation angle of rays through each pixel
	double focus_dist = 10;    // Distance from camera lookfrom point to plane of perfect focus

	std::vector<std::uint8_t> pixels;

	const int& getHeight() const  {
		return image_height;
	}

	/* Public Camera Parameters Here */

	void init() {
		initialize();
	}

	void update (const hittable& world) {

		std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

		for (int j = 0; j < image_height; j++) {
			std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
			for (int i = 0; i < image_width; i++) {
				auto pixel_center = pixel00_loc + ((double)i * pixel_delta_u) + ((double)j * pixel_delta_v);
				auto ray_direction = pixel_center - center;

				ray a(center, ray_direction);

				sf::Vector3 pixel_color(0.0,0.0,0.0);

				for (int sample = 0; sample < samples_per_pixel; sample++) {
					ray r = get_ray(i, j);
					pixel_color += ray_color(r, max_depth, world);
				}

				auto r = pixel_color.x;
				auto g = pixel_color.y;
				auto b = pixel_color.z;

				auto scale = 1.0 / samples_per_pixel;
				r = sqrt(r * scale);
				g = sqrt(g * scale);
				b = sqrt(b * scale);

				// Translate the [0,1] component values to the byte range [0,255].

				static const interval intensity(0.000, 0.999);
				int ir = int(256 * intensity.clamp(r));
				int ig = int(256 * intensity.clamp(g));
				int ib = int(256 * intensity.clamp(b));

				// Calculate index in our 1D pixel array
				int index = (j * image_width + i) * 4;
				pixels[index + 0] = ir;   // Red
				pixels[index + 1] = ig;   // Green
				pixels[index + 2] = ib;   // Blue
				pixels[index + 3] = 255;  // Alpha (Full Opacity)
			}
		}

		std::clog << "\rDone.                 \n";
	}

private:
	/* Private Camera Variables Here*/
	int image_height; // Rendered image height
	double pixel_samples_scale; // Color scale factor for a sum of pixel samples
	sf::Vector3<double> center; // Camera center
	sf::Vector3<double> pixel00_loc; // Location of pixel 0, 0
	sf::Vector3<double> pixel_delta_u; // Offset to pixel to the right
	sf::Vector3<double> pixel_delta_v; // Offset to pixel below
	sf::Vector3<double> u, v, w; // Camera frame basis vectors
	sf::Vector3<double> defocus_disk_u; // Defocus disk horizontal radius
	sf::Vector3<double> defocus_disk_v; // Defocus disk vertical radius

	void initialize() {

		image_height = int(image_width / aspect_ratio);
		image_height = (image_height < 1) ? 1 : image_height;

		pixels.resize(image_width * image_height * 4);

		pixel_samples_scale = 1.0 / samples_per_pixel;


		center = lookfrom;

		// Determine viewport dimensions
		auto theta = degrees_to_radians(vfov);
		auto h = std::tan(theta / 2.0);
		auto viewport_height = 2.0 * h * focus_dist;
		auto viewport_width = viewport_height * (double(image_width) / image_height);

		// Calculate the u,v,w unit basis vectors for the camera coordinate frame.
		w = (lookfrom - lookat).normalized();
		u = vup.cross(w).normalized();
		v = w.cross(u);

		// Calculate the vectors across the horizontal and down the vertical viewport edges.
		auto viewport_u = u * viewport_width; // Vector across viewport horizontal edge
		auto viewport_v = viewport_height * -v; // Vector down viewport vertical edge

		pixel_delta_u = viewport_u / (double)image_width;
		pixel_delta_v = viewport_v / (double)image_height;

		auto viewport_upper_left = center - (focus_dist * w) - viewport_u / 2.0 - viewport_v / 2.0;
		pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

		// Calculate the camera defocus disk basis vectors.
		auto defocus_radius = focus_dist * std::tan(degrees_to_radians(defocus_angle / 2.0));
		defocus_disk_u = u * defocus_radius;
		defocus_disk_v = v * defocus_radius;

		
	}

	ray get_ray(int i, int j) const {
		// Construct a camera ray originating from the defocus disk and directed at a randomly
		// sampled point around the pixel location i, j.

		auto offset = sample_squared();
		auto pixel_sample = pixel00_loc
			+ (((double)i + offset.x) * pixel_delta_u)
		    + (((double)j + offset.y) * pixel_delta_v);

		auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
		auto ray_direction = pixel_sample - ray_origin;

		return ray(ray_origin, ray_direction);
	}

	sf::Vector3<double> sample_squared() const {
		return sf::Vector3<double> {random_double() - 0.5, random_double() - 0.5, 0.0};
	}

	sf::Vector3<double> defocus_disk_sample() const {
		auto p = random_in_unit_disk();
		return center + (p.x * defocus_disk_u) + (p.y * defocus_disk_v);
	}


	sf::Vector3<double> ray_color(const ray& r, int depth, const hittable& world) const {
		// If we've exceeded the ray bounce limit, no more light is gathered.
		if (depth <= 0) {
			return sf::Vector3<double>(0.0, 0.0, 0.0);
		}

		hit_record rec;

		if (world.hit(r, interval(0.001, infinity), rec)) {
			ray scattered;
			sf::Vector3<double> attenuation;
			if (rec.mat->scatter(r, rec, attenuation, scattered)) {
				return attenuation.componentWiseMul(ray_color(scattered, depth - 1, world));
			}
			return sf::Vector3<double>(0.0, 0.0, 0.0);
		}

		sf::Vector3<double> unit_direction = r.direction().normalized();
		auto a = 0.5 * (unit_direction.y + 1.0);
		return (1.0 - a) * sf::Vector3<double>(1.0, 1.0, 1.0) + a * sf::Vector3<double>(0.5, 0.7, 1.0);
	}
};

#endif

