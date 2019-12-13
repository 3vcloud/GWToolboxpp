#pragma once

#include "ToolboxWindow.h"
#include <thread>

#include "../uMod_Main.h"

class TexmodWindow : public ToolboxWindow {
private:
    TexmodWindow() {};
    typedef void(__stdcall* uMod_pt)(void);
    std::thread uMod_Server;
    std::thread uMod_Injector;
    bool uMod_Server_Running = false;
    bool uMod_Server_NeedToStop = false;
    uMod_pt uMod_InstallHook;
    uMod_pt uMod_RemoveHook;

    HINSTANCE umod_d3d9_dll;

public:
    static TexmodWindow& Instance() {
        static TexmodWindow instance;
        return instance;
    }
    const char* Name() const override { return "Texmod"; }

    void Initialize() override;
    void Terminate() override;

    void StartuModServer();
    void StopuModServer();

    // Update. Will always be called every frame.
    void Update(float delta) override;

    // Draw user interface. Will be called every frame if the element is visible
    void Draw(IDirect3DDevice9* pDevice) override;
    void LoadSettings(CSimpleIni* ini) override;
    void SaveSettings(CSimpleIni* ini) override;
    void DrawSettingInternal() override;
private:
    bool LoadDll();
    bool UnloadDll();
};
