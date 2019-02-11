#include "cbase.h"
#include "bg3_vgui_shared.h"
#include "IGameUIFuncs.h"

void SimulateKeyCodePressed(vgui::KeyCode key) {
	const char* pszCommand = gameuifuncs->GetBindingForButtonCode(key);
	engine->ClientCmd(pszCommand);
}