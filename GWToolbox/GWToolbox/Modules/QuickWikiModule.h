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
	virtual void Terminate() override;

	bool CanTerminate() { return true; };
	bool HasSettings() { return true; };

	// virtual void DrawHelp() {};
	// virtual bool WndProc(UINT Message, WPARAM wParam, LPARAM lParam) { return false; };

	void LoadSettings(CSimpleIni* ini);
	void SaveSettings(CSimpleIni* ini);

	static void ItemClickCallback(GW::HookStatus*, uint32_t type, uint32_t slot, GW::Bag* bag);

	static const std::wstring wiki_url;

private:
	bool shift_click_show_wiki_url = true;
	bool shift_click_item_open_wiki = true;
	bool shift_click_parse_wiki_salvage = false;

	GW::HookEntry ItemClickCallback_Entry;
};
