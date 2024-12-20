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

int vendorID = 0;

static bool bShowWindow = false;

bool bDisplaySearch = true;
bool bGroupZonesByEra = false;

std::string portalStoneName;
#define PLUGINMSG "\ar[\a-tPortal Setter\ar]\ao::\aw "

struct zonePortalInfo
{
	const char* shortname;
	const char* longname;
	const char* buttonname;
	const char* stonename;
	int expansion;
};

std::vector<zonePortalInfo> displayableZones;

void setPortal(const std::string& setPortalStoneName);
void ImGui_OnUpdate();
int SetAndGetVendorID();

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
		ScopedTypeMember(PortalSetterMembers, Step);
		ScopedTypeMember(PortalSetterMembers, InProgress);
	};

	virtual bool GetMember(MQVarPtr VarPtr, const char* Member, char* Index, MQTypeVar& Dest) override
	{
		MQTypeMember* pMember = MQ2PortalSetterType::FindMember(Member);
		if (!pMember)
			return false;
		if (!pLocalPlayer)
			return false;
		switch ((PortalSetterMembers)pMember->ID)
		{
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
};

MQ2PortalSetterType* pPortalSetterType = nullptr;

bool PortalData(const char* szIndex, MQTypeVar& Dest)
{
	Dest.DWord = 1;
	Dest.Type = pPortalSetterType;
	return true;
}

// TODO:: find a clean way to allow optional expansion information available by either parenthetical (CoV) and/or mouseover
// -- this was originally not done because for duplicate zones, we should likely *always* have displayed differences, example: Cobalt Scar
// EXPANSION Info: Guild Hall (and thus porter) wasn't introduced until Dragons of Norrath.
const std::vector<zonePortalInfo> s_zoneinfo = {
#ifdef EXPANSION_TOB
	{ "hodstock", "Hodstock Hills", "Hodstock Hills", "Broken Auretite Chain", EXPANSION_TOB },
#endif
	{ "laurioninn", "Laurion's Inn", "Laurion's Inn", "Medal of Heroes", EXPANSION_LS },
	{ "sharvahltwo", "Shar Vahl, Divided", "Shar Vahl (NoS)", "Tri-Spirit Embossed Token", EXPANSION_NOS },
	{ "maidentwo", "Maiden's Eye", "Maiden's Eye (ToL)", "Gem of the Maiden's Tempest", EXPANSION_TOL },
	{ "cobaltscartwo", "Cobalt Scar", "Cobalt Scar (CoV)", "Othmir Clamshell", EXPANSION_COV },
	{ "eastwastestwo", "The Eastern Wastes", "Eastern Wastes (ToV)", "Brilliant Frigid Gemstone", EXPANSION_TOV },
	{ "stratos", "Stratos: Zephyr's Flight", "Stratos", "Burning Lamp", EXPANSION_TBL },
	{ "overtheretwo", "The Overthere", "Overthere", "Miniature Worker's Sledge Mallet", EXPANSION_RoS },
	{ "lceanium", "Lceanium", "Lceanium", "Fragment of the Combine Spire", EXPANSION_EoK },
	{ "kattacastrumb", "Katta Castrum, The Deluge", "Katta: Deluge", "Drowned Katta Castrum Powerstone", EXPANSION_TDS },
	{ "ethernere", "Ethernere Tainted West Karana", "Ethernere Tainted", "Stormstone of the West", EXPANSION_CotF },
	{ "shardslanding", "Shard's Landing", "Shard's Landing", "Stone of the Shard's Fall", EXPANSION_RoF },
	{ "argath", "Argath", "Argath", "Chunk of Argathian Steel", EXPANSION_VoA },
	{ "feerrott2", "The Feerrott (B)", "Feerrott: The Dream", "Crystallized Dream of the Feerrott", EXPANSION_HoT },
	{ "brellsrest", "Brell's Rest", "Brell's Rest", "Unrefined Brellium Ore", EXPANSION_UFT },
	{ "dragonscale", "Dragonscale Hills", "Dragonscale Hills", "Dragonscale Faycite", EXPANSION_SoF },
	{ "potimea", "The Plane of Time", "Plane of Time", "Broken Timestone", EXPANSION_DoN },
	{ "kattacastrum", "Katta Castrum", "Katta", "Katta Castrum Powerstone", EXPANSION_TBS },
	{ "mesa", "Gor`Kar Mesa", "Goru`kar Mesa", "Goru'kar Mesa Sandstone", EXPANSION_TSS }, // ' and not ` for the "Goru'kar mesa sandstone"
	{ "arcstone", "Arcstone", "Arcstone", "Arcstone Spirit Sapphire", EXPANSION_PoR },
	{ "airplane", "The Plane of Sky", "Plane of Sky", "Cloudy Stone of Veeshan", EXPANSION_DoN },
	{ "cobaltscar", "Cobalt Scar", "Cobalt Scar", "Velium Shard of Cobalt Scar", EXPANSION_DoN },
	{ "hateplane", "The Plane of Hate", "Plane of Hate", "Fuligan Soulstone of Innoruuk", EXPANSION_DoN },
	{ "barindu", "Barindu, Hanging Gardens", "Barindu", "Etched Marble of Barindu", EXPANSION_DoN },
	{ "wallofslaughter", "Wall of Slaughter", "Wall of Slaughter", "Chipped Shard of Slaughter", EXPANSION_DoN },
	{ "twilight", "The Twilight Sea", "Twilight Sea", "Shadowed Sand of the Twilight Sea", EXPANSION_DoN },
	{ "eastkorlach", "Undershore", "Undershore", "Undershore Coral", EXPANSION_DoD },
	{ "dreadlands", "Dreadlands", "Dreadlands", "Shattered Bone of the Dreadlands", EXPANSION_DoN },
	{ "stonebrunt", "Stonebrunt Mountains", "Stonebrunt", "Moss Agate of Stonebrunt", EXPANSION_DoN },
	{ "iceclad", "Iceclad Ocean", "Iceclad Ocean", "Frozen Shard of Iceclad", EXPANSION_DoN },
	{ "lavastorm", "Lavastorm", "Lavastorm", "Lavastorm Magma", EXPANSION_DoN },
	{ "tox", "Toxxulia", "Toxxulia", "Opal of Toxxulia", EXPANSION_DoN },
	{ "northkarana", "North Karana", "North Karana", "Karana Plains Pebble", EXPANSION_DoN },
	{ "commonlands", "Commonlands", "Commonlands", "Grassy Pebble of The Commonlands", EXPANSION_DoN },
	{ "gfaydark", "The Greater Faydark", "Greater Faydark", "Forest Emerald of Faydark", EXPANSION_DoN },
};

//We need to validate that we should display a particular portal based on the expansion it is available
void GetZonesToDisplay()
{
		displayableZones.clear();

	for (int i = 0; i < static_cast<int>(s_zoneinfo.size()); ++i)
	{
		if (HasExpansion(s_zoneinfo[i].expansion))
		{
			displayableZones.push_back(s_zoneinfo[i]);
		}
	}
}

PLUGIN_API void OnUpdateImGui()
{
	if (GetGameState() == GAMESTATE_INGAME)
	{
		if (pMerchantWnd && pMerchantWnd->IsVisible() && currentRoutineStep < 4 && SetAndGetVendorID() > 0)
		{
			if (!bShowWindow) {
				bShowWindow = true;
				GetZonesToDisplay();
			}
			ImGui_OnUpdate();
		}
		else
		{
			bShowWindow = false;
		}
	}
}

void SetStoneAndStep(const std::string& stoneName, int step = 1)
{
	portalStoneName = stoneName;
	currentRoutineStep = step;
}

void DrawPortalSetterPanel()
{
	const ImVec2 halfsize = ImVec2(ImGui::GetWindowSize().x * 0.5f, 0.0f);
	const ImVec2 fullsize = ImVec2(ImGui::GetWindowSize().x * 1.0f, 0.0f);
	// if displayableZones.size() is even, we want to to display 4 fullsized buttons, otherwise 5
	// this ensures we don't have an odd number of halfsized buttons.
	const int NumberOfFullSizeButtons = displayableZones.size() % 2 == 0 ? 4 : 5;

	if (bDisplaySearch)
	{
		static char input[64] = {};
		static const zonePortalInfo* save = nullptr;

		ImGui::Text("Type in short or long zone name.");
		ImGui::InputTextWithHint("zone name", "example: potime / the plane of time", input, IM_ARRAYSIZE(input));
		ImGui::SameLine();
		mq::imgui::HelpMarker("You can type in the zone's longname or shortname, and it will display the button to select for that zone.");

		for (const zonePortalInfo& info : displayableZones)
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
		for (int i = 0; i < NumberOfFullSizeButtons; i++)
		{
			const zonePortalInfo& info = displayableZones[i];
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
		for (unsigned int i = NumberOfFullSizeButtons; i < displayableZones.size(); i++)
		{
			const zonePortalInfo& info = displayableZones[i];
			// we need to alternate which side of they are on
			if (i % 2)
			{
				// if the number of fullsize buttons is odd
				// we need to sameline on the even side
				if (NumberOfFullSizeButtons % 2 == 0)
					ImGui::SameLine();

				if (ImGui::Button(info.buttonname, halfsize))
				{
					SetStoneAndStep(info.stonename);
				}
			}
			else
			{
				// if the number of fullsize buttons is even
				// we need to sameline on the odd side
				if (NumberOfFullSizeButtons % 2)
					ImGui::SameLine();

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

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 10);
	ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarRounding, 50);
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5);

	if (ImGui::Begin("Portal Setter", &bShowWindow, ImGuiWindowFlags_None))
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
	ImGui::PopStyleVar(3);
	ImGui::End();
}

PlayerClient* GetVendorSpawn() {
	if (PlayerClient* vendor = GetSpawnByPartialName("Zeflmin Werlikanin"))
	{
		return vendor;
	}

	return GetSpawnByPartialName("Teleportation Assistant");
}

const std::vector<const char*> vendorNames = {
	"Teleportation Assistant",
	"Zeflmin Werlikanin"
};

bool SpawnMatchesVendor() {

	char szCleanName[EQ_MAX_NAME] = { 0 };
	strcpy_s(szCleanName, pActiveMerchant->Name);
	CleanupName(szCleanName, sizeof(szCleanName), false, false);

	return string_equals(szCleanName, vendorNames.at(0)) || string_equals(szCleanName, vendorNames.at(1));
}

void TargetAndOpenVendor() {
	PlayerClient* vendor = nullptr;

	if (vendor = GetSpawnByID(vendorID)) {
		Target(pLocalPlayer, vendor->Name);
	}

	if (pTarget && vendor && pTarget->SpawnID == vendor->SpawnID) {
		if (pMerchantWnd ? !pMerchantWnd->IsVisible() : false) {
#ifndef ROF2EMU
			// only Live has /usetarget
			EzCommand("/usetarget");
#else
			EzCommand("/click right target");
#endif
		}
	}
}

int SetAndGetVendorID() {
	if (pActiveMerchant)
	{
		if (SpawnMatchesVendor())
		{
			vendorID = pActiveMerchant->SpawnID;
		}
		else
		{
			vendorID = 0;
		}
	}
	else {
		for (const auto& x : vendorNames) {
			// this requires using partial name due to (Teleportation Assistant) LastName
			if (PlayerClient* vendorSpawn = GetSpawnByPartialName(x)) {
				vendorID = vendorSpawn->SpawnID;
			}
		}
	}

	return vendorID;
}

bool inPortalMerchantRange() {
	// If we are checking if the portal merchant is in range, we should make sure we have one first.
	if (!vendorID)
		SetAndGetVendorID();
	if (PlayerClient* vendor = GetSpawnByID(vendorID))
	{
		return Distance3DToSpawn(pLocalPlayer, vendor) <= 20.0;
	}

	return false;
}

void setPortal(const std::string& setPortalStoneName) {
	switch (currentRoutineStep) {
		case 1: {
			if (SetAndGetVendorID() > 0) {
				TargetAndOpenVendor();
				// only go on to next step if we have the merchant wnd open
				if (pMerchantWnd && pMerchantWnd->IsVisible()) {
					currentRoutineStep++;
				}
			}
			break;
		}
		case 2: {
			if (GetPcProfile()->GetInventorySlot(InvSlot_Cursor)) {
				WriteChatf(PLUGINMSG "\ayYour cursor must be empty to use portal setter.");
				currentRoutineStep = 0;
			} else if (GetFreeInventory(0) < 1) {
				WriteChatf(PLUGINMSG, "\ayYou must have a free inventory slot to use portal setter.");
				currentRoutineStep = 0;
			} else if (FindInventoryItemCountByName(setPortalStoneName.c_str()) > 0) {
				WriteChatf(PLUGINMSG "\ayUsing existing %s", setPortalStoneName.c_str());
				currentRoutineStep = 4;
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
		case 3: {
			EzCommand("/ctrl /notify MerchantWnd MW_Buy_Button leftmouseup");
			currentRoutineStep++;
			break;
		}
		case 4: {
			if (FindItemByName(setPortalStoneName.c_str(), true)) {
				char zNotifyCommand[MAX_STRING];
				SendWndClick("MerchantWnd", "MW_DONE_BUTTON", "leftmouseup");
				sprintf_s(zNotifyCommand, "/itemnotify \"%s\" leftmouseup", setPortalStoneName.c_str());
				EzCommand(zNotifyCommand);
				currentRoutineStep++;
			}
			break;
		}
		case 5: {
			if (PlayerClient* vendor = GetSpawnByID(vendorID))
			{
				Target(pLocalPlayer, vendor->Name);
			}
			currentRoutineStep++;
			break;
		}
		case 6: {
			if (pTarget && inPortalMerchantRange()) {
				if (GetPcProfile()->GetInventorySlot(InvSlot_Cursor)) {
					EzCommand("/click left target");
					currentRoutineStep++;
				}
			} else {
				currentRoutineStep--;
			}
			if (pGiveWnd->IsVisible() && currentRoutineStep == 6) {
				bShowWindow = true;
				currentRoutineStep++;
			}
			break;
		}
		case 7: {
			if (pGiveWnd->IsVisible()) {
				SendWndClick("GiveWnd", "GVW_Give_Button", "leftmouseup");
				currentRoutineStep++;
			} else {
				currentRoutineStep--;
			}
			break;
		}
		case 8: {
			if (!pGiveWnd->IsVisible()) {
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
	GetMaybeQuotedArg(Arg, MAX_STRING, szLine, 1);

	if (Arg[0] != '\0')
	{
		// if we used a slash command to set the portal, but we don't have a vendorID, it is because we are not currently inside the vendor
		// or somehow there is no vendor
		if (SetAndGetVendorID() == 0) {
			WriteChatf(PLUGINMSG "\arYou don't have a \ayvendorID\ax, which means one was not found.");
			return;
		}

		for (const zonePortalInfo& info : s_zoneinfo)
		{
			if (ci_equals(Arg, info.shortname) || ci_equals(Arg, info.longname))
			{
				SetStoneAndStep(info.stonename);
				return;
			}
		}
	}

	WriteChatf(PLUGINMSG "\arPlease provide a long or shortname for the zone you wish to set to portal to.");
	WriteChatf(PLUGINMSG "\ayExample: \ao/portalsetter eastwastestwo \axor \ao/portalsetter \ayThe Eastern Wastes");
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
			if (SetAndGetVendorID() > 0) {
				WriteChatf(PLUGINMSG "\arOut of range of portal attendant, aborting.");
			}
			else {
				WriteChatf(PLUGINMSG "\arYou don't have a \ayvendorID\ax, which means one was not found.");
			}
			bShowWindow = false;
			currentRoutineStep = 0;
		}

		setPortal(portalStoneName);

		// Wait 1 second before running again
		PulseTimer = std::chrono::steady_clock::now() + std::chrono::seconds(1);
	}
}
