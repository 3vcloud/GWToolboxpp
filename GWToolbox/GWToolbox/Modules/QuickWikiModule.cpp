#include "stdafx.h"

#include <GWCA/Managers/ChatMgr.h>
#include <GWCA/Managers/ItemMgr.h>
#include <GWCA/Managers/UIMgr.h>

#include "QuickWikiModule.h"
#include "Modules/Resources.h"

#include "logger.h"
#include <Timer.h>

namespace {}

void QuickWikiModule::Initialize() {
	std::cout << "QuickWikiModule::Initialize" << std::endl;
	ToolboxModule::Initialize();
	GW::Items::RegisterItemClickCallback(&ItemClickCallback_Entry, QuickWikiModule::ItemClickCallback);
}

static void ExtractCraftingFromWiki(std::string wiki_page, std::string regex_str) {
	std::cout << regex_str << std::endl;

	std::smatch sm1;
	std::regex regex_check1(regex_str);

	// grab the right <td></td>
	std::regex_search(wiki_page, sm1, regex_check1);

	std::cout << "sm1.length: " << sm1.size() << std::endl;

	for (auto m : sm1) {
		std::cout << '[' << m << ']' << std::endl;
	}

	if (sm1.size() == 2) {
		// TODO: replace <br /> with newline

		// strip all tags
		std::regex regex_tags("<[^>]*>");
		std::string final = std::regex_replace(sm1.str(1), regex_tags, "");// "!");

		std::cout << final << std::endl;

		GW::Chat::WriteChat(GW::Chat::Channel::CHANNEL_GLOBAL, final.c_str());
	}
}

static void ParseWiki(std::string wiki_page) {
	std::cout << "wiki_page.length: " << wiki_page.length() << std::endl;
	wiki_page.erase(std::remove(wiki_page.begin(), wiki_page.end(), '\n'), wiki_page.end());
	wiki_page.erase(std::remove(wiki_page.begin(), wiki_page.end(), '\r'), wiki_page.end());
	wiki_page.erase(std::remove(wiki_page.begin(), wiki_page.end(), '\t'), wiki_page.end());

	//std::cout << wiki_page << std::endl;

	std::cout << "wiki_page.length: " << wiki_page.length() << std::endl;

	GW::Chat::WriteChat(GW::Chat::Channel::CHANNEL_GLOBAL, "Common_crafting_material");
	ExtractCraftingFromWiki(wiki_page, "Common_crafting_material.*?<td>(.*?)</td>");

	GW::Chat::WriteChat(GW::Chat::Channel::CHANNEL_GLOBAL, "Rare_crafting_material");
	ExtractCraftingFromWiki(wiki_page, "Rare_crafting_material.*?<td>(.*?)</td>");
}

static void WikiSalvageCallback(std::string html) {

	//std::string test_str = "asdasd a dADS  \n3 r234wr atr 34ta34 <tr valign=\"top\">\n<th style=\"background-color:#FA5;\"> <span\nstyle=\"white-space:nowrap;\"><a href=\"/wiki/Common_crafting_material\" title=\"Common crafting material\">Common salvage</a></span></th><td> <a href=\"/wiki/Pile_of_Glittering_Dust\" title=\"Pile of Glittering Dust\">Pile of Glittering Dust</a></td></tr><tr valign=\"top\">\n<th style=\"background-color:#FA5;\"> <span\nstyle=\"white-space:nowrap;\"><a href=\"/wiki/Common_crafting_material\" title=\"Common crafting material\">Common salvage</a></span></th><td> <a href=\"/wiki/Pile_of_Glittering_Dust\" title=\"Pile of Glittering Dust\">Pile of Glittering Dust</a></td></tr>";
	//std::string test_str2 = "<table align=\"right\" style=\"margin:0 0 0.5em 1em; border:1px solid silver; font-size:90%; text-align:left; background-color:white;\" cellpadding=\"5\" cellspacing=\"3\"><tr><th colspan=\"2\" style=\"font-size:110%; text-align:center; background-color:#FA5;\"> Pronged Fan</th></tr><tr><td colspan=\"2\" align=\"center\"> <a href=\"/wiki/File:Pronged_Fan.jpg\" class=\"image\"><img alt=\"Pronged Fan.jpg\" src=\"/images/e/e8/Pronged_Fan.jpg\" width=\"250\" height=\"379\" /></a></td></tr><tr valign=\"top\"><th style=\"background-color:#FA5;\"> <a href=\"/wiki/Weapon#Weapon_types\" title=\"Weapon\">Type</a></th><td> <a href=\"/wiki/Focus_item\" title=\"Focus item\">Focus item</a></td></tr><tr valign=\"top\"><th style=\"background-color:#FA5;\"> <a href=\"/wiki/Campaign\" title=\"Campaign\">Campaign(s)</a></th><td> <a href=\"/wiki/Factions\" class=\"mw-redirect\" title=\"Factions\">Factions</a></td></tr><tr valign=\"top\"><th style=\"background-color:#FA5;\"> <a href=\"/wiki/Attribute\" title=\"Attribute\">Attribute</a><br /><a href=\"/wiki/Requirement\" title=\"Requirement\">requirement(s)</a></th><td> <a href=\"/wiki/Divine_Favor\" title=\"Divine Favor\">Divine Favor</a><br /><a href=\"/wiki/Energy_Storage\" title=\"Energy Storage\">Energy Storage</a><br /><a href=\"/wiki/Inspiration_Magic\" title=\"Inspiration Magic\">Inspiration Magic</a><br /><a href=\"/wiki/Soul_Reaping\" title=\"Soul Reaping\">Soul Reaping</a><br /><a href=\"/wiki/Spawning_Power\" title=\"Spawning Power\">Spawning Power</a></td></tr><tr valign=\"top\"><th style=\"background-color:#FA5;\"> <span style=\"white-space:nowrap;\"><a href=\"/wiki/PvP_reward\" title=\"PvP reward\">PvP reward class</a></span></th><td> Exotic<br />5 <span style=\"position:relative;top:-1.5px\"><a href=\"/wiki/File:Gold_Zaishen_Coin.png\" class=\"image\" title=\"Gold Zaishen Coin\"><img alt=\"Gold Zaishen Coin\" src=\"/images/thumb/2/26/Gold_Zaishen_Coin.png/19px-Gold_Zaishen_Coin.png\" width=\"19\" height=\"19\" srcset=\"/images/thumb/2/26/Gold_Zaishen_Coin.png/29px-Gold_Zaishen_Coin.png 1.5x, /images/thumb/2/26/Gold_Zaishen_Coin.png/38px-Gold_Zaishen_Coin.png 2x\" /></a></span></td></tr><tr valign=\"top\"><th style=\"background-color:#FA5;\"> <span style=\"white-space:nowrap;\"><a href=\"/wiki/Common_crafting_material\" title=\"Common crafting material\">Common salvage</a></span></th><td> <a href=\"/wiki/Pile_of_Glittering_Dust\" title=\"Pile of Glittering Dust\">Pile of Glittering Dust</a></td></tr><tr valign=\"top\"><th style=\"background-color:#FA5;\"> <span style=\"white-space:nowrap;\"><a href=\"/wiki/Rare_crafting_material\" title=\"Rare crafting material\">Rare salvage</a></span></th><td> ~9 <a href=\"/wiki/Roll_of_Parchment\" title=\"Roll of Parchment\">Rolls of Parchment</a></td></tr><tr valign=\"top\"><th style=\"background-color:#FA5;\"> <span style=\"white-space:nowrap;\"><a href=\"/wiki/Inventory\" title=\"Inventory\">Inventory icon</a></span></th><td> <a href=\"/wiki/File:Pronged_Fan.png\" class=\"image\"><img alt=\"Pronged Fan.png\" src=\"/images/7/79/Pronged_Fan.png\" width=\"64\" height=\"64\" /></a></td></tr></table><table style=\"margin: 0.5em; border: 1px solid silver;\" cellpadding=\"5\" cellspacing=\"2\"><tr><td style=\"border:1px solid #aaa;\"> <a href=\"/wiki/File:Pronged_Fan_dye_chart.jpg\" class=\"image\"><img alt=\"Pronged Fan dye chart.jpg\" src=\"/images/thumb/9/97/Pronged_Fan_dye_chart.jpg/500px-Pronged_Fan_dye_chart.jpg\" width=\"500\" height=\"339\" srcset=\"/images/thumb/9/97/Pronged_Fan_dye_chart.jpg/750px-Pronged_Fan_dye_chart.jpg 1.5x, /images/9/97/Pronged_Fan_dye_chart.jpg 2x\" /></a></td></tr></table>";
	//ParseWiki(test_str);
	//ParseWiki(test_str2);
	ParseWiki(html);


}

static void WikiSalvageCheck(GW::Item* item) {
	std::wstring decoded;
	std::wstring wiki_url(L"https://wiki.guildwars.com/wiki/");

	GW::UI::AsyncDecodeStr(item->name_enc, &decoded);

	std::wstring item_url = std::regex_replace(decoded, std::wregex(L" "), std::wstring(L"_"));
	std::wstring final_url = wiki_url + item_url;
	std::wstring chat_url = std::wstring(L"<a=1>") + final_url + std::wstring(L"</a>");

	printf("<%ls>\n", decoded.c_str());
	printf("<%ls>\n", item_url.c_str());
	printf("<%ls>\n", chat_url.c_str());

	GW::Chat::WriteChat(GW::Chat::Channel::CHANNEL_GLOBAL, chat_url.c_str());
	ShellExecuteW(NULL, L"open", final_url.c_str(), NULL, NULL, SW_SHOWNORMAL);
	//Resources::Instance().Download(L"https://wiki.guildwars.com/wiki/Pronged_Fan", WikiSalvageCallback);
	Resources::Instance().Download(final_url, WikiSalvageCallback);
}


void QuickWikiModule::ItemClickCallback(GW::HookStatus*, uint32_t type, uint32_t slot, GW::Bag* bag) {
	std::cout << "QuickWikiModule::ItemClickCallback" << std::endl;
	if (false) return; // todo settings
	if (!ImGui::IsKeyDown(VK_SHIFT)) return;
	if (type != 7) return;

	bool is_inventory_item = bag->IsInventoryBag();
	bool is_storage_item = bag->IsStorageBag();
	if (!is_inventory_item && !is_storage_item) return;

	GW::Item* item = GW::Items::GetItemBySlot(bag, slot + 1);
	if (!item) return;

	printf("\nitem->model_file_id: %d\n", item->model_file_id);

	WikiSalvageCheck(item);
}


void QuickWikiModule::SignalTerminate() {
	std::cout << "QuickWikiModule::SignalTerminate" << std::endl;
}

void QuickWikiModule::Update(float delta) {
	//std::cout << "QuickWikiModule::Update" << std::endl;
}

void QuickWikiModule::LoadSettings(CSimpleIni* ini) {
	ToolboxModule::LoadSettings(ini);
	std::cout << "QuickWikiModule::LoadSettings" << std::endl;

	// enabled = ini->GetBoolValue(Name(), VAR_NAME(enabled), enabled);
}

void QuickWikiModule::SaveSettings(CSimpleIni* ini) {
	ToolboxModule::SaveSettings(ini);
	std::cout << "QuickWikiModule::LoadSettings" << std::endl;

	// ini->SetBoolValue(Name(), VAR_NAME(enabled), enabled);
}

void QuickWikiModule::DrawSettingInternal() {
	std::cout << "QuickWikiModule::DrawSettingInternal" << std::endl;
    ImGui::TextDisabled("QuickWikiModule::DrawSettingInternal");
}
