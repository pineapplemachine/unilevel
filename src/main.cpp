#include <iostream>

#include "config/freetype.h"
#include "config/imgui.h"
#include "config/raylib.h"

#include "app.hpp"

int main(int argc, char **argv) {
    App app = App();
    return app.main();
}
