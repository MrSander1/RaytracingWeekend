#include "rtweekend.h"

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

sf::Vector3<double> ray_color(const ray& r, const hittable& world) {
    hit_record rec;
    if (world.hit(r, interval(0, infinity), rec)) {
        return 0.5 * (rec.normal + sf::Vector3<double>(1.0, 1.0, 1.0));
    }

    sf::Vector3<double> unit_direction = r.direction().normalized();
    auto a = 0.5 * (unit_direction.y + 1.0);
    return (1.0 - a) * sf::Vector3<double>(1.0, 1.0, 1.0) + a * sf::Vector3<double>(0.5, 0.7, 1.0);
}

int main() {

    // Image 
    auto aspect_ratio = 16.0 / 9.0;
    int image_width = 400;

    int image_height = int(image_width / aspect_ratio);
    image_height = (image_height < 1) ? 1 : image_height;

    // World

    hittable_list world;

    world.add(make_shared<sphere>(sf::Vector3<double>(0.0, 0.0, -1.0), 0.5));
    world.add(make_shared<sphere>(sf::Vector3<double>(0.0, -100.5, -1.0), 100.0));

    
    // Camera

    auto focal_length = 1.0;
    auto viewport_height = 2.0;
    auto viewport_width = viewport_height * (double(image_width) / image_height);
    auto camera_center = sf::Vector3(0.0, 0.0, 0.0);

    // Calculate the vectors across the horizontal and down the vertical viewport edges  (problem section, double type changes could be funky)
    auto viewport_u = sf::Vector3(viewport_width, 0.0, 0.0);
    auto viewport_v = sf::Vector3(0.0, -viewport_height, 0.0);


    // Calculate the horizontal and vertical delta vectors from pixel to pixel.
    auto pixel_delta_u = viewport_u / (double)image_width;
    auto pixel_delta_v = viewport_v / (double)image_height;

    // Calculate the location of the upper left pixel.
    auto viewport_upper_left = camera_center
                             - sf::Vector3(0.0, 0.0, (double)focal_length) - viewport_u/2.0 - viewport_v/2.0;
    auto pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);




    sf::RenderWindow window(sf::VideoMode({ (unsigned int)image_width, (unsigned int)image_height }), "Ray Tracing in One Weekend");

    // 1. Create a texture and a sprite to display the pixels
    sf::Texture texture;
    if (!texture.resize({ (unsigned int)image_width, (unsigned int)image_height })) return -1;
    sf::Sprite sprite(texture);

    // 2. Create a pixel buffer (Width * Height * 4 components: RGBA)
    std::vector<std::uint8_t> pixels(image_width * image_height * 4);

    // 3. The "Output an Image" logic from Chapter 2
    for (int j = 0; j < image_height; j++) {
        for (int i = 0; i < image_width; i++) {
            auto pixel_center = pixel00_loc + ((double)i * pixel_delta_u) + ((double)j * pixel_delta_v);
            auto ray_direction = pixel_center - camera_center;

            ray x(camera_center, ray_direction);

            sf::Vector3 pixel_color = ray_color(x, world);

            auto r = pixel_color.x;
            auto g = pixel_color.y;
            auto b = pixel_color.z;

            int ir = int(255.999 * r);
            int ig = int(255.999 * g);
            int ib = int(255.999 * b);

            // Calculate index in our 1D pixel array
            int index = (j * image_width + i) * 4;
            pixels[index + 0] = ir;   // Red
            pixels[index + 1] = ig;   // Green
            pixels[index + 2] = ib;   // Blue
            pixels[index + 3] = 255;  // Alpha (Full Opacity)
        }
    }

    // Update texture once
    texture.update(pixels.data());

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