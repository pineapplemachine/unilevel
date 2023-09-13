#pragma once

#include "gui/command_palette.hpp"
#include "gui/context.hpp"
#include "input/controller.hpp"

class App {
public:
    InputController input;
    GUIContext gui_context;
    GUICommandPalette gui_command_palette;
    
    App();
    
    // Initialize the application.
    void init();
    // Returns true when the application should exit.
    bool done();
    // Runs once per frame.
    void update();
    // Runs as the application exits.
    int conclude();
    // Handy way to call `init`, `done`, `update`, and `conclude`.
    int main();
};
