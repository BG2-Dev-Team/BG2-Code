#ifndef BG3_PERMISSIONS_H
#define BG3_PERMISSIONS_H

#include "cbase.h"
#include "vector"
#include <string>

class CHL2MP_Player;

class Permissions {
public:
	Permissions();
	char m_name[16]; //name of the permission
	uint8 m_bPermissionManage : 1; //change permissions of other players
	uint8 m_bPlayerManage : 1; //kick, ban, slay, teleport, noclip, teamswitch
	uint8 m_bMapMode : 1; //change map and gamemode
	uint8 m_bBotManage : 1; //manage bots
	uint8 m_bConsoleAccess : 1; //general console access, supersedes everything else.
	typedef std::string id_t;
	std::vector<id_t> m_players; //which players have these permissions?
    // std::vector<std::string> m_players; //which players have these permissions?
private:
	Permissions* m_pNextPermission; //next permission in the list
	static Permissions* const s_pFirstPermission; //first permission in the list
public:

	static void LoadPermissionsForPlayer(CHL2MP_Player*);
	static void RefreshPermissionsFromFile();
	//static void RefreshPermissionsToFile();
	static Permissions* NullPermission() { return s_pFirstPermission; }
	static Permissions* FindPermissionByName(const char* pszName);
	//static void AssignPlayerPermission(CHL2MP_Player* pPlayer, Permissions* pPermissions);

};

void convertSteamIDToString(const CSteamID& id, char* buffer, int bufferSize);
inline bool verifyPermissions(CBasePlayer* pPlayer, bool cond, const char* pszFunctionName) {
	if (!cond) {
		Warning("Player named %s with STEAMID64 %llu attempted to access function %s without permission!\n",
			pPlayer->GetPlayerName(), pPlayer->GetSteamIDAsUInt64(), pszFunctionName);
	}
	return cond;
}

#endif //BG3_PERMISSIONS_H
