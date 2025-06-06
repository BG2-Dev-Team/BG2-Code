//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:		Player for HL2.
//
//=============================================================================//

#include "cbase.h"
#include "weapon_hl2mpbasehlmpcombatweapon.h"
#include "hl2mp_player.h"
#include "globalstate.h"
#include "game.h"
#include "gamerules.h"
#include "hl2mp_player_shared.h"
#include "predicted_viewmodel.h"
#include "in_buttons.h"
#include "hl2mp_gamerules.h"
#include "KeyValues.h"
#include "team.h"
#include "weapon_hl2mpbase.h"
#include "grenade_satchel.h"
#include "eventqueue.h"
#include "gamestats.h"
#include "player_resource.h"

#include "engine/IEngineSound.h"
#include "SoundEmitterSystem/isoundemittersystembase.h"

#include "ilagcompensationmanager.h"


//#includes - HairyPotter
#include "ammodef.h"
//

#include "bg3/bg3_class.h"

//BG2 - Tjoppen - #includes
#include "triggers.h"
#include "bg2/flag.h"
#include "bg2/vcomm.h"
#include "bg2/spawnpoint.h"
#include "bg2/weapon_bg2base.h"
#include "bg2/ctfflag.h"
#include "../shared/bg2/weapon_frag.h"
#include "bg3/Bots/bg3_bot_vcomms.h"
#include "../shared/bg3/bg3_buffs.h"
#include "../shared/bg3/bg3_class_quota.h"
#include "../shared/bg3/math/bg3_rand.h"
#include "../shared/viewport_panel_names.h"
#include "bg3/bg3_scorepreserve.h"
#include "bg3/Bots/bg3_bot_influencer.h"
#include "EntityFlame.h"
#include "bg3_gungame.h"
//

extern ConVar mp_autobalanceteams;
extern ConVar mp_autobalancetolerance;
extern ConVar mp_disable_firearms;
//BG2 - Tjoppen - sv_voicecomm_text, set to zero for realism
ConVar sv_voicecomm_text("sv_voicecomm_text", "1", FCVAR_GAMEDLL | FCVAR_NOTIFY, "Show voicecomm text on clients?");
ConVar sv_unlag_lmb("sv_unlag_lmb", "1", FCVAR_GAMEDLL | FCVAR_NOTIFY, "Do unlagging for left mouse button (primary attack)?");
ConVar sv_unlag_rmb("sv_unlag_rmb", "1", FCVAR_GAMEDLL | FCVAR_NOTIFY, "Do unlagging for right mouse button (primary attack)?");
ConVar sv_saferespawntime("sv_saferespawntime", "2.5f", FCVAR_GAMEDLL | FCVAR_NOTIFY, "Amount of time after respawn when player is immune to damage. (In Seconds)");
//

//BG2 - Spawn point optimization test - HairyPotter
CUtlVector<CBaseEntity *> g_MultiSpawns, g_MultiPrioritySpawns, g_AmericanSpawns, g_BritishSpawns, g_AmericanPrioritySpawns, g_BritishPrioritySpawns, g_FFA_Spawns;
//

ConVar weapon_test("weapon_test", "0", FCVAR_HIDDEN | FCVAR_GAMEDLL);
ConVar weapon_test_team("weapon_test_team", "2", FCVAR_HIDDEN | FCVAR_GAMEDLL);

int g_iLastCitizenModel = 0;
int g_iLastCombineModel = 0;

//BG2 - Tjoppen - away with these
/*CBaseEntity	 *g_pLastCombineSpawn = NULL;
CBaseEntity	 *g_pLastRebelSpawn = NULL;*/
//extern CBaseEntity				*g_pLastSpawn;
//

#define HL2MP_COMMAND_MAX_RATE 0.3

//BG2 - Tjoppen - don't need this
//void DropPrimedFragGrenade( CHL2MP_Player *pPlayer, CBaseCombatWeapon *pGrenade );
//

LINK_ENTITY_TO_CLASS(player, CHL2MP_Player);

//BG2 - Tjoppen - CSpawnPoint
BEGIN_DATADESC(CSpawnPoint)
DEFINE_KEYFIELD(m_iDefaultTeam, FIELD_INTEGER, "StartingTeam"),

DEFINE_INPUTFUNC(FIELD_VOID, "Enable", InputEnable),
DEFINE_INPUTFUNC(FIELD_VOID, "Disable", InputDisable),
DEFINE_INPUTFUNC(FIELD_VOID, "Toggle", InputToggle),

//Below are for info_player_multi
DEFINE_INPUTFUNC(FIELD_VOID, "ToggleTeam", InputToggleTeam),
DEFINE_INPUTFUNC(FIELD_VOID, "SetAmerican", InputAmerican),
DEFINE_INPUTFUNC(FIELD_VOID, "SetBritish", InputBritish),
//

END_DATADESC()
//

//BG2 - Tjoppen - info_player_american/british
LINK_ENTITY_TO_CLASS(info_player_american, CSpawnPoint);
LINK_ENTITY_TO_CLASS(info_player_british, CSpawnPoint);
LINK_ENTITY_TO_CLASS(info_player_multispawn, CSpawnPoint);
LINK_ENTITY_TO_CLASS(info_player_ffa, CSpawnPoint);
//

IMPLEMENT_SERVERCLASS_ST(CHL2MP_Player, DT_HL2MP_Player)
SendPropAngle(SENDINFO_VECTORELEM(m_angEyeAngles, 0), 11, SPROP_CHANGES_OFTEN),
SendPropAngle(SENDINFO_VECTORELEM(m_angEyeAngles, 1), 11, SPROP_CHANGES_OFTEN),
SendPropEHandle(SENDINFO(m_hRagdoll)),
//BG2 - Tjoppen - tweaking
SendPropInt(SENDINFO(m_iSpawnInterpCounter), 2, SPROP_UNSIGNED), //4 ),
//BG2 - Tjoppen - don't need this
//SendPropInt( SENDINFO( m_iPlayerSoundType), 3 ),
//

SendPropExclude("DT_BaseAnimating", "m_flPoseParameter"),
//SendPropExclude( "DT_BaseFlex", "m_viewtarget" ), //BG2 - Not needed anymore. -HairyPotter

//BG2 - Tjoppen - send stamina via C_HL2MP_Player <=> DT_HL2MP_Player <=> CHL2MP_Player
SendPropInt(SENDINFO(m_iStamina), 7, SPROP_UNSIGNED),	//0 <= stamina <= 100, 7 bits enough
//
//BG2 - Tjoppen - m_iClass and m_iCurrentAmmoKit are network vars
SendPropInt(SENDINFO(m_iClass), 3, SPROP_UNSIGNED),			//BG2 - Tjoppen - remember: max eight classes per team or increase this
SendPropInt(SENDINFO(m_iNextClass), 3, SPROP_UNSIGNED),		//BG2 - Tjoppen - remember: max eight classes per team or increase this
//SendPropInt(SENDINFO(m_iGunKit), 2, SPROP_UNSIGNED),
SendPropInt(SENDINFO(m_iCurrentAmmoKit), 2, SPROP_UNSIGNED),//BG2 - Tjoppen - remember: max four ammo kits or increase this
SendPropInt(SENDINFO(m_iSpeedModifier), 9, 0),
//Rallying info
SendPropInt(SENDINFO(m_iCurrentRallies), RALLY_NUM, SPROP_UNSIGNED), //BG3 - only RALY_NUM different rallies available, 1 bit per rally

//	SendPropExclude( "DT_ServerAnimationData" , "m_flCycle" ),	
//	SendPropExclude( "DT_AnimTimeMustBeFirst" , "m_flAnimTime" ),

END_SEND_TABLE()

BEGIN_DATADESC(CHL2MP_Player)
END_DATADESC()

#define NUM_DEATH_SOUNDS 25
#define NUM_DEATH_SOUNDS_FIRE 3
#define MAX_COMBINE_MODELS 6 // BG2 - VisualMelon - Looks like this should be 6 (formerly 4)
#define MODEL_CHANGE_INTERVAL 5.0f
#define TEAM_CHANGE_INTERVAL 5.0f

#define HL2MPPLAYER_PHYSDAMAGE_SCALE 4.0f

#pragma warning( disable : 4355 )

CHL2MP_Player::CHL2MP_Player() : m_PlayerAnimState(this)
{
	m_angEyeAngles.Init();

	m_iLastWeaponFireUsercmd = 0;

	m_iSpawnInterpCounter = 0;

	m_bEnterObserver = false;
	m_bReady = false;
	m_bMuted = false;
	m_bGagged = false;
	m_bOppressed = false;

	//BG2 - Default weapon kits. -Hairypotter
	m_iGunKit = 0;
	m_iGunGameKit = 0;
	m_bNoJoinMessage = false;
	m_iCurrentAmmoKit = m_iAmmoKit = AMMO_KIT_BALL;
	//
	//BG3 - Awesome - default rallying info
	m_iCurrentRallies = 0;

	m_pPermissions = NULL;
	//m_pPermissions = Permissions::NullPermission();

	BaseClass::ChangeTeam(0);

	//	UseClientSideAnimation();

	//BG2 - Tjoppen - don't pick a class..
	m_iClass = m_iNextClass = -1;//RandomInt( 0, 2 );
	m_pCurClass = PlayerClasses::g_pAInfantry; //this default class just avoids errors until we spawn
	//

	//BG2 - Tjoppen - tickets
	m_bDontRemoveTicket = true;

	m_bInSpawnRoom = m_bMonsterBot = false;
	m_flNextAmmoRefill = -FLT_MAX;
}

CHL2MP_Player::~CHL2MP_Player(void)
{
	//Msg("Destroying player object, info %i - %s\n", GetTeamNumber(), GetPlayerClass()->m_pszAbbreviation);
}

void CHL2MP_Player::UpdateOnRemove(void)
{
	//BG2 - Tjoppen - here's where we put code for multiple ragdolls
	RemoveRagdolls();

	BaseClass::UpdateOnRemove();
}

void CHL2MP_Player::Precache(void)
{
	BaseClass::Precache();

	PrecacheModel("sprites/glow01.vmt");
	PrecacheModel("models/player/other/burning_man.mdl");

	//Precache models from classes
	int i;
	const CPlayerClass* const * pClasses = CPlayerClass::asList();
	for (i = 0; i < CPlayerClass::numClasses(); i++) {
		const CPlayerClass* pClass = pClasses[i];

		PrecacheModel(pClass->m_pszPlayerModel);//precache player model

		//iterate through weapons to precache player model overrides
		for (int j = 0; j < pClass->numChooseableWeapons(); j++) {
			const char* pOverride = pClass->m_aWeapons[j].m_pszPlayerModelOverrideName;
			if (pOverride) {
				PrecacheModel(pOverride);
			}
		}

		//precache per-uniform overrides
		for (int j = 0; j < 4; j++) {
			if (pClass->m_pszUniformModelOverrides[j]) {
				PrecacheModel(pClass->m_pszUniformModelOverrides[j]);
			}
		}

		//precache any dropped hat models
		if (pClass->m_pszDroppedHat) PrecacheModel(pClass->m_pszDroppedHat);
	}

	PrecacheFootStepSounds();

	//BG2 - Tjoppen - precache sounds
	/*PrecacheScriptSound( "NPC_MetroPolice.Die" );
	PrecacheScriptSound( "NPC_CombineS.Die" );
	PrecacheScriptSound( "NPC_Citizen.die" );*/
	PrecacheScriptSound("BG2Player.die");
	PrecacheScriptSound("AmmoCrate.Give");
	//PrecacheScriptSound( "BG2Player.pain" );

	for (i = 0; i <= HITGROUP_RIGHTLEG; i++)
	{
		PrecacheScriptSound(GetHitgroupPainSound(i, TEAM_AMERICANS));
		PrecacheScriptSound(GetHitgroupPainSound(i, TEAM_BRITISH));
	}

	//precache voiceover sounds
	char name[256];
	for (i = 0; i < NUM_VOICECOMMS; i++)
	{
		Q_snprintf(name, sizeof name, "Voicecomms.American.Inf_%i", i + 1);
		PrecacheScriptSound(name);
		Q_snprintf(name, sizeof name, "Voicecomms.American.Off_%i", i + 1);
		PrecacheScriptSound(name);
		Q_snprintf(name, sizeof name, "Voicecomms.American.Rif_%i", i + 1);
		PrecacheScriptSound(name);

		Q_snprintf(name, sizeof name, "Voicecomms.British.Inf_%i", i + 1);
		PrecacheScriptSound(name);
		Q_snprintf(name, sizeof name, "Voicecomms.British.Off_%i", i + 1);
		PrecacheScriptSound(name);
		Q_snprintf(name, sizeof name, "Voicecomms.British.Rif_%i", i + 1);
		PrecacheScriptSound(name);
		Q_snprintf(name, sizeof name, "Voicecomms.British.Ski_%i", i + 1);
		PrecacheScriptSound(name);
	}
	//precache death sounds
	Q_snprintf(name, sizeof name, "BG3Player.Die");
	char* formatStart = name + strlen("BG3Player.Die");
	size_t maxSize = sizeof name - (formatStart - name);
	for (i = 1; i <= NUM_DEATH_SOUNDS; i++) {
		Q_snprintf(formatStart, maxSize, "%02i", i);
		PrecacheScriptSound(name);
	}
	PrecacheScriptSound("Weapon_All.Bullet_Skullcrack");
	PrecacheScriptSound("BG3Player.DieFire01");
	PrecacheScriptSound("BG3Player.DieFire02");
	PrecacheScriptSound("BG3Player.DieFire03");
}

void CHL2MP_Player::GiveAllItems(void)
{
	CBasePlayer::GiveAmmo(32, "357");
	CBasePlayer::GiveAmmo(32, "Grenade");

	//BG2 - Tjoppen - impulse 101
	GiveNamedItem("weapon_brownbess");
	GiveNamedItem("weapon_revolutionnaire");
	GiveNamedItem("weapon_brownbess_nobayo");
	GiveNamedItem("weapon_beltaxe");
	GiveNamedItem("weapon_tomahawk");
	GiveNamedItem("weapon_charleville");
	GiveNamedItem("weapon_pennsylvania");
	GiveNamedItem("weapon_jaeger");
	GiveNamedItem("weapon_pistol_a");
	GiveNamedItem("weapon_pistol_b");
	GiveNamedItem("weapon_sabre_a");
	GiveNamedItem("weapon_sabre_b");
	GiveNamedItem("weapon_hirschf");
	GiveNamedItem("weapon_knife");
	GiveNamedItem("weapon_frag");
}

ConVar mp_ammo_override = ConVar("mp_ammo_override", "0", FCVAR_GAMEDLL, "Overrides ammo counts to specified amount, if non-zero");

void CHL2MP_Player::GiveDefaultItems(void)
{
	//BG2 - Tjoppen - default equipment - also strip old equipment
	/*RemoveAllAmmo();
	Weapon_DropAll( true );*/
	RemoveAllItems(false);

	//check if we're being forced off of buckshot
	if (IsLinebattle() && lb_enforce_no_buckshot.GetBool())
		m_iAmmoKit = AMMO_KIT_BALL;

	//remember which ammo kit we spawned with
	m_iCurrentAmmoKit = m_iAmmoKit;

	//if we're a bot, randomize our gun kit
	ConVar* pWeaponKitEnforcer = GetTeamNumber() == TEAM_AMERICANS ? &lb_enforce_weapon_amer : &lb_enforce_weapon_brit;
	if ((IsFakeClient() && !pWeaponKitEnforcer->GetBool())) {
		m_iGunKit = RandomInt(0, m_pCurClass->numChooseableWeapons() - 1);;
	}

	//give the chosen weapons
	const CGunKit& k = m_pCurClass->m_aWeapons[m_iGunKit];
	//don't give shooting-only guns when ammo override == -1
	if (mp_ammo_override.GetInt() != -1 || CWeaponDef::GetDefForWeapon(k.m_pszWeaponPrimaryName)->m_Attackinfos[1].m_iAttacktype != ATTACKTYPE_NONE) {
		GiveNamedItem(k.m_pszWeaponPrimaryName);
	}
	if (k.m_pszWeaponSecondaryName) {
		GiveNamedItem(k.m_pszWeaponSecondaryName);
		if (k.m_pszWeaponTertiaryName) {
			GiveNamedItem(k.m_pszWeaponTertiaryName);
		}
	}

	//Apply kit-specific speed modifier
	AddSpeedModifier(k.m_iMovementSpeedModifier, ESpeedModID::Weapon);

	//Give primary and secondary ammo
	SetDefaultAmmoFull(true);

	//Give extra ammo from kit
	/*if (k.m_pszAmmoOverrideName) {
		CBasePlayer::SetAmmoCount(k.m_iAmmoOverrideCount, GetAmmoDef()->Index(k.m_pszAmmoOverrideName));
	}*/	
}

void CHL2MP_Player::GiveGunGameItems(void)
{
	//BG2 - Tjoppen - default equipment - also strip old equipment
	RemoveAllItems(false);

	//remember which ammo kit we spawned with
	m_iCurrentAmmoKit = m_iAmmoKit;

	//give the chosen weapons
	const NGunGame::SGunGameKit& ggk = *(NGunGame::GetGunKitFromIndex(m_iGunGameKit));
	const CGunKit* k = (ggk.m_pOrigKit);
	
	CBaseEntity* pGrenadeEnt = NULL;
	if (ggk.m_iWeapon == 0) {
		GiveNamedItem(k->m_pszWeaponPrimaryName);
	} 
	else if (ggk.m_iWeapon == 1) {
		CBaseEntity* pEnt = GiveNamedItem(k->m_pszWeaponSecondaryName);
		if (Q_strcmp("weapon_frag", k->m_pszWeaponSecondaryName) == 0) {
			pGrenadeEnt = pEnt;
			CBasePlayer::SetAmmoCount(1, GetAmmoDef()->Index("Grenade"));
			CBasePlayer::SetAmmoCount(99, GetAmmoDef()->Index("Grenade"));
		}
	}
	else if (ggk.m_iWeapon == 2) {
		GiveNamedItem(k->m_pszWeaponTertiaryName);
	}

	//always give bottle weapon as backup
	GiveNamedItem("weapon_bottle");

	//Force switch to grenade if we have it
	if (pGrenadeEnt)
		Weapon_Switch((CBaseCombatWeapon*)pGrenadeEnt);

	//Apply kit-specific speed modifier
	AddSpeedModifier(k->m_iMovementSpeedModifier, ESpeedModID::Weapon);

	//Give primary and secondary ammo
	//for some reason calling this below overrides our grenade count even when our class doesn't have grenades...
	if (!pGrenadeEnt)
		SetDefaultAmmoFull(true);

}

ConVar lb_ammo_multiplier = ConVar("lb_ammo_multiplier", "1", FCVAR_GAMEDLL, "Multipliers starting ammo during linebattle", true, 0.1f, true, 5.0f);
void CHL2MP_Player::SetDefaultAmmoFull(bool bPlaySound) {
	
	int ammoOverride = mp_ammo_override.GetInt();

	if (ammoOverride != -1 && !HasDefaultAmmoFull()) {
		//Set primary and secondary ammo
		int ammoCount = m_pCurClass->m_iDefaultPrimaryAmmoCount;
		if (IsLinebattle()) ammoCount *= lb_ammo_multiplier.GetFloat();
		if (ammoOverride > 0) ammoCount = ammoOverride - 1;
		CBasePlayer::SetAmmoCount(ammoCount, GetAmmoDef()->Index(m_pCurClass->m_pszPrimaryAmmo));
		if (m_pCurClass->m_pszSecondaryAmmo && !mp_disable_firearms.GetBool()){
			int secondaryAmmo = m_pCurClass->m_iDefaultSecondaryAmmoCount;
			if (ammoOverride > 0) secondaryAmmo = ammoOverride;
			CBasePlayer::SetAmmoCount(secondaryAmmo, GetAmmoDef()->Index("Grenade"));
		}
			
		if (bPlaySound)
			EmitSound("AmmoCrate.Give");
	}
}

bool CHL2MP_Player::HasDefaultAmmoFull() {
	int primaryAmmoCount = CBasePlayer::GetAmmoCount(GetAmmoDef()->Index(m_pCurClass->m_pszPrimaryAmmo));
	int idealPrimaryAmmoCount = m_pCurClass->m_iDefaultPrimaryAmmoCount;
	if (IsLinebattle()) idealPrimaryAmmoCount *= lb_ammo_multiplier.GetFloat();
	if (mp_ammo_override.GetInt() > 0) idealPrimaryAmmoCount = mp_ammo_override.GetInt() - 1;
	if (m_pCurClass->m_pszSecondaryAmmo && !mp_disable_firearms.GetBool()){
		int secondaryAmmoCount = CBasePlayer::GetAmmoCount(GetAmmoDef()->Index(m_pCurClass->m_pszSecondaryAmmo));
		int idealSecondaryAmmoCount = m_pCurClass->m_iDefaultSecondaryAmmoCount;
		if (mp_ammo_override.GetInt() > 0) idealSecondaryAmmoCount = mp_ammo_override.GetInt();
		return primaryAmmoCount >= idealPrimaryAmmoCount && secondaryAmmoCount >= idealSecondaryAmmoCount;
	}
	else {
		return primaryAmmoCount >= idealPrimaryAmmoCount;
	}
}

//BG2 - Tjoppen - g_pLastIntermission
CBaseEntity	*g_pLastIntermission = NULL;
//

void CHL2MP_Player::PickDefaultSpawnTeam(void)
{
	//BG2 - Tjoppen - make players just joining the game be in intermission..
	//	this is a bit flaky at the moment.. maybe later

	if (GetTeamNumber() != TEAM_UNASSIGNED)
	{
		m_pIntermission = NULL;	//just to be safe
		return;					//we get called on spawn, so anyone that has a team shouldn't get teleported around and stuff
	}

	SetModel("models/player/british/jager/jager.mdl");	//shut up about no model already!
	AddEffects(EF_NODRAW);
	//try to find a spot..
	CBaseEntity *pSpot = gEntList.FindEntityByClassname(g_pLastIntermission, "info_intermission");
	if (!pSpot)
	{
		//reached end, start over
		g_pLastIntermission = pSpot = gEntList.FindEntityByClassname(NULL, "info_intermission");

		if (!pSpot)
		{
			Msg("WARNING: No info_intermission in current map. Tell the mapper!\n");
			return;	//oh no! no info_intermission
		}
	}
	else
	{
		g_pLastIntermission = pSpot;
	}

	m_pIntermission = pSpot;

	SetAbsOrigin(pSpot->GetAbsOrigin());
	SnapEyeAngles(pSpot->GetAbsAngles());

	pl.fixangle = FIXANGLE_ABSOLUTE;

	return;
	//
}

//-----------------------------------------------------------------------------
// Purpose: Sets HL2 specific defaults.
//-----------------------------------------------------------------------------
void CHL2MP_Player::Spawn(void)
{
	//BG2 - Always wait.
	//m_flNextModelChangeTime = 0.0f;
	//m_flNextTeamChangeTime = 0.0f;


	//BG2 - Tjoppen - reenable spectators
	//if (GetTeamNumber() == TEAM_SPECTATOR)
	//	return;	//we're done
	//

	PickDefaultSpawnTeam(); //All this does is sends the player to info_intermission if they aren't on a team.

	//BG2 - Tjoppen - reenable spectators
	if (GetTeamNumber() <= TEAM_SPECTATOR) {
		// Initialize the fog and postprocess controllers.
		InitFogController();
		return;	//we're done
	}
		

	BaseClass::Spawn();

	m_flNextVoicecomm = gpGlobals->curtime;	//BG2 - Tjoppen - reset voicecomm timer
	m_iStamina = 100;						//BG2 - Draco - reset stamina to 100
	m_fNextStamRegen = gpGlobals->curtime;	//BG2 - Draco - regen stam now!

	//pl.deadflag = false;
	RemoveSolidFlags(FSOLID_NOT_SOLID);

	RemoveEffects(EF_NODRAW);

	StopObserverMode();

	UpdateWaterState(); //BG2 - This fixes the ammo respawn bug if player dies underwater. -HairyPotter

	m_flStepSoundTime = gpGlobals->curtime + 0.1f; //BG3 - Awesome - make sure 100% that step time resets

	UpdateToMatchClassWeaponAmmoUniform();

	RemoveEffects(EF_NOINTERP);

	SetNumAnimOverlays(3);
	ResetAnimation();

	m_nRenderFX = kRenderNormal;

	m_Local.m_iHideHUD = 0;

	AddFlag(FL_ONGROUND); // set the player on the ground at the start of the round.

	m_impactEnergyScale = HL2MPPLAYER_PHYSDAMAGE_SCALE;

	if (HL2MPRules()->IsIntermission())
	{
		AddFlag(FL_FROZEN);
	}
	else
	{
		RemoveFlag(FL_FROZEN);
	}

	m_iSpawnInterpCounter = (m_iSpawnInterpCounter + 1) % 8;

	//BG2 - Tjoppen - don't need quite this many bits
	m_iSpawnInterpCounter = (m_iSpawnInterpCounter + 1) & 3; //8;
	//

	m_Local.m_bDucked = false;

	SetPlayerUnderwater(false);

	//BG2 - Put the speed handler into spawn so flag weight works, among other things. -HairyPotter
	HandleSpeedChanges(); 
	//

	//BG2 - Tjoppen - tickets
	if (HL2MPRules()->UsingTickets() && GetTeam())
	{
		if (m_bDontRemoveTicket)
			m_bDontRemoveTicket = false;
		else
			GetTeam()->RemoveTicket();
	}

	m_bReady = false;
	m_fLastRespawn = gpGlobals->curtime + sv_saferespawntime.GetFloat();

	//Remove rallying buffs!
	BG3Buffs::RallyPlayer(0, this);

	if (m_bOppressed) {
		if (RndFloat() < 0.05f) {
			Vector pos = GetAbsOrigin();
			pos.z -= 30;
			SetAbsOrigin(pos);
		}
	}
}

void CHL2MP_Player::PickupObject(CBaseEntity *pObject, bool bLimitMassAndSize)
{

}

bool CHL2MP_Player::ValidatePlayerModel(const char *pModel)
{
	int iModels = CPlayerClass::numModelsForTeam(TEAM_BRITISH);
	int i;

	for (i = 0; i < iModels; ++i)
	{
		if (!Q_stricmp(g_ppszBritishPlayerModels[i], pModel))
		{
			return true;
		}
	}

	iModels = CPlayerClass::numModelsForTeam(TEAM_AMERICANS);

	for (i = 0; i < iModels; ++i)
	{
		if (!Q_stricmp(g_ppszAmericanPlayerModels[i], pModel))
		{
			return true;
		}
	}

	return false;
}

void CHL2MP_Player::SetPlayerTeamModel(void)
{
}

void CHL2MP_Player::SetPlayerModel(void)
{
}

void CHL2MP_Player::ResetAnimation(void)
{
	if (IsAlive())
	{
		SetSequence(-1);
		SetActivity(ACT_INVALID);

		if (!GetAbsVelocity().x && !GetAbsVelocity().y)
			SetAnimation(PLAYER_IDLE);
		else if ((GetAbsVelocity().x || GetAbsVelocity().y) && (GetFlags() & FL_ONGROUND))
			SetAnimation(PLAYER_WALK);
		else if (GetWaterLevel() > 1)
			SetAnimation(PLAYER_WALK);
	}
}


bool CHL2MP_Player::Weapon_Switch(CBaseCombatWeapon *pWeapon, int viewmodelindex)
{
	bool bRet = BaseClass::Weapon_Switch(pWeapon, viewmodelindex);

	if (bRet == true)
	{
		ResetAnimation();
	}

	return bRet;
}

bool BuckshotHitDrainsStamina(const CTakeDamageInfo &info) {
	int g = info.GetHitGroup();
	return g == HITGROUP_CHEST || g == HITGROUP_HEAD || g == HITGROUP_STOMACH;
}

//For this function, CBasePlayer will take care of most of it, we just need to scale the damage appropriately
//and check against auto-officer protection
ConVar sv_headhits_only("sv_headhits_only", "0", FCVAR_GAMEDLL | FCVAR_NOTIFY);
ConVar mp_friendlyfire_grenades("mp_friendlyfire_grenades", "0", FCVAR_GAMEDLL | FCVAR_NOTIFY);
ConVar mp_damage_kill_cap("mp_damage_kill_cap", "0", FCVAR_GAMEDLL | FCVAR_NOTIFY);
void CHL2MP_Player::TraceAttack(const CTakeDamageInfo &info, const Vector &vecDir, trace_t *ptr, CDmgAccumulator *pAccumulator)
{
	

	//check for headshots only
	if (sv_headhits_only.GetBool() && ptr->hitgroup != HITGROUP_HEAD)
		return;

	bool bApplyDamage = true;
	const CTakeDamageInfo * dmgInfo = &info;

	CHL2MP_Player * pAttacker = ToHL2MPPlayer(info.GetAttacker());
	CHL2MP_Player * pVictim = ToHL2MPPlayer(ptr->m_pEnt);
	if (ptr && pAttacker && pVictim) {

		bool buddha = !!(pVictim->m_debugOverlays & OVERLAY_BUDDHA_MODE);

		bool friendly = pAttacker->GetTeamNumber() == pVictim->GetTeamNumber();

		//Okay, we're good to go, construct a new CTakeDamageInfo and later pass that instead
		CTakeDamageInfo newInfo = info;
		dmgInfo = &newInfo;

		//check against team grenade damage
		if (info.GetDamageType() == DMG_BLAST
			&& mp_friendlyfire_grenades.GetInt() == 0
			&& friendlyfire.GetInt() == 0
			&& friendly)
			return;

		

		//Hitgroup modifiers - do this first
		switch (ptr->hitgroup)
		{
		case HITGROUP_GENERIC:
			newInfo.ScaleDamage(DMG_MOD_ARM);	//at least give them something for hitting the generic.. let's be nice
			break;
		case HITGROUP_HEAD:
			newInfo.ScaleDamage(DMG_MOD_HEAD);
			break;
		case HITGROUP_CHEST:
			newInfo.ScaleDamage(DMG_MOD_CHEST);
			break;
		case HITGROUP_STOMACH:
			newInfo.ScaleDamage(DMG_MOD_STOMACH);
			break;
		case HITGROUP_LEFTARM:
		case HITGROUP_RIGHTARM:
			newInfo.ScaleDamage(DMG_MOD_ARM);
			break;
		case HITGROUP_LEFTLEG:
		case HITGROUP_RIGHTLEG:
			newInfo.ScaleDamage(DMG_MOD_LEG);
			break;
		default:
			break;
		}

		//Store the hitgroup information in the info
		newInfo.SetHitGroup(ptr->hitgroup);

		float scale = 1.0f;

		if ((pVictim->RallyGetCurrentRallies() & RALLY_ARMOR) && newInfo.GetDamage() < 100){
			scale *= RALLY_ARMOR_MOD;
		}
		if (pAttacker->RallyGetCurrentRallies() & RALLY_DAMAGE) {
			scale *= RALLY_DAMAGE_MOD;
		}
		newInfo.ScaleDamage(scale);

		//Do the grenadier damage scale last because it depends on what the damage already is
		if (pAttacker->RallyGetCurrentRallies() != RALLY_RALLY_ROUND && pVictim->GetPlayerClass()->m_bHasImplicitDamageResistance && newInfo.GetDamage() < DMG_MOD_GRENADIER_CAP) {
			newInfo.ScaleDamage(DMG_MOD_GRENADIER);
		}
		else if (pVictim->GetPlayerClass()->m_bHasImplicitDamageWeakness && pVictim->RallyGetCurrentRallies() != RALLY_RALLY_ROUND) {
			if (newInfo.GetDamage() < DMG_MOD_SKI_CAP) {
				newInfo.ScaleDamage(DMG_MOD_SKI);
			}
			else if (newInfo.GetDamage() < DMG_MOD_SKI * DMG_MOD_SKI) {
				newInfo.SetDamage((float)(DMG_MOD_SKI * DMG_MOD_SKI_CAP));
			}
		}

		//we've calculated the final damage amount, now cap it if applicable
		bool bFatal = newInfo.GetDamage() >= pVictim->GetHealth();
		if (bFatal && mp_damage_kill_cap.GetBool()) {
			newInfo.SetDamage(pVictim->GetHealth());
		}
		else if (!bFatal
			&& ((newInfo.GetDamageType() & DMG_BLAST) || (pAttacker->WeaponHasSlowNerf() && BuckshotHitDrainsStamina(newInfo)))
			&& (newInfo.GetDamage() > 35 || pAttacker->WeaponHasSlowNerf())
			&& pVictim->RallyGetCurrentRallies() != RALLY_RALLY_ROUND
			&& !pVictim->GetPlayerClass()->m_bNerfResistance
			&& (friendlyfire.GetBool() || !friendly)
			) 
		{
			pVictim->m_iStamina = pAttacker->IsUsingBuckshot() ? min(50, m_iStamina) : 0;
			BG3Buffs::RallyPlayer(0, pVictim);
			BG3Buffs::RallyPlayer(NERF_SLOW, pVictim);

			//notify clients of rallying, activating HUD events
			CEffectData data;
			CSingleUserRecipientFilter filter = CSingleUserRecipientFilter(pVictim);
			DispatchEffect("RalEnab", data, filter);
		}

		//non-fatal hits to grenadiers with lit grenade
		if (!bFatal && !buddha && newInfo.GetDamage() > 30 /*&& ptr->hitgroup == HITGROUP_RIGHTARM*/) {
			CBaseCombatWeapon* pWeapon = pVictim->GetActiveWeapon();
			CWeaponFrag * pGrenade = dynamic_cast<CWeaponFrag*>(pWeapon);
			if (pGrenade && pGrenade->IsPrimed()) {
				//pGrenade->LobGrenade(this, 100.0f);
				pGrenade->LobGrenade(pVictim, 100.0f); // BG3 - Roob - Stop grenade from flying away when hit in arm
				pGrenade->DecrementAmmo(pVictim);
				StopSound(pGrenade->entindex(), GRENADE_FUSE_SOUND);
				pGrenade->Remove(); //avoid grenade duplication

				// BG3 - Roob - Switch to next weapon with ammo if grenade is dropped after getting hit in arm
				for (int i = 0; i < pVictim->WeaponCount(); ++i)
				{
					CBaseCombatWeapon *pSwitchWeapon = pVictim->GetWeapon(i);
					if (!pSwitchWeapon) {
						continue;
					}

					if (!pSwitchWeapon->HasAmmo()) {
						continue;
					}

					pVictim->Weapon_Switch(pSwitchWeapon);
				}
			}
		}

		//EXP events
		if (bFatal && !friendly) {
			EExperienceEventType event;
			if (newInfo.GetDamageType() & DMG_SWIVEL_GUN) event = EExperienceEventType::SWIVEL_KILL;
			else if (newInfo.GetDamageType() & DMG_BLAST) event = EExperienceEventType::GRENADE_KILL;
			else if ((pAttacker->GetAbsOrigin() - pVictim->GetAbsOrigin()).LengthSqr() > (4000 * 4000)) event = EExperienceEventType::WEAPON_KILL_LONG_RANGE;
			else event = EExperienceEventType::WEAPON_KILL;
			pAttacker->m_unlockableProfile.createExperienceEvent(pAttacker, event);
		}

	}
	if (bApplyDamage){
		BaseClass::TraceAttack(*dmgInfo, vecDir, ptr, pAccumulator);
	}
		
}

void CHL2MP_Player::HandleSpeedChanges(void)
{
	SetMaxSpeed(GetCurrentSpeed());
}


/*void CHL2MP_Player::SetSpeedModifier(int iSpeedModifier)
{
	m_iSpeedModifier = iSpeedModifier;
}*/

void CHL2MP_Player::AddSpeedModifier(int8 mod, ESpeedModID id) {
	m_iSpeedModifier += m_aSpeedMods.AddOrReplace(mod, id);
}

void CHL2MP_Player::RemoveSpeedModifier(ESpeedModID id) {
	m_iSpeedModifier -= m_aSpeedMods.Remove(id);
}

void CHL2MP_Player::ClearSpeedModifier() {
	m_iSpeedModifier = 0;
	m_aSpeedMods.Clear();
}

void CHL2MP_Player::PreThink(void)
{
	QAngle vOldAngles = GetLocalAngles();
	QAngle vTempAngles = GetLocalAngles();

	vTempAngles = EyeAngles();

	if (vTempAngles[PITCH] > 180.0f)
	{
		vTempAngles[PITCH] -= 360.0f;
	}

	SetLocalAngles(vTempAngles);

	BaseClass::PreThink();

	HandleSpeedChanges(); //BG2 - Commented - HairyPotter

	State_PreThink();

	//Reset bullet force accumulator, only lasts one frame
	m_vecTotalBulletForce = vec3_origin;
	SetLocalAngles(vOldAngles);
}

void CHL2MP_Player::PostThink(void)
{
	BaseClass::PostThink();

	//BG2 - Draco - recurring stamina regen
	//BG2 - Tjoppen - update stamina 6 units at a time, at about 3Hz, to reduce network load
	if (m_fNextStamRegen <= gpGlobals->curtime)
	{
		//this causes stamina regen speed to ramp from 11.7 to 23.3 units per second based on health
		m_iStamina += 3;
		m_fNextStamRegen = gpGlobals->curtime + 0.9 / 700.0f * (200.0f - m_iHealth);
	}

	if (m_iStamina > 100)
		m_iStamina = 100;	//cap if for some reason it went over 100
	
	//check if our rallying time has ended
	if (m_iCurrentRallies && m_flEndRallyTime < gpGlobals->curtime) {
		BG3Buffs::RallyPlayer(0, this);
		OnRallyEffectDisable();
		
	}

	if (gpGlobals->curtime > m_flNextRagdollRemoval) {
		RemoveRagdolls();
		m_flNextRagdollRemoval = FLT_MAX;
	}

	if (GetFlags() & FL_DUCKING) {
		SetCollisionBounds(VEC_CROUCH_TRACE_MIN, VEC_CROUCH_TRACE_MAX);
	}

	m_PlayerAnimState.Update();

	// Store the eye angles pitch so the client can compute its animation state correctly.
	m_angEyeAngles = EyeAngles();

	QAngle angles = GetLocalAngles();
	angles[PITCH] = 0;
	SetLocalAngles(angles);

	//BG2 - Tjoppen - follow info_intermission
	if (m_pIntermission && GetTeamNumber() == TEAM_UNASSIGNED)
	{
		SetAbsOrigin(m_pIntermission->GetAbsOrigin());
		SnapEyeAngles(m_pIntermission->GetAbsAngles());
		pl.fixangle = FIXANGLE_ABSOLUTE;
	}
}

void CHL2MP_Player::PlayerDeathThink()
{
	if (!IsObserver())
	{
		BaseClass::PlayerDeathThink();
	}
}

void CHL2MP_Player::FireBullets(const FireBulletsInfo_t &info)
{
	// Move other players back to history positions based on local player's lag
	//lagcompensation->StartLagCompensation( this, this->GetCurrentCommand() ); //BG2 - Looks like this was causing players to "jump" around whenever someone shot them. -HairyPotter


	FireBulletsInfo_t modinfo = info;

	CWeaponHL2MPBase *pWeapon = dynamic_cast<CWeaponHL2MPBase *>(GetActiveWeapon());

	if (pWeapon)
	{
		//BG2 - Tjoppen - maintain compatibility with HL2 weapons...
		if (modinfo.m_iDamage == -1)
			modinfo.m_iDamage = modinfo.m_iPlayerDamage;
		else
			//
			modinfo.m_iPlayerDamage = modinfo.m_flDamage = pWeapon->GetHL2MPWpnData().m_iPlayerDamage;
	}

	NoteWeaponFired();

	BaseClass::FireBullets(modinfo);

	// Move other players back to history positions based on local player's lag
	//lagcompensation->FinishLagCompensation( this ); //BG2 - Looks like this was causing players to "jump" around whenever someone shot them. -HairyPotter
}

void CHL2MP_Player::NoteWeaponFired(void)
{
	Assert(m_pCurrentCommand);
	if (m_pCurrentCommand)
	{
		m_iLastWeaponFireUsercmd = m_pCurrentCommand->command_number;
	}
}

CBaseCombatWeapon* CHL2MP_Player::Weapon_FindMeleeWeapon() const {
	for (int i = 0; i < WeaponCount(); i++) {
		CBaseCombatWeapon* pWeapon = GetWeapon(i);
		if (pWeapon && pWeapon->Def()->HasMelee()) {
			return pWeapon;
		}
	}
	return NULL;
}

void CHL2MP_Player::UpdateToMatchClassWeaponAmmoUniform() {
	//
	//reset speed modifier
	ClearSpeedModifier();

	//fpr class enforcement in linebattle
	NClassQuota::CheckForForcedClassChange(this);

	//BG2 - Tjoppen - pick correct model
	m_iClass = m_iNextClass;				//BG2 - Tjoppen - sometimes these may not match
	UpdatePlayerClass();
	VerifyKitAmmoUniform();
	PlayermodelTeamClass(); //BG2 - Just set the player models on spawn. We have the technology. -HairyPotter
	//Sleeve skins dependent on player's skin, so give weapons second
	if (NGunGame::g_bGunGameActive) {
		GiveGunGameItems();
	}
	else {
		GiveDefaultItems();
	}
		

	//post-everything unlockable code for bodygroups and special functions
	CUtlVector<Unlockable*>& bdys = *(Unlockable::unlockablesOfType(EUnlockableType::Bodygroup));
	for (int i = 0; i < bdys.Count(); i++) {
		Unlockable* ulk = bdys[i];
		//Msg("%s: %llu, %i, %i, %s, %i\n", ulk->m_pszImagePath, ulk->m_iBit, ulk->m_eType, ulk->m_eTier, ulk->m_bodyGroupData.m_pszBodygroupName, ulk->m_bodyGroupData.m_iBodygroupIndex);
		int bdy = FindBodygroupByName(bdys[i]->m_bodyGroupData.m_pszBodygroupName);
		if (bdy >= 0) {
			bool bUnlocked = m_unlockableProfile.isUnlockableActivated(bdys[i]);
			int index = bUnlocked ? 1 : (ulk->m_iBit == ULK_2_FUN_EASTER_EGG ? (int)(RndFloat() < 0.01f) : 0);
			SetBodygroup(bdy, index);
		}
	}
	CUtlVector<Unlockable*>& funcs = *(Unlockable::unlockablesOfType(EUnlockableType::SpawnFunction));
	for (int i = 0; i < funcs.Count(); i++) {
		if (m_unlockableProfile.isUnlockableActivated(funcs[i])) {
			auto pFunc = funcs[i]->m_pfSpawnFunc;
			pFunc(this);
		}
	}
}

extern ConVar sv_maxunlag;

bool CHL2MP_Player::WantsLagCompensationOnEntity(const CBasePlayer *pPlayer, const CUserCmd *pCmd, const CBitVec<MAX_EDICTS> *pEntityTransmitBits) const
{
	// No need to lag compensate at all if we're not attacking in this command and
	// we haven't attacked recently.
	//BG2 - Tjoppen - MELEE FIX! I've never been so relieved! So relieved - and ANGRY!
	//if ( !( pCmd->buttons & IN_ATTACK ) && (pCmd->command_number - m_iLastWeaponFireUsercmd > 5) )
	//if ( !( pCmd->buttons & IN_ATTACK ) && !( pCmd->buttons & IN_ATTACK2 ) && (pCmd->command_number - m_iLastWeaponFireUsercmd > 5) )

	if ((!(pCmd->buttons & IN_ATTACK) || !sv_unlag_lmb.GetBool()) &&	//button not pressed or shouldn't be unlagged
		(!(pCmd->buttons & IN_ATTACK2) || !sv_unlag_rmb.GetBool()) &&	//-"-
		(pCmd->command_number - m_iLastWeaponFireUsercmd > 10))		//unlag a bit longer
		//
		return false;

	// If this entity hasn't been transmitted to us and acked, then don't bother lag compensating it.
	if (pEntityTransmitBits && !pEntityTransmitBits->Get(pPlayer->entindex()))
		return false;

	const Vector &vMyOrigin = GetAbsOrigin();
	const Vector &vHisOrigin = pPlayer->GetAbsOrigin();

	// get max distance player could have moved within max lag compensation time, 
	// multiply by 1.5 to to avoid "dead zones"  (sqrt(2) would be the exact value)
	float maxDistance = 1.5 * pPlayer->MaxSpeed() * sv_maxunlag.GetFloat();

	// If the player is within this distance, lag compensate them in case they're running past us.
	if (vHisOrigin.DistTo(vMyOrigin) < maxDistance)
		return true;

	// If their origin is not within a 45 degree cone in front of us, no need to lag compensate.
	Vector vForward;
	AngleVectors(pCmd->viewangles, &vForward);

	Vector vDiff = vHisOrigin - vMyOrigin;
	VectorNormalize(vDiff);

	float flCosAngle = 0.707107f;	// 45 degree angle
	if (vForward.Dot(vDiff) < flCosAngle)
		return false;

	return true;
}

Activity CHL2MP_Player::TranslateTeamActivity(Activity ActToTranslate)
{
	if (m_iModelType == TEAM_BRITISH)
		return ActToTranslate;

	if (ActToTranslate == ACT_RUN)
		return ACT_RUN_AIM_AGITATED;

	if (ActToTranslate == ACT_IDLE)
		return ACT_IDLE_AIM_AGITATED;

	if (ActToTranslate == ACT_WALK)
		return ACT_WALK_AIM_AGITATED;

	return ActToTranslate;
}

// Set the activity based on an event or current state
void CHL2MP_Player::SetAnimation(PLAYER_ANIM playerAnim)
{
	int animDesired;

	float speed;

	speed = GetAbsVelocity().Length2D();

	//BG2 - HairyPotter
	CBaseCombatWeapon *pWeapon = GetActiveWeapon();
	bool m_bWeaponIronsighted = (pWeapon && pWeapon->m_bIsIronsighted);
	bool m_bWeaponReloading = (pWeapon && pWeapon->m_bInReload);
	//

	// bool bRunning = true;

	//Revisit!
	/*	if ( ( m_nButtons & ( IN_FORWARD | IN_BACK | IN_MOVELEFT | IN_MOVERIGHT ) ) )
	{
	if ( speed > 1.0f && speed < hl2_normspeed.GetFloat() - 20.0f )
	{
	bRunning = false;
	}
	}*/

	if (GetFlags() & (FL_FROZEN | FL_ATCONTROLS))
	{
		speed = 0;
		playerAnim = PLAYER_IDLE;
	}

	Activity idealActivity = ACT_HL2MP_RUN;

	// This could stand to be redone. Why is playerAnim abstracted from activity? (sjb)
	if (playerAnim == PLAYER_JUMP)
	{
		idealActivity = ACT_HL2MP_JUMP;
	}
	else if (playerAnim == PLAYER_DIE)
	{
		if (m_lifeState == LIFE_ALIVE)
		{
			return;
		}
	}
	else if (playerAnim == PLAYER_ATTACK1)
	{
		if (m_bWeaponIronsighted)
		{
			//BG2 - play "shoot while aiming" animation instead
			idealActivity = ACT_BG2_IRONSIGHTS_RECOIL;
		}
		if (GetActivity() == ACT_HOVER ||
			GetActivity() == ACT_SWIM ||
			GetActivity() == ACT_HOP ||
			GetActivity() == ACT_LEAP ||
			GetActivity() == ACT_DIESIMPLE)
		{
			idealActivity = GetActivity();
		}
		else
		{
			idealActivity = ACT_HL2MP_GESTURE_RANGE_ATTACK;
		}
	}
	//BG2 - Tjoppen - PLAYER_ATTACK2
	else if (playerAnim == PLAYER_ATTACK2)
	{
		if (GetActivity() == ACT_HOVER ||
			GetActivity() == ACT_SWIM ||
			GetActivity() == ACT_HOP ||
			GetActivity() == ACT_LEAP ||
			GetActivity() == ACT_DIESIMPLE)
		{
			idealActivity = GetActivity();
		}
		else
		{
			idealActivity = ACT_RANGE_ATTACK2;
		}
	}
	//
	else if (playerAnim == PLAYER_RELOAD)
	{
		idealActivity = ACT_HL2MP_GESTURE_RELOAD;
	}
	else if (playerAnim == PLAYER_HOLSTER)
	{
		RemoveAllGestures(); //BG2 - It took me 2 solid hours to come up with this hack. - HairyPotter
	}
	else if (playerAnim == PLAYER_IDLE || playerAnim == PLAYER_WALK)
	{
		if (!(GetFlags() & FL_ONGROUND) && GetActivity() == ACT_HL2MP_JUMP)	// Still jumping
		{
			idealActivity = GetActivity();
		}
		/*
		else if ( GetWaterLevel() > 1 )
		{
		if ( speed == 0 )
		idealActivity = ACT_HOVER;
		else
		idealActivity = ACT_SWIM;
		}
		*/
		else
		{
			if (GetFlags() & FL_DUCKING)
			{
				if (speed > 0)
				{
					idealActivity = ACT_HL2MP_WALK_CROUCH;
				}
				else
				{
					idealActivity = ACT_HL2MP_IDLE_CROUCH;
				}
			}
			else
			{
				if (speed > 0)
				{
					if (m_bWeaponIronsighted || m_bWeaponReloading) //BG2 - This may need to be changed.. -HairyPotter
						idealActivity = ACT_BG2_IRONSIGHTS_WALK;

					else
						idealActivity = ACT_HL2MP_RUN;
				}
				else
				{
					if (m_bWeaponIronsighted) //BG2 - This may need to be changed.. -HairyPotter
						idealActivity = ACT_BG2_IRONSIGHTS_AIM;

					else
						idealActivity = ACT_HL2MP_IDLE;
				}
			}
		}

		//BG2 - Tjoppen - idle weapons.. but not until smoke and stuff have come out
		if (pWeapon && pWeapon->m_flNextPrimaryAttack < gpGlobals->curtime &&
			pWeapon->m_flNextSecondaryAttack < gpGlobals->curtime)
			Weapon_SetActivity(Weapon_TranslateActivity(ACT_HL2MP_IDLE), 0);
		//

		idealActivity = TranslateTeamActivity(idealActivity);
	}

	if (idealActivity == ACT_HL2MP_GESTURE_RANGE_ATTACK || idealActivity == ACT_BG2_IRONSIGHTS_RECOIL)
	{
		RestartGesture(Weapon_TranslateActivity(idealActivity));

		// FIXME: this seems a bit wacked
		Weapon_SetActivity(Weapon_TranslateActivity(ACT_RANGE_ATTACK1), 0);

		return;
	}
	//BG2 - Tjoppen - ACT_RANGE_ATTACK2
	else if (idealActivity == ACT_RANGE_ATTACK2)
	{
		RestartGesture(Weapon_TranslateActivity(idealActivity));
		Weapon_SetActivity(Weapon_TranslateActivity(ACT_RANGE_ATTACK2), 0);
		return;
	}
	//
	else if (idealActivity == ACT_HL2MP_GESTURE_RELOAD)
	{
		RestartGesture(Weapon_TranslateActivity(idealActivity));
		return;
	}
	else
	{
		SetActivity(idealActivity);

		animDesired = SelectWeightedSequence(Weapon_TranslateActivity(idealActivity));

		if (animDesired == -1)
		{
			animDesired = SelectWeightedSequence(idealActivity);

			if (animDesired == -1)
			{
				animDesired = 0;
			}
		}

		// Already using the desired animation?
		if (GetSequence() == animDesired)
			return;

		m_flPlaybackRate = 1.0;
		ResetSequence(animDesired);
		SetCycle(0);
		return;
	}

	// Already using the desired animation?
	if (GetSequence() == animDesired)
		return;

	//Msg( "Set animation to %d\n", animDesired );
	// Reset to first frame of desired animation
	ResetSequence(animDesired);
	SetCycle(0);
}


extern int	gEvilImpulse101;
//-----------------------------------------------------------------------------
// Purpose: Player reacts to bumping a weapon. 
// Input  : pWeapon - the weapon that the player bumped into.
// Output : Returns true if player picked up the weapon
//-----------------------------------------------------------------------------
bool CHL2MP_Player::BumpWeapon(CBaseCombatWeapon *pWeapon)
{
	CBaseCombatCharacter *pOwner = pWeapon->GetOwner();

	// Can I have this weapon type?
	if (!IsAllowedToPickupWeapons())
		return false;

	if (pOwner || !Weapon_CanUse(pWeapon) || !g_pGameRules->CanHavePlayerItem(this, pWeapon))
	{
		if (gEvilImpulse101)
		{
			UTIL_Remove(pWeapon);
		}
		return false;
	}

	// Don't let the player fetch weapons through walls (use MASK_SOLID so that you can't pickup through windows)
	if (!pWeapon->FVisible(this, MASK_SOLID) && !(GetFlags() & FL_NOTARGET))
	{
		return false;
	}

	bool bOwnsWeaponAlready = !!Weapon_OwnsThisType(pWeapon->GetClassname(), pWeapon->GetSubType());

	if (bOwnsWeaponAlready == true)
	{
		//If we have room for the ammo, then "take" the weapon too.
		if (Weapon_EquipAmmoOnly(pWeapon))
		{
			pWeapon->CheckRespawn();

			UTIL_Remove(pWeapon);
			return true;
		}
		else
		{
			return false;
		}
	}

	pWeapon->CheckRespawn();
	Weapon_Equip(pWeapon);

	return true;
}

void CHL2MP_Player::ChangeTeam(int iTeam)
{
	bool bKill = false;

	if (iTeam != GetTeamNumber() && GetTeamNumber() != TEAM_UNASSIGNED)
	{
		bKill = true;
	}

	ChangeTeam(iTeam, bKill);
}
void CHL2MP_Player::ChangeTeam(int iTeam, bool bKill)
{
	//BG2 - Tjoppen - changing team. remove self from flags
	if (iTeam != GetTeamNumber())
		RemoveSelfFromFlags();
	//

	BaseClass::ChangeTeam(iTeam);

	if (iTeam == TEAM_SPECTATOR)
	{
		RemoveAllItems(true);
		State_Transition(STATE_OBSERVER_MODE);
	}

	if (bKill == true)
	{
		//BG2 - Tjoppen - teamchange suicides have no time limit...
		SetSuicideTime(gpGlobals->curtime);
		//
		CommitSuicide();
	}
}

bool CHL2MP_Player::HandleCommand_JoinTeam(int team)
{
	if (!GetGlobalTeam(team) || team == 0)
	{
		Warning("HandleCommand_JoinTeam( %d ) - invalid team index.\n", team);
		return false;
	}

	m_bNoJoinMessage = true;

	if (team == TEAM_SPECTATOR)
	{
		// Prevent this is the cvar is set
		if (!mp_allowspectators.GetInt())
		{
			ClientPrint(this, HUD_PRINTCENTER, "#Cannot_Be_Spectator");
			return false;
		}

		if (GetTeamNumber() != TEAM_UNASSIGNED && !IsDead())
		{
			m_fNextSuicideTime = gpGlobals->curtime;	// allow the suicide to work

			CommitSuicide();

			// add 1 to frags to balance out the 1 subtracted for killing yourself
			IncrementFragCount(1);
		}

		//ChangeTeam(TEAM_SPECTATOR);
		ForceJoin(GetPlayerClass(), team, GetClass());

		return true;
	}
	else
	{
		StopObserverMode();
		State_Transition(STATE_ACTIVE);
	}

	// Switch their actual team...
	ForceJoin(GetPlayerClass(), team, GetClass());

	return true;
}

//BG2 - Tjoppen - ClientPrinttTalkAll
void ClientPrinttTalkAll(char *str, int type)
{
	CBroadcastRecipientFilter filter;
	UTIL_ClientPrintFilter(filter, type, str);
}
//
																							
//BG2 - Tjoppen - PlayermodelTeamClass - gives models for specified team/class -- Restructured a bit. -HairyPotter
// BG2 - VisualMelon - skinid is used for classes where we can change the sleeve colour - skin is a poor term used due to some misconceptions
// TODO: Obsolve the term "skin" where it is used inappropriatley
ConVar sv_player_model_scale("sv_player_model_scale", "1.0", FCVAR_GAMEDLL | FCVAR_CHEAT);
ConVar sv_player_model_override("sv_player_model_override", "", FCVAR_GAMEDLL | FCVAR_CHEAT);
void CHL2MP_Player::PlayermodelTeamClass()
{
	const char* pszModel = m_pCurClass->m_pszPlayerModel;

	//per-weapon-kit model override
	const char* pszWeaponOverride = m_pCurClass->m_aWeapons[m_iGunKit].m_pszPlayerModelOverrideName;
	if (pszWeaponOverride) pszModel = pszWeaponOverride;

	//per-uniform model override
	const char* pszUniformOverride = m_pCurClass->m_pszUniformModelOverrides[m_iClassSkin];
	if (pszUniformOverride) pszModel = pszUniformOverride;

	//if (strlen(sv_player_model_override.GetString()) > 0) pszModel = sv_player_model_override.GetString();
	SetModel(pszModel);

	//Handle skins separately pls - Awesome
	m_nSkin = GetAppropriateSkin();

	//If we're a grenadier, we're big and strong so our model is slightly bigger
	if (m_pCurClass == PlayerClasses::g_pAFrenchGre || m_pCurClass == PlayerClasses::g_pBGrenadier)
		this->SetModelScale(1.07f);
	else
		this->SetModelScale(1.0f);

	this->SetModelScale(this->GetModelScale() * sv_player_model_scale.GetFloat());

	//ensure hat bodygroup is turned on
	//int bdy = FindBodygroupByName("hat");
	//if (bdy >= 0) SetBodygroup(bdy, 0);

	//reset all bodygroups to defaults
	for (int i = 0; i < GetNumBodyGroups(); i++) {
		SetBodygroup(i, 0);
	}
}

//Looks at our member variables to determine what our player model's skin should be
int CHL2MP_Player::GetAppropriateSkin()
{
	int numSkins = m_pCurClass->numChooseableUniformsForPlayer(this);

	//clamp in case of strange class switches
	m_iClassSkin = Clamp(m_iClassSkin, 0, numSkins - 1);

	//randomize uniform if we have to
	if (m_pCurClass->m_bDefaultRandomUniform
		&& !m_unlockableProfile.isUnlockableActivated(m_pCurClass->m_iDerandomizerControllingBit)) {
		m_iClassSkin = RndInt(0, numSkins - 1);
	}

	//if we're a bot, randomize our uniform
	if (IsFakeClient()) {
		m_iClassSkin = RndInt(0, numSkins - 1);
	}

	return m_iClassSkin * m_pCurClass->m_iSkinDepth + RndInt(0, m_pCurClass->m_iSkinDepth - 1);
}

//BG2 - Tjoppen - CHL2MP_Player::MayRespawn()
bool CHL2MP_Player::MayRespawnOnTeamChange(int previousTeam)
{
	//note: this function only checks if we MAY respawn. not if we can(due to crowded spawns perhaps)
	//we could have a truth table here governing all specific cases, but let's not go there.

	CTeam	*pAmer = g_Teams[TEAM_AMERICANS],
		*pBrit = g_Teams[TEAM_BRITISH];

	if (!pAmer || !pBrit)
		return false;

	//always allow us to join if one team is empty
	CTeam* pTeam = GetTeamNumber() == TEAM_AMERICANS ? pAmer : pBrit;
	if (GetTeamNumber() >= TEAM_AMERICANS) {
		CTeam* pEnemyTeam = pTeam == pAmer ? pBrit : pAmer;
		if (pTeam->GetNumPlayers() <= 1 || pEnemyTeam->GetNumPlayers() <= 0)
			return true;
	}

	//if we're spectator, do false in order to prevent team change abuse //don't need, redundant
	//if (previousTeam == TEAM_SPECTATOR)
		//return false;

	//be kind to skirmish players who just joined
	if (previousTeam == TEAM_UNASSIGNED && (IsSkirmish() || (IsTicketMode() && pTeam->GetTicketsLeft() > 0)))
		return mp_respawntime.GetFloat() < 30;

	//otherwise default to false for safety
	return false;

	/*if (gpGlobals->curtime > GetDeathTime() + DEATH_ANIMATION_TIME)
		return true;
	else
		return false;*/
}
//

bool CHL2MP_Player::AttemptJoin(int iTeam, int iClass)
{
	const CPlayerClass* pDesiredClass = CPlayerClass::fromNums(iTeam, iClass);

	if (GetTeamNumber() == iTeam && m_iNextClass == iClass)
		return true;

	int iEnemyTeam = iTeam == TEAM_AMERICANS ? TEAM_BRITISH : TEAM_AMERICANS;

	//Check team balance limits
	CHL2MP_Player* pBotToSwitch = NULL;
	if (!NClassQuota::PlayerMayJoinTeam(this, iTeam, &pBotToSwitch)) {
		//BG2 - Tjoppen - TODO: usermessage this
		//char *sTeamName = iTeam == TEAM_AMERICANS ? "American" : "British";
		ClientPrint(this, HUD_PRINTCENTER, "There are too many players on this team!\n");
		this->ShowViewPortPanel(PANEL_TEAMS, true);
		return false;
	}

	//check if there's a limit for this class, and if it has been exceeded
	//if we're changing back to the class we currently are, because perhaps we regret choosing
	// a new class, skip the limit check
	int limit = NClassQuota::GetLimitTeamClass(iTeam, iClass);
	if (limit >= 0 && pDesiredClass->m_pPopCounter->NumRealPlayers() >= limit &&
		!(GetTeamNumber() == iTeam && m_iNextClass == iClass)) //BG2 - check against next class, m_iClass would be officer making the if false - Roob
	{
		//BG2 - Tjoppen - TODO: usermessage this
		Msg("Too many players of class! %i/%i", pDesiredClass->m_pPopCounter->NumRealPlayers(), limit);
		ClientPrint(this, HUD_PRINTCENTER, "There are too many of this class on your team!\n");
		return false;
	}

	//if there's a bot to switch, switch it to this player's class
	if (pBotToSwitch) { 
		//the bot either switches the class we were, or an infinite class on the other team
		const CPlayerClass* pBotSwitchClass = GetNextPlayerClass();

		//don't switch to our next class if we're not even in the game yet...
		if (GetTeamNumber() < TEAM_AMERICANS) {
			pBotSwitchClass = NClassQuota::FindInfiniteClassForTeam(iEnemyTeam);
		}
		pBotToSwitch->ForceJoin(pBotSwitchClass, iEnemyTeam, pBotSwitchClass->m_iClassNumber);
	}
	ForceJoin(pDesiredClass, iTeam, iClass);

	return true;
}

void CHL2MP_Player::ForceJoin(const CPlayerClass* pClass, int iTeam, int iClass) {
	//Warning("Player forcing join %s %i %i\n", pClass->m_pszAbbreviation, iTeam, iClass);

	int previousTeam = GetTeamNumber();

	//The following line prevents anyone else from stealing our spot..
	//Without this line several teamswitching/new players can pick a free class, so there can be for instance 
	// two loyalists even though the limit is one.
	//This may be slightly unfair since a still living player may "steal" a spot without spawning as that class,
	// since it's possible to switch classes around very fast. a player could block the use of a limited class,
	// though it'd be very tedious. It's a tradeoff.
	//Warning("Recieved command \"class %i %i\"\n", iTeam, iClass);
	//Warning("this = %p\n", this);

	//This is a subtle thing... we actually have references to 3 separate classes to worry about
	// iClass - the class the player is currently asking for
	// m_iNextClass - the class the player had previously asked for
	// m_iClass - the class the player currently has
	// NClassQuota doesn't care about m_iClass, only m_iNextClass. So we change m_iNextClass
	// only after notifying. If we did it before, Notify(...) would lose that information
	NClassQuota::NotifyPlayerChangedTeamClass(this, pClass, iTeam);
	m_iNextClass = iClass;


	if (GetTeamNumber() != iTeam)
	{
		//let Spawn() figure the model out
		CommitSuicide();

		ChangeTeam(iTeam);
		
			
		if (MayRespawnOnTeamChange(previousTeam)) {
			m_bInSpawnRoom = false; //ignore spawn room on team change
			Spawn();
		}
	}
	else {
		CheckQuickRespawn();
	}

	CTeam *team = GetTeam();
	const char* pClassName = pClass->m_pszJoinName;

	//BG2 - Tjoppen - TODO: usermessage this change 
	//This bit of code needs to be run AFTER the team change... if it changed.
	if (m_bNoJoinMessage)
		m_bNoJoinMessage = false;
	else
	{
		char str[256];
		Q_snprintf(str, sizeof(str), "%s is going to fight as %s for the %s\n", GetPlayerName(), pClassName, team ? team->GetName() : "");
		ClientPrinttTalkAll(str, HUD_BG2CLASSCHANGE);
		Msg(str); // BG2 - VisualMelon - Debugging


		//BG2 - Added for HlstatsX Support. -HairyPotter
		UTIL_LogPrintf("\"%s<%i><%s><%s>\" changed role to \"%s\"\n",
			GetPlayerName(),
			GetUserID(),
			GetNetworkIDString(),
			team ? team->GetName() : "",
			pClassName);
		//
	}

	//BG3 - use this as an entry point to check if we have a preserved score available
	if (!DamageScoreCount()) {
		NScorePreserve::NotifyConnected(this->entindex());
	}
}

void CHL2MP_Player::CheckQuickRespawn() {

	//BG3 if we're not switching teams and we're in a spawn room, just respawn instantly
	extern ConVar sv_class_switch_time;
	extern ConVar sv_class_switch_time_lb_multiplier;
	float classSwitchTime = sv_class_switch_time.GetFloat();
	if (IsLinebattle()) {
		classSwitchTime *= sv_class_switch_time_lb_multiplier.GetFloat();
	}

	//Fast class switch
	if (GetTeamNumber() >= TEAM_AMERICANS
		&& (m_bInSpawnRoom || IsLinebattle()) //be more generous in linebattle
		&& IsAlive()
		&& GetHealth() == 100
		&& (HasLoadedWeapon() || IsLinebattle()) //in linebattle, forgive players who accidentally shoot in spawn
		&& (m_fLastRespawn + classSwitchTime > gpGlobals->curtime)
		&& !HL2MPRules()->IsRestartingRound()) {
		//m_bDontRemoveTicket = true;
		UpdateToMatchClassWeaponAmmoUniform();

		//HACK HACK this prevents players from class-switching more than once, until they spawn for some other reason
		if (!IsLinebattle())
			m_fLastRespawn = -FLT_MAX;
	}
}

/* HandleVoicecomm
*	plays supplied voicecomm by index
*/
void CHL2MP_Player::HandleVoicecomm(int comm)
{
	bool teamonly = comm != NUM_BATTLECRY;	//battlecries are not team only (global)

	if ( //only alive assigned player can do voice comms
		GetTeamNumber() > TEAM_SPECTATOR && IsAlive() && m_flNextVoicecomm <= gpGlobals->curtime &&

		//also make sure index not out of bounds
		comm >= 0 && comm < NUM_VOICECOMMS && //comm != VCOMM1_NUM && comm != VCOMM2_START+VCOMM2_NUM &&

		//make sure global voicecomms are only played ever so often - no FREEDOM! spam
		(teamonly || m_flNextGlobalVoicecomm <= gpGlobals->curtime))//&&

		//only officers may use officer-only voicecomms (commmenu2)
		//(comm < VCOMM2_START || m_iClass == CLASS_OFFICER) )
	{
		char snd[512];
		char *pClassString, *pTeamString;

		//BG2 - Make it a switch for great justice. -HairyPotter
		bool bFrenchOverride = false;
		switch (m_iClass)
		{
		case CLASS_INFANTRY:
			pClassString = "Inf";
			break;
		case CLASS_OFFICER:
			pClassString = "Off";

			//Check for french officer
			//checking model is easier, because player could change kit but not have spawned as it yet.
			if (GetTeamNumber() == TEAM_AMERICANS && strcmp(GetModelName().ToCStr(), MODEL_AFRENCHOFFICER) == 0) {
				//pClassString = "Gre";
				bFrenchOverride = true;
			}
			break;
		case CLASS_SNIPER:
			pClassString = "Rif";
			break;
		case CLASS_SKIRMISHER:
			pClassString = "Ski";
			break;
		case CLASS_LIGHT_INFANTRY:
			if (GetTeamNumber() == TEAM_AMERICANS) {
				bFrenchOverride = true;
				pClassString = "Gre"; //BG3 - Awesome - french grenadier is in light infantry slot...
			}
			else
				pClassString = "Linf";
			break;
			break;
		case CLASS_GRENADIER:
			pClassString = "Gre";
			break;
		default:
			return;
		}

		switch (GetTeamNumber())
		{
		case TEAM_AMERICANS:
			pTeamString = bFrenchOverride ? "French" : "American";
			break;
		case TEAM_BRITISH:
			pTeamString = "British";
			break;
		default:
			return;
		}
		//

		Q_snprintf(snd, sizeof snd, "Voicecomms.%s.%s_%i", pTeamString, pClassString, comm + 1);

		//play voicecomm, with attenuation
		//EmitSound( snd );

		//BG2 - Voice Comms are now Client-Side -HairyPotter
		CRecipientFilter recpfilter;
		//recpfilter.AddAllPlayers();
		recpfilter.AddRecipientsByPAS(GetAbsOrigin()); //Instead, let's send this to players that are at least slose enough to hear it.. -HairyPotter
		recpfilter.MakeReliable(); //More network priority.

		UserMessageBegin(recpfilter, "VCommSounds");
		WRITE_BYTE(entindex());
		WRITE_STRING(snd);
		MessageEnd();
		//

		//Inform the vcomm managers that a player has emitted a vcomm
		CBotComManager* pComms = CBotComManager::GetBotCommsOfPlayer(this);
		BotContext eContext = CBotComManager::ParseIntToContext(comm);
		pComms->ReceiveContext(this, eContext, true);

		//Inform Bot Influencer that we may have ordered our bots around
		NBotInfluencer::NotifyCommand(this, comm);

		//done. possibly also tell clients to draw text and stuff if sv_voicecomm_text is true
		m_flNextVoicecomm = gpGlobals->curtime + 2.0f;

		if (!teamonly)
			m_flNextGlobalVoicecomm = gpGlobals->curtime + 10.0f;

		if (sv_voicecomm_text.GetBool())
		{
			//figure out which players should get this message
			CRecipientFilter recpfilter;
			CBasePlayer *client = NULL;

			for (int i = 1; i <= gpGlobals->maxClients; i++)
			{
				client = UTIL_PlayerByIndex(i);
				if (!client || !client->edict())
					continue;

				if (!(client->IsNetClient()))	// Not a client ? (should never be true)
					continue;

				if (teamonly && client->GetTeamNumber() != GetTeamNumber() )
					continue;

				if (!client->CanHearAndReadChatFrom(this))
					continue;

				recpfilter.AddRecipient(client);
			}

			//make reliable and send
			recpfilter.MakeReliable();

			UserMessageBegin(recpfilter, "VoiceComm");
			WRITE_BYTE(entindex());	//voicecomm originator
			WRITE_BYTE(comm | (GetTeamNumber() == TEAM_AMERICANS ? 32 : 0));	//pack comm number and team
			WRITE_BYTE(bFrenchOverride ? CLASS_LIGHT_INFANTRY : m_iClass);	//HACK HACK send class number, but if french officer pretend we're french grenadier (which is in the fifth 'light infantry' slot)
			MessageEnd();
		}
	}
}

void CHL2MP_Player::VerifyKitAmmoUniform() {


	//check for forced weapon change
	//if (GetClass() == CLASS_INFANTRY) {
	ConVar* m_pWeaponKitEnforcer = GetTeamNumber() == TEAM_AMERICANS ? &lb_enforce_weapon_amer : &lb_enforce_weapon_brit;
	int forcedKit = m_pWeaponKitEnforcer->GetInt();
	forcedKit = Clamp(forcedKit, 0, (int)m_pCurClass->numChooseableWeapons());

	if (forcedKit)
		m_iGunKit = forcedKit - 1;

	//}

	//Clamp kit number to number of available kits
	m_iGunKit = Clamp(m_iGunKit, 0, m_pCurClass->numChooseableWeapons() - 1);

	//clamp skin to valid value
	m_iClassSkin = Clamp(m_iClassSkin, 0, (int)m_pCurClass->numChooseableUniformsForPlayer(this) - 1);

	//clamp ammo kit, then verify that our weapon allows for it
	m_iAmmoKit = Clamp(m_iAmmoKit, AMMO_KIT_BALL, AMMO_KIT_BUCKSHOT);
	if (!m_pCurClass->m_aWeapons[m_iGunKit].m_bAllowBuckshot) m_iAmmoKit = AMMO_KIT_BALL;
}

/*CBotComManager* CHL2MP_Player::GetComManager() const {
	if (GetTeamNumber() == TEAM_AMERICANS)
		return &g_BotAmerComms;
	else
		return &g_BotBritComms;
}*/

//BG2 - handle stamina change
void CHL2MP_Player::DrainStamina(int iAmount, bool noMax)
{
	extern ConVar sv_stamina;
	if (!sv_stamina.GetBool())
		return;

	//limit drain to be no more than 50 at once
	if (iAmount > 50 && !noMax)
		iAmount = 50;

	//scale the stamina if we have the stamina buff
	if (m_iCurrentRallies & RALLY_STAMINA)
		iAmount *= RALLY_STAMINA_MOD;

	m_iStamina -= iAmount;

	//clamp
	if (m_iStamina < 0)
		m_iStamina = 0;
}

CBaseBG2Weapon* CHL2MP_Player::GetActiveBG3Weapon() { 
	return dynamic_cast<CBaseBG2Weapon*>(GetActiveWeapon()); 
}

bool CHL2MP_Player::IsUsingBuckshot() {										
	return (m_iCurrentAmmoKit == AMMO_KIT_BUCKSHOT || (GetActiveWeapon() && GetActiveWeapon()->Def()->m_bShotOnly)) 
		&& (GetActiveBG3Weapon() && GetActiveBG3Weapon()->GetLastAttackType() == ATTACKTYPE_FIREARM); 
}

bool CHL2MP_Player::WeaponHasSlowNerf() {
	return (GetActiveBG3Weapon() && GetActiveBG3Weapon()->Def()->m_bSlowNerf && GetActiveBG3Weapon()->GetLastAttackType() == ATTACKTYPE_FIREARM);
}

//visual effects, not functionality. Exact functionality depends on m_iRallyFlags
void CHL2MP_Player::OnRallyEffectEnable() {
	//snipers don't get FOV adjustments, so that their aiming still works as normal
	if (m_iClass == CLASS_SNIPER || mp_competitive.GetBool())
		return;

	float FOVoffset = FOV_ADJUST_DEFAULT;
	float FOVfadeTime = FOV_ADJUST_DEFAULT_INTIME;

	switch (m_iCurrentRallies) {
		case RALLY_ADVANCE:
			FOVoffset = FOV_ADJUST_ADVANCE;
			FOVfadeTime = FOV_ADJUST_ADVANCE_INTIME;
			break;
		case RALLY_FIRE:
			if (IsLinebattle())
				FOVoffset = FOV_ADJUST_FIRE_LB;
			else
				FOVoffset = FOV_ADJUST_FIRE;
			break;
		case NERF_SLOW:
			FOVoffset = FOV_ADJUST_SLOW;
	}
	this->SetFOV(this, GetDefaultFOV() + FOVoffset, FOVfadeTime);
}

void CHL2MP_Player::OnRallyEffectDisable() {
	float fadeOutTime = FOV_ADJUST_DEFAULT_OUTTIME;

	//rally flags haven't been cleared yet
	switch (m_iCurrentRallies) {
		case RALLY_ADVANCE:
			fadeOutTime = FOV_ADJUST_ADVANCE_OUTTIME;
			break;
	}

	this->SetFOV(this, 0, fadeOutTime);
}

bool CHL2MP_Player::ClientCommand(const CCommand &args)
{
	const char *cmd = args[0];
	//Msg("Player %s received command %s\n", GetPlayerName(), cmd);

	//Check if the command is in our map of commands, otherwise pass it down
	int commandIndex = s_mPlayerCommands.Find(cmd);
	//ensure permissions are up-to-date
	Permissions::LoadPermissionsForPlayer(this);
	if (commandIndex != s_mPlayerCommands.InvalidIndex()) {
		PlayerCommandFunc pcf = s_mPlayerCommands[commandIndex];
		pcf(this, args);
		return true;
	}

	return BaseClass::ClientCommand(args);

}

void CHL2MP_Player::CheatImpulseCommands(int iImpulse)
{
	switch (iImpulse)
	{
	case 101:
	{
		if (sv_cheats->GetBool())
		{
			GiveAllItems();
		}
	}
	break;

	default:
		BaseClass::CheatImpulseCommands(iImpulse);
	}
}

CHL2MP_Player * CHL2MP_Player::GetNearestPlayerOfTeam(int iTeam, float& loadedDistance)
{
	CHL2MP_Player * nearestPlayer = nullptr;
	float minDistance = 100000.0f;
	Vector ourPosition = this->GetAbsOrigin();

	CHL2MP_Player * curPlayer;
	for (int i = 1; i <= gpGlobals->maxClients; i++) {
		curPlayer = ToHL2MPPlayer(UTIL_PlayerByIndex(i));

		if (curPlayer && curPlayer->GetTeamNumber() == iTeam) {
			float displacementLength = (curPlayer->GetAbsOrigin() - ourPosition).Length();
			if (displacementLength < minDistance) {
				nearestPlayer = curPlayer;
				minDistance = displacementLength;
			}
		}
	}

	loadedDistance = minDistance;
	return nearestPlayer;
}

bool CHL2MP_Player::ShouldRunRateLimitedCommand(const CCommand &args)
{
	int i = m_RateLimitLastCommandTimes.Find(args[0]);
	if (i == m_RateLimitLastCommandTimes.InvalidIndex())
	{
		m_RateLimitLastCommandTimes.Insert(args[0], gpGlobals->curtime);
		return true;
	}
	else if ((gpGlobals->curtime - m_RateLimitLastCommandTimes[i]) < HL2MP_COMMAND_MAX_RATE)
	{
		// Too fast.
		return false;
	}
	else
	{
		m_RateLimitLastCommandTimes[i] = gpGlobals->curtime;
		return true;
	}
}

void CHL2MP_Player::CreateViewModel(int index /*=0*/)
{
	Assert(index >= 0 && index < MAX_VIEWMODELS);

	if (GetViewModel(index))
		return;

	CPredictedViewModel *vm = (CPredictedViewModel *)CreateEntityByName("predicted_viewmodel");
	if (vm)
	{
		vm->SetAbsOrigin(GetAbsOrigin());
		vm->SetOwner(this);
		vm->SetIndex(index);
		DispatchSpawn(vm);
		vm->FollowEntity(this, false);
		m_hViewModel.Set(index, vm);
	}
}

/*void CHL2MP_Player::HUD_GameMessage(const char* pszText, EHANDLE hPlayerName) {

}*/

bool CHL2MP_Player::BecomeRagdollOnClient(const Vector &force)
{
	return true;
}

// -------------------------------------------------------------------------------- //
// Ragdoll entities.
// -------------------------------------------------------------------------------- //
ConVar sv_ragdoll_staytime("sv_ragdoll_staytime", "10", FCVAR_GAMEDLL);
class CHL2MPRagdoll : public CBaseAnimatingOverlay
{
public:
	DECLARE_CLASS(CHL2MPRagdoll, CBaseAnimatingOverlay);
	DECLARE_SERVERCLASS();

	// Transmit ragdolls to everyone.
	virtual int UpdateTransmitState()
	{
		return SetTransmitState(FL_EDICT_ALWAYS);
	}

public:
	

	// In case the client has the player entity, we transmit the player index.
	// In case the client doesn't have it, we transmit the player's model index, origin, and angles
	// so they can create a ragdoll in the right place.
	CNetworkHandle(CBaseEntity, m_hPlayer);	// networked entity handle 
	CNetworkHandle(CBaseEntity, m_hHat);
	CNetworkVector(m_vecRagdollVelocity);
	CNetworkVector(m_vecRagdollOrigin);
	CNetworkVar(bool, m_bDropHat);
};

LINK_ENTITY_TO_CLASS(hl2mp_ragdoll, CHL2MPRagdoll);

IMPLEMENT_SERVERCLASS_ST_NOBASE(CHL2MPRagdoll, DT_HL2MPRagdoll)
SendPropVector(SENDINFO(m_vecRagdollOrigin), -1, SPROP_COORD),
SendPropEHandle(SENDINFO(m_hPlayer)),
SendPropEHandle(SENDINFO(m_hHat)),
SendPropModelIndex(SENDINFO(m_nModelIndex)),
SendPropInt(SENDINFO(m_nForceBone), 8, 0),
SendPropVector(SENDINFO(m_vecForce), -1, SPROP_NOSCALE),
SendPropVector(SENDINFO(m_vecRagdollVelocity)),
SendPropBool(SENDINFO(m_bDropHat))
END_SEND_TABLE()

CON_COMMAND(destroy_ragdolls, "") {
	CBaseEntity* pEntity = NULL;
	while ((pEntity = gEntList.FindEntityByClassname(pEntity, "hl2mp_ragdoll")) != NULL)
	{
		CHL2MPRagdoll *pDoll = dynamic_cast<CHL2MPRagdoll*>(pEntity);
		pDoll->Remove();
	}
}

ConVar sv_grenade_flame_probabiity_casual = ConVar("sv_grenade_flame_probabiity_casual", "0.2f", FCVAR_GAMEDLL | FCVAR_NOTIFY);
ConVar sv_grenade_flame_probabiity_comp = ConVar("sv_grenade_flame_probabiity_comp", "0.016f", FCVAR_GAMEDLL | FCVAR_NOTIFY);

void CHL2MP_Player::CreateRagdollEntity(const CTakeDamageInfo& killingInfo)
{
	RemoveRagdolls();

	// If we already have a ragdoll, don't make another one.
	//CHL2MPRagdoll *pRagdoll = dynamic_cast< CHL2MPRagdoll* >( m_hRagdoll.Get() );// This makes no sense? We just removed our ragdoll.. 
	// Now we're trying to cast to it?

	//BG2 - Tjoppen - here's another place where we put code for multiple ragdolls
	//if ( !pRagdoll )
	//{
	// create a new one
	CHL2MPRagdoll *pRagdoll = dynamic_cast< CHL2MPRagdoll* >(CreateEntityByName("hl2mp_ragdoll"));
	//}

	if (pRagdoll)
	{
		pRagdoll->m_hPlayer = this;
		pRagdoll->m_vecRagdollOrigin = GetAbsOrigin();
		pRagdoll->m_vecRagdollVelocity = GetAbsVelocity();
		pRagdoll->m_nModelIndex = m_nModelIndex;
		pRagdoll->m_nForceBone = m_nForceBone;
		//BG2 - Tjoppen - clamp bullet force
		if (m_vecTotalBulletForce.Length() > 512.f)
		{
			VectorNormalize(m_vecTotalBulletForce);
			m_vecTotalBulletForce *= 512.f;
		}
		//
		pRagdoll->m_vecForce = m_vecTotalBulletForce;
		pRagdoll->SetAbsOrigin(GetAbsOrigin());

		//if we're on fire or killed by fire, set our ragdoll on fire
		bool bIgnite = false;
		if ((GetFlags() & FL_ONFIRE) || (killingInfo.GetDamageType() & DMG_BURN)) {
			bIgnite = true;
		}
		else if (killingInfo.GetDamageType() & DMG_BLAST) {
			if (mp_competitive.GetBool() && RndBool(sv_grenade_flame_probabiity_comp.GetFloat())) {
				bIgnite = true;
			}
			else {
				bIgnite = !mp_competitive.GetBool() && RndBool(sv_grenade_flame_probabiity_casual.GetFloat());
			}
		}
		if (bIgnite) {
			pRagdoll->Ignite(RndFloat(3.f, 5.f), false, 0.5f);
		}
		

		//have a chance of having hat fall off
		if (m_pCurClass->m_pszDroppedHat && bot_randfloat() < 0.1f) {
			
			CBaseEntity* pHat = CreateEntityByName("prop_physics_multiplayer");

			if (pHat) {
				pRagdoll->m_bDropHat = true;
				pHat->SetModel(m_pCurClass->m_pszDroppedHat);
				//pHat->m_nSkin = m_iClassSkin;
				pHat->SetName(MAKE_STRING("hat"));

				Vector pos; QAngle ang;
				pRagdoll->GetBonePosition(LookupBone("ValveBiped.Bip01_Head1"), pos, ang);

				pHat->SetAbsOrigin(pos);
				pHat->SetSolidFlags(FSOLID_NOT_SOLID);
				pHat->SetCollisionGroup(COLLISION_GROUP_DEBRIS);

				IPhysicsObject* pPhys = pHat->VPhysicsInitNormal(SOLID_VPHYSICS, FSOLID_NOT_STANDABLE, false);
				Vector vel = GetAbsVelocity() * 1.1f;
				AngularImpulse imp;
				pPhys->SetVelocity(&vel, &imp);
	
				
				pRagdoll->m_hHat = pHat;
			}
		}
	}

	// ragdolls will be removed on round restart automatically
	m_hRagdoll = pRagdoll;
}


void CHL2MP_Player::Weapon_Drop(CBaseCombatWeapon *pWeapon, const Vector *pvecTarget, const Vector *pVelocity)
{
	BaseClass::Weapon_Drop(pWeapon, pvecTarget, pVelocity);
}


//BG2 - Tjoppen - don't need this
/*void CHL2MP_Player::DetonateTripmines( void )
{
CBaseEntity *pEntity = NULL;

while ((pEntity = gEntList.FindEntityByClassname( pEntity, "npc_satchel" )) != NULL)
{
CSatchelCharge *pSatchel = dynamic_cast<CSatchelCharge *>(pEntity);
if (pSatchel->m_bIsLive && pSatchel->GetThrower() == this )
{
g_EventQueue.AddEvent( pSatchel, "Explode", 0.20, this, this );
}
}

// Play sound for pressing the detonator
EmitSound( "Weapon_SLAM.SatchelDetonate" );
}*/
void SignalMonsterKilled();
void CHL2MP_Player::Event_Killed(const CTakeDamageInfo &info)
{
	//Take away Rallying effects
	BG3Buffs::RallyPlayer(0, this);
	OnRallyEffectDisable();

	//if we're holding a live grenade, drop it before we die!
	CBaseCombatWeapon* pWeapon = GetActiveWeapon();
	CWeaponFrag * pGrenade = dynamic_cast<CWeaponFrag*>(pWeapon);
	if (pGrenade && pGrenade->IsPrimed()) {
		pGrenade->LobGrenade(this, 100.0f);
		pGrenade->DecrementAmmo(this);
		StopSound(pGrenade->entindex(), GRENADE_FUSE_SOUND);
		pGrenade->Remove(); //avoid grenade duplication
	}


	//update damage info with our accumulated physics force
	CTakeDamageInfo subinfo = info;
	subinfo.SetDamageForce(m_vecTotalBulletForce);

	SetNumAnimOverlays(0);

	// Note: since we're dead, it won't draw us on the client, but we don't set EF_NODRAW
	// because we still want to transmit to the clients in our PVS.
	//BG2 - Tjoppen - reenable spectators - no dead bodies raining from the sky
	if (GetTeamNumber() > TEAM_SPECTATOR && sv_ragdoll_staytime.GetFloat() > 1) {
		CreateRagdollEntity(info);
		extern ConVar sv_ragdoll_staytime;
		m_flNextRagdollRemoval = gpGlobals->curtime + sv_ragdoll_staytime.GetFloat();
	}
		

	RemoveSelfFromFlags();

	BaseClass::Event_Killed(subinfo);

	/*if (m_hRagdoll) {
		if (info.GetDamageType() & DMG_DISSOLVE) {
			m_hRagdoll->GetBaseAnimating()->Dissolve(NULL, gpGlobals->curtime, false, ENTITY_DISSOLVE_NORMAL);
		}
	}*/

	CBaseEntity *pAttacker = info.GetAttacker();
	//do speed increase here
	if (pAttacker && pAttacker->GetTeamNumber() != this->GetTeamNumber())
	{
		CHL2MP_Player* pPlayer = static_cast<CHL2MP_Player*>(pAttacker);
		pWeapon = pPlayer->GetActiveWeapon();
		if (pWeapon && pWeapon->Def()->m_iOwnerSpeedModOnKill) {
			pPlayer->m_iSpeedModifier += pWeapon->Def()->m_iOwnerSpeedModOnKill;
			if (pPlayer->m_iSpeedModifier > 20)
					pPlayer->m_iSpeedModifier = 20;
		}
	}

	m_lifeState = LIFE_DEAD;

	RemoveEffects(EF_NODRAW);	// still draw player body
	//StopZooming();

	//If we're a monster bot, announce our death then leave the server
	if (m_bMonsterBot) {
		CHL2MP_Player* pAttackingPlayer = ToHL2MPPlayer(pAttacker);
		if (pAttackingPlayer) {
			MSay("%s killed %s!", pAttackingPlayer->GetPlayerName(), GetPlayerName());
		}
		else {
			MSay("%s has been vanquished!", GetPlayerName());
		}

		char buffer[6 + MAX_PLAYER_NAME_LENGTH];
		Q_snprintf(buffer, 6 + MAX_PLAYER_NAME_LENGTH, "kick \"%s\"\n", GetPlayerName());
		engine->ServerCommand(buffer);

		Warning("Monster killed, calling Signal!\n");
		SignalMonsterKilled();
	}
	else {
		//If we have a fire entity attatched to us, extinguish it as otherwise there will be ghost fire
		CBaseEntity* pEffect = GetEffectEntity();
		if (pEffect) {
			CEntityFlame* pFlame = dynamic_cast<CEntityFlame*>(pEffect);
			if (pFlame) {
				pFlame->SetLifetime(-FLT_MAX);
			}
		}
	}
	//no matter what make sure we're not on fire after dying
	Extinguish();
}

//BG2 - Tjoppen - GetHitgroupPainSound
const char* CHL2MP_Player::GetHitgroupPainSound(int hitgroup, int team)
{
	if (team == TEAM_AMERICANS)
	{
		switch (hitgroup)
		{
		default:
		case HITGROUP_GENERIC: return "BG2Player.American.pain_generic";
		case HITGROUP_HEAD: return "BG2Player.American.pain_head";
		case HITGROUP_CHEST: return "BG2Player.American.pain_chest";
		case HITGROUP_STOMACH: return "BG2Player.American.pain_stomach";
		case HITGROUP_LEFTARM: return "BG2Player.American.pain_arm";
		case HITGROUP_RIGHTARM: return "BG2Player.American.pain_arm";
		case HITGROUP_LEFTLEG: return "BG2Player.American.pain_leg";
		case HITGROUP_RIGHTLEG: return "BG2Player.American.pain_leg";
		}
	}
	else if (team == TEAM_BRITISH)
	{
		switch (hitgroup)
		{
		default:
		case HITGROUP_GENERIC: return "BG2Player.British.pain_generic";
		case HITGROUP_HEAD: return "BG2Player.British.pain_head";
		case HITGROUP_CHEST: return "BG2Player.British.pain_chest";
		case HITGROUP_STOMACH: return "BG2Player.British.pain_stomach";
		case HITGROUP_LEFTARM: return "BG2Player.British.pain_arm";
		case HITGROUP_RIGHTARM: return "BG2Player.British.pain_arm";
		case HITGROUP_LEFTLEG: return "BG2Player.British.pain_leg";
		case HITGROUP_RIGHTLEG: return "BG2Player.British.pain_leg";
		}
	}
	else
		return 0;
}
//
ConVar mp_teamdamage_score_multiplier("mp_teamdamage_score_multiplier", "-2", FCVAR_GAMEDLL | FCVAR_NOTIFY);
int CHL2MP_Player::OnTakeDamage(const CTakeDamageInfo &inputInfo)
{
	bool bFatal = inputInfo.GetDamage() >= GetHealth();

	//special cases with burn damage
	if (!bFatal && (inputInfo.GetDamageType() & DMG_BURN)) {
		//set us aflame if we aren't already
		if (!(GetFlags() & FL_ONFIRE)) {
			Ignite(4.f, false);
		}

		//monster bots don't take burn damage
		if (m_bMonsterBot) {
			return 0;
		}
	}

	

	//BG2 - Draco
	//CBaseEntity * pInflictor = inputInfo.GetInflictor();
	CBaseEntity * pAttacker = inputInfo.GetAttacker();
	if (pAttacker->IsPlayer())
	{
		CBasePlayer * pAttacker2 = (CBasePlayer *)pAttacker;
		//subtract damage for attacking teammates!
		pAttacker2->IncrementDamageScoreCount((int)inputInfo.GetDamage()
			* (GetTeamNumber() == pAttacker2->GetTeamNumber() && !IsFFA() ? mp_teamdamage_score_multiplier.GetInt() : 1));
	}

	//BG2 - Tjoppen - take damage => lose some stamina
	//have bullets drain a lot more stamina than melee damage
	//this makes firearms more useful, and avoids melee turning too slow due to stamina loss
	//have leg damage drain the most stamina. works as a consolation prize and also makes more sense
	//finally, arms drain the least stamina
	float scale;

	if (inputInfo.GetDamageType() & DMG_BULLET)
	{
		//legs = 75%
		//body/head = 50%
		//arms = 30% (keep same as melee for simplicity)
		if (LastHitGroup() == HITGROUP_LEFTLEG || LastHitGroup() == HITGROUP_RIGHTLEG)
			scale = 0.75f;
		else if (LastHitGroup() == HITGROUP_LEFTARM || LastHitGroup() == HITGROUP_RIGHTARM)
			scale = 0.3f;
		else
			scale = 0.5f;
	}
	else
		scale = 0.3f;

	DrainStamina(inputInfo.GetDamage() * scale);
	//

	m_vecTotalBulletForce += inputInfo.GetDamageForce();

	//BG2 - Tjoppen - print hit verification
	if (inputInfo.GetAttacker()->IsPlayer())
	{
		CBasePlayer *pVictim = this,
			*pAttackingPlayer = ToBasePlayer(inputInfo.GetAttacker());
		Assert(pAttackingPlayer != NULL);

		CBaseBG2Weapon *pWeapon = dynamic_cast<CBaseBG2Weapon*>(pAttackingPlayer->GetActiveWeapon());
		int attackType = pWeapon ? pWeapon->m_iLastAttackType : 0;

		//use usermessage instead and let the client figure out what to print. saves precious bandwidth
		//send one to attacker and one to victim
		//the "Damage" usermessage is more detailed, but adds up all damage in the current frame. it is therefore
		//hard to determine who is the attacker since the victim can be hit multiple times

		CRecipientFilter recpfilter;
		recpfilter.AddRecipient(pAttackingPlayer);
		recpfilter.AddRecipient(pVictim);
		recpfilter.MakeReliable();

		UserMessageBegin(recpfilter, "HitVerif");
		WRITE_BYTE(pAttacker->entindex());				//attacker id
		WRITE_BYTE(pVictim->entindex());					//victim id
		WRITE_BYTE(LastHitGroup() + (attackType << 4));	//where?
		WRITE_SHORT(inputInfo.GetDamage());				//damage
		MessageEnd();
	}
	//BG3 - moved this HitVerif part to be before the sound part, because the sound part has some early returns
	//Otherwise, I think this might prove problematic in cases where there's lots of damages going around

	//BG2 - Tjoppen - pain sound
	const char *pModelName = STRING(GetModelName());

	CSoundParameters params;
	//if ( GetParametersForSound( "BG2Player.pain", params, pModelName ) == false )
	if (GetParametersForSound(GetHitgroupPainSound(LastHitGroup(), GetTeamNumber()), params, pModelName) == false)
		return BaseClass::OnTakeDamage(inputInfo);

	Vector vecOrigin = GetAbsOrigin();

	CRecipientFilter filter;
	filter.AddRecipientsByPAS(vecOrigin);

	EmitSound_t ep;
	ep.m_nChannel = params.channel;
	ep.m_pSoundName = params.soundname;
	ep.m_flVolume = params.volume;
	ep.m_SoundLevel = params.soundlevel;
	ep.m_nFlags = 0;
	ep.m_nPitch = params.pitch;
	ep.m_pOrigin = &vecOrigin;

	EmitSound(filter, entindex(), ep);

	g_pPlayerResource->DispatchUpdateTransmitState();
	
	//
	//gamestats->Event_PlayerDamage( this, inputInfo );

	return BaseClass::OnTakeDamage(inputInfo);
}

void CHL2MP_Player::DeathSound(const CTakeDamageInfo &info)
{
	//if (m_hRagdoll && m_hRagdoll->GetBaseAnimating()->IsDissolving())
		//return;

	//BG2 - Tjoppen - unassigned/spectator don't make deathsound
	if (GetTeamNumber() <= TEAM_SPECTATOR)
		return;
	//

	//BG3 - Awesome - we're indexing here instead of using the built-in "rndwave" parameter
	// because "rndwave" has a cap on number of sounds
	// and because valve's RNG functions are crap
	char szStepSound[32];
	if (info.GetDamageType() & DMG_BURN) {
		int iSound = RndInt(1, NUM_DEATH_SOUNDS_FIRE);
		Q_snprintf(szStepSound, sizeof(szStepSound), "BG3Player.DieFire%02i" /*, GetPlayerModelSoundPrefix()*/, iSound);
	}
	else {
		int iSound = RndInt(1, NUM_DEATH_SOUNDS);
		Q_snprintf(szStepSound, sizeof(szStepSound), "BG3Player.Die%02i" /*, GetPlayerModelSoundPrefix()*/, iSound);
	}
	

	CSoundParameters params;
	if (GetParametersForSound(szStepSound, params, NULL/*pModelName*/) == false) {
		//Warning("Could not find death sound %s\n", szStepSound);
		return;
	}
		

	Vector vecOrigin = GetAbsOrigin();

	CRecipientFilter filter;
	filter.AddRecipientsByPAS(vecOrigin);

	EmitSound_t ep;
	ep.m_nChannel = params.channel;
	ep.m_pSoundName = params.soundname;
	ep.m_flVolume = params.volume;
	ep.m_SoundLevel = params.soundlevel;
	ep.m_nFlags = 0;
	ep.m_nPitch = params.pitch;
	ep.m_pOrigin = &vecOrigin;

	EmitSound(filter, entindex(), ep);

	if (LastHitGroup() == HITGROUP_HEAD && (info.GetDamageType() & DMG_BULLET)) {
		if (GetParametersForSound("Weapon_All.Bullet_Skullcrack", params, NULL/*pModelName*/) == false)
			return;

		//Msg("Playing skull crush sound!\n");
		EmitSound_t ep;
		ep.m_nChannel = params.channel;
		ep.m_pSoundName = params.soundname;
		ep.m_flVolume = params.volume;
		ep.m_SoundLevel = params.soundlevel;
		ep.m_nFlags = 0;
		ep.m_nPitch = params.pitch;
		ep.m_pOrigin = &vecOrigin;

		EmitSound(filter, entindex(), ep);
	}
}


CBaseEntity* CHL2MP_Player::HandleSpawnList(const CUtlVector<CBaseEntity *>* spawnLists[NUM_SPAWN_LISTS])
{
	for (int i = 0; i < NUM_SPAWN_LISTS; i++) {
		const CUtlVector<CBaseEntity*>& spawns = *spawnLists[i];

		//BG2 - Tjoppen - the code below was broken out of EntSelectSpawnPoint()
		int offset = RandomInt(0, spawns.Count() - 1);	//start at a random offset into the list

		bool competitive = mp_competitive.GetBool();

		for (int x = 0; x < spawns.Count(); x++)
		{
			CSpawnPoint *pSpawn = static_cast<CSpawnPoint*>(spawns[(x + offset) % spawns.Count()]);
			if (pSpawn
				&& pSpawn->GetTeam() == GetTeamNumber()
				&& !pSpawn->m_bReserved
				&& pSpawn->CanSpawnClass(m_iNextClass)
				&& pSpawn->IsEnabled()
				&& (!IsFakeClient() || pSpawn->SupportsBots())
				&& (!pSpawn->BotsOnly() || IsFakeClient())
				&& ((competitive && pSpawn->IsCompetitive())
					|| (!competitive && pSpawn->IsCasual())
					)
				)
			{
				//Msg("Found potential spawnpoint for player %s\n", GetPlayerName());

				CBaseEntity *ent = NULL;
				bool IsTaken = false;
				//32 world units seems... safe.. -HairyPotter
				for (CEntitySphereQuery sphere(pSpawn->GetAbsOrigin(), 32); (ent = sphere.GetCurrentEntity()) != NULL; sphere.NextEntity())
				{
					// Check to see if the ent is a player.
					if (ent->IsPlayer() && ent->IsAlive())
					{
						IsTaken = true;
						//Warning("\tSpawnpoint blocked by %s\n", ((CBasePlayer*)(ent))->GetPlayerName());
					}
				}
				if (IsTaken) //Retry?
					continue;

				pSpawn->m_bReserved = true;
				return pSpawn;
			}
		}
	}

	return NULL;
}

CBaseEntity* CHL2MP_Player::HandleSpawnListFFA()
{
	const CUtlVector<CBaseEntity*>& spawns = g_FFA_Spawns;

	//BG2 - Tjoppen - the code below was broken out of EntSelectSpawnPoint()
	int offset = RandomInt(0, spawns.Count() - 1);	//start at a random offset into the list

	bool competitive = mp_competitive.GetBool();

	for (int x = 0; x < spawns.Count(); x++)
	{
		CSpawnPoint *pSpawn = static_cast<CSpawnPoint*>(spawns[(x + offset) % spawns.Count()]);
		if (pSpawn
			&& !pSpawn->m_bReserved
			&& pSpawn->CanSpawnClass(m_iNextClass)
			&& pSpawn->IsEnabled()
			&& (!IsFakeClient() || pSpawn->SupportsBots())
			&& (!pSpawn->BotsOnly() || IsFakeClient())
			&& ((competitive && pSpawn->IsCompetitive())
				|| (!competitive && pSpawn->IsCasual())
				)
			)
		{

			CBaseEntity *ent = NULL;
			bool IsTaken = false;
			//Sphere did have range of 32 units, but for FFA let's move it farther apart to give more breathing room
			for (CEntitySphereQuery sphere(pSpawn->GetAbsOrigin(), 256); (ent = sphere.GetCurrentEntity()) != NULL; sphere.NextEntity())
			{
				// Check to see if the ent is a player.
				if (ent->IsPlayer() && ent->IsAlive())
				{
					IsTaken = true;
					//Warning("\tSpawnpoint blocked by %s\n", ((CBasePlayer*)(ent))->GetPlayerName());
				}
			}
			if (IsTaken) //Retry?
				continue;

			pSpawn->m_bReserved = true;
			return pSpawn;
		}
	}
	

	return NULL;
}

ConVar sv_class_switch_time("sv_class_switch_time", "10", FCVAR_GAMEDLL | FCVAR_NOTIFY, "How long after spawning a player will be able to change class\n");
ConVar sv_class_switch_time_lb_multiplier("sv_class_switch_time_lb_multiplier", "3", FCVAR_GAMEDLL | FCVAR_NOTIFY, "Multiplier for sv_class_switch_time for linebattle mode\n");
CBaseEntity* CHL2MP_Player::EntSelectSpawnPoint(void)
{
	float classSwitchTime = sv_class_switch_time.GetFloat();
	if (IsLinebattle()) {
		classSwitchTime *= sv_class_switch_time_lb_multiplier.GetFloat();
	}

	//no need for spawn points if we're in a room
	if (m_bInSpawnRoom && m_fLastRespawn + classSwitchTime > gpGlobals->curtime)
		return NULL;

	if (GetTeamNumber() <= TEAM_SPECTATOR)
		return NULL;	//BG2 - Tjoppen - spectators/unassigned don't spawn..

	
	CBaseEntity *pSpot = NULL;
	if (IsFFA()) {
		pSpot = HandleSpawnListFFA();
	}
	else {
		CUtlVector<CBaseEntity *>* spots;
		CUtlVector<CBaseEntity *>* prioritySpots;

		if (GetTeamNumber() == TEAM_AMERICANS) {
			spots = &g_AmericanSpawns;
			prioritySpots = &g_AmericanPrioritySpawns;
		}
		else if (GetTeamNumber() == TEAM_BRITISH) {
			spots = &g_BritishSpawns;
			prioritySpots = &g_BritishPrioritySpawns;
		}
		else
			return NULL;

		const CUtlVector<CBaseEntity*>* spawnLists[] = { prioritySpots, &g_MultiPrioritySpawns, spots, &g_MultiSpawns };

		pSpot = HandleSpawnList(spawnLists);
	}
	

	if (pSpot)
		return pSpot;

	if (IsFFA()) {
		CSay("PutClientInServer: There are too few Free-For-All spawns or they are too close together, or someone was obstructing them.\n");
	}
	else {
		switch (GetTeamNumber())
		{
		case TEAM_BRITISH:
			CSay("PutClientInServer: There are too few British spawns or they are too close together, or someone was obstructing them.\n");
			break;
		case TEAM_AMERICANS:
			CSay("PutClientInServer: There are too few American spawns or they are too close together, or someone was obstructing them.\n");
			break;
		}
	}
	

	//Send them to the info_itermission first, rather than potentially sending them way outside of the borders of the map. -HairyPotter
	if (m_pIntermission)
		return m_pIntermission;

	return CBaseEntity::Instance(INDEXENT(0));

}


CON_COMMAND_F(timeleft, "prints the time remaining in the match", FCVAR_CLIENTCMD_CAN_EXECUTE)
{
	CHL2MP_Player *pPlayer = ToHL2MPPlayer(UTIL_GetCommandClient());

	int iTimeRemaining = (int)HL2MPRules()->GetMapRemainingTime();

	if (iTimeRemaining == 0)
	{
		if (pPlayer)
		{
			ClientPrint(pPlayer, HUD_PRINTTALK, "This game has no timelimit.");
		}
		else
		{
			Msg("* No Time Limit *\n");
		}
	}
	else
	{
		int iMinutes, iSeconds;
		iMinutes = iTimeRemaining / 60;
		iSeconds = iTimeRemaining % 60;

		char minutes[8];
		char seconds[8];

		Q_snprintf(minutes, sizeof(minutes), "%d", iMinutes);
		Q_snprintf(seconds, sizeof(seconds), "%2.2d", iSeconds);

		if (pPlayer)
		{
			ClientPrint(pPlayer, HUD_PRINTTALK, "Time left in map: %s1:%s2", minutes, seconds);
		}
		else
		{
			Msg("Time Remaining:  %s:%s\n", minutes, seconds);
		}
	}
}

void CHL2MP_Player::Reset()
{
	ResetDamageScoreCount();
	ResetFragCount();
}

bool CHL2MP_Player::IsReady()
{
	return m_bReady;
}

void CHL2MP_Player::SetReady(bool bReady)
{
	m_bReady = bReady;
}

void CHL2MP_Player::CheckChatText(char *p, int bufsize)
{
	//Look for escape sequences and replace

	char *buf = new char[bufsize];
	int pos = 0;

	// Parse say text for escape sequences
	for (char *pSrc = p; pSrc != NULL && *pSrc != 0 && pos < bufsize - 1; pSrc++)
	{
		// copy each char across
		buf[pos] = *pSrc;
		pos++;
	}

	buf[pos] = '\0';

	// copy buf back into p
	Q_strncpy(p, buf, bufsize);

	delete[] buf;

	const char *pReadyCheck = p;

	HL2MPRules()->CheckChatForReadySignal(this, pReadyCheck);
}

void CHL2MP_Player::State_Transition(HL2MPPlayerState newState)
{
	State_Leave();
	State_Enter(newState);
}


void CHL2MP_Player::State_Enter(HL2MPPlayerState newState)
{
	m_iPlayerState = newState;
	m_pCurStateInfo = State_LookupInfo(newState);

	// Initialize the new state.
	if (m_pCurStateInfo && m_pCurStateInfo->pfnEnterState)
		(this->*m_pCurStateInfo->pfnEnterState)();
}


void CHL2MP_Player::State_Leave()
{
	if (m_pCurStateInfo && m_pCurStateInfo->pfnLeaveState)
	{
		(this->*m_pCurStateInfo->pfnLeaveState)();
	}
}


void CHL2MP_Player::State_PreThink()
{
	if (m_pCurStateInfo && m_pCurStateInfo->pfnPreThink)
	{
		(this->*m_pCurStateInfo->pfnPreThink)();
	}
}


CHL2MPPlayerStateInfo *CHL2MP_Player::State_LookupInfo(HL2MPPlayerState state)
{
	// This table MUST match the 
	static CHL2MPPlayerStateInfo playerStateInfos[] =
	{
		{ STATE_ACTIVE, "STATE_ACTIVE", &CHL2MP_Player::State_Enter_ACTIVE, NULL, &CHL2MP_Player::State_PreThink_ACTIVE },
		{ STATE_OBSERVER_MODE, "STATE_OBSERVER_MODE", &CHL2MP_Player::State_Enter_OBSERVER_MODE, NULL, &CHL2MP_Player::State_PreThink_OBSERVER_MODE }
	};

	for (int i = 0; i < ARRAYSIZE(playerStateInfos); i++)
	{
		if (playerStateInfos[i].m_iPlayerState == state)
			return &playerStateInfos[i];
	}

	return NULL;
}

bool CHL2MP_Player::StartObserverMode(int mode)
{
	Extinguish(); //no on-fire observors

	//we only want to go into observer mode if the player asked to, not on a death timeout
	/*if ( m_bEnterObserver == true )
	{
	VPhysicsDestroyObject();
	return BaseClass::StartObserverMode( mode );
	}*/
	//BG2 - Tjoppen - reenable spectators
	return BaseClass::StartObserverMode(mode);
	//
	//Do nothing.
	return false;
}

void CHL2MP_Player::StopObserverMode()
{
	m_bEnterObserver = false;
	BaseClass::StopObserverMode();
}

void CHL2MP_Player::State_Enter_OBSERVER_MODE()
{
	int observerMode = m_iObserverLastMode;
	if (IsNetClient())
	{
		const char *pIdealMode = engine->GetClientConVarValue(engine->IndexOfEdict(edict()), "cl_spec_mode");
		if (pIdealMode)
		{
			observerMode = atoi(pIdealMode);
			if (observerMode <= OBS_MODE_FIXED || observerMode > OBS_MODE_ROAMING)
			{
				observerMode = m_iObserverLastMode;
			}
		}
	}
	m_bEnterObserver = true;
	StartObserverMode(observerMode);
}

void CHL2MP_Player::State_PreThink_OBSERVER_MODE()
{
	// Make sure nobody has changed any of our state.
	//	Assert( GetMoveType() == MOVETYPE_FLY );
	Assert(m_takedamage == DAMAGE_NO);
	Assert(IsSolidFlagSet(FSOLID_NOT_SOLID));
	//	Assert( IsEffectActive( EF_NODRAW ) );

	// Must be dead.
	Assert(m_lifeState == LIFE_DEAD);
	Assert(pl.deadflag);
}


void CHL2MP_Player::State_Enter_ACTIVE()
{
	SetMoveType(MOVETYPE_WALK);

	// md 8/15/07 - They'll get set back to solid when they actually respawn. If we set them solid now and mp_forcerespawn
	// is false, then they'll be spectating but blocking live players from moving.
	// RemoveSolidFlags( FSOLID_NOT_SOLID );

	m_Local.m_iHideHUD = 0;
}


void CHL2MP_Player::State_PreThink_ACTIVE()
{
	//we don't really need to do anything here. 
	//This state_prethink structure came over from CS:S and was doing an assert check that fails the way hl2dm handles death
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CHL2MP_Player::CanHearAndReadChatFrom(CBasePlayer *pPlayer)
{
	// can always hear the console unless we're ignoring all chat
	if (!pPlayer)
		return false;

	return m_bGagged == ToHL2MPPlayer(pPlayer)->m_bGagged;
}

void CHL2MP_Player::RemoveRagdolls() {
	//BG2 - Tjoppen - here's where we put code for multiple ragdolls
	if (m_hRagdoll) //One already exists.. remove it.
	{
		CHL2MPRagdoll* pDoll = static_cast<CHL2MPRagdoll*>(m_hRagdoll.Get());
		UTIL_RemoveImmediate(pDoll->m_hHat);
		pDoll->m_hHat = NULL;
		UTIL_RemoveImmediate(m_hRagdoll);
		m_hRagdoll = NULL;
	}
}

//BG2
void CHL2MP_Player::RemoveSelfFromFlags(void)
{
	//remove ourself from any overload list on all flags

	CFlag *pFlag = NULL;
	while ((pFlag = static_cast<CFlag*>(gEntList.FindEntityByClassname(pFlag, "flag"))) != NULL)
	{
		pFlag->m_vOverloadingPlayers.FindAndRemove(this);
		pFlag->m_vTriggerBritishPlayers.FindAndRemove(this);
		pFlag->m_vTriggerAmericanPlayers.FindAndRemove(this);
	}

	//Also have the player drop CTF Flags if they are holding one. -HairyPotter
	CtfFlag *ctfFlag = NULL;
	while ((ctfFlag = static_cast<CtfFlag*>(gEntList.FindEntityByClassname(ctfFlag, "ctf_flag"))) != NULL)
	{
		if (ctfFlag->GetParent() && ctfFlag->GetParent() == this)
			ctfFlag->DropFlag();
	}
}

//BG2 - Tjoppen - HACKHACK: no more weapon_physcannon
//void PlayerPickupObject( CBasePlayer *pPlayer, CBaseEntity *pObject ){}
//bool PlayerHasMegaPhysCannon( void ){return false;}
//void PhysCannonForceDrop( CBaseCombatWeapon *pActiveWeapon, CBaseEntity *pOnlyIfHoldingThis ){}
//void PhysCannonBeginUpgrade( CBaseAnimating *pAnim ){}
//bool PlayerPickupControllerIsHoldingEntity( CBaseEntity *pPickupControllerEntity, CBaseEntity *pHeldEntity ){return false;}
//float PhysCannonGetHeldObjectMass( CBaseCombatWeapon *pActiveWeapon, IPhysicsObject *pHeldObject ){return 0;}
//CBaseEntity *PhysCannonGetHeldEntity( CBaseCombatWeapon *pActiveWeapon ){return NULL;}
//float PlayerPickupGetHeldObjectMass( CBaseEntity *pPickupControllerEntity, IPhysicsObject *pHeldObject ){return 0;}
