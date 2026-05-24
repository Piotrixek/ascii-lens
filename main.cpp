#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <print>
#include <mdspan>
#include <memory>
#include <string_view>
#include <expected>
#include <cstdlib>
#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#endif

namespace ascii {

constexpr std::string_view palette = " .:-=+*#%@";

constexpr int max_console_width = 100;

std::expected<void, std::string_view> render_image(const char* filepath) {
    int width{}, height{}, channels{};
    
    unsigned char* raw_data = stbi_load(filepath, &width, &height, &channels, 3);
    
    if (!raw_data) {
        return std::unexpected("Failed to decode the image file via stb");
    }
    
    std::unique_ptr<unsigned char, decltype(&stbi_image_free)> image_data(raw_data, stbi_image_free);
    
    std::mdspan<unsigned char, std::dextents<size_t, 3>> image_view(image_data.get(), height, width, 3);
    
    size_t step_x = std::max<size_t>(1, width / max_console_width);
    size_t step_y = std::max<size_t>(1, step_x * 2);
    
    for (size_t y = 0; y < image_view.extent(0); y += step_y) {
        for (size_t x = 0; x < image_view.extent(1); x += step_x) {
            
            unsigned char r = image_view[y, x, 0];
            unsigned char g = image_view[y, x, 1];
            unsigned char b = image_view[y, x, 2];
            
            unsigned char luminance = static_cast<unsigned char>(0.2126f * r + 0.7152f * g + 0.0722f * b);
            
            size_t palette_idx = (luminance * (palette.size() - 1)) / 255;
            
            std::print("\x1b[38;2;{};{};{}m{}", r, g, b, palette[palette_idx]);
        }
        std::println("\x1b[0m"); 
    }
    
    return {};
}

}

void enable_vt_mode() {
#ifdef _WIN32
    HANDLE h_out = GetStdHandle(STD_OUTPUT_HANDLE);
    if (h_out == INVALID_HANDLE_VALUE) return;
    
    DWORD dw_mode = 0;
    if (!GetConsoleMode(h_out, &dw_mode)) return;
    
    dw_mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(h_out, dw_mode);
#endif
}

int main(int argc, char** argv) {
    enable_vt_mode();

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
