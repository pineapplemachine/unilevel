#pragma once

#include "imgui.h"

typedef int GUIFontSize;
enum GUIFontSize_ {
    GUIFontSize_None,
    GUIFontSize_Small,
    GUIFontSize_Normal,
    GUIFontSize_Big,
    GUIFontSize_COUNT
};

struct GUIFont {
    GUIFontSize size;
    bool bold = false;
};

const GUIFont GUIFont_Small = GUIFont{GUIFontSize_Small, false};
const GUIFont GUIFont_Small_Bold = GUIFont{GUIFontSize_Small, true};
const GUIFont GUIFont_Normal = GUIFont{GUIFontSize_Normal, false};
const GUIFont GUIFont_Normal_Bold = GUIFont{GUIFontSize_Normal, true};
const GUIFont GUIFont_Big = GUIFont{GUIFontSize_Big, false};
const GUIFont GUIFont_Big_Bold = GUIFont{GUIFontSize_Big, true};

class GUIContext {
public:
    int font_small_size_px = 14;
    ImFont* font_small = nullptr;
    ImFont* font_small_bold = nullptr;
    int font_normal_size_px = 18;
    ImFont* font_normal = nullptr;
    ImFont* font_normal_bold = nullptr;
    int font_big_size_px = 24;
    ImFont* font_big = nullptr;
    ImFont* font_big_bold = nullptr;
    
    void load();
    void load_fonts();
    int get_font_size_px(GUIFontSize size);
    int get_font_size_px(GUIFont font);
    ImFont* get_imgui_font(GUIFont font);
};
