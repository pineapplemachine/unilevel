#pragma once

#include <functional>
#include <string>
#include <vector>

#include "imgui.h"

class App; // Forward declaration for App from app.hpp

typedef ImGuiKey InputKey;

typedef int InputKeyState;
enum InputKeyState_ {
    InputKeyState_None = 0,
    InputKeyState_Down = 1,
    InputKeyState_Pressed = 2,
    InputKeyState_Released = 3,
    InputKeyState_COUNT
};

typedef int InputModifierKey;
enum InputModifierKey_ {
    InputModifierKey_None = 0x00,
    InputModifierKey_Ctrl = 0x01,
    InputModifierKey_Shift = 0x02,
    InputModifierKey_CtrlShift = 0x03,
    InputModifierKey_Alt = 0x04,
    InputModifierKey_CtrlAlt = 0x05,
    InputModifierKey_ShiftAlt = 0x06,
    InputModifierKey_CtrlShiftAlt = 0x07,
    InputModifierKey_Any = -1
};

typedef int InputContext;
enum InputContext_ {
    // Input is not recognized in any context.
    InputContext_None = 0x00,
    // Default/general editor view.
    InputContext_General = 0x01,
    // Active when navigating a 3D view using mouselook.
    InputContext_Mouselook = 0x02,
    // Active when interacting with GUI elements, e.g. text inputs.
    InputContext_GUI = 0x04,
    // Active when interacting with the command palette.
    InputContext_CommandPalette = 0x08,
    // Input is recognized in any and all contexts.
    InputContext_All = 0xffffffff
};

struct InputModifiedKey {
    InputKey key = ImGuiKey_None;
    InputModifierKey modifier = InputModifierKey_None;
};

const InputModifiedKey InputModifiedKey_None = (
    InputModifiedKey{ImGuiKey_None, InputModifierKey_None}
);

struct InputAction;
void InputAction_NoCallback(InputAction* action);

struct InputAction {
    // Readable and uniquely identifying name for this action
    std::string name;
    // Action is registered during any of the given contexts.
    InputContext context = InputContext_All;
    // Callback function to run when the input occurs or is occurring.
    std::function<void(InputAction* action)> active_callback = &InputAction_NoCallback;
    // Set by the InputController.
    // Check this to see if the input occurred/is occurring during
    // the current frame.
    bool active = false;
};

struct InputActionKeyBind {
    InputModifiedKey key;
    InputKeyState key_state;
    InputAction* action;
};

class InputController {
public:
    InputController() {};
    InputController(App* app): app(app) {};
    
    App* app = nullptr;
    
    // When true, left and right Ctrl keys count as
    // Ctrl modifier keys.
    bool ctrl_use_normal = true;
    // When true, left and right super keys count as
    // Ctrl modifier keys. (Cmd keys on MacOS.)
    bool ctrl_use_super = false;
    
    // TODO: use eastl::fixed_vector?
    std::vector<InputContext> context_stack;
    //
    std::vector<InputAction> actions;
    //
    std::vector<InputActionKeyBind> action_key_binds;
    // Handle inputs and actions. Should be run at the beginning
    // of the application's main loop.
    void update();
    
    
    
    // InputKeyBind ui_key_left = InputKeyBind(
    //     InputModifiedKey(ImGuiKey_LeftArrow)
    // );
    // InputKeyBind ui_key_right = InputKeyBind(
    //     InputModifiedKey(ImGuiKey_RightArrow)
    // );
    // InputKeyBind ui_key_up = InputKeyBind(
    //     InputModifiedKey(ImGuiKey_UpArrow)
    // );
    // InputKeyBind ui_key_down = InputKeyBind(
    //     InputModifiedKey(ImGuiKey_DownArrow)
    // );
    // InputKeyBind ui_key_line_home = InputKeyBind(
    //     InputModifiedKey(ImGuiKey_LeftArrow, InputModifierKey_Ctrl)
    // );
    // InputKeyBind ui_key_line_end = InputKeyBind(
    //     InputModifiedKey(ImGuiKey_RightArrow, InputModifierKey_Ctrl)
    // );
    // InputKeyBind ui_key_page_up = InputKeyBind(
    //     InputModifiedKey(ImGuiKey_PageUp)
    // );
    // InputKeyBind ui_key_page_down = InputKeyBind(
    //     InputModifiedKey(ImGuiKey_PageDown)
    // );
    // InputKeyBind ui_key_home = InputKeyBind(
    //     InputModifiedKey(ImGuiKey_Home),
    //     InputModifiedKey(ImGuiKey_UpArrow, InputModifierKey_Ctrl)
    // );
    // InputKeyBind ui_key_end = InputKeyBind(
    //     InputModifiedKey(ImGuiKey_End),
    //     InputModifiedKey(ImGuiKey_DownArrow, InputModifierKey_Ctrl)
    // );
    // InputKeyBind ui_key_enter = InputKeyBind(
    //     InputModifiedKey(ImGuiKey_Enter)
    // );
    // InputKeyBind ui_key_back = InputKeyBind(
    //     InputModifiedKey(ImGuiKey_Backspace)
    // );
    // InputKeyBind ui_key_escape = InputKeyBind(
    //     InputModifiedKey(ImGuiKey_Escape, InputModifierKey_Any)
    // );
    // InputKeyBind ui_key_next_item = InputKeyBind(
    //     InputModifiedKey(ImGuiKey_Tab)
    // );
    // InputKeyBind ui_key_prev_item = InputKeyBind(
    //     InputModifiedKey(ImGuiKey_Tab, InputModifierKey_Shift)
    // );
    
    void add_action(InputAction action);
    void add_action_key_bind(InputActionKeyBind bind);
    
    bool is_ctrl_key_down();
    bool is_shift_key_down();
    bool is_alt_key_down();
    bool is_modifier_key_down(InputModifierKey modifier);
    bool is_key_down(InputKey key);
    bool is_key_down(InputModifiedKey modified_key);
    bool is_key_pressed(InputKey key);
    bool is_key_pressed(InputModifiedKey modified_key);
    bool is_key_released(InputKey key);
    bool is_key_released(InputModifiedKey modified_key);
    bool is_key_state(InputKeyState state, InputKey key);
    bool is_key_state(InputKeyState state, InputModifiedKey modified_key);
};

// input.capture();
