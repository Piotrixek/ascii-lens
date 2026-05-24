#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <print>
#include <mdspan>
#include <memory>
#include <string_view>
#include <expected>
#include <cstdlib>
#include <algorithm>

namespace ascii {


constexpr std::string_view palette = " .:-=+*#%@";


constexpr int max_console_width = 100;

std::expected<void, std::string_view> render_image(const char* filepath) {
    int width{}, height{}, channels{};
    
    
    unsigned char* raw_data = stbi_load(filepath, &width, &height, &channels, 1);
    
    if (!raw_data) {
        return std::unexpected("Failed to decode the image file via stb");
    }
    
    
    std::unique_ptr<unsigned char, decltype(&stbi_image_free)> image_data(raw_data, stbi_image_free);
    
    
    std::mdspan<unsigned char, std::dextents<size_t, 2>> image_view(image_data.get(), height, width);
    
    
    size_t step_x = std::max<size_t>(1, width / max_console_width);
    size_t step_y = std::max<size_t>(1, step_x * 2); 
    
    
    for (size_t y = 0; y < image_view.extent(0); y += step_y) {
        for (size_t x = 0; x < image_view.extent(1); x += step_x) {
            
            
            unsigned char luminance = image_view[y, x];
            
            size_t palette_idx = (luminance * (palette.size() - 1)) / 255;
            std::print("{}", palette[palette_idx]);
        }
        std::println(); 
    }
    
    return {};
}

} 

int main(int argc, char** argv) {
    
    if (argc != 2) {
        std::println(stderr, "Usage: {} <image_path>", argv[0]);
        return EXIT_FAILURE;
    }
    
    
    if (auto result = ascii::render_image(argv[1]); !result) {
        std::println(stderr, "Error: {}", result.error());
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}