// Project: MQ2PortalSetter
// Author: DigitalPixel
// Version: October 31, 2014 6:45 PM
// Updated for Burning Lands - Dewey 12-11-2018


#include <mq/Plugin.h>
#include <imgui.h>
#include <mq/imgui/ImGuiUtils.h>

PreSetup("MQ2PortalSetter");
PLUGIN_VERSION(2021.1216);

constexpr int ZONEID_GUILD_HALL = 345;

int currentRoutineStep = 0;

static bool bShowWindow = false;

bool bDisplaySearch = true;
bool bGroupZonesByEra = false;

std::string portalStoneName;

void setPortal(const std::string& setPortalStoneName);
void ImGui_OnUpdate();
bool isMerchantPortalSetter();

class MQ2PortalSetterType : public MQ2Type
{
public:
	enum PortalSetterMembers
	{
		Version = 1,
		Step,
		InProgress,
	};

	MQ2PortalSetterType() : MQ2Type("PortalSetter")
	{
		ScopedTypeMember(PortalSetterMembers, Version);
		ScopedTypeMember(PortalSetterMembers, Step);
		ScopedTypeMember(PortalSetterMembers, InProgress);
	};

	~MQ2PortalSetterType()
	{
	}

	virtual bool GetMember(MQVarPtr VarPtr, const char* Member, char* Index, MQTypeVar& Dest) override
	{
		MQTypeMember* pMember = MQ2PortalSetterType::FindMember(Member);
		if (!pMember)
			return false;
		if (!pLocalPlayer)
			return false;
		switch ((PortalSetterMembers)pMember->ID)
		{
			case Version:
				Dest.Float = MQ2Version;
				Dest.Type = mq::datatypes::pFloatType;
				return true;

			case Step:
				Dest.Int = currentRoutineStep;
				Dest.Type = mq::datatypes::pIntType;
				return true;

			case InProgress:
				Dest.Int = currentRoutineStep > 0;
				Dest.Type = mq::datatypes::pBoolType;
				return true;
		
			default:
				break;
		}
		return false;
	}

	bool ToString(MQVarPtr VarPtr, const char* Destination)
	{
		return true;
	}
};

MQ2PortalSetterType* pPortalSetterType = nullptr;

bool PortalData(const char* szIndex, MQTypeVar& Dest)
{
	Dest.DWord = 1;
	Dest.Type = pPortalSetterType;
	return true;
}

PLUGIN_API void OnUpdateImGui()
{
	if (GetGameState() == GAMESTATE_INGAME)
	{
		if (pMerchantWnd && pMerchantWnd->IsVisible() && currentRoutineStep < 4 && isMerchantPortalSetter())
		{
			bShowWindow = true;
			ImGui_OnUpdate();
		}
	}
}

void SetStoneAndStep(const std::string& stoneName, int step = 1)
{
	portalStoneName = stoneName;
	currentRoutineStep = step;
}

struct zonePortalInfo
{
	const char* shortname;
	const char* longname;
	const char* buttonname;
	const char* stonename;
};

// TODO:: add indicator for each xpac they come available for usage to only display if that xpac is unlocked
// TODO:: find a clean way to allow optional expansion information available by either parenthetical (CoV) and/or mouseover
// -- this was originally not done because for duplicate zones, we should likely *always* have displayed differences, example: Cobalt Scar
const std::vector<zonePortalInfo> s_zoneinfo = {
	{ "maidentwo", "Maiden's Eye", "Maiden's Eye (ToL)", "Gem of the Maiden's Tempest" },
	{ "cobaltscartwo", "Cobalt Scar", "Cobalt Scar (CoV)", "Othmir Clamshell" },
	{ "eastwastetwo", "The Eastern Wastes", "Eastern Wastes (ToV)", "Brilliant Frigid Gemstone" },
	{ "stratos", "Stratos: Zephyr's Flight", "Stratos", "Burning Lamp" },
	{ "overtheretwo", "The Overthere", "Overthere", "Miniature Worker's Sledge Mallet" },
	{ "lcaenium", "Lcaenium", "Lcaenium", "Fragment of the Combine Spire" },
	{ "kattacastrumb", "Katta Castrum, The Deluge", "Katta: Deluge", "Drowned Katta Castrum Powerstone" },
	{ "ethernere", "Ethernere Tainted West Karana", "Ethernere Tainted", "Stormstone of the West" },
	{ "shardslanding", "Shard's Landing", "Shard's Landing", "Stone of the Shard's Fall" },
	{ "Argath", "Argath", "Argath", "Chunk of Argathian Steel" },
	{ "feerrott2", "The Feerrott (B)", "Feerrott: The Dream", "Crystallized Dream of the Feerrott" },
	{ "brellsrest", "Brell's Rest", "Brell's Rest", "Unrefined Brellium Ore" },
	{ "dragonscale", "Dragonscale Hills", "Dragonscale Hills", "Dragonscale Faycite" },
	{ "potime", "The Plane of Time", "Plane of Time", "Broken Timestone" },
	{ "kattacastrum", "Katta Castrum", "Katta", "Katta Castrum Powerstone" },
	{ "mesa", "Gor`Kar Mesa", "Goru`kar Mesa", "Goru'kar Mesa Sandstone" }, // ' and not ` for the "Goru'kar mesa sandstone"
	{ "arcstone", "Arcstone", "Arcstone", "Arcstone Spirit Sapphire" },
	{ "posky", "The Plane of Sky", "Plane of Sky", "Cloudy Stone of Veeshan" },
	{ "cobaltscar", "Cobalt Scar", "Cobalt Scar", "Velium Shard of Cobalt Scar" },
	{ "pohate", "The Plane of Hate", "Plane of Hate", "Fuligan Soulstone of Innoruuk" },
	{ "barindu", "Barindu, Hanging Gardens", "Barindu", "Etched Marble of Barindu" },
	{ "wallofslaughter", "Wall of Slaughter", "Wall of Slaughter", "Chipped Shard of Slaughter" },
	{ "twilight", "The Twilight Sea", "Twilight Sea", "Shadowed Sand of the Twilight Sea" },
	{ "eastkorlach", "Undershore", "Undershore", "Undershore Coral" },
	{ "dreadlands", "Dreadlands", "Dreadlands", "Shattered Bone of the Dreadlands" },
	{ "stonebrunt", "Stonebrunt Mountains", "Stonebrunt", "Moss Agate of Stonebrunt" },
	{ "iceclad", "Iceclad Ocean", "Iceclad Ocean", "Frozen Shard of Iceclad" },
	{ "lavastorm", "Lavastorm", "Lavastorm", "Lavastorm Magma" },
	{ "tox", "Toxxulia", "Toxxulia", "Opal of Toxxulia" },
	{ "northkarana", "North Karana", "North Karana", "Karana Plains Pebble" },
	{ "commonlands", "Commonlands", "Commonlands", "Grassy Pebble of the Commonalnds" },
	{ "gfaydark", "The Greater Faydark", "Greater Faydark", "Forest Emerald of Faydark" },
};

void DrawPortalSetterPanel()
{
	const ImVec2 halfsize = ImVec2(ImGui::GetWindowSize().x * 0.5f, 0.0f);
	const ImVec2 fullsize = ImVec2(ImGui::GetWindowSize().x * 1.0f, 0.0f);
	const bool bEven = s_zoneinfo.size() % 2 == 0;
	const int NumberOfButtons = 4;

	if (bDisplaySearch)
	{
		static char input[64] = {};
		static const zonePortalInfo* save = nullptr;

		ImGui::Text("Type in short or long zone name.");
		ImGui::InputTextWithHint("zone name", "example: potime / the plane of time", input, IM_ARRAYSIZE(input));
		ImGui::SameLine();
		mq::imgui::HelpMarker("You can type in the zone's longname or shortname, and it will display the button to select for that zone.");

		for (const zonePortalInfo& info : s_zoneinfo)
		{
			if (ci_equals(input, info.shortname) || ci_equals(input, info.longname))
			{
				save = &info;
				break;
			}
		}

		if (save)
		{
			if (ImGui::Button(save->buttonname, fullsize))
			{
				SetStoneAndStep(save->stonename);
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

	bool bModern = true;
	if (bGroupZonesByEra)
	{
		bModern = ImGui::CollapsingHeader("Modern");
	}

	if (bModern)
	{
		// if vZoneInfo.size() is even, we want to to display 4 fullsized buttons, otherwise 5
		// this ensures we don't have an odd number of halfsized buttons.
		for (int i = 0; i < (bEven ? NumberOfButtons : NumberOfButtons + 1); i++)
		{
			const zonePortalInfo& info = s_zoneinfo[i];

			if (ImGui::Button(info.buttonname, fullsize))
			{
				SetStoneAndStep(info.stonename);
			}
		}
	}
	ImGui::Separator();

	bool bOlder = true;
	if (bGroupZonesByEra)
	{
		bOlder = ImGui::CollapsingHeader("Older");
	}

	if (bOlder)
	{
		for (unsigned int i = (bEven ? NumberOfButtons : NumberOfButtons + 1); i < s_zoneinfo.size(); i++)
		{
			const zonePortalInfo& info = s_zoneinfo[i];
			if (i % 2)
			{
				ImGui::SameLine();
				if (ImGui::Button(info.buttonname, halfsize))
				{
					SetStoneAndStep(info.stonename);
				}
			}
			else {
				if (ImGui::Button(info.buttonname, halfsize))
				{
					SetStoneAndStep(info.stonename);
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

	if (ImGui::Checkbox("Group Modern / Older Zones", &bGroupZonesByEra))
	{
		WritePrivateProfileBool("Settings", "GroupZonesByEra", bGroupZonesByEra, INIFileName);
	}
	ImGui::SameLine();
	mq::imgui::HelpMarker("Allows grouping of buttons in \"Modern\" and \"Older\" as a collapsible header.\n\nINI Setting: GroupZonesByEra");
}

void ImGui_OnUpdate()
{
	if (!bShowWindow)
		return;

	ImGui::SetNextWindowSize(ImVec2(400, 440), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("Portal Setter", &bShowWindow, ImGuiWindowFlags_MenuBar))
	{
		if (ImGui::BeginTabBar("PortalSetterTab", ImGuiTabBarFlags_None))
		{
			if (ImGui::BeginTabItem("Portal Setter"))
			{
				DrawPortalSetterPanel();
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Settings"))
			{
				DrawSettingsPanel();
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

void setPortal(const std::string& setPortalStoneName) {
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
				if (CXWnd* merchantwnd = pMerchantWnd)
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
			if(FindItemByName(setPortalStoneName.c_str(), true)) {
				char zNotifyCommand[MAX_STRING];
				SendWndClick("MerchantWnd", "MW_DONE_BUTTON", "leftmouseup");
				sprintf_s(zNotifyCommand, "/itemnotify \"%s\" leftmouseup", setPortalStoneName.c_str());
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
				bShowWindow = true;
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

void PortalSetterCmd(SPAWNINFO* pChar, char* szLine)
{
	char Arg[MAX_STRING] = { 0 };
	GetArg(Arg, szLine, 1);
	if (strlen(Arg)) {

		static const zonePortalInfo* save = nullptr;
		for (const zonePortalInfo& info : s_zoneinfo)
		{
			if (ci_equals(Arg, info.shortname) || ci_equals(Arg, info.longname))
			{
				save = &info;
				SetStoneAndStep(save->stonename);
				return;
			}
			else {

			}
		}
	}
	else {
		WriteChatf("\ar[\a-tMQ2PortalSetter\ar]\ao:: \arPlease provide a long or shortname for the zone you wish to set to portal to.");
		WriteChatf("\ar[\a-tMQ2PortalSetter\ar]\ao:: \ayExample: \ao /portalsetter eastwastetwo \ax or \ao /portalsetter \ay\"The Eastern Wastes\"");
	}

}

PLUGIN_API void InitializePlugin()
{
	DebugSpewAlways("Initializing MQ2PortalSetter");
	AddCommand("/portalsetter", PortalSetterCmd, false, true, true);
	pPortalSetterType = new MQ2PortalSetterType;
	AddMQ2Data("PortalSetter", PortalData);
	LoadPortalSetterSettings();
	AddSettingsPanel("plugins/PortalSetter", DrawSettingsPanel);
}

PLUGIN_API void ShutdownPlugin()
{
	DebugSpewAlways("Shutting down MQ2PortalSetter");

	RemoveCommand("/portalsetter");
	RemoveMQ2Data("PortalSetter");
	delete pPortalSetterType;
	RemoveSettingsPanel("plugins/PortalSetter");
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
			WriteChatf("\ar[\a-tMQ2PortalSetter\ar]\ao:: \arOut of range of portal attendant, aborting.");
			bShowWindow = false;
			currentRoutineStep = 0;
		}

		setPortal(portalStoneName.c_str());

		// Wait 1 second before running again
		PulseTimer = std::chrono::steady_clock::now() + std::chrono::seconds(1);
	}
}
