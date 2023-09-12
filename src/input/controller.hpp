#pragma once

#include "imgui.h"

typedef ImGuiKey InputKey;

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
    InputModifierKey_Any = -1,
};

struct InputModifiedKey {
    InputKey key = ImGuiKey_None;
    InputModifierKey modifier = InputModifierKey_None;
};

const InputModifiedKey InputModifiedKey_None = (
    InputModifiedKey{ImGuiKey_None, InputModifierKey_None}
);

struct InputKeyBind {
    InputModifiedKey options[4];
    
    InputKeyBind(
        InputModifiedKey option_0
    ) {
        this->options[0] = option_0;
        this->options[1] = InputModifiedKey_None;
        this->options[2] = InputModifiedKey_None;
        this->options[3] = InputModifiedKey_None;
    }
    InputKeyBind(
        InputModifiedKey option_0,
        InputModifiedKey option_1
    ) {
        this->options[0] = option_0;
        this->options[1] = option_1;
        this->options[2] = InputModifiedKey_None;
        this->options[3] = InputModifiedKey_None;
    }
    InputKeyBind(
        InputModifiedKey option_0,
        InputModifiedKey option_1,
        InputModifiedKey option_2
    ) {
        this->options[0] = option_0;
        this->options[1] = option_1;
        this->options[2] = option_2;
        this->options[3] = InputModifiedKey_None;
    }
    InputKeyBind(
        InputModifiedKey option_0,
        InputModifiedKey option_1,
        InputModifiedKey option_2,
        InputModifiedKey option_3
    ) {
        this->options[0] = option_0;
        this->options[1] = option_1;
        this->options[2] = option_2;
        this->options[3] = option_3;
    }
};

class InputController {
public:
    // When true, left and right Ctrl keys count as
    // Ctrl modifier keys.
    bool ctrl_use_normal = true;
    // When true, left and right super keys count as
    // Ctrl modifier keys. (Cmd keys on MacOS.)
    bool ctrl_use_super = false;
    
    // TODO use eastl::fixed_vector
    // std::array<int, 16> context_stack;
    
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
    
    bool is_ctrl_key_down();
    bool is_shift_key_down();
    bool is_alt_key_down();
    bool is_modifier_key_down(InputModifierKey modifier);
    bool is_key_down(InputKey key);
    bool is_key_down(InputModifiedKey modified_key);
    bool is_key_down(InputKeyBind &key_bind);
    bool is_key_pressed(InputKey key);
    bool is_key_pressed(InputModifiedKey modified_key);
    bool is_key_pressed(InputKeyBind &key_bind);
    bool is_key_released(InputKey key);
    bool is_key_released(InputModifiedKey modified_key);
    bool is_key_released(InputKeyBind &key_bind);
};

// input.capture();
