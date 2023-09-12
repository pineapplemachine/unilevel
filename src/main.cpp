#include <iostream>

#include "config/freetype.h"
#include "config/imgui.h"
#include "config/raylib.h"

#include "app.hpp"

App app;

int main(int argc, char **argv) {
    std::cout << "Hello world";
    app = App();
    return app.main();
}
