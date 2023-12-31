#pragma once

#include <functional>
#include <string>
#include <vector>

#include "imgui.h"

#include "key.hpp"

class App; // Forward declaration for App from app.hpp

/**
 * Enumeration of recognized input contexts.
 * An InputAction will only trigger when the InputController's
 * current context is a match fpr the action's context bitmask
 * property.
 */
enum InputContext : int {
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
    InputContext_All = -1
};

// Forward declaration for InputAction_NoCallback.
struct InputAction;
// Default callback for InputAction instances. Does nothing.
void InputAction_NoCallback(InputAction* action);

struct InputAction {
    // Readable and uniquely identifying name for this action
    std::string name;
    // Action is registered during any of the given contexts.
    InputContext context = InputContext_All;
    // Callback function to run when the input occurs or is occurring.
    std::function<void(InputAction* action)> active_callback = &InputAction_NoCallback;
    // State flag set by the InputController.
    // Check this to see if the input occurred/is occurring during
    // the current frame.
    bool active = false;
};

typedef int InputActionHandle;
const InputActionHandle InputActionHandle_None = -1;

struct InputActionKeyBind {
    InputActionHandle action;
    InputModifiedKey key;
    InputKeyState key_state;
    std::string key_name;
    
    InputActionKeyBind(
        InputActionHandle action,
        InputModifiedKey key,
        InputKeyState key_state = InputKeyState_Pressed
    );
    InputActionKeyBind(
        InputActionHandle action,
        std::string key_name,
        InputKeyState key_state = InputKeyState_Pressed
    );
    
    InputActionKeyBind(
        InputActionHandle action,
        InputModifiedKey key,
        InputKeyState key_state,
        std::string key_name
    ):
        action(action),
        key(key),
        key_state(key_state),
        key_name(key_name)
    {};
};

typedef int InputActionKeyBindHandle;
const InputActionKeyBindHandle InputActionKeyBindHandle_None = -1;

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
    
    InputActionHandle add_action(InputAction action);
    InputActionKeyBindHandle add_action_key_bind(InputActionKeyBind bind);
    InputAction* get_action(InputActionHandle handle);
    std::string get_action_name(InputActionHandle handle);
    InputContext get_action_context(InputActionHandle handle);
    InputActionKeyBind* get_action_key_bind(InputActionKeyBindHandle handle);
    bool is_action_active(InputActionHandle handle);
    void activate_action(InputActionHandle handle);
    
    InputContext get_current_context();
    void push_context(InputContext context);
    void pop_context(InputContext context);
    
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
