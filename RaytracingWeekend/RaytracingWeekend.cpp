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

    auto material_ground = make_shared<lambertian>(sf::Vector3<double>(0.8, 0.8, 0.0));
    auto material_center = make_shared<lambertian>(sf::Vector3<double>(0.1, 0.2, 0.5));
    auto material_left = make_shared<metal>(sf::Vector3<double>(0.8, 0.8, 0.8), 0.3);
    auto material_right = make_shared<metal>(sf::Vector3<double>(0.8, 0.6, 0.2), 1.0);

    world.add(make_shared<sphere>(sf::Vector3<double>(0.0, -100.5, -1.0), 100.0, material_ground));
    world.add(make_shared<sphere>(sf::Vector3<double>(0.0, 0.0, -1.2), 0.5, material_center));
    world.add(make_shared<sphere>(sf::Vector3<double>(-1.0, 0.0, -1.0), 0.5, material_left));
    world.add(make_shared<sphere>(sf::Vector3<double>(1.0, 0.0, -1.0), 0.5, material_right));

    camera cam;

    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;

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