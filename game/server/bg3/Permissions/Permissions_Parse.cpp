#include "cbase.h"
#include "Permissions.h"
#include "filesystem.h"

#define PERMISSIONS_PATH "cfg/permissions.res"
#define PERMISSIONS_ASSIGNMENTS_PATH "cfg/permissions_assignments.res"

static KeyValues* kv = NULL;
void Permissions::RefreshPermissionsFromFile() {

	//First, delete all existing permissions except for the first one
	Permissions* nextPermission = s_pFirstPermission->m_pNextPermission;
	while (nextPermission) {
		Permissions* deleteMe = nextPermission;
		nextPermission = nextPermission->m_pNextPermission;
		delete deleteMe;
	}


	if (!kv)
		kv = new KeyValues("kv");
	kv->Clear();
	kv->LoadFromFile(filesystem, "cfg/permissions.res");
	nextPermission = s_pFirstPermission;
	for (KeyValues *pKey = kv->GetFirstSubKey(); pKey; pKey = pKey->GetNextKey()) {

		//only create the permissions if there is a list of players which have the permission
		if (pKey->FindKey("players")) {
			nextPermission->m_pNextPermission = new Permissions();
			nextPermission = nextPermission->m_pNextPermission;

			strcpy(nextPermission->m_name, pKey->GetName());
			nextPermission->m_bPermissionManage = pKey->FindKey("permissions_manage") != NULL;
			nextPermission->m_bPlayerManage = pKey->FindKey("player_manage") != NULL;
			nextPermission->m_bMapMode = pKey->FindKey("mapmode") != NULL;
			nextPermission->m_bBotManage = pKey->FindKey("bot_manage") != NULL;
			nextPermission->m_bConsoleAccess = pKey->FindKey("console_access") != NULL;

			KeyValues* pPlayers = pKey->FindKey("players");
			for (KeyValues *pSubKey = pPlayers->GetFirstSubKey(); pSubKey; pSubKey = pSubKey->GetNextKey()) {
				Msg("\tadding %s to permission %s\n", pSubKey->GetName(), nextPermission->m_name);
				nextPermission->m_players.push_back(pSubKey->GetName());
			}
		}
		/*for (KeyValues *pSubKey = pKey->GetFirstSubKey(); pSubKey; pSubKey = pSubKey->GetNextKey()) {
			Msg("\tKey name: %s\n", pSubKey->GetName());
		}*/
	}
}

/*CON_COMMAND(refreshpermissions, "") {
	Permissions::RefreshPermissionsFromFile();
}*/

/*void Permissions::RefreshPermissionsToFile() {

}*/
