#include <SDL.h>
#include <iostream>

int main() {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << "\n";
        return 1;
    }

    std::cout << "Current audio driver: " << SDL_GetCurrentAudioDriver() << "\n\n";

    int count = SDL_GetNumAudioDrivers();
    std::cout << "Available audio drivers (" << count << "):\n";
    for (int i = 0; i < count; i++) {
        std::cout << "  " << i << ": " << SDL_GetAudioDriver(i) << "\n";
    }

    SDL_Quit();
    return 0;
}
