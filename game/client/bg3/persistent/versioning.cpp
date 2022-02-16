#include "cbase.h"
#include "versioning.h"
#include "../bg3_soft_info.h"
#include "../shared/hl2mp/hl2mp_gamerules.h"
#include <string>

extern ConVar mp_timelimit;

/*Basically, this system is a cheat where if a player participates in the beta, we'll write a secret file,
whose existance we can check for later.*/
namespace NVersioning {
#define BETA_TEST_MARK_FILEPATH "bg3/sound/playersounds.cache"

	static bool g_bHadDoneBetaTestParticipation = false;
	static bool g_bHasCheckedForBetaTestParticipation = false;

	bool HasBetaTestParticipationMark() {
		bool result = false;

		FILE* pFile;
		fopen_s(&pFile, BETA_TEST_MARK_FILEPATH, "r");
		if (pFile) {
			result = true;
			fclose(pFile);
		}

		return result;
	}

	bool HadDoneBetaTestParticipation() {
		if (!g_bHasCheckedForBetaTestParticipation) {
			g_bHasCheckedForBetaTestParticipation = true;
			g_bHadDoneBetaTestParticipation = HasBetaTestParticipationMark();
		}
		return g_bHadDoneBetaTestParticipation;
	}

	void MarkBetaTestParticipation() {

		//Warning("Check branch info...\n");
		//do nothing if we're already a beta tester or if we're not on beta
		if (g_bHadDoneBetaTestParticipation || !IsOnBetaBranch())
			return;

		//Warning("Marking participation...\n");
		//check that we have enough players on the server and aren't just cheesing it
		if (mp_timelimit.GetInt() < 10 || g_PR->GetNumNonSpectatingPlayers() < 10)
			return;

		g_bHadDoneBetaTestParticipation = true;

		FILE* pFile;
		fopen_s(&pFile, BETA_TEST_MARK_FILEPATH, "w");
		if (pFile) {
			fprintf_s(pFile, "1776");
			fclose(pFile);
		}
	}

	bool IsOnBetaBranch() {
		//Warning("%s\n", HL2MPRules()->MapName());
		return ((std::string)HL2MPRules()->MapName() == (std::string)"bg_canal_test");
		//return false; //TODO - develop a system better than changing this between versions
	}
}