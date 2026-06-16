#include "rtweekend.h"

#include "camera.h"
#include "hittable.h"
#include "hittable_list.h"
#include "material.h"
#include "sphere.h"

#include <vector>
#include <optional>
#include <cmath>

/*
Create a Pixel Buffer: Use a std::vector<std::uint8_t> to hold your image data (4 bytes per pixel for RGBA).
Translate Raytracing Math to Buffer: Instead of std::cout, you calculate the index in your vector and store the color there.
Update the Texture: Send the vector data to an sf::Texture every frame
*/


// not the traditional issue probabbly something with gamma 
int main() {

    hittable_list world;

    auto ground_material = make_shared<lambertian>(sf::Vector3<double>(0.5, 0.5, 0.5));
    world.add(make_shared<sphere>(sf::Vector3<double>(0, -1000, 0), 1000, ground_material));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            sf::Vector3<double> center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

            if ((center - sf::Vector3<double>(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = random().componentWiseMul(random());
                    sphere_material = make_shared<lambertian>(albedo);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
                else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
                else {
                    // glass
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(sf::Vector3<double>(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<lambertian>(sf::Vector3<double>(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(sf::Vector3<double>(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(sf::Vector3<double>(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(sf::Vector3<double>(4, 1, 0), 1.0, material3));

    camera cam;

    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 1200;
    cam.samples_per_pixel = 500;
    cam.max_depth = 50;

    cam.vfov = 20;
    cam.lookfrom = sf::Vector3<double>(13, 2, 3);
    cam.lookat = sf::Vector3<double>(0, 0, 0);
    cam.vup = sf::Vector3<double>(0, 1, 0);

    cam.defocus_angle = 0.6;
    cam.focus_dist = 10.0;

    cam.init();

    sf::RenderWindow window(sf::VideoMode({ (unsigned int)cam.image_width, (unsigned int)cam.getHeight() }), "Ray Tracing in One Weekend");

    sf::Texture texture;
    if (!texture.resize({ (unsigned int)cam.image_width, (unsigned int)cam.getHeight()})) return -1;
    sf::Sprite sprite(texture);

    cam.update(world);

    texture.update(cam.pixels.data());

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) window.close();
        }

        window.clear();
        window.draw(sprite); // Draw the rendered pixel buffer
        window.display();
    }

    return 0;
}