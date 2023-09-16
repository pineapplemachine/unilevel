#include "context.hpp"

#include "imgui_freetype.h"
#include "rlImGui.h"

#include "app.hpp"
#include "input/controller.hpp"

void GUIContext::init() {
    this->init_fonts();
}

void GUIContext::init_fonts() {
    // TODO: don't hardcode font paths
    // looks good at 18px
    const auto font_path_regular = "assets/fonts/PublicSans/PublicSans-Regular.ttf";
    const auto font_path_bold = "assets/fonts/PublicSans/PublicSans-Bold.ttf";
    // looks alright at 16px
    // const auto font_path_regular = "assets/fonts/Atkinson-Hyperlegible/Atkinson-Hyperlegible-Regular-102.ttf";
    // const auto font_path_bold = "assets/fonts/Atkinson-Hyperlegible/Atkinson-Hyperlegible-Bold-102.ttf";
    ImFontConfig font_config;
    font_config.FontBuilderFlags = (
        ImGuiFreeTypeBuilderFlags_ForceAutoHint
    );
    ImGuiIO& io = ImGui::GetIO();
    this->font_small = io.Fonts->AddFontFromFileTTF(
        font_path_regular,
        this->font_small_size_px,
        &font_config
    );
    this->font_small_bold = io.Fonts->AddFontFromFileTTF(
        font_path_bold,
        this->font_small_size_px,
        &font_config
    );
    this->font_normal = io.Fonts->AddFontFromFileTTF(
        font_path_regular,
        this->font_normal_size_px,
        &font_config
    );
    this->font_normal_bold = io.Fonts->AddFontFromFileTTF(
        font_path_bold,
        this->font_normal_size_px,
        &font_config
    );
    this->font_big = io.Fonts->AddFontFromFileTTF(
        font_path_regular,
        this->font_big_size_px,
        &font_config
    );
    this->font_big_bold = io.Fonts->AddFontFromFileTTF(
        font_path_bold,
        this->font_big_size_px,
        &font_config
    );
    rlImGuiReloadFonts();
}

int GUIContext::get_font_size_px(GUIFontSize size) {
    switch(size) {
        case GUIFontSize_Small: {
            return this->font_small_size_px;
        }
        case GUIFontSize_Normal: {
            return this->font_normal_size_px;
        }
        case GUIFontSize_Big: {
            return this->font_big_size_px;
        }
        default: {
            return 0;
        }
    }
}

int GUIContext::get_font_size_px(GUIFont font) {
    return this->get_font_size_px(font.size);
}

ImFont* GUIContext::get_imgui_font(GUIFont font) {
    switch(font.size) {
        case GUIFontSize_Small: {
            return font.bold ? this->font_small_bold : this->font_small;
        }
        case GUIFontSize_Normal: {
            return font.bold ? this->font_normal_bold : this->font_normal;
        }
        case GUIFontSize_Big: {
            return font.bold ? this->font_big_bold : this->font_big;
        }
        default: {
            return nullptr;
        }
    }
}
