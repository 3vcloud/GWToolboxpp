#pragma once

#include <imgui.h>
#include <d3d9.h>
#include <SimpleIni.h>

#include <GWCA/GameEntities/Item.h>
#include <ToolboxModule.h>
#include <ToolboxUIElement.h>

class QuickWikiModule : public ToolboxModule {
public:
	QuickWikiModule() {}
	~QuickWikiModule() {};

public:
	static QuickWikiModule& Instance() {
		static QuickWikiModule instance;
		return instance;
	}

	const char* Name() const override { return "Quick Wiki"; }
	
	void Initialize() override;
	void SignalTerminate() override;
	void Update(float delta) override;
	void DrawSettingInternal() override;

	bool CanTerminate() { return true; };
	bool HasSettings() { return true; };

	// virtual void DrawHelp() {};
	// virtual void Terminate() {};
	// virtual bool WndProc(UINT Message, WPARAM wParam, LPARAM lParam) { return false; };

	void LoadSettings(CSimpleIni* ini);
	void SaveSettings(CSimpleIni* ini);

	static void ItemClickCallback(GW::HookStatus*, uint32_t type, uint32_t slot, GW::Bag* bag);

private:
	GW::HookEntry ItemClickCallback_Entry;
};
