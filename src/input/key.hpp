#pragma once

#include <string>

#include "imgui.h"

/**
 * Enumeration of key states that may trigger an InputAction
 * via an InputActionKeyBind.
 */
enum InputKeyState : int {
    // Action is never triggered.
    InputKeyState_None = 0,
    // Action is triggered every frame that the key is held down.
    InputKeyState_Down = 1,
    // Action is triggered as the key is pressed down.
    InputKeyState_Pressed = 2,
    // Action is triggered as the key is released from a down state.
    InputKeyState_Released = 3,
    InputKeyState_COUNT
};

/**
 * Enumeration of modifier keys and modifier key combinations,
 * e.g. Shift or Ctrl+Shift.
 */
enum InputModifierKey : int {
    // No modifiers.
    InputModifierKey_None = 0x00,
    // Ctrl (and/or Super, depending on InputController config)
    InputModifierKey_Ctrl = 0x01,
    // Shift
    InputModifierKey_Shift = 0x02,
    // Ctrl/Super and Shift
    InputModifierKey_CtrlShift = 0x03,
    // Alt
    InputModifierKey_Alt = 0x04,
    // Ctrl and Alt
    InputModifierKey_CtrlAlt = 0x05,
    // Shift and Alt
    InputModifierKey_ShiftAlt = 0x06,
    // Ctrl/Super, Shift, and Alt
    InputModifierKey_CtrlShiftAlt = 0x07,
    // All modifier keys together: Ctrl/Super, Shift, and Alt
    InputModifierKey_All = InputModifierKey_CtrlShiftAlt,
    // Special case. Input is recognized for any combination of modifiers.
    InputModifierKey_Any = -1
};

enum InputKey : int {
    InputKey_None = ImGuiKey_None,
    InputKey_Tab = ImGuiKey_Tab,
    InputKey_LeftArrow = ImGuiKey_LeftArrow,
    InputKey_RightArrow = ImGuiKey_RightArrow,
    InputKey_UpArrow = ImGuiKey_UpArrow,
    InputKey_DownArrow = ImGuiKey_DownArrow,
    InputKey_PageUp = ImGuiKey_PageUp,
    InputKey_PageDown = ImGuiKey_PageDown,
    InputKey_Home = ImGuiKey_Home,
    InputKey_End = ImGuiKey_End,
    InputKey_Insert = ImGuiKey_Insert,
    InputKey_Delete = ImGuiKey_Delete,
    InputKey_Backspace = ImGuiKey_Backspace,
    InputKey_Space = ImGuiKey_Space,
    InputKey_Enter = ImGuiKey_Enter,
    InputKey_Escape = ImGuiKey_Escape,
    InputKey_LeftCtrl = ImGuiKey_LeftCtrl,
    InputKey_LeftShift = ImGuiKey_LeftShift,
    InputKey_LeftAlt = ImGuiKey_LeftAlt,
    InputKey_LeftSuper = ImGuiKey_LeftSuper,
    InputKey_RightCtrl = ImGuiKey_RightCtrl,
    InputKey_RightShift = ImGuiKey_RightShift,
    InputKey_RightAlt = ImGuiKey_RightAlt,
    InputKey_RightSuper = ImGuiKey_RightSuper,
    InputKey_Menu = ImGuiKey_Menu,
    InputKey_0 = ImGuiKey_0,
    InputKey_1 = ImGuiKey_1,
    InputKey_2 = ImGuiKey_2,
    InputKey_3 = ImGuiKey_3,
    InputKey_4 = ImGuiKey_4,
    InputKey_5 = ImGuiKey_5,
    InputKey_6 = ImGuiKey_6,
    InputKey_7 = ImGuiKey_7,
    InputKey_8 = ImGuiKey_8,
    InputKey_9 = ImGuiKey_9,
    InputKey_A = ImGuiKey_A,
    InputKey_B = ImGuiKey_B,
    InputKey_C = ImGuiKey_C,
    InputKey_D = ImGuiKey_D,
    InputKey_E = ImGuiKey_E,
    InputKey_F = ImGuiKey_F,
    InputKey_G = ImGuiKey_G,
    InputKey_H = ImGuiKey_H,
    InputKey_I = ImGuiKey_I,
    InputKey_J = ImGuiKey_J,
    InputKey_K = ImGuiKey_K,
    InputKey_L = ImGuiKey_L,
    InputKey_M = ImGuiKey_M,
    InputKey_N = ImGuiKey_N,
    InputKey_O = ImGuiKey_O,
    InputKey_P = ImGuiKey_P,
    InputKey_Q = ImGuiKey_Q,
    InputKey_R = ImGuiKey_R,
    InputKey_S = ImGuiKey_S,
    InputKey_T = ImGuiKey_T,
    InputKey_U = ImGuiKey_U,
    InputKey_V = ImGuiKey_V,
    InputKey_W = ImGuiKey_W,
    InputKey_X = ImGuiKey_X,
    InputKey_Y = ImGuiKey_Y,
    InputKey_Z = ImGuiKey_Z,
    InputKey_F1 = ImGuiKey_F1,
    InputKey_F2 = ImGuiKey_F2,
    InputKey_F3 = ImGuiKey_F3,
    InputKey_F4 = ImGuiKey_F4,
    InputKey_F5 = ImGuiKey_F5,
    InputKey_F6 = ImGuiKey_F6,
    InputKey_F7 = ImGuiKey_F7,
    InputKey_F8 = ImGuiKey_F8,
    InputKey_F9 = ImGuiKey_F9,
    InputKey_F10 = ImGuiKey_F10,
    InputKey_F11 = ImGuiKey_F11,
    InputKey_F12 = ImGuiKey_F12,
    InputKey_Apostrophe = ImGuiKey_Apostrophe,
    InputKey_Comma = ImGuiKey_Comma,
    InputKey_Minus = ImGuiKey_Minus,
    InputKey_Period = ImGuiKey_Period,
    InputKey_Slash = ImGuiKey_Slash,
    InputKey_Semicolon = ImGuiKey_Semicolon,
    InputKey_Equal = ImGuiKey_Equal,
    InputKey_LeftBracket = ImGuiKey_LeftBracket,
    InputKey_Backslash = ImGuiKey_Backslash,
    InputKey_RightBracket = ImGuiKey_RightBracket,
    InputKey_GraveAccent = ImGuiKey_GraveAccent,
    InputKey_CapsLock = ImGuiKey_CapsLock,
    InputKey_ScrollLock = ImGuiKey_ScrollLock,
    InputKey_NumLock = ImGuiKey_NumLock,
    InputKey_PrintScreen = ImGuiKey_PrintScreen,
    InputKey_Pause = ImGuiKey_Pause,
    InputKey_Keypad0 = ImGuiKey_Keypad0,
    InputKey_Keypad1 = ImGuiKey_Keypad1,
    InputKey_Keypad2 = ImGuiKey_Keypad2,
    InputKey_Keypad3 = ImGuiKey_Keypad3,
    InputKey_Keypad4 = ImGuiKey_Keypad4,
    InputKey_Keypad5 = ImGuiKey_Keypad5,
    InputKey_Keypad6 = ImGuiKey_Keypad6,
    InputKey_Keypad7 = ImGuiKey_Keypad7,
    InputKey_Keypad8 = ImGuiKey_Keypad8,
    InputKey_Keypad9 = ImGuiKey_Keypad9,
    InputKey_KeypadDecimal = ImGuiKey_KeypadDecimal,
    InputKey_KeypadDivide = ImGuiKey_KeypadDivide,
    InputKey_KeypadMultiply = ImGuiKey_KeypadMultiply,
    InputKey_KeypadSubtract = ImGuiKey_KeypadSubtract,
    InputKey_KeypadAdd = ImGuiKey_KeypadAdd,
    InputKey_KeypadEnter = ImGuiKey_KeypadEnter,
    InputKey_KeypadEqual = ImGuiKey_KeypadEqual,
    InputKey_GamepadStart = ImGuiKey_GamepadStart,
    InputKey_GamepadBack = ImGuiKey_GamepadBack,
    InputKey_GamepadFaceLeft = ImGuiKey_GamepadFaceLeft,
    InputKey_GamepadFaceRight = ImGuiKey_GamepadFaceRight,
    InputKey_GamepadFaceUp = ImGuiKey_GamepadFaceUp,
    InputKey_GamepadFaceDown = ImGuiKey_GamepadFaceDown,
    InputKey_GamepadDpadLeft = ImGuiKey_GamepadDpadLeft,
    InputKey_GamepadDpadRight = ImGuiKey_GamepadDpadRight,
    InputKey_GamepadDpadUp = ImGuiKey_GamepadDpadUp,
    InputKey_GamepadDpadDown = ImGuiKey_GamepadDpadDown,
    InputKey_GamepadL1 = ImGuiKey_GamepadL1,
    InputKey_GamepadR1 = ImGuiKey_GamepadR1,
    InputKey_GamepadL2 = ImGuiKey_GamepadL2,
    InputKey_GamepadR2 = ImGuiKey_GamepadR2,
    InputKey_GamepadL3 = ImGuiKey_GamepadL3,
    InputKey_GamepadR3 = ImGuiKey_GamepadR3,
    InputKey_GamepadLStickLeft = ImGuiKey_GamepadLStickLeft,
    InputKey_GamepadLStickRight = ImGuiKey_GamepadLStickRight,
    InputKey_GamepadLStickUp = ImGuiKey_GamepadLStickUp,
    InputKey_GamepadLStickDown = ImGuiKey_GamepadLStickDown,
    InputKey_GamepadRStickLeft = ImGuiKey_GamepadRStickLeft,
    InputKey_GamepadRStickRight = ImGuiKey_GamepadRStickRight,
    InputKey_GamepadRStickUp = ImGuiKey_GamepadRStickUp,
    InputKey_GamepadRStickDown = ImGuiKey_GamepadRStickDown,
    InputKey_MouseLeft = ImGuiKey_MouseLeft,
    InputKey_MouseRight = ImGuiKey_MouseRight,
    InputKey_MouseMiddle = ImGuiKey_MouseMiddle,
    InputKey_MouseX1 = ImGuiKey_MouseX1,
    InputKey_MouseX2 = ImGuiKey_MouseX2,
    InputKey_MouseWheelX = ImGuiKey_MouseWheelX,
    InputKey_MouseWheelY = ImGuiKey_MouseWheelY
};

struct InputModifiedKey {
    InputKey key = InputKey_None;
    InputModifierKey modifier = InputModifierKey_None;
};

const InputModifiedKey InputModifiedKey_None = (
    InputModifiedKey{InputKey_None, InputModifierKey_None}
);

// Given an InputKeyState value, get a name string.
const char* InputKeyState_GetName(InputKeyState state);

// Given an InputKey value, get a name string.
const char* InputKey_GetName(InputKey key);

// Given a key name string, get an InputKey value.
InputKey InputKey_GetFromName(std::string name);

// Given an InputModifierKey value, get a name string.
const char* InputModifierKey_GetName(InputModifierKey modifier);

// Given a modifier key name string, get an InputModifierKey value.
// Recognizes "Ctrl", "Shift", and "Alt".
// Does not recognize modifier key combinations.
InputModifierKey InputModifierKey_GetFromName(std::string name);

// Given a key string e.g. "Enter", "Escape", "Ctrl+S", "Ctrl+Shift+Z"
// get an InputModifiedKey struct representing that key combination.
InputModifiedKey InputModifiedKey_Parse(std::string text);

// Given a key with modifier, get a string representing that key
// combination in a readable way, e.g. "Ctrl+Shift+Z".
std::string InputModifiedKey_ToString(InputModifiedKey& key);

const char* const InputKeyState_Unknown_Name = "[Unknown]";

const char* const InputKeyState_Names[] = {
    "Never",
    "Down",
    "Pressed",
    "Released",
};

const char* const InputModifierKey_None_Name = "";
const char* const InputModifierKey_Ctrl_Name = "Ctrl";
const char* const InputModifierKey_Shift_Name = "Shift";
const char* const InputModifierKey_Alt_Name = "Alt";
const char* const InputModifierKey_Any_Name = "Any";
const char* const InputModifierKey_Separator = "+";

const char* const InputModifierKey_Names[] = {
    "",
    "Ctrl",
    "Shift",
    "Ctrl+Shift",
    "Alt",
    "Ctrl+Alt",
    "Shift+Alt",
    "Ctrl+Shift+Alt",
};

const char* const InputKey_None_Name = "";
const char* const InputKey_Unknown_Name = "[Unknown]";

// TODO: provide multiple names for parsing to recognize
// e.g. "Left Arrow" and "LeftArrow"
const char* const InputKey_Names[] = {
    "Tab",
    "LeftArrow",
    "RightArrow",
    "UpArrow",
    "DownArrow",
    "PageUp",
    "PageDown",
    "Home",
    "End",
    "Insert",
    "Delete",
    "Backspace",
    "Space",
    "Enter",
    "Escape",
    "LeftCtrl",
    "LeftShift",
    "LeftAlt",
    "LeftSuper",
    "RightCtrl",
    "RightShift",
    "RightAlt",
    "RightSuper",
    "Menu",
    "0",
    "1",
    "2",
    "3",
    "4",
    "5",
    "6",
    "7",
    "8",
    "9",
    "A",
    "B",
    "C",
    "D",
    "E",
    "F",
    "G",
    "H",
    "I",
    "J",
    "K",
    "L",
    "M",
    "N",
    "O",
    "P",
    "Q",
    "R",
    "S",
    "T",
    "U",
    "V",
    "W",
    "X",
    "Y",
    "Z",
    "F1",
    "F2",
    "F3",
    "F4",
    "F5",
    "F6",
    "F7",
    "F8",
    "F9",
    "F10",
    "F11",
    "F12",
    "Apostrophe",
    "Comma",
    "Minus",
    "Period",
    "Slash",
    "Semicolon",
    "Equal",
    "LeftBracket",
    "Backslash",
    "RightBracket",
    "GraveAccent",
    "CapsLock",
    "ScrollLock",
    "NumLock",
    "PrintScreen",
    "Pause",
    "Keypad0",
    "Keypad1",
    "Keypad2",
    "Keypad3",
    "Keypad4",
    "Keypad5",
    "Keypad6",
    "Keypad7",
    "Keypad8",
    "Keypad9",
    "KeypadDecimal",
    "KeypadDivide",
    "KeypadMultiply",
    "KeypadSubtract",
    "KeypadAdd",
    "KeypadEnter",
    "KeypadEqual",
    "GamepadStart",
    "GamepadBack",
    "GamepadFaceLeft",
    "GamepadFaceRight",
    "GamepadFaceUp",
    "GamepadFaceDown",
    "GamepadDpadLeft",
    "GamepadDpadRight",
    "GamepadDpadUp",
    "GamepadDpadDown",
    "GamepadL1",
    "GamepadR1",
    "GamepadL2",
    "GamepadR2",
    "GamepadL3",
    "GamepadR3",
    "GamepadLStickLeft",
    "GamepadLStickRight",
    "GamepadLStickUp",
    "GamepadLStickDown",
    "GamepadRStickLeft",
    "GamepadRStickRight",
    "GamepadRStickUp",
    "GamepadRStickDown",
    "MouseLeft",
    "MouseRight",
    "MouseMiddle",
    "MouseX1",
    "MouseX2",
    "MouseWheelX",
    "MouseWheelY"
};
