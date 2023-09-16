#include "app.hpp"

#include "raylib.h"
#include "raymath.h"
#include "rlImGui.h"
#include "imgui.h"
#include "spdlog/spdlog.h"

App::App() {
    this->input = InputController(this);
    this->gui_context = GUIContext(this);
    this->gui_command_palette = GUICommandPalette(this, &this->gui_context);
}

void App::init() {
    // TODO: make configurable
    spdlog::set_level(spdlog::level::trace);
    // Raylib window setup
    // TODO: remember window size and position
    RaylibInitWindow(1280, 720, "Unilevel");
    RaylibSetWindowMinSize(640, 480);
    RaylibSetWindowState(RAYLIB_FLAG_WINDOW_RESIZABLE);
    RaylibSetExitKey(RAYLIB_KEY_NULL); // TODO uncomment
    RaylibSetTargetFPS(60); // TODO: make configurable
    // ImGui setup
    rlImGuiSetup(true);
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags = ImGuiConfigFlags_NavNoCaptureKeyboard; // ?
    // Initialize components
    this->gui_context.init(); // Loads fonts
    this->gui_command_palette.init();
}

bool App::done() {
    return RaylibWindowShouldClose();
}

void App::update() {
    RaylibBeginDrawing();
    rlImGuiBegin();
    this->input.update();
    this->gui_command_palette.update();
    RaylibClearBackground(RaylibColor{32, 24, 24});
    ImGui::PushFont(this->gui_context.font_normal);
    ImGui::TextColored(
        ImVec4(RaylibIsKeyDown(RAYLIB_KEY_TAB) ? 0.1 : 0.9, 0.9, 0.9, 1),
        "Hello world"
    );
    ImGui::PopFont();
    this->gui_command_palette.draw();
    rlImGuiEnd();
    RaylibEndDrawing();
}

int App::conclude() {
    rlImGuiShutdown();
    RaylibCloseWindow();
    return 0;
}

int App::main() {
    this->init();
    while(!this->done()) {
        this->update();
    }
    return this->conclude();
}
