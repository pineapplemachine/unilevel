#pragma once

#include <functional>
#include <string>
#include <vector>

#include "context.hpp"
#include "input/controller.hpp"

// TODO get and show keyboard shortcuts?
// TODO array of aliases for easier search
struct GUICommandPaletteCommand {
    // Readable, uniquely identifying title for the command
    std::string title;
    // Brief help text explaining the command
    std::string summary;
    // Get whether the command is currently usable or not.
    // If false, the result will be de-prioritized and will
    // appear grayed-out.
    std::function<bool()> get_active_callback;
    // Callback is invoked when the command is selected
    // from the palette.
    std::function<void()> activated_callback;
};

// TODO: give better score to recently used commands
struct GUICommandPaletteResult {
    // Index of command in GUICommandPalette's commands list.
    int command;
    // Lower score numbers appear higher in the results list.
    int sort_score;
    // False for commands that are not available right now.
    bool active;
};

class GUICommandPalette {
public:
    GUICommandPalette() {};
    GUICommandPalette(App* app, GUIContext* context):
        app(app),
        context(context)
    {};
    
    App* app;
    GUIContext* context;
    GUIFont font = GUIFont_Normal;
    bool showing = false;
    bool show_init = false;
    int selected_result_index = 0;
    int hovered_result_index = -1;
    int pressed_result_index = -1;
    char input_text[1024] = {};
    std::vector<GUICommandPaletteCommand> commands;
    std::vector<GUICommandPaletteResult> results;
    
    InputActionHandle action_show = InputActionHandle_None;
    InputActionHandle action_activate = InputActionHandle_None;
    InputActionHandle action_escape = InputActionHandle_None;
    InputActionHandle action_up = InputActionHandle_None;
    InputActionHandle action_down = InputActionHandle_None;
    InputActionHandle action_home = InputActionHandle_None;
    InputActionHandle action_end = InputActionHandle_None;
    
    // 
    void init();
    // Show the command palette if it is not shown already
    void show();
    // Hide the command palette if it is being shown
    void hide();
    // Returns true if the command palette is currently visible
    bool is_showing();
    // Draw the command palette
    void draw();
    // Handle input
    void update();
    // Insert command into list, order alphabetically
    void add_command(GUICommandPaletteCommand command);
    // Get the command currently selected in the command
    // palette, or nullptr if there is no current selection.
    GUICommandPaletteCommand* get_selected_command();
    
    bool draw_result(
        int result_index,
        GUICommandPaletteResult result,
        const ImVec2& size
    );
    void activate_result(GUICommandPaletteResult result);
    void update_results();
};
