#include "imgui_util.hpp"

#include "imgui.h"
#include "imgui_internal.h"

// Based on ImGui::SetTooltip, but without string formatting
void ImGuiUtil::SetTooltipUnformatted(
    const char* text, const char* text_end
) {
    bool tooltip_ok = ImGui::BeginTooltipEx(
        ImGuiTooltipFlags_OverridePrevious,
        ImGuiWindowFlags_None
    );
    if(!tooltip_ok) {
        return;
    }
    ImGui::TextUnformatted(text, text_end);
    ImGui::EndTooltip();
}
