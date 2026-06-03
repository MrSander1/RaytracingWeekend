#include "rtweekend.h"

#include "camera.h"
#include "hittable.h"
#include "hittable_list.h"
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

    world.add(make_shared<sphere>(sf::Vector3<double>(0.0, 0.0, -1.0), 0.5));
    world.add(make_shared<sphere>(sf::Vector3<double>(0.0, -100.5, -1.0), 100.0));

    camera cam;

    cam.aspect_ratio = 16.0 / 9.0;

    cam.image_width = 400;

    cam.samples_per_pixel = 100;

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