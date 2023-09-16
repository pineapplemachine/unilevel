#include "controller.hpp"

#include "spdlog/spdlog.h"

void InputAction_NoCallback(InputAction* action) {}

InputActionKeyBind::InputActionKeyBind(
    InputActionHandle action,
    InputModifiedKey key,
    InputKeyState key_state
) {
    this->action = action;
    this->key = key;
    this->key_state = key_state;
    this->key_name = InputModifiedKey_ToString(key);
}

InputActionKeyBind::InputActionKeyBind(
    InputActionHandle action,
    std::string key_name,
    InputKeyState key_state
) {
    this->action = action;
    this->key = InputModifiedKey_Parse(key_name);
    this->key_state = key_state;
    this->key_name = key_name;
}

void InputController::update() {
    InputContext context = InputContext_General; // TODO
    for(auto action : this->actions) {
        action.active = false;
    }
    for(auto bind : this->action_key_binds) {
        bool active = (
            (bind.action->context & context) != 0 &&
            this->is_key_state(bind.key_state, bind.key)
        );
        if(active) {
            this->activate_action(bind.action);
        }
        #ifdef DEBUG
        if(active && bind.key_state == InputKeyState_Pressed) {
            spdlog::trace(
                "Action '{}' activated by key press '{}'.",
                this->get_action_name(bind.action), bind.key_name
            );
        }
        else if(active && bind.key_state == InputKeyState_Released) {
            spdlog::trace(
                "Action '{}' activated by key release '{}'.",
                this->get_action_name(bind.action), bind.key_name
            );
        }
        else if(active && bind.key_state == InputKeyState_Down &&
            this->is_key_pressed(bind.key)
        ) {
            spdlog::trace(
                "Action '{}' activated by key down '{}'.",
                this->get_action_name(bind.action), bind.key_name
            );
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
    spdlog::debug(
        "Added key bind: Action '{}' bound to key '{}' {}.",
        this->get_action_name(bind.action),
        bind.key_name,
        InputKeyState_GetName(bind.key_state)
    );
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

void InputController::activate_action(InputActionHandle handle) {
    auto action = &this->actions.at(handle);
    if(!action->active) {
        action->active = true;
        if(action) {
            action->active_callback(action);
        }
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
