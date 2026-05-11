#include <SFML/Graphics.hpp>
#include <vector>
#include <optional>

/*
Create a Pixel Buffer: Use a std::vector<std::uint8_t> to hold your image data (4 bytes per pixel for RGBA).
Translate Raytracing Math to Buffer: Instead of std::cout, you calculate the index in your vector and store the color there.
Update the Texture: Send the vector data to an sf::Texture every frame
*/

int main() {
    const int image_width = 256;
    const int image_height = 256;

    sf::RenderWindow window(sf::VideoMode({ image_width, image_height }), "Ray Tracing in One Weekend");

    // 1. Create a texture and a sprite to display the pixels
    sf::Texture texture;
    if (!texture.resize({ (unsigned int)image_width, (unsigned int)image_height })) return -1;
    sf::Sprite sprite(texture);

    // 2. Create a pixel buffer (Width * Height * 4 components: RGBA)
    std::vector<std::uint8_t> pixels(image_width * image_height * 4);

    // 3. The "Output an Image" logic from Chapter 2
    for (int j = 0; j < image_height; j++) {
        for (int i = 0; i < image_width; i++) {
            auto r = double(i) / (image_width - 1);
            auto g = double(j) / (image_height - 1);
            auto b = 0.0;

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