#include "controller.hpp"

void InputAction_NoCallback(InputAction* action) {}

void InputController::add_action(InputAction action) {
    this->actions.push_back(action);
}

void InputController::add_action_key_bind(InputActionKeyBind bind) {
    this->action_key_binds.push_back(bind);
}

void InputController::update() {
    InputContext context = InputContext_General; // TODO
    for(auto bind : this->action_key_binds) {
        bool active = (
            (bind.action->context & context) != 0 &&
            this->is_key_state(bind.key_state, bind.key)
        );
        bind.action->active = active;
        if(active) {
            bind.action->active_callback(bind.action);
        }
    }
}

bool InputController::is_ctrl_key_down() {
    return (
        (this->ctrl_use_normal && (
            ImGui::IsKeyPressed(ImGuiKey_LeftCtrl) ||
            ImGui::IsKeyPressed(ImGuiKey_RightCtrl)
        )) ||
        (this->ctrl_use_super && (
            ImGui::IsKeyPressed(ImGuiKey_LeftSuper) ||
            ImGui::IsKeyPressed(ImGuiKey_RightSuper)
        ))
    );
}

bool InputController::is_shift_key_down() {
    return (
        ImGui::IsKeyPressed(ImGuiKey_LeftShift) ||
        ImGui::IsKeyPressed(ImGuiKey_RightShift)
    );
}

bool InputController::is_alt_key_down() {
    return (
        ImGui::IsKeyPressed(ImGuiKey_LeftAlt) ||
        ImGui::IsKeyPressed(ImGuiKey_RightAlt)
    );
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
    return ImGui::IsKeyDown(key);
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
    return ImGui::IsKeyPressed(key);
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
    return ImGui::IsKeyReleased(key);
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
            return ImGui::IsKeyDown(key);
        }
        case InputKeyState_Pressed: {
            return ImGui::IsKeyPressed(key);
        }
        case InputKeyState_Released: {
            return ImGui::IsKeyReleased(key);
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
