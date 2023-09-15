#include "command_palette.hpp"

#include <algorithm>
#include <utility>

#include "imgui.h"
#include "imgui_internal.h"

#include "app.hpp"
#include "util/string.hpp"

void GUICommandPalette::init() {
    this->action_show = this->app->input.add_action(InputAction{
        "ui_command_palette_show",
        InputContext_General
    });
    this->action_activate = this->app->input.add_action(InputAction{
        "ui_command_palette_activate",
        InputContext_CommandPalette
    });
    this->action_escape = this->app->input.add_action(InputAction{
        "ui_command_palette_escape",
        InputContext_CommandPalette
    });
    this->action_up = this->app->input.add_action(InputAction{
        "ui_command_palette_up",
        InputContext_CommandPalette
    });
    this->action_down = this->app->input.add_action(InputAction{
        "ui_command_palette_down",
        InputContext_CommandPalette
    });
    this->action_home = this->app->input.add_action(InputAction{
        "ui_command_palette_home",
        InputContext_CommandPalette
    });
    this->action_end = this->app->input.add_action(InputAction{
        "ui_command_palette_end",
        InputContext_CommandPalette
    });
    // TODO: Don't hardcode keybinds
    this->app->input.add_action_key_bind(InputActionKeyBind{
        .key=InputModifiedKey_Parse("Ctrl+Space"),
        .key_state=InputKeyState_Pressed,
        .action=this->action_show
    });
    this->app->input.add_action_key_bind(InputActionKeyBind{
        .key=InputModifiedKey_Parse("Ctrl+Shift+P"),
        .key_state=InputKeyState_Pressed,
        .action=this->action_show
    });
    this->app->input.add_action_key_bind(InputActionKeyBind{
        .key=InputModifiedKey_Parse("Enter"),
        .key_state=InputKeyState_Pressed,
        .action=this->action_activate
    });
    this->app->input.add_action_key_bind(InputActionKeyBind{
        .key=InputModifiedKey_Parse("Escape"),
        .key_state=InputKeyState_Pressed,
        .action=this->action_escape
    });
    this->app->input.add_action_key_bind(InputActionKeyBind{
        .key=InputModifiedKey_Parse("UpArrow"),
        .key_state=InputKeyState_Pressed,
        .action=this->action_up
    });
    this->app->input.add_action_key_bind(InputActionKeyBind{
        .key=InputModifiedKey_Parse("DownArrow"),
        .key_state=InputKeyState_Pressed,
        .action=this->action_down
    });
    this->app->input.add_action_key_bind(InputActionKeyBind{
        .key=InputModifiedKey_Parse("Ctrl+UpArrow"),
        .key_state=InputKeyState_Pressed,
        .action=this->action_home
    });
    this->app->input.add_action_key_bind(InputActionKeyBind{
        .key=InputModifiedKey_Parse("Ctrl+DownArrow"),
        .key_state=InputKeyState_Pressed,
        .action=this->action_end
    });
}

void GUICommandPalette::show() {
    if(this->showing) {
        return;
    }
    this->showing = true;
    this->show_init = true;
    this->selected_result_index = 0;
    this->hovered_result_index = -1;
    this->pressed_result_index = -1;
    this->input_text[0] = 0;
    this->update_results();
    this->app->input.push_context(InputContext_CommandPalette);
}

void GUICommandPalette::hide() {
    this->showing = false;
    this->show_init = false;
    this->app->input.pop_context(InputContext_CommandPalette);
}

bool GUICommandPalette::is_showing() {
    return this->showing;
}

void GUICommandPalette::draw() {
    this->hovered_result_index = -1;
    if(!this->showing) {
        return;
    }
    ImGuiContext& im_context = *GImGui;
    const ImGuiStyle& style = im_context.Style;
    ImVec2 viewport = ImGui::GetMainViewport()->Size;
    ImGui::SetNextWindowPos(
        ImVec2(viewport.x / 2.0f, 0),
        ImGuiCond_None,
        ImVec2(0.5f, 0.0f)
    );
    const auto font_size_px = (
        this->context->get_font_size_px(this->font)
    );
    const float size_width = ImMin(
        0.75f * viewport.x,
        (float) (32 * font_size_px)
    );
    const ImVec2 window_size = ImVec2(size_width, 0.0f);
    ImGui::SetNextWindowSize(window_size);
    bool visible = ImGui::Begin(
        "Command Palette",
        nullptr,
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoTitleBar
    );
    const float window_content_width = (
        ImGui::GetWindowContentRegionWidth()
    );
    const ImVec2 result_size = ImVec2(
        window_content_width,
        ((float) font_size_px) + style.FramePadding.y
    );
    if(this->show_init) {
        ImGui::SetKeyboardFocusHere();
        this->show_init = false;
    }
    ImGui::SetNextItemWidth(window_content_width);
    bool input_modified = ImGui::InputText(
        "##Input",
        this->input_text,
        IM_ARRAYSIZE(this->input_text)
    );
    if(input_modified) {
        this->selected_result_index = 0;
        this->update_results();
    }
    const int window_max_results_height = ImClamp(
        (int) (viewport.y / result_size.y), 2, 12
    );
    const int window_results_height = ImMin(
        window_max_results_height, (int) this->results.size()
    );
    const float window_content_height = (
        result_size.y * window_results_height
    );
    ImGui::BeginChild(
        "##Results",
        ImVec2(window_content_width, window_content_height)
    );
    for(int i = 0; i < this->results.size(); ++i) {
        auto result = this->results[i];
        const bool result_pressed = this->draw_result(
            i, result, result_size
        );
        if(result_pressed) {
            this->pressed_result_index = i;
        }
    }
    ImGui::EndChild();
}

void GUICommandPalette::update() {
    if(!this->showing) {
        if(this->action_show->active) {
            this->show();
        }
        else {
            return;
        }
    }
    else if(this->action_escape->active) {
        this->hide();
        return;
    }
    if(this->action_activate->active) {
        if(this->selected_result_index > 0 &&
            this->selected_result_index < this->results.size()
        ) {
            auto result = this->results[this->selected_result_index];
            this->activate_result(result);
        }
        this->hide();
        return;
    }
    else if(this->action_home->active) {
        this->selected_result_index = 0;
    }
    else if(this->action_end->active) {
        this->selected_result_index = ((int) this->results.size()) - 1;
    }
    else if(this->action_up->active) {
        this->selected_result_index--;
        if(this->selected_result_index < 0) {
            this->selected_result_index = 0;
        }
    }
    else if(this->action_down->active) {
        int max_result_index = ((int) this->results.size()) - 1;
        this->selected_result_index++;
        if(this->selected_result_index > max_result_index) {
            this->selected_result_index = max_result_index;
        }
    }
    if(this->pressed_result_index > 0 &&
        this->pressed_result_index < this->results.size()
    ) {
        this->activate_result(
            this->results[this->pressed_result_index]
        );
    }
}

void GUICommandPalette::add_command(
    GUICommandPaletteCommand command
) {
    // Order commands alphabetically by title
    auto comparator = [](
        const GUICommandPaletteCommand& a,
        const GUICommandPaletteCommand& b
    ) -> bool {
        return (ImStricmp(
            a.title.c_str(),
            b.title.c_str()
        ) < 0);
    };
    auto location = std::lower_bound(
        this->commands.begin(),
        this->commands.end(),
        command,
        comparator
    );
    this->commands.insert(location, std::move(command));
}

// Based on ImGui::ButtonEx implementation in imgui_widgets.cpp
bool GUICommandPalette::draw_result(
    int result_index,
    GUICommandPaletteResult result,
    const ImVec2& size
) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if(window->SkipItems) {
        return false;
    }
    if(result.command < 0 ||
        result.command >= this->commands.size()
    ) {
        return false;
    }
    GUICommandPaletteCommand& command = this->commands[result.command];
    ImGuiContext& im_context = *GImGui;
    const ImGuiStyle& style = im_context.Style;
    const ImGuiID id = window->GetID(result_index);
    const ImVec2 pos = window->DC.CursorPos;
    const ImRect box = ImRect(pos, pos + size);
    if(!ImGui::ItemAdd(box, id)) {
        return false;
    }
    const ImGuiButtonFlags flags = ImGuiButtonFlags_MouseButtonLeft;
    bool selected = (result_index == this->selected_result_index);
    bool hovered = false;
    bool held = false;
    bool pressed = ImGui::ButtonBehavior(box, id, &hovered, &held, flags);
    const ImU32 fill_color = ImGui::GetColorU32(
        (held && hovered && result.active) ? ImGuiCol_ButtonActive :
        ((hovered && result.active) || selected) ? ImGuiCol_ButtonHovered :
        ImGuiCol_Button
    );
    window->DrawList->AddRectFilled(box.Min, box.Max, fill_color, 0.0f);
    const float border_size = ImMax(
        1.0f, im_context.Style.FrameBorderSize
    );
    if(selected) {
        const ImU32 border_color = ImGui::GetColorU32(ImGuiCol_Border);
        window->DrawList->AddRect(
            box.Min, box.Max, border_color, 0.0f, 0, border_size
        );
    }
    else {
        const ImU32 line_color = ImGui::GetColorU32(ImGuiCol_Separator);
        window->DrawList->AddLine(
            box.GetTL(), box.GetTR(), line_color, border_size
        );
    }
    if(im_context.LogEnabled) {
        ImGui::LogSetNextTextDecoration("[", "]");
    }
    const ImVec2 title_label_size = ImGui::CalcTextSize(
        command.title.c_str(), nullptr, true
    );
    const ImU32 text_color = ImGui::GetColorU32(
        result.active ? ImGuiCol_Text : ImGuiCol_TextDisabled
    );
    // void ImGui::RenderTextClipped(
    //     const ImVec2& pos_min,
    //     const ImVec2& pos_max,
    //     const char* text,
    //     const char* text_end,
    //     const ImVec2* text_size_if_known,
    //     const ImVec2& align,
    //     const ImRect* clip_rect
    // )
    // ImGui::RenderTextClipped(
    //     box.Min + style.FramePadding,
    //     box.Max - style.FramePadding,
    //     command.title.c_str(),
    //     nullptr,
    //     &title_label_size,
    //     ImVec2(0.0f, 0.5f),
    //     &box
    // );
    window->DrawList->AddText(
        this->context->get_imgui_font(this->font),
        (float) this->context->get_font_size_px(this->font),
        box.Min + style.FramePadding,
        text_color,
        command.title.c_str(),
        nullptr,
        0.0f,
        nullptr
    );
    if(hovered && command.summary.size() > 0) {
        ImGui::SetTooltip(command.summary.c_str());
    }
    if(hovered) {
        this->hovered_result_index = result_index;
    }
    return pressed;
}

void GUICommandPalette::activate_result(GUICommandPaletteResult result) {
    if(result.command < 0 ||
        result.command >= this->commands.size()
    ) {
        return;
    }
    GUICommandPaletteCommand& command = this->commands[result.command];
    this->hide();
    command.activated_callback();
}

void GUICommandPalette::update_results() {
    this->results.clear();
    if(this->input_text[0] == 0 ||
        this->input_text[IM_ARRAYSIZE(this->input_text) - 1] != 0
    ) {
        for(int i = 0; i < this->commands.size(); ++i) {
            auto command = this->commands[i];
            bool active = command.get_active_callback();
            auto result = GUICommandPaletteResult{i, 0, active};
            this->results.push_back(result);
        }
    }
    else {
        for(int i = 0; i < this->commands.size(); ++i) {
            // TODO: also compare alias strings, in addition to title
            auto command = this->commands[i];
            bool active = command.get_active_callback();
            auto input_str = std::string(this->input_text); // TODO: memory?
            int sort_score = 0;
            if(string_starts_with_insensitive(command.title, input_str)) {
                bool exact = (command.title.size() == input_str.size());
                sort_score = (exact ? -1 : 0);
            }
            else {
                sort_score = string_sub_distance_insensitive(
                    input_str, command.title
                );
            }
            if(sort_score <= 1 + input_str.size()) {
                if(!active) {
                    sort_score += 1 + input_str.size();
                }
                auto result = GUICommandPaletteResult{i, sort_score, active};
                this->results.push_back(result);
            }
        }
        auto comparator = [](
            const GUICommandPaletteResult& a,
            const GUICommandPaletteResult& b
        ) -> bool {
            return a.sort_score < b.sort_score;
        };
        std::sort(
            this->results.begin(),
            this->results.end(),
            comparator
        );
    }
}
