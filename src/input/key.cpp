#include "key.hpp"

#include <array>

const char* InputKeyState_GetName(InputKeyState state) {
    if(state < 0 || state >= std::size(InputKeyState_Names)) {
        return InputKeyState_Unknown_Name;
    }
    else {
        return InputKeyState_Names[state];
    }
}

const char* InputKey_GetName(InputKey key) {
    if(key == InputKey_None) {
        return InputKey_None_Name;
    }
    else if(key < InputKey_Tab) {
        return InputKey_Unknown_Name;
    }
    auto i = key - InputKey_Tab;
    if(i < 0 || i >= std::size(InputKey_Names)) {
        return InputKey_Unknown_Name;
    }
    else {
        return InputKey_Names[i];
    }
}

InputKey InputKey_GetFromName(std::string name) {
    // TODO: Make string comparisons case insensitive
    for(int i = 0; i < std::size(InputKey_Names); ++i) {
        if(name == InputKey_Names[i]) {
            return (InputKey) (InputKey_Tab + i);
        }
    }
    return InputKey_None;
}

const char* InputModifierKey_GetName(InputModifierKey modifier) {
    if(modifier == InputModifierKey_Any) {
        return InputModifierKey_Any_Name;
    }
    else {
        return InputKey_Names[modifier & InputModifierKey_All];
    }
}

InputModifierKey InputModifierKey_GetFromName(std::string name) {
    // TODO: Make string comparisons case insensitive
    if(name == InputModifierKey_Ctrl_Name) {
        return InputModifierKey_Ctrl;
    }
    else if(name == InputModifierKey_Shift_Name) {
        return InputModifierKey_Shift;
    }
    else if(name == InputModifierKey_Alt_Name) {
        return InputModifierKey_Alt;
    }
    else {
        return InputModifierKey_None;
    }
}

InputModifiedKey InputModifiedKey_Parse(std::string text) {
    int modifier = InputModifierKey_None;
    int i = 0;
    int i_begin = 0;
    while(i < text.size()) {
        if(text[i] == '+') {
            auto mod_name = text.substr(i_begin, i - i_begin);
            modifier |= InputModifierKey_GetFromName(mod_name);
            i_begin = i + 1;
        }
        i++;
    }
    auto key_name = text.substr(i_begin, i - i_begin);
    InputKey key = InputKey_GetFromName(key_name);
    return InputModifiedKey{key, (InputModifierKey) modifier};
}

std::string InputModifiedKey_ToString(InputModifiedKey& key) {
    auto key_name = InputKey_GetName(key.key);
    auto mod_name = InputModifierKey_GetName(key.modifier);
    if(mod_name == nullptr || *mod_name == '\0')  {
        return std::string(key_name);
    }
    else {
        return (
            std::string(mod_name) +
            std::string(InputModifierKey_Separator) +
            std::string(key_name)
        );
    }
}
