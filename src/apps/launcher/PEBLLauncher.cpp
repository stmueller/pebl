// PEBLLauncher.cpp - Main entry point for PEBL Launcher
// Copyright (c) 2026 Shane T. Mueller
// Licensed under GPL
//
// =============================================================================
// PORTABLE/STANDALONE MODE DETECTION
// =============================================================================
//
// The launcher supports two operating modes:
//
// 1. INSTALLED MODE (default)
//    - Workspace is created in Documents/pebl-exp.2.3/
//    - First run triggers resource copying dialog (demos, tutorials, docs)
//    - Standard installation via installer or package manager
//
// 2. PORTABLE/STANDALONE MODE
//    - Workspace is the current working directory
//    - No first-run dialog (resources already in place)
//    - Designed for USB drives or self-contained distributions
//
// PORTABLE MODE DETECTION (in priority order):
//
//   1. STANDALONE.txt marker file
//      - Check for ./STANDALONE.txt, ../STANDALONE.txt, or ../../STANDALONE.txt
//      - Most explicit and reliable trigger
//      - Batch file can create this before launching
//
//   2. PORTABLE.txt marker file
//      - Check for ./PORTABLE.txt, ../PORTABLE.txt, or ../../PORTABLE.txt
//      - Legacy support
//
//   3. PEBL_PORTABLE environment variable
//      - Set PEBL_PORTABLE=1 before launching
//      - Useful for batch files
//
// NOTE: No automatic detection based on directory structure. Only explicit
// marker files or environment variable will trigger portable mode.
//
// RECOMMENDED STANDALONE DIRECTORY STRUCTURE:
//
//   /MyPEBLDistribution/
//     runme.bat              <- Batch file to launch (sets CWD, runs launcher)
//     STANDALONE.txt         <- Marker file (REQUIRED for portable mode)
//     PEBL/
//       bin/
//         pebl-launcher.exe
//         pebl2.exe
//         *.dll
//       battery/
//       pebl-lib/
//       media/
//     my_studies/            <- User's studies (in CWD, not inside PEBL/)
//     snapshots/
//
// EXAMPLE runme.bat:
//
//   @echo off
//   cd /d "%~dp0"
//   REM Optional: create marker file for explicit detection
//   echo. > STANDALONE.txt
//   PEBL\bin\pebl-launcher.exe
//   REM Optional: clean up marker file
//   del STANDALONE.txt
//
// This ensures:
//   - Running via batch file -> portable mode (marker file created)
//   - Double-clicking exe directly -> installed mode (no marker file present)
//
// See WorkspaceManager::IsPortableMode() for implementation details.
// =============================================================================

#include "imgui.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_sdlrenderer2.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

#include "LauncherUI.h"
#include "LauncherConfig.h"

int main(int, char**)
{
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

    // Load configuration first (need window size)
    LauncherConfig config;
    config.LoadConfig();

    // Setup window with HiDPI support
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Window* window = SDL_CreateWindow(
        "PEBL Experiment Launcher",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        config.GetWindowWidth(),
        config.GetWindowHeight(),
        window_flags
    );

    if (window == nullptr)
    {
        printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
        return -1;
    }

    // Create renderer with VSync
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr)
    {
        printf("Error creating SDL_Renderer: %s\n", SDL_GetError());
        return -1;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

    // Set imgui.ini path to settings directory (must be static to persist)
    static std::string imguiIniPath;
    {
        fs::path settingsDir = fs::path(config.GetWorkspacePath()) / "settings";
        try {
            fs::create_directories(settingsDir);
        } catch (...) {}
        imguiIniPath = (settingsDir / "imgui.ini").string();
        io.IniFilename = imguiIniPath.c_str();
    }

    // Set font size from configuration
    ImFontConfig fontConfig;
    fontConfig.SizePixels = config.GetFontSize();
    io.Fonts->AddFontDefault(&fontConfig);

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);

    // Initialize launcher UI
    LauncherUI launcherUI(&config, renderer);

    // Main loop
    bool done = false;
    while (!done)
    {
        // Poll and handle events
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
            if (event.type == SDL_WINDOWEVENT &&
                event.window.event == SDL_WINDOWEVENT_CLOSE &&
                event.window.windowID == SDL_GetWindowID(window))
                done = true;
        }

        // Start the Dear ImGui frame
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        // Render launcher UI
        launcherUI.Render(&done);

        // Rendering
        ImGui::Render();
        SDL_RenderSetScale(renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
        SDL_SetRenderDrawColor(renderer, 45, 45, 48, 255);
        SDL_RenderClear(renderer);
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
        SDL_RenderPresent(renderer);
    }

    // Save window size to configuration before exit
    int windowWidth, windowHeight;
    SDL_GetWindowSize(window, &windowWidth, &windowHeight);
    config.SetWindowWidth(windowWidth);
    config.SetWindowHeight(windowHeight);
    config.SaveConfig();

    // Cleanup
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
