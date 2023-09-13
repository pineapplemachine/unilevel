#include "app.hpp"

#include "raylib.h"
#include "raymath.h"
#include "rlImGui.h"
#include "imgui.h"

App::App() {
    this->input = InputController(this);
    this->gui_context = GUIContext(this);
    this->gui_command_palette = GUICommandPalette(this, &this->gui_context);
}

void App::init() {
    // Raylib window setup
    // TODO: remember window size and position
    InitWindow(1280, 720, "Unilevel");
    SetWindowMinSize(640, 480);
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetExitKey(KEY_NULL);
    SetTargetFPS(60); // TODO: make configurable
    // ImGui setup
    rlImGuiSetup(true);
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags = ImGuiConfigFlags_NavNoCaptureKeyboard; // ?
    // Initialize components
    this->gui_context.init(); // Loads fonts
    this->gui_command_palette.init();
}

bool App::done() {
    return WindowShouldClose();
}

void App::update() {
    this->input.update();
    BeginDrawing();
    ClearBackground(Color{32, 24, 24});
    rlImGuiBegin();
    // imgui goes here
    ImGui::PushFont(this->gui_context.font_normal);
    ImGui::TextColored(ImVec4(0.9, 0.9, 0.9, 1), "Hello world");
    ImGui::PopFont();
    rlImGuiEnd();
    EndDrawing();
}

int App::conclude() {
    rlImGuiShutdown();
    CloseWindow();
    return 0;
}

int App::main() {
    this->init();
    while(!this->done()) {
        this->update();
    }
    return this->conclude();
}
