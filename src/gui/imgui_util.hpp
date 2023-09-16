#pragma once

namespace ImGuiUtil {
    // Helper to set an unformatted tooltip string.
    // Behaves like ImGui::SetTooltip, but does not accept
    // formatting arugments.
    void SetTooltipUnformatted(
        const char* text,
        const char* text_end = nullptr
    );
}
