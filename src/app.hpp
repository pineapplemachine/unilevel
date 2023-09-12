#pragma once

#include "gui/context.hpp"

class App {
public:
    GUIContext context;
    
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
