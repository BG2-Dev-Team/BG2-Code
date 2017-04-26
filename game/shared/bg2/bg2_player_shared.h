/*
BG3 - Awesome - I'm tired of magic numbers floating around everywhere so I though I'd define everything here
*/
#pragma once

//BG2 - Tjoppen - class system - don't change these
#if 1
#define	CLASS_INFANTRY			0
#define	CLASS_OFFICER			1
#define	CLASS_SNIPER			2
#define	CLASS_SKIRMISHER		3 //Native or Militia
#define	CLASS_LIGHT_INFANTRY	4 //Light Infantry or State Militia
#define CLASS_GRENADIER			5
#endif
//

/*
//PLAYER SPEEDS - used in hl2mp_player_shared.cpp
*/
#if 1
#define SPEED_INFANTRY		195
#define SPEED_OFFICER		200 //BG3 - was 210
#define SPEED_OFFICER_HEAVY	195	//BG3 - speed of officers carrying carbine
#define SPEED_SNIPER		205
#define SPEED_SKIRMISHER	210 //BG3 - was 200 - native and minutemen
#define SPEED_LIGHT_INF		200 //BG3 - was 198
#define SPEED_GRENADIER		185

#define SPEED_WALK			120
#define SPEED_SPECTATE		240

//These are multipliers
#define SPEED_MOD_IRONSIGHT 0.3f
#define SPEED_MOD_RELOAD	0.5f


#endif

/*
//STAMINA DRAIN AMOUNTS - USED IN A LOT OF PLACES
*/
#if 1
#define STAMINA_REGEN_AMOUNT	3 //server-side, per tick
#define STAMINA_DRAIN_JUMP		40
#define STAMINA_DRAIN_CROUCH	25
#endif

/*
ACCURACY MODIFIERS
Individual accuracies defined in the weapons, general things can go here
*/
#define ACCURACY_WALK_LERP	0.6f 
//weapon_bg2base.h - when walking, accuracy is a lerp between still and running.
//0 is running, 1 is still

/*
AMMO AMOUNTS - used in hl2mp_gamerules.h
*/
#if 1
#define AMMO_INFANTRY		15
#define AMMO_OFFICER		6
#define AMMO_SNIPER			12
#define AMMO_SKIRMISHER		12
#define AMMO_LIGHT_INFANTRY	12
#define AMMO_GRENADIER		6	//grenade count is always 1
#endif


/*
FLAG CARRY WEIGHT MULTIPLIERS - used in ctfflag.cpp
the weight of a flag, which slows down a player, is multiplied by this, depending on class
Higher values slow down the player more
*/
#if 1
#define SPEED_MOD_CARRY_INFANTRY		1
#define SPEED_MOD_CARRY_OFFICER			1.6f
#define SPEED_MOD_CARRY_SNIPER			1.2f
#define SPEED_MOD_CARRY_SKIRMISHER		1.1f
#define SPEED_MOD_CARRY_LIGHT_INFANTRY	1.05f
#define SPEED_MOD_CARRY_GRENADIER		0.6f //grenadiers are strong
#endif

/*
HITGROUP MODIFIERS - used player.cpp - don't change these unless you have a really good reason
*/
#if 1
#define DMG_MOD_HEAD		1.8378f
#define DMG_MOD_CHEST		1.0f
#define DMG_MOD_STOMACH		0.9865f
#define DMG_MOD_ARM			0.6757f
#define DMG_MOD_LEG			0.7297f
#endif

/*
GRENADE INFORMATION - used in basegrenade_shared.h and other places - I tried to put these definitions in weapon_bg2base.cpp but it wouldn't resolve the externals - Awesome
*/
#define DMG_GRENADE 150.0f
#define GRENADE_FUSE_LENGTH 5.0f

/*
CLASS-BASED DAMAGE MODIFIERS
*/
#define DMG_MOD_GRENADIER		0.8f //low-damage attacks while be multiplied by this amount
#define DMG_MOD_GRENADIER_CAP	70	//damage amounts below this will have the multiplier applied to them

#define DMG_MOD_NATIVE			1.15f //multiplier
#define DMG_MOD_NATIVE_CAP		65

/*
RAW RALLY TYPES - DON"T CHANGE THESE UNLESS YOU' KNOW WHAT YOU'RE DOING
m_iCurrentRallies is a bit field so that a player can have multiple rallies at once!
*/
#if 1
#define RALLY_DURATION			10.0f	//default until you change the derived macros
#define RALLY_INTERVAL			200.0f //time an officer must wait before doing another rally

#define	RALLY_SPEED				(1 << 0)
#define RALLY_SPEED_MOD			1.1f //Speed multiplier for affected players

#define RALLY_STAMINA			(1 << 1)
#define RALLY_STAMINA_MOD		0.5f //Multiplier for stamina drain of affected players

#define RALLY_ARMOR				(1 << 2)
#define RALLY_ARMOR_MOD			0.9f //Received damage less than 100 is multiplied by this, for affected players

#define RALLY_DAMAGE			(1 << 3)
#define RALLY_DAMAGE_MOD		1.1f //multiplier for damage dealt to enemies

#define RALLY_ACCURACY			(1 << 4)
#define RALLY_ACCURACY_MOD		0.9f //Multiplier for ironsights accuracy radius - smaller is better

#define RALLY_SPEED_RELOAD		(1 << 5) //NOT RELOAD SPEED, but player movement speed while reloading!
#define RALLY_SPEED_RELOAD_LERP	0.5f //Lerp between reload movement speed and running speed. 0 is reloading, 1 is running

#define RALLY_SPEED_AIM			(1 << 6) //for player movement speed while aiming!
#define RALLY_SPEED_AIM_MOD		2.5f //A post-multiplier for player movement speed while aiming

#define RALLY_RADIUS_SKIRM		512.0f	//5.12 meters. This is used in skirm/ctf/siege

//used in linebattle
#define RALLY_RADIUS_LINEBATTLE	2048.0f
#define RALLY_DURATION_LB_MOD	2.0f /*Duration of rally effects is multiplied by this*/
#endif

/*
FOV OFFSETS FOR RALLYING EFFECTS - used by CHL2MP_Player
Positive values increase FOV and make player feel faster, lower FOV tightens aim and feels slower
*/
#if 1
#define FOV_ADJUST_ADVANCE			14.0
#define FOV_ADJUST_ADVANCE_INTIME	8.0 //players will feel like they're getting faster and faster
#define FOV_ADJUST_ADVANCE_OUTTIME	3.0 //let's not give them a headache

#define FOV_ADJUST_FIRE				-2.0 //zoom in!
#define FOV_ADJUST_FIRE_LB			-7.0 //zoom in MORE

#define FOV_ADJUST_DEFAULT			4.0 //these are used if a rally-specific isn't specified
#define FOV_ADJUST_DEFAULT_INTIME	1.0
#define FOV_ADJUST_DEFAULT_OUTTIME	1.0
#endif

/*
VCOMM RALLYING DEFINITIONS - these are just mergers of the raw rally types defined above
	these are tied to the individual vcomm commands
*/
#if 1
#define RALLY_ADVANCE			(RALLY_SPEED | RALLY_SPEED_RELOAD | RALLY_SPEED_AIM)
#define RALLY_ADVANCE_DURATION	RALLY_DURATION

#define RALLY_RALLY_ROUND			(RALLY_STAMINA | RALLY_ARMOR)
#define RALLY_RALLY_ROUND_DURATION	RALLY_DURATION

#define RALLY_FIRE				(RALLY_ACCURACY | RALLY_DAMAGE)
#define RALLY_FIRE_DURATION			RALLY_DURATION

#define RALLY_RETREAT			(RALLY_SPEED_RELOAD | RALLY_ARMOR)
#define RALLY_RETREAT_DURATION		RALLY_DURATION
#endif


/*
VCOMM aliases
These are used to match keyboard keys with different voice commands. Used in determining 
chat messages, sounds, and rallying binds
*/
#if 1
#define VCOMM_AYE			0
#define	VCOMM_NO			1
#define VCOMM_FOLLOW		2
#define VCOMM_BEHIND_US		3
#define VCOMM_ENEMY_AHEAD	4
#define VCOMM_AREA_SECURE	5
#define VCOMM_ON_THE_FLANKS	6
#define VCOMM_FLANK_LEFT	7
#define VCOMM_FLANK_RIGHT	8

#define VCOMM2_ADVANCE		9
#define VCOMM2_RETREAT		10
#define VCOMM2_RALLY_ROUND	11
#define VCOMM2_HALT			12
#define VCOMM2_OBJECTIVE	13
#define VCOMM2_MAKE_READY	14
#define VCOMM2_PRESENT		15
#define VCOMM2_FIRE			16
#define VCOMM2_CEASE_FIRE	17
#endif

/*
BG3 VIEWMODEL SKIN DEFINITIONS
These are used to match a viewmodels sleeves with the skin of the player
Prefer to just add new defs instead of reorganizing it, because if you shift
numbers around you'll have to fix the viewmodels themselves
*/
#if 1
#define SLEEVE_AMER_REG		0
#define SLEEVE_AMER_REG_ALT	5
	
#define SLEEVE_AMER_SNIPER	1
#define SLEEVE_AMER_MIL		2
#define SLEEVE_AMER_FRENCH	6

#define SLEEVE_BRIT_REG		3
#define SLEEVE_BRIT_REG_ALT 7

#define SLEEVE_BRIT_SNIPER	4
#endif

/*
BG3 WORLD MODEL SKIN DEFINTIONS
Used when a player spawns to detemine the appropriate world model skin
Regular infantry models have 24 actual skins with different faces, backpacks
etc. These indexes represent the starting points for the different overarching class
skins. These are used dynamically to randomly pick a skin within the corrent range
*/
#if 1
#define SKIN_MAX 23
#define SKIN_DEFAULT 0

#define SKIN_BRITISH_REG		SKIN_DEFAULT
#define SKIN_BRITISH_REG_ALT	8
#define SKIN_BRITISH_REG_ALT2	16

#define SKIN_AMER_REG			SKIN_DEFAULT
#define SKIN_AMER_REG_ALT		8
#define SKIN_AMER_REG_ALT2		16
#endif
