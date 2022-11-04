#include "Application.hpp"
#include "DearImGui.hpp"
#include "DistrhoUI.hpp"

#ifdef ISPLUGIN

// ------- Plugin
#include "ResizeHandle.hpp"
START_NAMESPACE_DISTRHO
typedef UI _UI;
// -------

#else

// ------- Native
#include <cstdio>
typedef ImGuiStandaloneWindow _UI;
// -------

#endif 

#ifdef ISNATIVE
void editParameter(int index, bool active) {
    printf("%s\n", (active ? "Editing" : "Not editing"));
}
void setParameterValue(int index, float value) {
    printf("Set value %f\n", value);
}
#endif

class ImGuiPluginUI : public _UI
{
    float fGain = 0.0f;
#ifdef ISPLUGIN
    ResizeHandle fResizeHandle;
#endif

    // ----------------------------------------------------------------------------------------------------------------

public:
   /**
      UI class constructor.
      The UI should be initialized to a default state that matches the plugin side.
    */
#ifdef ISPLUGIN
    ImGuiPluginUI()
        : UI(DISTRHO_UI_DEFAULT_WIDTH, DISTRHO_UI_DEFAULT_HEIGHT),
          fResizeHandle(this)
#else
    ImGuiPluginUI(Application& app)
        : ImGuiStandaloneWindow(app)
#endif
    {
        setGeometryConstraints(DISTRHO_UI_DEFAULT_WIDTH, DISTRHO_UI_DEFAULT_HEIGHT, true);

        #ifdef ISPLUGIN
        // hide handle if UI is resizable
        if (isResizable())
            fResizeHandle.hide();
        #endif
    }

protected:
    // ----------------------------------------------------------------------------------------------------------------
    // DSP/Plugin Callbacks

    #ifdef ISPLUGIN
    /**
      A parameter has changed on the plugin side.@n
      This is called by the host to inform the UI about parameter changes.
    */
    void parameterChanged(uint32_t index, float value) override
    {
        DISTRHO_SAFE_ASSERT_RETURN(index == 0,);

        fGain = value;
        repaint();
    }
    #endif

    // ----------------------------------------------------------------------------------------------------------------
    // Widget Callbacks

   /**
      ImGui specific onDisplay function.
    */
    void onImGuiDisplay() override
    {
        const float width = getWidth();
        const float height = getHeight();
        const float margin = 20.0f * getScaleFactor();

        ImGui::SetNextWindowPos(ImVec2(margin, margin));
        ImGui::SetNextWindowSize(ImVec2(width - 2 * margin, height - 2 * margin));

        #ifdef ISPLUGIN
        const char* title = "Simple gain - plugin";
        #else
        const char* title = "Simple gain - native";
        #endif

        if (ImGui::Begin(title, nullptr, ImGuiWindowFlags_NoResize))
        {
            static char aboutText[256] = "This is a demo plugin made with ImGui.\n";
            ImGui::InputTextMultiline("About", aboutText, sizeof(aboutText));

            if (ImGui::SliderFloat("Gain (dB)", &fGain, -90.0f, 30.0f))
            {
                if (ImGui::IsItemActivated())
                    editParameter(0, true);

                setParameterValue(0, fGain);
            }

            if (ImGui::IsItemDeactivated())
            {
                editParameter(0, false);
            }
        }
        ImGui::End();
    }

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ImGuiPluginUI)
};

#ifdef ISPLUGIN
// --------------------------------------------------------------------------------------------------------------------

UI* createUI()
{
    return new ImGuiPluginUI();
}

// --------------------------------------------------------------------------------------------------------------------

END_NAMESPACE_DISTRHO
#else
int main(int, char**)
{
    Application app;
    ImGuiPluginUI win(app);
    win.show();
    app.exec();
    return 0;
}
#endif