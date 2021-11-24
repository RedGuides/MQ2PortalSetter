// Project: MQ2PortalSetter
// Author: DigitalPixel
// Version: October 31, 2014 6:45 PM
// Updated for Burning Lands - Dewey 12-11-2018


#include <mq/Plugin.h>
#include <imgui.h>
#include <mq/imgui/ImGuiUtils.h>

PreSetup("MQ2PortalSetter");
PLUGIN_VERSION(2021.1123);

constexpr int ZONEID_GUILD_HALL = 345;

int currentRoutineStep = 0;

static bool s_showWindow = false;
static bool s_focusWindow = false;

bool bDisplaySearch = true;
bool bGroupZonesByEra = false;

std::string portalStoneName;

void setPortal(std::string setPortalStoneName);
void ImGui_OnUpdate();
bool isMerchantPortalSetter();

PLUGIN_API void OnUpdateImGui()
{
	if (GetGameState() == GAMESTATE_INGAME)
	{
		if (pMerchantWnd && pMerchantWnd->IsVisible() && currentRoutineStep < 4 && isMerchantPortalSetter())
		{
			s_showWindow = true;
			ImGui_OnUpdate();
		}
	}
}

void SetStoneAndStep(std::string string, int step = 1)
{
	portalStoneName = string;
	currentRoutineStep = step;
}

// order matters here, so map vs unordered.
// TODO:: add indicator for each xpac they come available for usage to only display if that xpac is unlocked
// { "shortname", std::make_tuple("Zone Long Name", "Button Display", "Portal Stone Item Name") }
std::map<const char* , const std::tuple<const char* , const char*, const char*>> mZoneInfo = {
	{ "maidentwo", std::make_tuple("Maiden's Eye", "Maiden's Eye (ToL)", "Gem of the Maiden's Tempest") },
	{ "cobaltscartwo", std::make_tuple("Cobalt Scar", "Cobalt Scar (CoV)", "Othmir Clamshell") },
	{ "eastwastetwo", std::make_tuple("The Eastern Wastes", "Eastern Wastes (ToV)", "Brilliant Frigid Gemstone") },
	{ "stratos", std::make_tuple("Stratos: Zephyr's Flight", "Stratos", "Burning Lamp") },
	{ "overtheretwo", std::make_tuple("The Overthere", "Overthere", "Miniature Worker's Sledge Mallet") },
	{ "lcaenium", std::make_tuple("Lcaenium", "Lcaenium", "Fragment of the Combine Spire") },
	{ "kattacastrumb", std::make_tuple("Katta Castrum, The Deluge", "Katta", "Drowned Katta Castrum Powerstone") },
	{ "ethernere", std::make_tuple("Ethernere Tainted West Karana", "Ethernere Tainted", "Stormstone of the West") },
	{ "shardslanding", std::make_tuple("Shard's Landing", "Shard's Landing", "Stone of the Shard's Fall") },
	{ "Argath", std::make_tuple("Argath", "Argath", "Chunk of Argathian Steel") },
	{ "feerrott2", std::make_tuple("The Feerrott: The Dream", "Feerrott: The Dream", "Crystallized Dream of the Feerott") },
	{ "brellsrest", std::make_tuple("Brell's Rest", "Brell's Rest", "Unrefined Brellium Ore") },
	{ "dragonscale", std::make_tuple("Dragonscale Hills", "Dragonscale Hills", "Dragonscale Faycite") },
	{ "potime", std::make_tuple("The Plane of Time", "Plane of Time", "Broken Timestone") },
	{ "kattacastrum", std::make_tuple("Katta Castrum", "Katta", "Katta Castrum Powerstone") },
	{ "mesa", std::make_tuple("Gor`Kar Mesa", "Goru`kar Mesa", "Goru'kar Mesa Sandstone") }, // ' and not ` for the "Goru'kar mesa sandstone"
	{ "arcstone", std::make_tuple("Arcstone", "Arcstone", "Arcstone Spirit Sapphire") },
	{ "posky", std::make_tuple("The Plane of Sky", "Plane of Sky", "Cloudy Stone of Veeshan") },
	{ "cobaltscar", std::make_tuple("Cobalt Scar", "Cobalt Scar", "Velium Shard of Cobalt Scar") },
	{ "pohate", std::make_tuple("The Plane of Hate", "Plane of Hate", "Fuligan Soulstone of Innoruuk") },
	{ "barindu", std::make_tuple("Barindu, Hanging Gardens", "Barindu", "Etched Marble of Barindu") },
	{ "wallofslaughter", std::make_tuple("Wall of Slaughter", "Wall of Slaughter", "Chipped Shard of Slaughter") },
	{ "twilight", std::make_tuple("The Twilight Sea", "Twilight Sea", "Shadowed Sand of the Twilight Sea") },
	{ "eastkorlach", std::make_tuple("Undershore", "Undershore", "Undershore Coral") },
	{ "dreadlands", std::make_tuple("Dreadlands", "Dreadlands", "Shattered Bone of the Dreadlands") },
	{ "stonebrunt", std::make_tuple("Stonebrunt Mountains", "Stonebrunt", "Moss Agate of Stonebrunt") },
	{ "iceclad", std::make_tuple("Iceclad Ocean", "Iceclad Ocean", "Frozen Shard of Iceclad") },
	{ "lavastorm", std::make_tuple("Lavastorm", "Lavastorm", "Lavastorm Magma") },
	{ "tox",  std::make_tuple("Toxxulia","Toxxulia", "Opal of Toxxulia") },
	{ "northkarana", std::make_tuple("North Karana", "North Karana", "Karana Plains Pebble") },
	{ "commonlands", std::make_tuple("Commonlands", "Commonlands", "Grassy Pebble of the Commonalnds") },
	{ "gfaydark", std::make_tuple("The Greater Faydark", "Greater Faydark", "Forest Emerald of Faydark") },
};

void DrawPortalSetterPanel()
{
	const ImVec2 halfsize = ImVec2(ImGui::GetWindowSize().x * 0.5f, 0.0f);
	const ImVec2 fullsize = ImVec2(ImGui::GetWindowSize().x * 1.0f, 0.0f);
	const bool bEven = mZoneInfo.size() % 2 == 0;
	char input[64] = {};
	static char spellName[64] = { 0 };

	if (bDisplaySearch) {
		ImGui::Text("Type in short or long zone name.");
		bool shortNameInput = ImGui::InputTextWithHint("zone name", "example: potime / the plane of time", input, IM_ARRAYSIZE(input));
		ImGui::SameLine();
		mq::imgui::HelpMarker("You can type in the zone's longname or shortname, and it will display the button to select for that zone.");

		for (auto it = mZoneInfo.begin(); it != mZoneInfo.end(); it++)
		{
			static int iMatch = -1; // iMatch is static so we continue to display while clicking or typing
			if (ci_equals(input, it->first) || ci_equals(input, std::get<0>(it->second)))
				iMatch = std::distance(mZoneInfo.begin(), it);

			if (iMatch == std::distance(mZoneInfo.begin(), it))
			{
				if (ImGui::Button(std::get<1>(it->second), fullsize))
				{
					SetStoneAndStep(std::get<2>(it->second));
				}
			}
		}
	}

	ImGui::Separator();
	// TODO: Would like a way to get the information from the guild window "Banner" tab
	// so we can display if and where the guild banner is already assigned to
	if (ImGui::Button("Guild Banner", fullsize))
	{
		SetStoneAndStep("Splinter from a Guild Standard");
	}
	ImGui::Separator();

	static bool bModern = false;
	if (bGroupZonesByEra) {
		bModern = ImGui::CollapsingHeader("Modern");
	}

	if (bGroupZonesByEra ? bModern : true)
	{
		// if vZoneInfo.size() is even, we want to to display 4 elements, otherwise 5
		// this ensures we don't have an odd number of halfsized buttons.
		for (int i = 0; i < (bEven ? 4 : 5); i++)
		{
			auto it = mZoneInfo.begin();
			std::advance(it, i);
			if (ImGui::Button(std::get<1>(it->second), fullsize))
			{
				SetStoneAndStep(std::get<2>(it->second));
			}
		}
	}
	ImGui::Separator();

	static bool bOlder = false;

	if (bGroupZonesByEra) {
		bOlder = ImGui::CollapsingHeader("Older");
	}

	if (bGroupZonesByEra ? bOlder : true)
	{
		for (unsigned int i = (bEven ? 4 : 5); i < mZoneInfo.size(); i++)
		{
			auto it = mZoneInfo.begin();
			std::advance(it, i);
			if (i % 2)
			{
				ImGui::SameLine();
				if (ImGui::Button(std::get<1>(it->second), halfsize))
				{
					SetStoneAndStep(std::get<2>(it->second));
				}
			}
			else {
				if (ImGui::Button(std::get<1>(it->second), halfsize))
				{
					SetStoneAndStep(std::get<2>(it->second));
				}
			}
		}
	}
}

void DrawSettingsPanel()
{
	if (ImGui::Checkbox("Display Zone Search", &bDisplaySearch))
	{
		WritePrivateProfileBool("Settings", "DisplaySearch", bDisplaySearch, INIFileName);
	}
	ImGui::SameLine();
	mq::imgui::HelpMarker("Allows the zone shortname / longname search bar.\n\nINI Setting: DisplaySearch");

	if (ImGui::Checkbox("Group Modern / Older Zones,", &bGroupZonesByEra))
	{
		WritePrivateProfileBool("Settings", "GroupZonesByEra", bGroupZonesByEra, INIFileName);
	}
	ImGui::SameLine();
	mq::imgui::HelpMarker("Allows grouping of buttons in \"Modern\" and \"Older\" as a collapsible header.\n\nINI Setting: GroupZonesByEra");
}

void ImGui_OnUpdate()
{
	if (!s_showWindow)
		return;

	ImGui::SetNextWindowSize(ImVec2(400, 440), ImGuiCond_FirstUseEver);

	if (s_focusWindow)
	{
		s_focusWindow = false;
		ImGui::SetNextWindowFocus();
	}

	if (ImGui::Begin("Portal Setter", &s_showWindow, ImGuiWindowFlags_MenuBar))
	{

		if (ImGui::BeginTabBar("PortalSetterTab", ImGuiTabBarFlags_None))
		{

			if (ImGui::BeginTabItem("Portal Setter"))
			{
				if (ImGui::BeginChild("##Portal Setter"))
				{
					DrawPortalSetterPanel();
				}
				ImGui::EndChild();
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Settings"))
			{
				if (ImGui::BeginChild("##Settings"))
				{
					DrawSettingsPanel();
				}
				ImGui::EndChild();
				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}
	}
	ImGui::End();
}

// char* for Target to work
char* getPortalVendorName() {
	if (pLocalPC->zoneId == ZONEID_GUILD_HALL)
		return "Zeflmin Werlikanin";
	return "Teleportation Assistant";
}

bool isMerchantPortalSetter() {
	if (pActiveMerchant)
	{
		char zMerchantName[EQ_MAX_NAME];
		strcpy_s(zMerchantName, pActiveMerchant->Name);
		CleanupName(zMerchantName, sizeof(zMerchantName), false, false);
		if (strstr(zMerchantName, getPortalVendorName()))
		{
			return true;
		}
	}
	return false;
}

bool inPortalMerchantRange() {
	MQSpawnSearch ssSpawn;
	ClearSearchSpawn(&ssSpawn);
	ssSpawn.FRadius = 20;
	ssSpawn.SpawnType = NPC;
	strcpy_s(ssSpawn.szName, getPortalVendorName());
	return SearchThroughSpawns(&ssSpawn, pControlledPlayer) != nullptr;
}

void setPortal(std::string setPortalStoneName) {
	switch (currentRoutineStep) {
		case 1: {
			if (GetPcProfile()->GetInventorySlot(InvSlot_Cursor)) {
				WriteChatColor("[MQ2PortalSetter] Your cursor must be empty to use portal setter.", CONCOLOR_YELLOW);
				currentRoutineStep = 0;
			} else if (GetFreeInventory(0) < 1) {
				WriteChatColor("[MQ2PortalSetter] You must have a free inventory slot to use portal setter.", CONCOLOR_YELLOW);
				currentRoutineStep = 0;
			} else if (FindInventoryItemCountByName(setPortalStoneName.c_str()) > 0) {
				WriteChatf("[MQ2PortalSetter] Using existing %s", setPortalStoneName.c_str());
				currentRoutineStep = 3;
			} else {
				if (CXWnd* merchantwnd = FindMQ2Window("MerchantWnd"))
				{
					if (CListWnd* cLWnd = (CListWnd*)merchantwnd->GetChildItem("MW_ItemList"))
					{
						for (int i = 0; i < cLWnd->ItemsArray.GetLength(); ++i)
						{
							CXStr itemName = cLWnd->GetItemText(i, 1);
							if (string_equals(setPortalStoneName, itemName))
							{
								SendListSelect("MerchantWnd", "MW_ItemList", i);
								currentRoutineStep++;
								break;
							}
						}
					}
				}
			}
			break;
		}
		case 2: {
			EzCommand("/ctrl /notify MerchantWnd MW_Buy_Button leftmouseup");
			currentRoutineStep++;
			break;
		}
		case 3: {
			if(FindItemByName(&setPortalStoneName[0], true)) {
				char zNotifyCommand[MAX_STRING];
				SendWndClick("MerchantWnd", "MW_DONE_BUTTON", "leftmouseup");
				sprintf_s(zNotifyCommand, "/itemnotify \"%s\" leftmouseup", &setPortalStoneName[0]);
				EzCommand(zNotifyCommand);
				currentRoutineStep++;
			}
			break;
		}
		case 4: {
			Target(GetCharInfo()->pSpawn, getPortalVendorName());
			currentRoutineStep++;
			break;
		}
		case 5: {
			if (pTarget && inPortalMerchantRange()) {
				if (GetPcProfile()->GetInventorySlot(InvSlot_Cursor)) {
					EzCommand("/click left target");
					currentRoutineStep++;
				}
			} else {
				currentRoutineStep--;
			}
			if (pGiveWnd->IsVisible() && currentRoutineStep == 5) {
				s_showWindow = true;
				currentRoutineStep++;
			}
			break;
		}
		case 6: {
			if (pGiveWnd->IsVisible()) {
				SendWndClick("GiveWnd", "GVW_Give_Button", "leftmouseup");
				currentRoutineStep++;
			} else {
				currentRoutineStep--;
			}
			break;
		}
		case 7: {
			if(!pGiveWnd->IsVisible()) {
				currentRoutineStep = 0;
			}
			break;
		}
		default:
			currentRoutineStep = 0;
	}
}

void LoadPortalSetterSettings()
{
	bDisplaySearch = GetPrivateProfileBool("Settings", "DisplaySearch", bDisplaySearch, INIFileName);
	bGroupZonesByEra = GetPrivateProfileBool("Settings", "GroupZonesByEra", bGroupZonesByEra, INIFileName);
}


PLUGIN_API void InitializePlugin()
{
	DebugSpewAlways("Initializing MQ2PortalSetter");
	LoadPortalSetterSettings();
}

PLUGIN_API void ShutdownPlugin()
{
	DebugSpewAlways("Shutting down MQ2PortalSetter");
}

PLUGIN_API void OnPulse()
{
	                                                        // Catch all types of Guild Halls
	if (GetGameState() != GAMESTATE_INGAME || !pZoneInfo || strstr(pZoneInfo->LongName, "Guild Hall") == nullptr)
		return;

	static std::chrono::steady_clock::time_point PulseTimer = std::chrono::steady_clock::now();
	// Run only after timer is up
	if (std::chrono::steady_clock::now() > PulseTimer)
	{
		//-- If we are out of range then reset state.
		if (currentRoutineStep > 0 && !inPortalMerchantRange())
		{
			WriteChatColor("[MQ2PortalSetter] Out of range of portal attendant, aborting.", CONCOLOR_RED);
			s_showWindow = false;
			currentRoutineStep = 0;
		}

		setPortal(portalStoneName);

		// Wait 1 second before running again
		PulseTimer = std::chrono::steady_clock::now() + std::chrono::seconds(1);
	}
}
