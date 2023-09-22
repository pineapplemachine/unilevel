#pragma once

#include <functional>
#include <string>
#include <vector>

#include "context.hpp"
#include "input/controller.hpp"

// Forward declaration for GUICommandPaletteCommand_AlwaysActiveCallback.
struct GUICommandPaletteCommand;
// Default get_active_callback for GUICommandPaletteCommand instances.
// Always returns true.
bool GUICommandPaletteCommand_AlwaysActiveCallback(
    GUICommandPaletteCommand* command
);

// TODO get and show keyboard shortcuts?
// TODO array of aliases for easier search
struct GUICommandPaletteCommand {
    // Readable, uniquely identifying title for the command
    std::string name;
    // Brief help text explaining the command
    std::string summary;
    // Callback is invoked when the command is selected
    // from the palette.
    std::function<void()> activated_callback;
    // Get whether the command is currently usable or not.
    // If false, the result will be de-prioritized and will
    // appear grayed-out.
    std::function<bool(GUICommandPaletteCommand* command)> get_active_callback = (
        GUICommandPaletteCommand_AlwaysActiveCallback
    );
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

typedef int GUICommandPaletteCommandTime;

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
    bool input_text_submitted = false;
    bool input_text_modified = false;
    GUICommandPaletteCommandTime command_time = 0;
    std::vector<GUICommandPaletteCommand> commands;
    std::vector<GUICommandPaletteCommandTime> command_activated_times;
    std::vector<GUICommandPaletteResult> results;
    
    InputActionHandle action_show = InputActionHandle_None;
    InputActionHandle action_activate = InputActionHandle_None;
    InputActionHandle action_escape = InputActionHandle_None;
    InputActionHandle action_up = InputActionHandle_None;
    InputActionHandle action_down = InputActionHandle_None;
    InputActionHandle action_home = InputActionHandle_None;
    InputActionHandle action_end = InputActionHandle_None;
    
    // Initialize command palette data
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
    void activate_result(GUICommandPaletteResult& result);
    void update_results();
    void update_command_result(const int i);
};
