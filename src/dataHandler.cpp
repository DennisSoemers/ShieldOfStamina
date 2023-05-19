#include "dataHandler.h"
#include "SimpleIni.h"
void dataHandler::readSettings() {
	logger::info("reading settings");
	using namespace settings;
	CSimpleIniA ini;
#define SETTINGFILE_PATH "Data\\SKSE\\Plugins\\ShieldOfStamina.ini"
	ini.LoadFile(SETTINGFILE_PATH);
	bckShdStaminaMult_PC_Block_NPC = static_cast<float>(ini.GetDoubleValue("General", "bckShdStaminaMult_PC_Block_NPC", 1));
	bckWpnStaminaMult_PC_Block_NPC = static_cast<float>(ini.GetDoubleValue("General", "bckWpnStaminaMult_PC_Block_NPC", 1));
	bckShdStaminaMult_NPC_Block_PC = static_cast<float>(ini.GetDoubleValue("General", "bckShdStaminaMult_NPC_Block_PC", 1));
	bckWpnStaminaMult_NPC_Block_PC = static_cast<float>(ini.GetDoubleValue("General", "bckWpnStaminaMult_NPC_Block_PC", 1));
	bckShdStaminaMult_NPC_Block_NPC = static_cast<float>(ini.GetDoubleValue("General", "bckShdStaminaMult_NPC_Block_NPC", 1));
	bckWpnStaminaMult_NPC_Block_NPC = static_cast<float>(ini.GetDoubleValue("General", "bckWpnStaminaMult_NPC_Block_NPC", 1));

	guardBreak = ini.GetBoolValue("General", "GuardBreak", true);
	noRegenOnBlock = ini.GetBoolValue("General", "noRegenOnBlock", true);
}

namespace settings
{
	float bckShdStaminaMult_PC_Block_NPC = 1.f;
	float bckWpnStaminaMult_PC_Block_NPC = 1.f;

	float bckShdStaminaMult_NPC_Block_PC = 1.f; //stamina penalty mult for NPCs blockign a player hit with a shield
	float bckWpnStaminaMult_NPC_Block_PC = 1.f;

	float bckShdStaminaMult_NPC_Block_NPC = 1.f;
	float bckWpnStaminaMult_NPC_Block_NPC = 1.f;

	bool guardBreak = true;
	bool noRegenOnBlock = true;
}