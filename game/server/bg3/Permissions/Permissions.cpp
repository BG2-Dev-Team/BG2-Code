#include "cbase.h"
#include "KeyValues.h"
#include "Permissions.h"
#include "hl2mp/hl2mp_player.h"

using namespace std;

static Permissions g_NullPermission;
Permissions* const Permissions::s_pFirstPermission = &g_NullPermission;

Permissions::Permissions() {
	memset(m_name, 0, sizeof(m_name));
	m_bPermissionManage = m_bPlayerManage = m_bMapMode = m_bBotManage = m_bConsoleAccess = 0;
	m_pNextPermission = NULL; //null terminator in linked list
}

void Permissions::LoadPermissionsForPlayer(CHL2MP_Player* pPlayer) {
	//if the player already has permissions, skip
	if (pPlayer->m_pPermissions)
		return;

	Permissions* pPermission = NULL;
	CSteamID sid;
	pPlayer->GetSteamID(&sid);
	char buffer[64];
	convertSteamIDToString(sid, buffer, sizeof(buffer));
	CUtlString id = buffer;
	Msg("Getting permissions for player with STEAMID: %s\n", buffer);

	//search for existing permission set with our SteamID
	Permissions* pNextPermission = s_pFirstPermission;
	while (pNextPermission && !pPermission) {
		for (size_t i = 0; i < pNextPermission->m_players.size(); i++) {
			if (pNextPermission->m_players[i] == id) {
				pPermission = pNextPermission;
				goto endloop;
			}
		}

		pNextPermission = pNextPermission->m_pNextPermission;
	}

endloop:
	if (!pPermission) pPermission = s_pFirstPermission;
	pPlayer->m_pPermissions = pPermission;
	Msg(" permission is %s\n", pPermission->m_name);
}

Permissions* Permissions::FindPermissionByName(const char* pszName) {
	//search through all permissions
	Permissions* result = NULL;

	Permissions* pNextPermission = s_pFirstPermission->m_pNextPermission;
	while (pNextPermission) {
		if (Q_strcmp(pszName, pNextPermission->m_name) == 0) {
			result = pNextPermission;
			break;
		}
		pNextPermission = pNextPermission->m_pNextPermission;
	}
	return result;
}

/*void Permissions::AssignPlayerPermission(CHL2MP_Player* pPlayer, Permissions* pPermissions) {
	CSteamID sid;
	pPlayer->GetSteamID(&sid);
	char buffer[64];
	convertSteamIDToString(sid, buffer, sizeof(buffer));
	std::string id = buffer;

	//remove player from prior permission
	Permissions* pOldPermission = pPlayer->m_pPermissions;
	if (pOldPermission != s_pFirstPermission) {
		//don't ask me why std::vector doesn't have indexOf or findAndRemove

	}

	//add player to new permission

	//write to file
	RefreshPermissionsToFile();
}*/

void convertSteamIDToString(const CSteamID& id, char* buffer, int bufferSize) {
	Q_snprintf(buffer, bufferSize, "STEAM_%u:%u:%u", id.GetEUniverse(), id.GetAccountID() & 1, id.GetAccountID() >> 1);
}
