#include "controller.hpp"

#ifdef DEBUG
#include <iostream>
#endif

void InputAction_NoCallback(InputAction* action) {}

void InputController::update() {
    #ifdef DEBUG
    if(ImGui::IsKeyPressed((ImGuiKey) InputKey_X)) {
        std::cout << "This ptr: " << (long long int) this << "\n";
        std::cout << "App ptr: " << (long long int) this->app << "\n";
        std::cout << "Actions: " << this->actions.size() << "\n";
        std::cout << "Keybinds: " << this->action_key_binds.size() << "\n";
    }
    #endif
    InputContext context = InputContext_General; // TODO
    // for(auto bind : this->action_key_binds) {
    for(int i = 0; i < this->action_key_binds.size(); ++i) {
        auto bind = this->action_key_binds[i];
        #ifdef DEBUG
        if(ImGui::IsKeyDown((ImGuiKey) InputKey_Z)) {
            std::cout << "? " << this->get_action_name(bind.action).c_str() << "\n";
        }
        #endif
        bool active = (
            // (bind.action->context & context) != 0 &&
            this->is_key_state(bind.key_state, bind.key)
        );
        this->set_action_active(bind.action, active);
        #ifdef DEBUG
        if(bind.key_state == InputKeyState_Pressed) {
            std::cout << "Action activated by key press:";
            std::cout << this->get_action_name(bind.action).c_str() << "\n";
        }
        else if(bind.key_state == InputKeyState_Released) {
            std::cout << "Action activated by key release:";
            std::cout << this->get_action_name(bind.action).c_str() << "\n";
        }
        else if(bind.key_state == InputKeyState_Down && this->is_key_pressed(bind.key)) {
            std::cout << "Action activated by key down:";
            std::cout << this->get_action_name(bind.action).c_str() << "\n";
        }
        #endif
    }
}

InputActionHandle InputController::add_action(InputAction action) {
    auto handle = this->actions.size();
    this->actions.push_back(action);
    return (InputActionHandle) handle;
}

InputActionKeyBindHandle InputController::add_action_key_bind(
    InputActionKeyBind bind
) {
    auto handle = this->action_key_binds.size();
    this->action_key_binds.push_back(bind);
    #ifdef DEBUG
    std::cout << "This ptr: " << (long long int) this << "\n";
    std::cout << "App ptr: " << (long long int) this->app << "\n";
    std::cout << "Added a keybind. Total: " << this->action_key_binds.size() << "\n";
    #endif
    return (InputActionKeyBindHandle) handle;
}

InputAction* InputController::get_action(
    InputActionHandle handle
) {
    return &this->actions.at(handle);
}

std::string InputController::get_action_name(
    InputActionHandle handle
) {
    return this->actions.at(handle).name;
}

InputContext InputController::get_action_context(
    InputActionHandle handle
) {
    return this->actions.at(handle).context;
}

InputActionKeyBind* InputController::get_action_key_bind(
    InputActionKeyBindHandle handle
) {
    return &this->action_key_binds.at(handle);
}

bool InputController::is_action_active(InputActionHandle handle) {
    return this->actions.at(handle).active;
}

void InputController::set_action_active(InputActionHandle handle, bool active) {
    auto action = &this->actions.at(handle);
    action->active = active;
    if(action) {
        action->active_callback(action);
    }
}

InputContext InputController::get_current_context() {
    if(this->context_stack.size() <= 0) {
        return InputContext_None;
    }
    else {
        return this->context_stack.back();
    }
}

void InputController::push_context(InputContext context) {
    this->context_stack.push_back(context);
}

void InputController::pop_context(InputContext context) {
    if(this->context_stack.size() > 0 &&
        this->context_stack.back() == context
    ) {
        this->context_stack.pop_back();
    }
}

bool InputController::is_ctrl_key_down() {
    return (
        (this->ctrl_use_normal && ImGui::IsKeyDown(ImGuiMod_Ctrl)) ||
        (this->ctrl_use_super && ImGui::IsKeyDown(ImGuiMod_Super))
    );
}

bool InputController::is_shift_key_down() {
    return ImGui::IsKeyDown(ImGuiMod_Shift);
}

bool InputController::is_alt_key_down() {
    return ImGui::IsKeyDown(ImGuiMod_Alt);
}

bool InputController::is_modifier_key_down(InputModifierKey modifier) {
    if(modifier == InputModifierKey_Any) {
        return true;
    }
    return (
        ((modifier & InputModifierKey_Ctrl) != 0 ?
            this->is_ctrl_key_down() :
            !this->is_ctrl_key_down()
        ) &&
        ((modifier & InputModifierKey_Shift) != 0 ?
            this->is_shift_key_down() :
            !this->is_shift_key_down()
        ) &&
        ((modifier & InputModifierKey_Alt) != 0 ?
            this->is_alt_key_down() :
            !this->is_alt_key_down()
        )
    );
}

bool InputController::is_key_down(InputKey key) {
    ImGuiIO& io = ImGui::GetIO();
    if(io.WantCaptureKeyboard) {
        return false;
    }
    return ImGui::IsKeyDown((ImGuiKey) key);
}

bool InputController::is_key_down(InputModifiedKey modified_key) {
    return (
        this->is_modifier_key_down(modified_key.modifier) &&
        this->is_key_down(modified_key.key)
    );
}

bool InputController::is_key_pressed(InputKey key) {
    ImGuiIO& io = ImGui::GetIO();
    if(io.WantCaptureKeyboard) {
        return false;
    }
    return ImGui::IsKeyPressed((ImGuiKey) key);
}

bool InputController::is_key_pressed(InputModifiedKey modified_key) {
    return (
        this->is_modifier_key_down(modified_key.modifier) &&
        this->is_key_pressed(modified_key.key)
    );
}

bool InputController::is_key_released(InputKey key) {
    ImGuiIO& io = ImGui::GetIO();
    if(io.WantCaptureKeyboard) {
        return false;
    }
    return ImGui::IsKeyReleased((ImGuiKey) key);
}

bool InputController::is_key_released(InputModifiedKey modified_key) {
    return (
        this->is_modifier_key_down(modified_key.modifier) &&
        this->is_key_released(modified_key.key)
    );
}

bool InputController::is_key_state(
    InputKeyState state, InputKey key
) {
    ImGuiIO& io = ImGui::GetIO();
    // TODO: make toggleable depending on action/keybind?
    if(io.WantCaptureKeyboard) {
        return false;
    }
    switch(state) {
        case InputKeyState_Down: {
            return ImGui::IsKeyDown((ImGuiKey) key);
        }
        case InputKeyState_Pressed: {
            return ImGui::IsKeyPressed((ImGuiKey) key);
        }
        case InputKeyState_Released: {
            return ImGui::IsKeyReleased((ImGuiKey) key);
        }
        default: {
            return false;
        }
    }
}

bool InputController::is_key_state(
    InputKeyState state, InputModifiedKey modified_key
) {
    return (
        this->is_modifier_key_down(modified_key.modifier) &&
        this->is_key_state(state, modified_key.key)
    );
}
