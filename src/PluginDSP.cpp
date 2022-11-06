 // * Copyright (C) 2021 Jean Pierre Cimalando <jp-dev@inbox.ru>
 // * Copyright (C) 2021-2022 Filipe Coelho <falktx@falktx.com>
// 2022 Simon-L

#include "DistrhoPlugin.hpp"
#include "sysex.h"

#include <memory>
#include <vector>

START_NAMESPACE_DISTRHO

class ImGuiPluginDSP : public Plugin
{
    enum Parameters {
        kParamCount
    };

    double fSampleRate = getSampleRate();
    float fTime = 0.0f;

    bool bGoPads = false;
    bool bGoScreen = false;

    MidiEvent midiPads;
    std::vector<uint8_t> sysexPads;

    MidiEvent midiScreen;
    std::vector<uint8_t> sysexScreen;
    bool screenHello = false;
    bool screenChecker = false;
    bool screenClear = false;
    char screenText[14] = "\0";

    float flastSend = 0.0f;

    char padColor[3] = {0, 0, 0};

    MidiEvent initMidi;
    bool initMidiDone = false;

    MidiEvent padZeroMidi;
    bool padZeroBlue = false;
    bool padZeroOff = false;

public:
    ImGuiPluginDSP()
        : Plugin(kParamCount, 0, 0) // parameters, programs, states
    {
        sysexPads.reserve(2048);
        sysexScreen.reserve(2048);

        initMidi.frame = 0;
        initMidi.size = 3;
        initMidi.data[0] = 0xb0;
        initMidi.data[1] = 127;
        initMidi.data[2] = 0;

        padZeroMidi.frame = 0;
        padZeroMidi.size = 12;
        padZeroMidi.dataExt = padZeroSysex;
    }

protected:
    const char* getLabel() const noexcept override
    {
        return "SysexTest";
    }

    const char* getDescription() const override
    {
        return "A simple plugin to test SysEx";
    }

    const char* getMaker() const noexcept override
    {
        return "Jean Pierre Cimalando, falkTX, Simon-L";
    }

    const char* getLicense() const noexcept override
    {
        return "ISC";
    }

    void setState(const char* key, const char* value) override
    {
        if (std::strcmp(key, "color") == 0) {
            if (fTime - flastSend >= 0.05) {
                mkSysexPads(value);

                bGoPads = true;
                flastSend = fTime;
            }
        }
        if (std::strcmp(key, "init") == 0) {
            initMidiDone = false;
        }
        if (std::strcmp(key, "on") == 0) padZeroBlue = true;
        if (std::strcmp(key, "off") == 0) padZeroOff = true;
        if (std::strcmp(key, "hello") == 0) screenHello = true;
        if (std::strcmp(key, "checker") == 0) screenChecker = true;
        if (std::strcmp(key, "clear") == 0) screenClear = true;
        if (std::strcmp(key, "text") == 0) std::strcpy(screenText, value);
    }

    void stateChanged(const char* key, const char* value) {}

    uint32_t getVersion() const noexcept override
    {
        return d_version(1, 0, 0);
    }

    int64_t getUniqueId() const noexcept override
    {
        return d_cconst('a', 'k', 'f', 'i');
    }

    void initParameter(uint32_t index, Parameter& parameter) override
    {
    }

    float getParameterValue(uint32_t index) const override
    {
        DISTRHO_SAFE_ASSERT_RETURN(index == 0, 0.0f);

        return 0.0;
    }

    void setParameterValue(uint32_t index, float value) override
    {
    }

    void mkSysexScreen(uint8_t* source) {
        sysexScreen.clear();
        sysexScreen.insert(sysexScreen.begin(), sysexIntro, sysexIntro+sizeof(sysexIntro));
        sysexScreen.at(4) = 0x0E; // WRITE OLED command
        // trim the data sent to work around 1024 bytes limitation
        sysexScreen.insert(sysexScreen.end(), source, source+1016);
        sysexScreen.at(5) = (sysexScreen.size() - 7) >> 7;
        sysexScreen.at(6) = (sysexScreen.size() - 7) & 0x7F;
        sysexScreen.push_back(0xF7);
        printf("Size: %d\n", sysexScreen.size());
        midiScreen.frame = 0;
        midiScreen.size = sysexScreen.size();
        midiScreen.dataExt = sysexScreen.data();
    }

    void mkSysexPads(const char col[3]) {
        sysexPads.clear();
        sysexPads.insert(sysexPads.end(), sysexIntro, sysexIntro + sizeof(sysexIntro));
        for (int i = 0x0; i <= 0x3F; ++i)
        {
            sysexPads.insert(sysexPads.end(), {i, col[0], col[1], col[2]});
        }
        int payload = sysexPads.size() - sizeof(sysexIntro);
        sysexPads.push_back(0xF7);
        sysexPads[5] = payload >> 7;
        sysexPads[6] = payload & 0x7F;
        midiPads.frame = 0;
        midiPads.size = sysexPads.size();
        midiPads.dataExt = sysexPads.data();
    }


    void activate() override
    {   
        const char s[3]{0,0,0};
        mkSysexPads(s);
    }

    void run(const float** inputs, float** outputs, uint32_t frames) override
    {   
        fTime += frames * (1.0/getSampleRate());

        if (!initMidiDone) {
            writeMidiEvent(initMidi);
            initMidiDone = true;
        }
        if (padZeroBlue) {
            padZeroSysex[10] = 0x7F;
            writeMidiEvent(padZeroMidi);
            padZeroBlue = false;
        }
        if (padZeroOff) {
            padZeroSysex[10] = 0x00;
            writeMidiEvent(padZeroMidi);
            padZeroOff = false;
        }
        if (screenClear) {
            clearScreen();
            mkSysexScreen(OLEDBitmap);
            writeMidiEvent(midiScreen);
            screenClear = false;
        }
        if (screenHello) {
            clearScreen();
            drawString("Hello", 5, 40, 15);
            drawString("world!", 6, 40, 32);
            mkSysexScreen(OLEDBitmap);
            writeMidiEvent(midiScreen);
            screenHello = false;
        }
        if (std::strcmp(screenText, "\0") != 0) {
            clearScreen();
            drawString(screenText, 14, 5, 24);
            mkSysexScreen(OLEDBitmap);
            writeMidiEvent(midiScreen);
            screenText[0] = '\0';
        }
        if (screenChecker) {
            clearScreen();
            mkSysexScreen(checker);
            writeMidiEvent(midiScreen);
            screenChecker = false;            
        }

        if (bGoPads) {
            printf("Sending new color array: %u %u %u\n", sysexPads[8], sysexPads[9], sysexPads[10]);
            bGoPads = false;
            writeMidiEvent(midiPads);
        }
    }

    void sampleRateChanged(double newSampleRate) override
    {
        fSampleRate = newSampleRate;
    }

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ImGuiPluginDSP)
};

Plugin* createPlugin()
{
    return new ImGuiPluginDSP();
}

END_NAMESPACE_DISTRHO
