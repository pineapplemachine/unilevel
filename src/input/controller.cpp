#include "controller.hpp"

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

bool InputController::is_key_down(InputKeyBind &key_bind) {
    return (
        this->is_key_down(key_bind.options[0]) ||
        this->is_key_down(key_bind.options[1]) ||
        this->is_key_down(key_bind.options[2]) ||
        this->is_key_down(key_bind.options[3])
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

bool InputController::is_key_pressed(InputKeyBind &key_bind) {
    return (
        this->is_key_pressed(key_bind.options[0]) ||
        this->is_key_pressed(key_bind.options[1]) ||
        this->is_key_pressed(key_bind.options[2]) ||
        this->is_key_pressed(key_bind.options[3])
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

bool InputController::is_key_released(InputKeyBind &key_bind) {
    return (
        this->is_key_released(key_bind.options[0]) ||
        this->is_key_released(key_bind.options[1]) ||
        this->is_key_released(key_bind.options[2]) ||
        this->is_key_released(key_bind.options[3])
    );
}

// TODO
// bool InputController::update() {
//     const int i_begin = ImGuiKey_NamedKey_BEGIN;
//     const int i_end = ImGuiKey_NamedKey_END;
//     for(int i = i_begin; i < i_end; ++i) {
//     }
// }
