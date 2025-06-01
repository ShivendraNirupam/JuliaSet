#include <SDL2/SDL.h>
#include <complex>
#include <iostream>
#include <string>
#include <cmath>
#include <vector>

constexpr int WIDTH = 1920;
constexpr int HEIGHT = 1080;

// Map a value from one range to another
double map(double value, double start1, double stop1, double start2, double stop2) {
    return start2 + (stop2 - start2) * ((value - start1) / (stop1 - start1));
}

class JuliaSet {
public:
    JuliaSet(int width, int height)
        : width_(width), height_(height), max_iter_(300), radius_(2.0),
          c_{0.282, -0.58}, pixels_(width * height, 0), update_c_(false) {}

    // Calculate iterations to escape
    int iterations(std::complex<double> z) const {
        int i = 0;
        while (std::abs(z) <= radius_ && i < max_iter_) {
            z = z * z + c_;
            ++i;
        }
        return i;
    }

    // Render the Julia set pixels
    void render() {
        for (int px = 0; px < width_; ++px) {
            for (int py = 0; py < height_; ++py) {
                // Map pixel to complex plane coordinates
                double x0 = map(px, 0, width_, -radius_, radius_);
                double y0 = map(py, 0, height_, -radius_ * height_ / width_, radius_ * height_ / width_);
                std::complex<double> z(x0, y0);

                int iter = iterations(z);

                Uint8 r = 0, g = 0, b = 0;
                if (iter < max_iter_) {
                    double t = static_cast<double>(iter) / max_iter_;
                    r = static_cast<Uint8>(9 * (1 - t) * t * t * t * 255);
                    g = static_cast<Uint8>(15 * (1 - t) * (1 - t) * t * t * 255);
                    b = static_cast<Uint8>(8.5 * (1 - t) * (1 - t) * (1 - t) * t * 255);
                }
                pixels_[py * width_ + px] = (255 << 24) | (r << 16) | (g << 8) | b;
            }
        }
    }

    // Update parameter c based on mouse position
    void update_c(int mouse_x, int mouse_y) {
        double real_part = map(mouse_x, 0, width_, -1.0, 1.0);
        double imag_part = map(mouse_y, 0, height_, -1.0, 1.0);
        c_ = std::complex<double>(real_part, imag_part);
    }

    // Get pixel buffer pointer for SDL_UpdateTexture
    const Uint32* pixels() const { return pixels_.data(); }

    // Get current c as string
    std::string c_string() const {
        char buffer[64];
        snprintf(buffer, sizeof(buffer), "c = %.3f %+.3fi", c_.real(), c_.imag());
        return std::string(buffer);
    }

    void set_update_c(bool flag) { update_c_ = flag; }
    bool updating_c() const { return update_c_; }

private:
    int width_, height_;
    int max_iter_;
    double radius_;
    std::complex<double> c_;
    std::vector<Uint32> pixels_;
    bool update_c_;
};

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << "\n";
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Julia Set SDL2 C++",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          WIDTH, HEIGHT,
                                          SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << "\n";
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1,
                                                SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << "\n";
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_Texture* texture = SDL_CreateTexture(renderer,
                                             SDL_PIXELFORMAT_ARGB8888,
                                             SDL_TEXTUREACCESS_STREAMING,
                                             WIDTH, HEIGHT);
    if (!texture) {
        std::cerr << "SDL_CreateTexture Error: " << SDL_GetError() << "\n";
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    JuliaSet julia(WIDTH, HEIGHT);

    bool quit = false;
    SDL_Event event;

    while (!quit) {
        // Event handling
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    quit = true;
                    break;
                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_ESCAPE || event.key.keysym.sym == SDLK_1) {
                        quit = true;
                    } else if (event.key.keysym.sym == SDLK_SPACE) {
                        julia.set_update_c(true);
                    }
                    break;
                case SDL_MOUSEMOTION:
                    if (julia.updating_c()) {
                        julia.update_c(event.motion.x, event.motion.y);
                    }
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if (julia.updating_c()) {
                        julia.set_update_c(false);
                    }
                    break;
                default:
                    break;
            }
        }

        julia.render();

        // Update texture pixels
        SDL_UpdateTexture(texture, nullptr, julia.pixels(), WIDTH * sizeof(Uint32));

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);

        // Set window title with current c
        SDL_SetWindowTitle(window, julia.c_string().c_str());

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
