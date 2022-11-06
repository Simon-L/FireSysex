 // * Copyright (C) 2021 Jean Pierre Cimalando <jp-dev@inbox.ru>
 // * Copyright (C) 2021-2022 Filipe Coelho <falktx@falktx.com>
 // 2022 Simon-L
#include "DistrhoUI.hpp"
#include "ResizeHandle.hpp"
#include <algorithm>

START_NAMESPACE_DISTRHO

class ImGuiPluginUI : public UI
{
    ResizeHandle fResizeHandle;

    ImVec4 color = ImVec4(0.5f, 0.5f, 0.50f, 0.5f);
    char buf[14] = "One Two Three";

public:
    ImGuiPluginUI()
        : UI(DISTRHO_UI_DEFAULT_WIDTH, DISTRHO_UI_DEFAULT_HEIGHT),
          fResizeHandle(this)
    {
        setGeometryConstraints(DISTRHO_UI_DEFAULT_WIDTH, DISTRHO_UI_DEFAULT_HEIGHT, true);

        if (isResizable())
            fResizeHandle.hide();
    }

protected:
    void parameterChanged(uint32_t index, float value) override
    {
        repaint();
    }

    void stateChanged(const char* key, const char* value) {}

    void onImGuiDisplay() override
    {
        const float width = getWidth();
        const float height = getHeight();
        const float margin = 20.0f * getScaleFactor();

        ImGui::SetNextWindowPos(ImVec2(margin, margin));
        ImGui::SetNextWindowSize(ImVec2(width - 2 * margin, height - 2 * margin));

        if (ImGui::Begin("AKAI Fire SysEx test", nullptr, ImGuiWindowFlags_NoResize))
        {
            if (ImGui::Button("Turn off all LEDs")) setState("init", "\0");
            ImGui::Separator();
            if (ImGui::Button("Turn pad 0 blue")) setState("on", "\0");
            if (ImGui::Button("Turn pad 0 off")) setState("off", "\0");
            ImGui::Separator();
            if (ImGui::ColorPicker3("Pads color", (float*)&color)) {
                char stR = std::floor(color.x * 127);
                char stG = std::floor(color.y * 127);
                char stB = std::floor(color.z * 127);
                const char col[4] = {stR, stG, stB, '\0'};
                setState("color", col);
            }
            ImGui::Separator();
            ImGui::InputText("Screen text", buf, 14);
            if (ImGui::Button("Display text")) setState("text", buf);
            if (ImGui::Button("Display hello world on screen")) setState("hello", "\0");
            if (ImGui::Button("Display checker on screen")) setState("checker", "\0");
            if (ImGui::Button("Clear screen")) setState("clear", "\0");
        }
        ImGui::End();
    }

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ImGuiPluginUI)
};

UI* createUI()
{
    return new ImGuiPluginUI();
}

END_NAMESPACE_DISTRHO
