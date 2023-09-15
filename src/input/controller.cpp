#include "controller.hpp"

#ifdef DEBUG
#include <iostream>
#endif

void InputAction_NoCallback(InputAction* action) {}

void InputController::update() {
    #ifdef DEBUG
    if(ImGui::IsKeyPressed((ImGuiKey) InputKey_X)) {
        std::cout << "TEST";
    }
    #endif
    InputContext context = InputContext_General; // TODO
    for(auto bind : this->action_key_binds) {
        bool active = (
            (bind.action->context & context) != 0 &&
            this->is_key_state(bind.key_state, bind.key)
        );
        bind.action->active = active;
        #ifdef DEBUG
        if(bind.key_state == InputKeyState_Pressed) {
            std::cout << "Action activated by key press:";
            std::cout << bind.action->name << "\n";
        }
        else if(bind.key_state == InputKeyState_Released) {
            std::cout << "Action activated by key release:";
            std::cout << bind.action->name << "\n";
        }
        else if(bind.key_state == InputKeyState_Down && this->is_key_pressed(bind.key)) {
            std::cout << "Action activated by key down:";
            std::cout << bind.action->name << "\n";
        }
        #endif
        if(active) {
            bind.action->active_callback(bind.action);
        }
    }
}

InputAction* InputController::add_action(InputAction action) {
    this->actions.push_back(action);
    return &this->actions.back();
}

InputActionKeyBind* InputController::add_action_key_bind(
    InputActionKeyBind bind
) {
    this->action_key_binds.push_back(bind);
    return &this->action_key_binds.back();
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
