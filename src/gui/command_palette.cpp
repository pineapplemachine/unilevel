#include "command_palette.hpp"

#include <algorithm>
#include <utility>

#include "imgui.h"
#include "imgui_internal.h"
#include "spdlog/spdlog.h"

#include "app.hpp"
#include "util/string.hpp"
#include "gui/imgui_util.hpp"

bool GUICommandPaletteCommand_AlwaysActiveCallback(
    GUICommandPaletteCommand* command
) {
    return true;
}

void GUICommandPalette::init() {
    spdlog::debug("Initializing GUICommandPalette.");
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
    this->app->input.add_action_key_bind(
        InputActionKeyBind(this->action_show, "Ctrl+Space")
    );
    this->app->input.add_action_key_bind(
        InputActionKeyBind(this->action_show, "Ctrl+Shift+P")
    );
    this->app->input.add_action_key_bind(
        InputActionKeyBind(this->action_activate, "Enter")
    );
    this->app->input.add_action_key_bind(
        InputActionKeyBind(this->action_activate, "KeypadEnter")
    );
    this->app->input.add_action_key_bind(
        InputActionKeyBind(this->action_escape, "Escape")
    );
    this->app->input.add_action_key_bind(
        InputActionKeyBind(this->action_up, "UpArrow")
    );
    this->app->input.add_action_key_bind(
        InputActionKeyBind(this->action_up, "Shift+Tab")
    );
    this->app->input.add_action_key_bind(
        InputActionKeyBind(this->action_down, "DownArrow")
    );
    this->app->input.add_action_key_bind(
        InputActionKeyBind(this->action_down, "Tab")
    );
    this->app->input.add_action_key_bind(
        InputActionKeyBind(this->action_home, "Ctrl+UpArrow")
    );
    this->app->input.add_action_key_bind(
        InputActionKeyBind(this->action_end, "Ctrl+DownArrow")
    );
}

void GUICommandPalette::show() {
    if(this->showing) {
        return;
    }
    spdlog::debug("Showing GUICommandPalette.");
    this->showing = true;
    this->show_init = true;
    this->selected_result_index = 0;
    this->hovered_result_index = -1;
    this->pressed_result_index = -1;
    this->input_text[0] = 0;
    this->input_text_submitted = false;
    this->update_results();
    this->app->input.push_context(InputContext_CommandPalette);
}

void GUICommandPalette::hide() {
    spdlog::debug("Hiding GUICommandPalette.");
    this->showing = false;
    this->show_init = false;
    this->app->input.pop_context(InputContext_CommandPalette);
}

bool GUICommandPalette::is_showing() {
    return this->showing;
}

void GUICommandPalette::draw() {
    this->input_text_modified = false;
    this->hovered_result_index = -1;
    if(!this->showing) {
        return;
    }
    ImGuiContext& im_context = *GImGui;
    const ImGuiStyle& style = im_context.Style;
    ImVec2 viewport = ImGui::GetMainViewport()->Size;
    ImGui::SetNextWindowPos(
        ImVec2(viewport.x / 2.0f, viewport.y > 400 ? 0.05f : 0.0f),
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
    auto im_font = this->context->get_imgui_font(this->font);
    ImGui::PushFont(im_font);
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
    bool input_submitted = ImGui::InputText(
        "##Input",
        this->input_text,
        IM_ARRAYSIZE(this->input_text),
        ImGuiInputTextFlags_EnterReturnsTrue
    );
    if(input_submitted) {
        this->input_text_submitted = true;
        spdlog::trace("GUICommandPalette InputText field submitted.");
    }
    else if(ImGui::IsItemDeactivated()) {
        spdlog::trace("GUICommandPalette InputText field deactivated.");
        this->hide();
    }
    else if(ImGui::IsItemEdited()) {
        this->input_text_modified = true;
    }
    const int window_max_results_height = ImClamp(
        (int) (viewport.y / result_size.y), 2, 12
    );
    const int window_results_height = ImMax(1, ImMin(
        window_max_results_height, (int) this->results.size()
    ));
    const float window_content_height = (
        result_size.y * window_results_height
    );
    ImGui::Dummy(ImVec2(0.0f, 0.125f * font_size_px));
    ImGui::BeginChild(
        "##Results",
        ImVec2(window_content_width, window_content_height)
    );
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0.0f, 0.0f});
    for(int i = 0; i < this->results.size(); ++i) {
        auto result = this->results[i];
        const bool result_pressed = this->draw_result(
            i, result, result_size
        );
        if(result_pressed) {
            this->pressed_result_index = i;
        }
    }
    ImGui::PopStyleVar();
    if(this->results.size() == 0) {
        const ImU32 no_match_color = (
            ImGui::GetColorU32(ImGuiCol_TextDisabled)
        );
        ImGui::SetCursorPosX(
            ImGui::GetCursorPosX() + style.FramePadding.x
        );
        ImGui::PushStyleColor(ImGuiCol_Text, no_match_color);
        ImGui::TextUnformatted("No matching commands.");
        ImGui::PopStyleColor();
    }
    ImGui::PopFont();
    ImGui::EndChild();
    ImGui::End();
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
    ImGui::ItemSize(size, 0.0f);
    if(!ImGui::ItemAdd(box, id)) {
        return false;
    }
    const ImGuiButtonFlags flags = ImGuiButtonFlags_MouseButtonLeft;
    bool selected = (result_index == this->selected_result_index);
    bool hovered = false;
    bool held = false;
    bool pressed = ImGui::ButtonBehavior(box, id, &hovered, &held, flags);
    const ImU32 fill_color = ImGui::GetColorU32(
        (held && hovered && result.active) ? ImGuiCol_FrameBgHovered :
        ((hovered && result.active) || selected) ? ImGuiCol_FrameBg :
        ImGuiCol_WindowBg
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
        // const ImU32 line_color = ImGui::GetColorU32(ImGuiCol_Separator);
        // window->DrawList->AddLine(
        //     box.GetTL(), box.GetTR(), line_color, border_size
        // );
    }
    if(im_context.LogEnabled) {
        ImGui::LogSetNextTextDecoration("[", "]");
    }
    const ImVec2 name_label_size = ImGui::CalcTextSize(
        command.name.c_str(), nullptr, true
    );
    const ImU32 text_color = ImGui::GetColorU32(
        result.active ? ImGuiCol_Text : ImGuiCol_TextDisabled
    );
    window->DrawList->AddText(
        this->context->get_imgui_font(this->font),
        (float) this->context->get_font_size_px(this->font),
        box.Min + style.FramePadding,
        text_color,
        command.name.c_str(),
        nullptr,
        0.0f,
        nullptr
    );
    if(hovered && command.summary.size() > 0) {
        ImGuiUtil::SetTooltipUnformatted(command.summary.c_str());
    }
    if(hovered) {
        this->hovered_result_index = result_index;
    }
    return pressed;
}

void GUICommandPalette::update() {
    if(!this->showing) {
        if(this->app->input.is_action_active(this->action_show)) {
            this->show();
        }
        else {
            return;
        }
    }
    else if(this->app->input.is_action_active(this->action_escape)) {
        this->hide();
        return;
    }
    if(this->input_text_submitted ||
        this->app->input.is_action_active(this->action_activate)
    ) {
        if(this->selected_result_index > 0 &&
            this->selected_result_index < this->results.size()
        ) {
            this->activate_result(
                this->results[this->selected_result_index]
            );
        }
        this->hide();
        return;
    }
    else if(this->app->input.is_action_active(this->action_home)) {
        this->selected_result_index = 0;
    }
    else if(this->app->input.is_action_active(this->action_end)) {
        this->selected_result_index = ((int) this->results.size()) - 1;
    }
    else if(this->app->input.is_action_active(this->action_up)) {
        this->selected_result_index--;
        if(this->selected_result_index < 0) {
            this->selected_result_index = 0;
        }
    }
    else if(this->app->input.is_action_active(this->action_down)) {
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
    if(this->input_text_modified) {
        this->selected_result_index = 0;
        this->update_results();
    }
}

void GUICommandPalette::add_command(
    GUICommandPaletteCommand command
) {
    spdlog::debug(
        "Adding GUICommandPalette command '{}'.",
        command.name
    );
    // Order commands alphabetically by name
    auto comparator = [](
        const GUICommandPaletteCommand& a,
        const GUICommandPaletteCommand& b
    ) -> bool {
        return (ImStricmp(
            a.name.c_str(),
            b.name.c_str()
        ) > 0);
    };
    auto location = std::lower_bound(
        this->commands.begin(),
        this->commands.end(),
        command,
        comparator
    );
    this->commands.insert(location, std::move(command));
    this->command_activated_times.push_back(-1024);
}

void GUICommandPalette::activate_result(
    GUICommandPaletteResult& result
) {
    IM_ASSERT(
        this->commands.size() ==
        this->command_activated_times.size()
    );
    spdlog::trace("Activating GUICommandPalette result.");
    if(result.command < 0 ||
        result.command >= this->commands.size()
    ) {
        return;
    }
    GUICommandPaletteCommand& command = (
        this->commands[result.command]
    );
    this->hide();
    spdlog::debug(
        "Activating GUICommandPalette command '{}'.",
        command.name
    );
    command.activated_callback();
    this->command_activated_times[result.command] = (
        this->command_time
    );
    this->command_time++;
}

void GUICommandPalette::update_command_result(const int i) {
    // TODO: also compare alias strings, in addition to name?
    auto& command = this->commands[i];
    auto input_str = std::string(this->input_text);
    const auto match = string_fuzzy_match(input_str, command.name);
    // If there are too many characters in the search string that
    // did not match the command name, then cut it from the results
    const int unmatched_chars = input_str.size() - match.matched;
    if(unmatched_chars >= 8) {
        return;
    }
    // Otherwise take the match score and modify for recency and
    // inactive/disabled commands
    const int command_age = (
        this->command_time -
        this->command_activated_times[i]
    );
    const bool command_active = (
        command.get_active_callback(&command)
    );
    const int recency_score = ImMax(0, 16 - command_age);
    const int inactivity_score = command_active ? 0 : -32;
    const int sort_score = match.score + recency_score + inactivity_score;
    spdlog::trace(
        "string_fuzzy_match: score {}, matched {}",
        match.score, match.matched
    );
    spdlog::trace(
        "GUICommandPalette sort_score is {} for input '{}' and command '{}'.",
        sort_score, input_str, command.name
    );
    auto result = GUICommandPaletteResult{
        .command = i,
        .sort_score = sort_score,
        .active = command_active
    };
    this->results.push_back(result);
}

void GUICommandPalette::update_results() {
    IM_ASSERT(
        this->commands.size() ==
        this->command_activated_times.size()
    );
    spdlog::trace(
        "Updating GUICommandPalette results for input text '{}'.",
        this->input_text
    );
    this->results.clear();
    if(this->input_text[0] == 0 ||
        this->input_text[IM_ARRAYSIZE(this->input_text) - 1] != 0
    ) {
        for(int i = 0; i < this->commands.size(); ++i) {
            auto& command = this->commands[i];
            bool active = command.get_active_callback(&command);
            auto result = GUICommandPaletteResult{i, 0, active};
            this->results.push_back(result);
        }
    }
    else {
        for(int i = 0; i < this->commands.size(); ++i) {
            this->update_command_result(i);
        }
        auto comparator = [](
            const GUICommandPaletteResult& a,
            const GUICommandPaletteResult& b
        ) -> bool {
            return a.sort_score > b.sort_score;
        };
        std::sort(
            this->results.begin(),
            this->results.end(),
            comparator
        );
    }
}
