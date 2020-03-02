#include "stdafx.h"

#include <GWCA/Managers/ChatMgr.h>
#include <GWCA/Managers/ItemMgr.h>
#include <GWCA/Managers/UIMgr.h>

#include "QuickWikiModule.h"
#include "Modules/Resources.h"

#include "logger.h"
#include <Timer.h>

#define TEST_STR  "asdasd a dADS  \n3 r234wr atr 34ta34 <tr valign=\"top\">\n<th style=\"background-color:#FA5;\"> <span\nstyle=\"white-space:nowrap;\"><a href=\"/wiki/Common_crafting_material\" title=\"Common crafting material\">Common salvage</a></span></th><td> <a href=\"/wiki/Pile_of_Glittering_Dust\" title=\"Pile of Glittering Dust\">Pile of Glittering Dust</a></td></tr><tr valign=\"top\">\n<th style=\"background-color:#FA5;\"> <span\nstyle=\"white-space:nowrap;\"><a href=\"/wiki/Common_crafting_material\" title=\"Common crafting material\">Common salvage</a></span></th><td> <a href=\"/wiki/Pile_of_Glittering_Dust\" title=\"Pile of Glittering Dust\">Pile of Glittering Dust</a></td></tr>";
#define TEST_STR2 "<table align=\"right\" style=\"margin:0 0 0.5em 1em; border:1px solid silver; font-size:90%; text-align:left; background-color:white;\" cellpadding=\"5\" cellspacing=\"3\"><tr><th colspan=\"2\" style=\"font-size:110%; text-align:center; background-color:#FA5;\"> Pronged Fan</th></tr><tr><td colspan=\"2\" align=\"center\"> <a href=\"/wiki/File:Pronged_Fan.jpg\" class=\"image\"><img alt=\"Pronged Fan.jpg\" src=\"/images/e/e8/Pronged_Fan.jpg\" width=\"250\" height=\"379\" /></a></td></tr><tr valign=\"top\"><th style=\"background-color:#FA5;\"> <a href=\"/wiki/Weapon#Weapon_types\" title=\"Weapon\">Type</a></th><td> <a href=\"/wiki/Focus_item\" title=\"Focus item\">Focus item</a></td></tr><tr valign=\"top\"><th style=\"background-color:#FA5;\"> <a href=\"/wiki/Campaign\" title=\"Campaign\">Campaign(s)</a></th><td> <a href=\"/wiki/Factions\" class=\"mw-redirect\" title=\"Factions\">Factions</a></td></tr><tr valign=\"top\"><th style=\"background-color:#FA5;\"> <a href=\"/wiki/Attribute\" title=\"Attribute\">Attribute</a><br /><a href=\"/wiki/Requirement\" title=\"Requirement\">requirement(s)</a></th><td> <a href=\"/wiki/Divine_Favor\" title=\"Divine Favor\">Divine Favor</a><br /><a href=\"/wiki/Energy_Storage\" title=\"Energy Storage\">Energy Storage</a><br /><a href=\"/wiki/Inspiration_Magic\" title=\"Inspiration Magic\">Inspiration Magic</a><br /><a href=\"/wiki/Soul_Reaping\" title=\"Soul Reaping\">Soul Reaping</a><br /><a href=\"/wiki/Spawning_Power\" title=\"Spawning Power\">Spawning Power</a></td></tr><tr valign=\"top\"><th style=\"background-color:#FA5;\"> <span style=\"white-space:nowrap;\"><a href=\"/wiki/PvP_reward\" title=\"PvP reward\">PvP reward class</a></span></th><td> Exotic<br />5 <span style=\"position:relative;top:-1.5px\"><a href=\"/wiki/File:Gold_Zaishen_Coin.png\" class=\"image\" title=\"Gold Zaishen Coin\"><img alt=\"Gold Zaishen Coin\" src=\"/images/thumb/2/26/Gold_Zaishen_Coin.png/19px-Gold_Zaishen_Coin.png\" width=\"19\" height=\"19\" srcset=\"/images/thumb/2/26/Gold_Zaishen_Coin.png/29px-Gold_Zaishen_Coin.png 1.5x, /images/thumb/2/26/Gold_Zaishen_Coin.png/38px-Gold_Zaishen_Coin.png 2x\" /></a></span></td></tr><tr valign=\"top\"><th style=\"background-color:#FA5;\"> <span style=\"white-space:nowrap;\"><a href=\"/wiki/Common_crafting_material\" title=\"Common crafting material\">Common salvage</a></span></th><td> <a href=\"/wiki/Pile_of_Glittering_Dust\" title=\"Pile of Glittering Dust\">Pile of Glittering Dust</a></td></tr><tr valign=\"top\"><th style=\"background-color:#FA5;\"> <span style=\"white-space:nowrap;\"><a href=\"/wiki/Rare_crafting_material\" title=\"Rare crafting material\">Rare salvage</a></span></th><td> ~9 <a href=\"/wiki/Roll_of_Parchment\" title=\"Roll of Parchment\">Rolls of Parchment</a></td></tr><tr valign=\"top\"><th style=\"background-color:#FA5;\"> <span style=\"white-space:nowrap;\"><a href=\"/wiki/Inventory\" title=\"Inventory\">Inventory icon</a></span></th><td> <a href=\"/wiki/File:Pronged_Fan.png\" class=\"image\"><img alt=\"Pronged Fan.png\" src=\"/images/7/79/Pronged_Fan.png\" width=\"64\" height=\"64\" /></a></td></tr></table><table style=\"margin: 0.5em; border: 1px solid silver;\" cellpadding=\"5\" cellspacing=\"2\"><tr><td style=\"border:1px solid #aaa;\"> <a href=\"/wiki/File:Pronged_Fan_dye_chart.jpg\" class=\"image\"><img alt=\"Pronged Fan dye chart.jpg\" src=\"/images/thumb/9/97/Pronged_Fan_dye_chart.jpg/500px-Pronged_Fan_dye_chart.jpg\" width=\"500\" height=\"339\" srcset=\"/images/thumb/9/97/Pronged_Fan_dye_chart.jpg/750px-Pronged_Fan_dye_chart.jpg 1.5x, /images/9/97/Pronged_Fan_dye_chart.jpg 2x\" /></a></td></tr></table>";
	
const std::wstring wiki_url(L"https://wiki.guildwars.com/wiki/");

void QuickWikiModule::Initialize() {
	//std::cout << "QuickWikiModule::Initialize" << std::endl;
	ToolboxModule::Initialize();
	GW::Items::RegisterItemClickCallback(&ItemClickCallback_Entry, QuickWikiModule::ItemClickCallback);
}

inline static std::wstring BuildWikiLink(std::wstring &item_url) {
	return wiki_url + item_url;
}

inline static std::wstring BuildChatLink(std::wstring& url) {
	return std::wstring(L"<a=1>") + url + std::wstring(L"</a>");
}

static std::wstring DecodeToWikiLink(wchar_t *inp) {
	std::wstring decoded;
	GW::UI::AsyncDecodeStr(inp, &decoded);
	std::wstring item_url = std::regex_replace(decoded, std::wregex(L" "), std::wstring(L"_"));
#ifdef _DEBUG
	printf("<%ls>\n", decoded.c_str());
	printf("<%ls>\n", item_url.c_str());
#endif
	return BuildWikiLink(item_url);
	//std::wstring chat_url = BuildChatLink(final_url);

}


static void ExtractMaterialFromWiki(std::string wiki_page, std::string regex_str) {
	std::smatch sm1;
	std::regex_search(wiki_page, sm1, std::regex(regex_str));

#if _DEBUG
	std::cout << regex_str << std::endl;
	std::cout << "sm1.length: " << sm1.size() << std::endl;
	for (auto m : sm1) {
		std::cout << '[' << m << ']' << std::endl;
	}
#endif
	// sm1 should contain two items if we found a proper table entry.
	if (sm1.size() == 2) {
		// replace <br /> with separator
		std::string final = std::regex_replace(sm1.str(1), std::regex("<br[^>]*>"), ";");

		// strip all tags (usually links)
		final = std::regex_replace(final, std::regex("<[^>]*>"), "");

#if _DEBUG
		std::cout << final << std::endl;
#endif

		GW::Chat::WriteChat(GW::Chat::Channel::CHANNEL_GLOBAL, final.c_str());
	}
}

static void WikiParseSalvage(std::string wiki_page) {
#if _DEBUG
	std::cout << "wiki_page.length: " << wiki_page.length() << std::endl;
#endif
	wiki_page.erase(std::remove_if(wiki_page.begin(), wiki_page.end(), 
		[](char c) {return c < 0x20 or c > 0x7e;}
		), wiki_page.end());
#if _DEBUG
	std::cout << "wiki_page.length: " << wiki_page.length() << std::endl;
#endif

	// Check if this in the string, the regex is slow if it's not.
	if (wiki_page.find("Common_crafting_material") != std::string::npos) {
		GW::Chat::WriteChat(GW::Chat::Channel::CHANNEL_GLOBAL, "Common_crafting_material");
		// The information is in a <td> entry, with a header containing a link to wiki/Common_crafting_material.
		// On salvagable material pages, there will be multiple material links. The table we are looking for is
		// right aligned with align="right" or "float:right"
		ExtractMaterialFromWiki(wiki_page, "<th.*?Common_crafting_material.*?</th>.*?<td>(.*?)</td>");
	}

	if (wiki_page.find("Rare_crafting_material") != std::string::npos) {
		GW::Chat::WriteChat(GW::Chat::Channel::CHANNEL_GLOBAL, "Rare_crafting_material");
		ExtractMaterialFromWiki(wiki_page, "<th.*?Rare_crafting_material.*?</th>.*?<td>(.*?)</td>");
	}
}

static void WikiSalvageCheckCallback(std::string html) {
	WikiParseSalvage(html);
}

static void WikiSalvageCheck(std::wstring wiki_url) {
	Resources::Instance().Download(wiki_url, WikiSalvageCheckCallback);
}

static void WikiItemOpen(std::wstring wiki_url) {
	ShellExecuteW(NULL, L"open", wiki_url.c_str(), NULL, NULL, SW_SHOWNORMAL);
}

void QuickWikiModule::ItemClickCallback(GW::HookStatus*, uint32_t type, uint32_t slot, GW::Bag* bag) {
	std::cout << "QuickWikiModule::ItemClickCallback" << std::endl;
	if (!ImGui::IsKeyDown(VK_SHIFT)) return;
	if (!(QuickWikiModule::Instance().shift_click_show_wiki_url or
		  QuickWikiModule::Instance().shift_click_item_open_wiki or 
		  QuickWikiModule::Instance().shift_click_parse_wiki_salvage)) return;
	if (type != 7) return;

	bool is_inventory_item = bag->IsInventoryBag();
	bool is_storage_item = bag->IsStorageBag();
	if (!is_inventory_item && !is_storage_item) return;

	GW::Item* item = GW::Items::GetItemBySlot(bag, slot + 1);
	if (!item) return;
#ifdef _DEBUG
	printf("\nitem->model_file_id: %d\n", item->model_file_id);
#endif

	std::wstring wiki_url = DecodeToWikiLink(item->name_enc);

	if (QuickWikiModule::Instance().shift_click_show_wiki_url)
		GW::Chat::WriteChat(GW::Chat::Channel::CHANNEL_GLOBAL, BuildChatLink(wiki_url).c_str());

	if (QuickWikiModule::Instance().shift_click_item_open_wiki)
		WikiItemOpen(wiki_url);

	if (QuickWikiModule::Instance().shift_click_parse_wiki_salvage and item->is_material_salvageable)
		WikiSalvageCheck(wiki_url);
}

void QuickWikiModule::SignalTerminate() {
	//std::cout << "QuickWikiModule::SignalTerminate" << std::endl;
}

void QuickWikiModule::Terminate() {
	GW::Items::RemoveItemClickCallback(&ItemClickCallback_Entry);
}

void QuickWikiModule::Update(float delta) {
	//std::cout << "QuickWikiModule::Update" << std::endl;
}

void QuickWikiModule::LoadSettings(CSimpleIni* ini) {
	ToolboxModule::LoadSettings(ini);

	shift_click_show_wiki_url = ini->GetBoolValue(Name(), VAR_NAME(shift_click_show_wiki_url), shift_click_show_wiki_url);
	shift_click_item_open_wiki = ini->GetBoolValue(Name(), VAR_NAME(shift_click_item_open_wiki), shift_click_item_open_wiki);
	shift_click_parse_wiki_salvage = ini->GetBoolValue(Name(), VAR_NAME(shift_click_parse_wiki_salvage), shift_click_parse_wiki_salvage);
}

void QuickWikiModule::SaveSettings(CSimpleIni* ini) {
	ToolboxModule::SaveSettings(ini);

	ini->SetBoolValue(Name(), VAR_NAME(shift_click_show_wiki_url), shift_click_show_wiki_url);
	ini->SetBoolValue(Name(), VAR_NAME(shift_click_item_open_wiki), shift_click_item_open_wiki);
	ini->SetBoolValue(Name(), VAR_NAME(shift_click_parse_wiki_salvage), shift_click_parse_wiki_salvage);
}

void QuickWikiModule::DrawSettingInternal() {
    ImGui::Text("Shift + Click item");
	ImGui::Indent();
	ImGui::Checkbox("shows (potential) wiki url", &shift_click_show_wiki_url);
	ImGui::Checkbox("opens (potential) wiki page", &shift_click_item_open_wiki);
	ImGui::Checkbox("attemps to read salvage info from wiki (EXPERIMENTAL)", &shift_click_parse_wiki_salvage);
}
