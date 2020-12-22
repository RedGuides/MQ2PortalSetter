// Project: MQ2PortalSetter
// Author: DigitalPixel
// Version: October 31, 2014 6:45 PM
// Updated for Burning Lands - Dewey 12-11-2018

// FIXME:  All the ParseMacroData calls are unnecessary.

#include <mq/Plugin.h>

PreSetup("MQ2PortalSetter");
PLUGIN_VERSION(2019.1223);

bool pluginEnabled = false;
int currentRoutineStep = 0;
unsigned long nextCommandAtTick = 0;
std::string portalStoneName;

void setPortal(std::string setPortalStoneName);

class CPortalSetterWindow : public CCustomWnd {
 public:
  CPortalSetterWindow(char *Template):CCustomWnd(Template) {
    CobaltScarTwo_Button      = (CButtonWnd*)GetChildItem("CobaltScarTwoButton");
    EasternWasteTwo_Button    = (CButtonWnd*)GetChildItem("EasternWastesTwoButton");
    Stratos_button            = (CButtonWnd*)GetChildItem("StratosButton");
    Overthere_button          = (CButtonWnd*)GetChildItem("OverthereButton");
    Lceanium_button           = (CButtonWnd*)GetChildItem("LceaniumButton");
    kattaCastrumDeluge_button = (CButtonWnd*)GetChildItem("KattaCastrumDelugeButton");
    westKarana_button         = (CButtonWnd*)GetChildItem("WestKaranaButton");
    shardsLanding_button      = (CButtonWnd*)GetChildItem("ShardsLandingButton");
    argath_button             = (CButtonWnd*)GetChildItem("ArgathButton");
    guildBanner_button        = (CButtonWnd*)GetChildItem("GuildBannerButton");
    feerrott_button           = (CButtonWnd*)GetChildItem("FeerrottButton");
    brellsRest_button         = (CButtonWnd*)GetChildItem("BrellsRestButton");
    dragonscaleHills_button   = (CButtonWnd*)GetChildItem("DragonscaleHillsButton");
    planeOfTime_button        = (CButtonWnd*)GetChildItem("PlaneOfTimeButton");
    kattaCastrum_button       = (CButtonWnd*)GetChildItem("KattaCastrumButton");
    gorukarMesa_button        = (CButtonWnd*)GetChildItem("GorukarMesaButton");
    arcstone_button           = (CButtonWnd*)GetChildItem("ArcstoneButton");
    planeOfSky_button         = (CButtonWnd*)GetChildItem("PlaneOfSkyButton");
    cobaltScar_button         = (CButtonWnd*)GetChildItem("CobaltScarButton");
    planeOfHate_button        = (CButtonWnd*)GetChildItem("PlaneOfHateButton");
    barindu_button            = (CButtonWnd*)GetChildItem("BarinduButton");
    wallOfSlaughter_button    = (CButtonWnd*)GetChildItem("WallOfSlaughterButton");
    twilightSea_button        = (CButtonWnd*)GetChildItem("TwilightSeaButton");
    undershore_button         = (CButtonWnd*)GetChildItem("UndershoreButton");
    dreadlands_button         = (CButtonWnd*)GetChildItem("DreadlandsButton");
    stonebrunt_button         = (CButtonWnd*)GetChildItem("StonebruntButton");
    icecladOcean_button       = (CButtonWnd*)GetChildItem("IcecladOceanButton");
    lavastorm_button          = (CButtonWnd*)GetChildItem("LavastormButton");
    toxxulia_button           = (CButtonWnd*)GetChildItem("ToxxuliaButton");
    northKarana_button        = (CButtonWnd*)GetChildItem("NorthKaranaButton");
    commonlands_button        = (CButtonWnd*)GetChildItem("CommonlandsButton");
    greaterFaydark_button     = (CButtonWnd*)GetChildItem("GreaterFaydarkButton");
  }

  ~CPortalSetterWindow() {}

  int WndNotification(CXWnd *pWnd, unsigned int Message, void *unknown);
  CButtonWnd* CobaltScarTwo_Button;
  CButtonWnd* EasternWasteTwo_Button;
  CButtonWnd* Stratos_button;
  CButtonWnd* Overthere_button;
  CButtonWnd* Lceanium_button;
  CButtonWnd* kattaCastrumDeluge_button;
  CButtonWnd* westKarana_button;
  CButtonWnd* shardsLanding_button;
  CButtonWnd* argath_button;
  CButtonWnd* guildBanner_button;
  CButtonWnd* feerrott_button;
  CButtonWnd* brellsRest_button;
  CButtonWnd* dragonscaleHills_button;
  CButtonWnd* planeOfTime_button;
  CButtonWnd* kattaCastrum_button;
  CButtonWnd* gorukarMesa_button;
  CButtonWnd* arcstone_button;
  CButtonWnd* planeOfSky_button;
  CButtonWnd* cobaltScar_button;
  CButtonWnd* planeOfHate_button;
  CButtonWnd* barindu_button;
  CButtonWnd* wallOfSlaughter_button;
  CButtonWnd* twilightSea_button;
  CButtonWnd* undershore_button;
  CButtonWnd* dreadlands_button;
  CButtonWnd* stonebrunt_button;
  CButtonWnd* icecladOcean_button;
  CButtonWnd* lavastorm_button;
  CButtonWnd* toxxulia_button;
  CButtonWnd* northKarana_button;
  CButtonWnd* commonlands_button;
  CButtonWnd* greaterFaydark_button;
};

int CPortalSetterWindow::WndNotification(CXWnd *pWnd, unsigned int Message, void *unknown) {
  if (pWnd==0) {
    if (Message==XWM_CLOSE) {
        SetVisible(true);
        return 1;
    }
  }

  if (Message == XWM_LCLICK) {
    currentRoutineStep = 1;
    if (pWnd == (CXWnd*)CobaltScarTwo_Button) {
      portalStoneName = "Othmir Clamshell";
    } else if (pWnd == (CXWnd*)EasternWasteTwo_Button) {
      portalStoneName = "Brilliant Frigid Gemstone";
    } else if (pWnd == (CXWnd*)Stratos_button) {
      portalStoneName = "Burning Lamp";
    } else if (pWnd == (CXWnd*)Overthere_button) {
      portalStoneName = "Miniature Worker's Sledge Mallet";
    } else if (pWnd == (CXWnd*)Lceanium_button) {
      portalStoneName = "Fragment of the Combine Spire";
    } else if (pWnd==(CXWnd*)kattaCastrumDeluge_button) {
      portalStoneName = "Drowned Katta Castrum Powerstone";
    } else if (pWnd==(CXWnd*)westKarana_button) {
      portalStoneName = "Stormstone of the West";
    } else if(pWnd==(CXWnd*)shardsLanding_button) {
      portalStoneName = "Stone of the Shard's Fall";
    } else if(pWnd==(CXWnd*)argath_button) {
      portalStoneName = "Chunk of Argathian Steel";
    } else if(pWnd==(CXWnd*)guildBanner_button) {
      portalStoneName = "Splinter from a Guild Standard";
    } else if(pWnd==(CXWnd*)feerrott_button) {
      portalStoneName = "Crystallized Dream of the Feerrott";
    } else if(pWnd==(CXWnd*)brellsRest_button) {
      portalStoneName = "Unrefined Brellium Ore";
    } else if(pWnd==(CXWnd*)dragonscaleHills_button) {
      portalStoneName = "Dragonscale Faycite";
    } else if(pWnd==(CXWnd*)planeOfTime_button) {
      portalStoneName = "Broken Timestone";
    } else if(pWnd==(CXWnd*)kattaCastrum_button) {
      portalStoneName = "Katta Castrum Powerstone";
    } else if(pWnd==(CXWnd*)gorukarMesa_button) {
      portalStoneName = "Goru'kar Mesa Sandstone";
    } else if(pWnd==(CXWnd*)arcstone_button) {
      portalStoneName = "Arcstone Spirit Sapphire";
    } else if(pWnd==(CXWnd*)planeOfSky_button) {
      portalStoneName = "Cloudy Stone of Veeshan";
    } else if(pWnd==(CXWnd*)cobaltScar_button) {
      portalStoneName = "Velium Shard of Cobalt Scar";
    } else if(pWnd==(CXWnd*)planeOfHate_button) {
      portalStoneName = "Fuligan Soulstone of Innoruuk";
    } else if(pWnd==(CXWnd*)barindu_button) {
      portalStoneName = "Etched Marble of Barindu";
    } else if(pWnd==(CXWnd*)wallOfSlaughter_button) {
      portalStoneName = "Chipped Shard of Slaughter";
    } else if(pWnd==(CXWnd*)twilightSea_button) {
      portalStoneName = "Shadowed Sand of the Twilight Sea";
    } else if(pWnd==(CXWnd*)undershore_button) {
      portalStoneName = "Undershore Coral";
    } else if(pWnd==(CXWnd*)dreadlands_button) {
      portalStoneName = "Shattered Bone of the Dreadlands";
    } else if(pWnd==(CXWnd*)stonebrunt_button) {
      portalStoneName = "Moss Agate of Stonebrunt";
    } else if(pWnd==(CXWnd*)icecladOcean_button) {
      portalStoneName = "Frozen Shard of Iceclad";
    } else if(pWnd==(CXWnd*)lavastorm_button) {
      portalStoneName = "Lavastorm Magma";
    } else if(pWnd==(CXWnd*)toxxulia_button) {
      portalStoneName = "Opal of Toxxulia";
    } else if(pWnd==(CXWnd*)northKarana_button) {
      portalStoneName = "Karana Plains Pebble";
    } else if(pWnd==(CXWnd*)commonlands_button) {
      portalStoneName = "Grassy Pebble Of The Commonlands";
    } else if(pWnd==(CXWnd*)greaterFaydark_button) {
      portalStoneName = "Forest Emerald of Faydark";
    } else {
      currentRoutineStep = 0;
    }

    if (currentRoutineStep == 1) {
      setPortal(portalStoneName);
    }
  }
  return CSidlScreenWnd::WndNotification(pWnd,Message,unknown);
};

CPortalSetterWindow *pCPortalSetterWindow = nullptr;

void CreatePortalSetterWindow() {
  if (pCPortalSetterWindow) return;
  pCPortalSetterWindow = new CPortalSetterWindow("PortalSetterWindow");
  pCPortalSetterWindow->SetVisible(false);
  pCPortalSetterWindow->SetZLayer(9999);
};

void DestroyPortalSetterWindow() {
  if (!pCPortalSetterWindow) return;
  delete pCPortalSetterWindow;
  pCPortalSetterWindow = nullptr;
};

static char* getPortalVendorName() {
  if (GetCharInfo()->zoneId == 345) return "Zeflmin Werlikanin";
  return "Teleportation Assistant";
}

static double getPortalMerchantDistance() {
  char zPortalMerchantDistance[MAX_STRING];
  sprintf_s(zPortalMerchantDistance, "${Spawn[%s].Distance}", getPortalVendorName());
  ParseMacroData(zPortalMerchantDistance,MAX_STRING);
  return atof(zPortalMerchantDistance);
}

static bool isMerchantPortalSetter() {
	char zMerchantName[MAX_STRING];
	sprintf_s(zMerchantName, "${Merchant.CleanName}");
	ParseMacroData(zMerchantName, MAX_STRING);
	if (strstr(zMerchantName, getPortalVendorName())) {
		return true;
	}
	return false;
}


static bool inPortalMerchantRange() {
  if (getPortalMerchantDistance() < 20.0f) return true;
  return false;
}

static bool inGuildHall() {
  if(strstr(((PZONEINFO)pZoneInfo)->LongName, "Guild Hall") != NULL) return true;
  return false;
}

void setPortal(std::string setPortalStoneName) {
  if (currentRoutineStep == 1) {
    if (GetPcProfile()->pInventoryArray->Inventory.Cursor) {
      WriteChatColor("[MQ2PortalSetter] Your cursor must be empty to use portal setter.", CONCOLOR_YELLOW);
      currentRoutineStep = 0;
    }
    char zFreeInventory[MAX_STRING] = "${Me.FreeInventory}";
    ParseMacroData(zFreeInventory,MAX_STRING);
    if(atoi(zFreeInventory) == 0) {
      WriteChatColor("[MQ2PortalSetter] You must have a free inventory slot to use portal setter.", CONCOLOR_YELLOW);
      currentRoutineStep = 0;
    }
  }
  switch (currentRoutineStep) {
    case 1: {
      char zStoneListPosition[MAX_STRING];
      sprintf_s(zStoneListPosition, "${Window[MerchantWnd].Child[MW_ItemList].List[=%s,2]}", &setPortalStoneName[0]);
      ParseMacroData(zStoneListPosition, MAX_STRING);
      SendListSelect("MerchantWnd", "MW_ItemList", (atoi(zStoneListPosition) - 1));
      currentRoutineStep++;
      break;
    }
    case 2: {
      EzCommand("/ctrl /notify MerchantWnd MW_Buy_Button leftmouseup");
      currentRoutineStep++;
      break;
    }
    case 3: {
      CONTENTS* stone = FindItemByName(&setPortalStoneName[0], true);
      if(stone) {
        char zNotifyCommand[MAX_STRING];
        pCPortalSetterWindow->SetVisible(0);
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
        if (GetPcProfile()->pInventoryArray->Inventory.Cursor) {
          EzCommand("/click left target");
          currentRoutineStep++;
        }
      } else {
        currentRoutineStep--;
      }
      if (pGiveWnd->IsVisible() && currentRoutineStep == 5) {
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
  }
};

PLUGIN_API void InitializePlugin()
{
  DebugSpewAlways("Initializing MQ2PortalSetter");
  AddXMLFile("MQUI_PortalSetterWindow.xml");
}

PLUGIN_API void ShutdownPlugin()
{
  DebugSpewAlways("Shutting down MQ2PortalSetter");
  DestroyPortalSetterWindow();
  RemoveXMLFile("MQUI_PortalSetterWindow.xml");
}

PLUGIN_API void OnCleanUI()
{
  DebugSpewAlways("MQ2PortalSetter::OnCleanUI()");
  DestroyPortalSetterWindow();
}

PLUGIN_API void OnReloadUI()
{
  DebugSpewAlways("MQ2PortalSetter::OnReloadUI()");
  CreatePortalSetterWindow();
}

PLUGIN_API void SetGameState(int GameState)
{
  DebugSpewAlways("MQ2PortalSetter::SetGameState()");
  if (GameState == GAMESTATE_INGAME) {
    if(inGuildHall()) {
      if (!pluginEnabled) {
        pluginEnabled = true;
      }
    } else {
      if (pluginEnabled) {
        pluginEnabled = false;
      }
    }
  } else {
    if (pluginEnabled) {
      pluginEnabled = false;
    }
  }
}

PLUGIN_API VOID OnPulse(VOID)
{
	//DebugSpewAlways("MQ2PortalSetter::OnPulse()");
	if (!pluginEnabled) return;
	DWORD currentTick = GetTickCount();
	if (currentTick < nextCommandAtTick) return;
	if (!pCPortalSetterWindow) return;

	//-- Check to see if we need to hide / show the portal window
	if (pMerchantWnd->IsVisible() == true) {
		if (currentRoutineStep < 4) {
			if(pCPortalSetterWindow->IsVisible() == false && inPortalMerchantRange() && isMerchantPortalSetter()) {
				pCPortalSetterWindow->SetVisible(true);
			}
		}
	} else {
		if(pCPortalSetterWindow->IsVisible() == true) {
			pCPortalSetterWindow->SetVisible(false);
		}
	}

	//-- If we are out of range then reset state.
	if (currentRoutineStep && !inPortalMerchantRange()) {
		WriteChatColor("[MQ2PortalSetter] Out of range of portal attendant, aborting.", CONCOLOR_RED);
		currentRoutineStep = 0;
	}
	setPortal(portalStoneName);
	nextCommandAtTick = currentTick + 60;
}
