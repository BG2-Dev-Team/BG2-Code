#include "cbase.h"
#include "banlist.h"
#include "hl2mp/hl2mp_player.h"
#include "../shared/bg3/bg3_unlockable.h"
#include <chrono>

enum EBanType {
	BAN_DEVICE,
	BAN_STEAMID,
	BAN_STEAMID_TEMP,
};

struct PlayerBan {
	EBanType m_eType;

	CSteamID m_steamId;
	char m_deviceId[DEVICE_ID_MAX_LENGTH];

	long long m_iUnixTimeEnd;
};

namespace NBans {
	static CUtlVector<PlayerBan> g_banList;

	long long GetUnixTime() {
		const auto p1 = std::chrono::system_clock::now();
		return std::chrono::duration_cast<std::chrono::seconds>(
			p1.time_since_epoch()).count();
	}

	void AddPlayerToBanList(CHL2MP_Player* pPlayer, bool bPerma, bool bDevice, int durationMinutes) {
		//don't save bans to bots
		if (pPlayer->IsFakeClient())
			return;

		//create ban object
		PlayerBan ban;
		bool bFileWrite = false;
		if (bDevice) {
			ban.m_eType = BAN_DEVICE;
			bFileWrite = true;
			const char* pszId = engine->GetClientConVarValue(pPlayer->entindex(), "cl_deviceid");
			Msg("Deviceid of %s is %s", pPlayer->GetPlayerName(), pszId);
			strcpy_s(ban.m_deviceId, DEVICE_ID_MAX_LENGTH, pszId);
		}
		else if (bPerma) {
			ban.m_eType = BAN_STEAMID;
			bFileWrite = true;
			pPlayer->GetSteamID(&ban.m_steamId);
		}
		else {
			ban.m_eType = BAN_STEAMID_TEMP;
			ban.m_iUnixTimeEnd = GetUnixTime() + (durationMinutes * 60);
			pPlayer->GetSteamID(&ban.m_steamId);
		}

		//add it to list of bans
		g_banList.AddToTail(ban);

		//if it's perma or device, save to file
		if (bPerma || bDevice) {
			SaveBanListFile();
		}
	}

	bool PlayerIsBanned(CHL2MP_Player* pPlayer) {
		bool bResult = false;

		//check against ban list
		for (int i = 0; i < g_banList.Count(); i++) {
			PlayerBan& ban = g_banList[i];
			if (ban.m_eType == BAN_DEVICE) {
				const char* pszId = engine->GetClientConVarValue(pPlayer->entindex(), "cl_deviceid");
				bResult = strncmp(pszId, ban.m_deviceId, DEVICE_ID_MAX_LENGTH) == 0;
			}
			else {
				CSteamID id;
				pPlayer->GetSteamID(&id);
				if (id == ban.m_steamId) {
					//so this player was banned in the past, now just check if they're still banned
					if (ban.m_eType == BAN_STEAMID) {
						bResult = true;
					}
					else {
						bResult = GetUnixTime() < ban.m_iUnixTimeEnd;
					}
				}
			}
		}
		return bResult;
	}

	void SaveBanListFile() {

	}

	void LoadFromBanListFile();
}