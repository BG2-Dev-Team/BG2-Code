/*
	The Battle Grounds 2 - A Source modification
	Copyright (C) 2005, The Battle Grounds 2 Team and Contributors

	The Battle Grounds 2 free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	The Battle Grounds 2 is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

	Contact information:
		Tomas "Tjoppen" Härdin		mail, in reverse: se . gamedev @ tjoppen
		Jason "Draco" Houston		iamlorddraco@gmail.com

	You may also contact us via the Battle Grounds website and/or forum at:
		www.bgmod.com
*/
#include "cbase.h"
#include "hudelement.h"
#include "hl2mp_gamerules.h"
//BG2 - Tjoppen - #includes
#include "engine/IEngineSound.h"
#include "../shared/bg2/weapon_bg2base.h"
#include "bg2_hud_main.h"

#include "bg3/bg3_hud_compass.h"
//

// hintbox header
//#include "hintbox.h" //For now at least. -HairyPotter


// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;

DECLARE_HUDELEMENT( CHudBG2 );
DECLARE_HUD_MESSAGE( CHudBG2, HitVerif );
DECLARE_HUD_MESSAGE( CHudBG2, WinMusic );
DECLARE_HUD_MESSAGE( CHudBG2, CaptureSounds ); //HairyPotter
DECLARE_HUD_MESSAGE( CHudBG2, VCommSounds );

CHudBG2 *CHudBG2::s_pInstance = NULL;
CHudBG2 *CHudBG2::GetInstance()
{
	return CHudBG2::s_pInstance;
}
//==============================================
// CHudBG2's CHudFlags
// Constructor
//==============================================
CHudBG2::CHudBG2( const char *pElementName ) :
	CHudElement( pElementName ), BaseClass( NULL, "HudBG2" ),
	m_IsVictimAccumulator(true), m_IsAttackerAccumulator(false)
{
	vgui::Panel *pParent = g_pClientMode->GetViewport();
	SetParent( pParent );

	s_pInstance = this; //BG2 - HairyPotter (Nifty: Why is this here? If you're inside of the object, the "this" keyword is the address to the current object, and it's always available. Outside of the object, you can get access to an object's address by properly getting the reference.)

	SetHiddenBits( HIDEHUD_ALL );//HIDEHUD_MISCSTATUS );

	m_Base = NULL; 
	m_AmerHealthBase = m_AmerHealth = m_AmerStamina = NULL;
	m_BritHealthBase = m_BritHealth = m_BritStamina = NULL;
	m_SwingometerRed = m_SwingometerBlue = NULL;
	basex = basey = basew = baseh = 0;
	swingx = swingy = swingw = swingh = 0;
	staminax = staminay = staminaw = staminah = 0;
	healthbasex = healthbasey = healthbasew = healthbaseh = 0;
	healthx = healthy = healthw = healthh = 0;

	Color ColourWhite( 255, 255, 255, 255 );

	m_pLabelBScore = new vgui::Label( this, "RoundState_warmup", "");
	m_pLabelBScore->SetPaintBackgroundEnabled( false );
	m_pLabelBScore->SetPaintBorderEnabled( false );
	m_pLabelBScore->SizeToContents();
	m_pLabelBScore->SetContentAlignment( vgui::Label::a_west );
	m_pLabelBScore->SetFgColor( ColourWhite );

	m_pLabelAScore = new vgui::Label( this, "RoundState_warmup", "");
	m_pLabelAScore->SetPaintBackgroundEnabled( false );
	m_pLabelAScore->SetPaintBorderEnabled( false );
	m_pLabelAScore->SizeToContents();
	m_pLabelAScore->SetContentAlignment( vgui::Label::a_west );
	m_pLabelAScore->SetFgColor( ColourWhite );

	m_pLabelBTickets = new vgui::Label( this, "RoundState_warmup", "");
	m_pLabelBTickets->SetPaintBackgroundEnabled( false );
	m_pLabelBTickets->SetPaintBorderEnabled( false );
	m_pLabelBTickets->SizeToContents();
	m_pLabelBTickets->SetContentAlignment( vgui::Label::a_west );
	m_pLabelBTickets->SetFgColor( ColourWhite );

	m_pLabelATickets = new vgui::Label( this, "RoundState_warmup", "");
	m_pLabelATickets->SetPaintBackgroundEnabled( false );
	m_pLabelATickets->SetPaintBorderEnabled( false );
	m_pLabelATickets->SizeToContents();
	m_pLabelATickets->SetContentAlignment( vgui::Label::a_west );
	m_pLabelATickets->SetFgColor( ColourWhite );

	m_pLabelCurrentRound= new vgui::Label( this, "RoundState_warmup", "");
	m_pLabelCurrentRound->SetPaintBackgroundEnabled( false );
	m_pLabelCurrentRound->SetPaintBorderEnabled( false );
	m_pLabelCurrentRound->SizeToContents();
	m_pLabelCurrentRound->SetContentAlignment( vgui::Label::a_west );
	m_pLabelCurrentRound->SetFgColor( ColourWhite );

	m_pLabelAmmo = new vgui::Label( this, "RoundState_warmup", "");
	m_pLabelAmmo->SetPaintBackgroundEnabled( false );
	m_pLabelAmmo->SetPaintBorderEnabled( false );
	m_pLabelAmmo->SizeToContents();
	m_pLabelAmmo->SetContentAlignment( vgui::Label::a_west );
	m_pLabelAmmo->SetFgColor( ColourWhite );

	m_pLabelWaveTime = new vgui::Label( this, "RoundState_warmup", "");
	m_pLabelWaveTime->SetPaintBackgroundEnabled( false );
	m_pLabelWaveTime->SetPaintBorderEnabled( false );
	m_pLabelWaveTime->SizeToContents();
	m_pLabelWaveTime->SetContentAlignment( vgui::Label::a_west );
	m_pLabelWaveTime->SetFgColor( ColourWhite );

	m_pLabelRoundTime = new vgui::Label( this, "RoundState_warmup", "");
	m_pLabelRoundTime->SetPaintBackgroundEnabled( false );
	m_pLabelRoundTime->SetPaintBorderEnabled( false );
	m_pLabelRoundTime->SizeToContents();
	m_pLabelRoundTime->SetContentAlignment( vgui::Label::a_west );
	m_pLabelRoundTime->SetFgColor( ColourWhite );

	m_pLabelDamageTaken = new vgui::Label( pParent, "RoundState_warmup", "");
	m_pLabelDamageTaken->SetPaintBackgroundEnabled( false );
	m_pLabelDamageTaken->SetPaintBorderEnabled( false );
	m_pLabelDamageTaken->SizeToContents();
	m_pLabelDamageTaken->SetContentAlignment( vgui::Label::a_west );
	m_pLabelDamageTaken->SetFgColor( ColourWhite );

	m_pLabelDamageGiven = new vgui::Label( pParent, "RoundState_warmup", "");
	m_pLabelDamageGiven->SetPaintBackgroundEnabled( false );
	m_pLabelDamageGiven->SetPaintBorderEnabled( false );
	m_pLabelDamageGiven->SizeToContents();
	m_pLabelDamageGiven->SetContentAlignment( vgui::Label::a_west );
	m_pLabelDamageGiven->SetFgColor( ColourWhite );

	m_pLabelLMS = new vgui::Label( this, "RoundState_warmup", "");
	m_pLabelLMS->SetPaintBackgroundEnabled( false );
	m_pLabelLMS->SetPaintBorderEnabled( false );
	m_pLabelLMS->SizeToContents();
	m_pLabelLMS->SetContentAlignment( vgui::Label::a_west );
	m_pLabelLMS->SetFgColor( ColourWhite );

	CBaseEntity::PrecacheScriptSound( "Americans.win" );
	CBaseEntity::PrecacheScriptSound( "British.win" );

	Reset();
}

void CHudBG2::ApplySettings(KeyValues *inResourceData)
{
	float factor = 2;

	//we've designed for half size at 640x480, meaning full size at 1280x960
	//note that this isn't a particularly common resolution, but whatever
	//the following makes it so the hud scales down for lower resolutions but never scales up (consider 1920x1080 or so)
	if (ScreenHeight() < 960)
		factor = ScreenHeight() / 480.0f;

	//since the position and size of the hud is set automagically we need to fix them up here
	//also, I'm a bit paranoid about parsing ypos myself
	//hence I simply invented yposr whose value is "80" instead of "r80" and thus easily parsed
	SetPos(factor * inResourceData->GetInt("xpos"),
		   ScreenHeight() - factor * inResourceData->GetInt("yposr"));
	SetWide(factor * inResourceData->GetInt("wide"));
	SetTall(factor * inResourceData->GetInt("tall"));

#define GET_COORD(name) factor * inResourceData->GetInt(#name)
#define GET(name) name = GET_COORD(name)

	//grab and scale coordinates and dimensions
	m_pLabelAScore      ->SetPos(GET_COORD(ascorex),   GET_COORD(ascorey));
	m_pLabelBScore      ->SetPos(GET_COORD(bscorex),   GET_COORD(bscorey));
	m_pLabelATickets    ->SetPos(GET_COORD(aticketsx), GET_COORD(aticketsy));
	m_pLabelBTickets    ->SetPos(GET_COORD(bticketsx), GET_COORD(bticketsy));
	m_pLabelLMS         ->SetPos(GET_COORD(lmsx),      GET_COORD(lmsy));
	m_pLabelCurrentRound->SetPos(GET_COORD(curroundx), GET_COORD(curroundy));
	m_pLabelRoundTime   ->SetPos(GET_COORD(roundx),    GET_COORD(roundy));
	m_pLabelWaveTime    ->SetPos(GET_COORD(wavex),     GET_COORD(wavey));
	m_pLabelAmmo        ->SetPos(GET_COORD(ammox),     GET_COORD(ammoy));

	GET(basex);       GET(basey);       GET(basew);       GET(baseh);
	GET(swingx);      GET(swingy);      GET(swingw);      GET(swingh);
	GET(staminax);    GET(staminay);    GET(staminaw);    GET(staminah);
	GET(healthbasex); GET(healthbasey); GET(healthbasew); GET(healthbaseh);
	GET(healthx);     GET(healthy);     GET(healthw);     GET(healthh);

	/*NHudCompass::Init();
	NHudCompass::SetSize(GET_COORD(compassw), GET_COORD(compassh));
	NHudCompass::SetPos(GET_COORD(compassx), GET_COORD(compassy));*/

	BaseClass::ApplySettings(inResourceData);
}

//==============================================
// CHudBG2's ApplySchemeSettings
// applies the schemes
//==============================================
void CHudBG2::ApplySchemeSettings( IScheme *scheme )
{
	vgui::HFont font = scheme->GetFont("HudBG2Font");
	m_pLabelAScore->SetFont(font);
	m_pLabelBScore->SetFont(font);
	m_pLabelATickets->SetFont(font);
	m_pLabelBTickets->SetFont(font);
	m_pLabelLMS->SetFont(font);
	m_pLabelCurrentRound->SetFont(font);
	m_pLabelRoundTime->SetFont(font);
	m_pLabelWaveTime->SetFont(font);
	m_pLabelAmmo->SetFont(font);

	BaseClass::ApplySchemeSettings( scheme );
	SetPaintBackgroundEnabled( false );

}

//==============================================
// CHudBG2's Init
// Inits any vars needed
//==============================================
void CHudBG2::Init( void )
{
	HOOK_HUD_MESSAGE( CHudBG2, HitVerif );
	HOOK_HUD_MESSAGE( CHudBG2, WinMusic );
	HOOK_HUD_MESSAGE( CHudBG2, CaptureSounds );
	HOOK_HUD_MESSAGE( CHudBG2, VCommSounds );
	//BG2 - Tjoppen - serverside blood, a place to hook as good as any
	extern void  __MsgFunc_ServerBlood( bf_read &msg );
	HOOK_MESSAGE( ServerBlood );
	//
}

//==============================================
// CHudBG2's VidInit
// Inits any textures needed
//==============================================
void CHudBG2::VidInit( void )
{
	m_Base = gHUD.GetIcon( "hud_base" );
	m_AmerHealthBase = gHUD.GetIcon("hud_amer_health_base");
	m_AmerHealth     = gHUD.GetIcon("hud_amer_health");
	m_AmerStamina    = gHUD.GetIcon("hud_amer_stamina");
	m_BritHealthBase = gHUD.GetIcon("hud_brit_health_base");
	m_BritHealth     = gHUD.GetIcon("hud_brit_health");
	m_BritStamina    = gHUD.GetIcon("hud_brit_stamina");
	m_SwingometerRed = gHUD.GetIcon("hud_swingometer_red");
	m_SwingometerBlue= gHUD.GetIcon("hud_swingometer_blue");

	
}

//==============================================
// CHudBG2's ShouldDraw
// whether the panel should be drawing
//==============================================
bool CHudBG2::ShouldDraw( void )
{
	bool temp = CHudElement::ShouldDraw();
	HideShowAll(temp);	//BG2 - Tjoppen - HACKHACK
	return temp;
}

//==============================================
// CHudBG2's Paint
// errr... paints the panel
//==============================================
static ConVar cl_draw_lms_indicator( "cl_draw_lms_indicator", "1", FCVAR_CLIENTDLL, "Draw last man standing indicator string?" );

static Color colorForFlash( float flFlashEnd )
{
	//flash from red back to white
	//hold at red for 250 ms, fade to white in 250 ms
	float value = (gpGlobals->curtime - flFlashEnd) * 4 * 256;
	value = clamp(value, 0, 255);

	return Color(255, value, value, 255);
}

// BG2 - VisualMelon - Complain to me about these being in the wrong place
extern int hitVerificationHairs;
extern int hitVerificationLatency;
ConVar cl_hitveriflatency( "cl_hitveriflatency", "2.0", FCVAR_ARCHIVE | FCVAR_CLIENTDLL, "How many seconds to show hit verification for?" ); // BG2 - VisualMelon - I'm bad at names

void CHudBG2::Paint()
{
	if( !m_Base || !m_AmerHealthBase || !m_AmerHealth || !m_AmerStamina ||
		!m_BritHealthBase || !m_BritHealth || !m_BritStamina ||
		!m_SwingometerRed || !m_SwingometerBlue )
		return;

	//BG2 - Tjoppen - Always paint damage label, so it becomes visible while using iron sights
	//fade out the last second
	float alphaTaken = (m_flTakenExpireTime - gpGlobals->curtime) * 255.0f;
	float alphaGiven = (m_flGivenExpireTime - gpGlobals->curtime) * 255.0f;

	if( alphaTaken < 0.0f )
		alphaTaken = 0.0f;
	else if( alphaTaken > 255.0f )
		alphaTaken = 255.0f;

	if( alphaGiven < 0.0f )
		alphaGiven = 0.0f;
	else if( alphaGiven > 255.0f )
		alphaGiven = 255.0f;

	if( alphaGiven > 0 )
	{
		m_pLabelDamageGiven->SizeToContents();

		//center and put one somewhat above crosshair
		m_pLabelDamageGiven->SetPos((ScreenWidth()-m_pLabelDamageGiven->GetWide())/2, (ScreenHeight()*5)/7 - m_pLabelDamageTaken->GetTall()); // verticle offset was formerly -20
		//m_pLabelDamageVerificator->SetFgColor( Color( 255, 255, 255, (int)alpha ) );
		m_pLabelDamageGiven->SetAlpha( (int)alphaGiven );

		m_pLabelDamageGiven->SetVisible( true );
	}
	else
		m_pLabelDamageGiven->SetVisible( false );

	if( alphaTaken > 0 )
	{
		m_pLabelDamageTaken->SizeToContents();

		//center and put one somewhat below crosshair
		m_pLabelDamageTaken->SetPos((ScreenWidth()-m_pLabelDamageTaken->GetWide())/2, (ScreenHeight()*5)/7);
		//m_pLabelDamageVerificator->SetFgColor( Color( 255, 255, 255, (int)alpha ) );
		m_pLabelDamageTaken->SetAlpha( (int)alphaTaken );

		m_pLabelDamageTaken->SetVisible( true );

	}
	else
		m_pLabelDamageTaken->SetVisible( false );

	C_HL2MP_Player *pHL2Player = dynamic_cast<C_HL2MP_Player*>(C_HL2MP_Player::GetLocalPlayer());
	if (!pHL2Player || !pHL2Player->IsAlive())
	{
		//spectating
		int n = GetSpectatorTarget();
		if( n <= 0 )
		{
			HideShowAll(false);
			return;	//don't look at worldspawn..
		}

		pHL2Player = dynamic_cast<C_HL2MP_Player*>(UTIL_PlayerByIndex(n));

		if( !pHL2Player )
		{
			HideShowAll(false);
			return;
		}
	}

	C_BaseCombatWeapon *wpn = pHL2Player->GetActiveWeapon();
	if (!wpn)
	{
		HideShowAll(false);
		return;
	}
	// Don't draw hud if we're using Iron Sights. -HairyPotter
	if (wpn->m_bIsIronsighted)
	{
		HideShowAll(false);
		return;
	}
	//

	HideShowAll(true);

	Color ColourRed( 255, 0, 0, 255 );
	Color ColourWhite( 255, 255, 255, 255 );

	char msg2[512];

	m_Base->DrawSelf(basex,basey,basew,baseh,ColourWhite);

	C_Team *pAmer = GetGlobalTeam(TEAM_AMERICANS);
	C_Team *pBrit = GetGlobalTeam(TEAM_BRITISH);

	//swingometer
	//displays based on tickets in ticket mode, score otherwise
	int swinga = 0, swingb = 0;

	if( HL2MPRules()->UsingTickets() )
	{
		swinga = pAmer ? pAmer->m_iTicketsLeft : 0;
		swingb = pBrit ? pBrit->m_iTicketsLeft : 0;
	}
	else
	{
		swinga = pAmer ? pAmer->Get_Score() : 0;
		swingb = pBrit ? pBrit->Get_Score() : 0;
	}

	float swing = m_flLastSwing;
	int tot = swinga + swingb;

	//guard against negatives. you never know..
	if( swinga >= 0 && swingb >= 0 && tot > 0 )
		swing = swinga / (float)tot;
	else
		swing = 0.5f;	//returns to center when both scores are zero (round reset for instance)

	//move swing value towards m_flLastSwing
	float swingdiff = swing - m_flLastSwing;	//useful for setting alpha on bars
	float swingadjust = swingdiff;

	//cap swing rate so each percent takes 20 ms (meaning a full swing = 2 sec)
	float swingrate = 0.5f;

	if( swingadjust < -swingrate * gpGlobals->frametime ) swingadjust = -swingrate * gpGlobals->frametime;
	if( swingadjust >  swingrate * gpGlobals->frametime ) swingadjust =  swingrate * gpGlobals->frametime;

	swing = m_flLastSwing + swingadjust;

	//crop and scale to fit in swingw x swingh rectangle
	m_SwingometerBlue->DrawSelfCropped( swingx, swingy, 0, 0,
	                                    m_SwingometerBlue->Width() * swing, m_SwingometerBlue->Height(),
										swingw * swing, swingh, ColourWhite );
	m_SwingometerRed ->DrawSelfCropped( swingx + swingw * swing, swingy, m_SwingometerRed->Width() * swing, 0,
	                                    m_SwingometerRed->Width() * (1 - swing), m_SwingometerRed->Height(),
										swingw * (1 - swing), swingh, ColourWhite );

	m_flLastSwing = swing;

	//figure out whether our ticket meter should flash
	extern ConVar mp_tickets_drain_a, mp_tickets_drain_b;

	if( HL2MPRules()->UsingTickets() )
	{
		//Avoid spamming the player - only flash their team's number
		//HACKHACK: Assume ticket decrease >= drain means we're being drained.
		//          That or a whole bunch of players on our team spawned.
		//          Require the drain/delta to be at least 2 though.
		int minDelta = 2;

		if( pHL2Player->GetTeamNumber() == TEAM_AMERICANS && mp_tickets_drain_a.GetInt() >= minDelta &&
			swinga <= m_iLastSwingA - mp_tickets_drain_a.GetInt() )
			m_flAFlashEnd = gpGlobals->curtime + 0.5f;

		if( pHL2Player->GetTeamNumber() == TEAM_BRITISH   && mp_tickets_drain_b.GetInt() >= minDelta &&
			swingb <= m_iLastSwingB - mp_tickets_drain_b.GetInt() )
			m_flBFlashEnd = gpGlobals->curtime + 0.5f;
	}

	m_iLastSwingA = swinga;
	m_iLastSwingB = swingb;

	CHudTexture *pHealthBase, *pHealth, *pStamina;

	if (pHL2Player->GetTeamNumber() == TEAM_AMERICANS)
	{
		pHealthBase  = m_AmerHealthBase;
		pHealth      = m_AmerHealth;
		pStamina     = m_AmerStamina;
	}
	else
	{
		pHealthBase  = m_BritHealthBase;
		pHealth      = m_BritHealth;
		pStamina     = m_BritStamina;
	}

	float health = pHL2Player->GetHealth() / 100.0f;
	float stamina = pHL2Player->m_iStamina  / 100.0f;

	pStamina->DrawSelfCropped( staminax, staminay + staminah * (1 - stamina), 0, pStamina->Height() * (1 - stamina),
	                           pStamina->Width(), pStamina->Height() * stamina,
							   staminaw, staminah * stamina, ColourWhite );

	//crop both even though normally pHealth draws over pHealthBase
	//this way users can mod so that the base is the actual meter, like the old HUD
	//just to clarify, pHealthBase == amount of health left while pHealth == damage
	pHealthBase->DrawSelfCropped( healthx, healthy + healthh * (1 - health), 0, pHealthBase->Height() * (1 - health),
	                              pHealth->Width(), pHealth->Height() * health,
	                              healthw, healthh * health, ColourWhite );

	pHealth->DrawSelfCropped( healthx, healthy, 0, 0,
	                          pHealth->Width(), pHealth->Height() * (1 - health),
	                          healthw, healthh * (1 - health), ColourWhite );

	if( HL2MPRules()->UsingTickets())
	{
		Q_snprintf( msg2, 512, "%i ", pBrit ? pBrit->Get_Score() : 0);	//BG2 - Tjoppen - avoid NULL
		m_pLabelBScore->SetText(msg2);
		m_pLabelBScore->SizeToContents();
		m_pLabelBScore->SetFgColor( ColourWhite );
	
		Q_snprintf( msg2, 512, "%i ", pAmer ? pAmer->Get_Score() : 0);	//BG2 - Tjoppen - avoid NULL
		m_pLabelAScore->SetText(msg2);
		m_pLabelAScore->SizeToContents();
		m_pLabelAScore->SetFgColor( ColourWhite );
	}

	extern ConVar mp_rounds;
	bool bHasRoundCount = mp_rounds.GetBool();
	if (bHasRoundCount) {
		Q_snprintf(msg2, 512, "Round %i/%i ", HL2MPRules()->m_iCurrentRound, mp_rounds.GetInt());
		m_pLabelCurrentRound->SetText(msg2);
		m_pLabelCurrentRound->SizeToContents();
		m_pLabelCurrentRound->SetFgColor(ColourWhite);
	}

	Q_snprintf( msg2, 512, "%i ", swingb);
	m_pLabelBTickets->SetText(msg2);
	m_pLabelBTickets->SizeToContents();
	m_pLabelBTickets->SetFgColor( colorForFlash(m_flBFlashEnd) );

	Q_snprintf( msg2, 512, "%i ", swinga);
	m_pLabelATickets->SetText(msg2);
	m_pLabelATickets->SizeToContents();
	m_pLabelATickets->SetFgColor( colorForFlash(m_flAFlashEnd) );

	int iAmmoCount = pHL2Player->GetAmmoCount(wpn->GetPrimaryAmmoType());
	if (wpn->Clip1() > 0)
		iAmmoCount++;
	if( iAmmoCount > 0)
	{
		Q_snprintf( msg2, 512, "%i ", iAmmoCount);
		m_pLabelAmmo->SetText(msg2);
		if (wpn->Clip1() == 0)
		{
			m_pLabelAmmo->SetFgColor(ColourRed);
		}
		else
		{
			m_pLabelAmmo->SetFgColor(ColourWhite);
		}
		m_pLabelAmmo->SizeToContents();
	}
	else
		m_pLabelAmmo->SetVisible( false );

	int wavetime = ceilf(HL2MPRules()->m_fLastRespawnWave + mp_respawntime.GetFloat() - gpGlobals->curtime);
	if (wavetime < 1800) {
		if (wavetime < 0)
			wavetime = 0;
		Q_snprintf(msg2, 512, "%i:%02i ", wavetime / 60, wavetime % 60);
		m_pLabelWaveTime->SetText(msg2);
	} else {
		m_pLabelWaveTime->SetText(" ");
	}
	m_pLabelWaveTime->SizeToContents();
	m_pLabelWaveTime->SetFgColor( ColourWhite );

	//update round timer text
	int roundtime;
	if (bHasRoundCount) {
		roundtime = ceilf(HL2MPRules()->m_fLastRoundRestart + mp_roundtime.GetFloat() - gpGlobals->curtime);
	} else {
		roundtime = ceilf(HL2MPRules()->GetMapRemainingTime()); // -gpGlobals->curtime);
	}
	//only show actual time if it's a reasonable time
	if (roundtime < 3600) {
		if (roundtime < 0)
			roundtime = 0;
		Q_snprintf(msg2, 32, "%i:%02i ", roundtime / 60, roundtime % 60);
		m_pLabelRoundTime->SetText(msg2);
	} else {
		m_pLabelRoundTime->SetText(" ");
	}
	m_pLabelRoundTime->SizeToContents();
	m_pLabelRoundTime->SetFgColor( ColourWhite );

	m_pLabelLMS->SetText( g_pVGuiLocalize->Find("#LMS") );
	m_pLabelLMS->SizeToContents();
	m_pLabelLMS->SetFgColor( ColourWhite );

	//Paint compass
	//NHudCompass::Paint();
}

//==============================================
// CHudBG2's OnThink
// every think check if we're hidden, if so take away the text
//==============================================
void CHudBG2::OnThink()
{
	// display hintbox if stamina drops below 20%
	//C_HL2MP_Player *pHL2Player = dynamic_cast<C_HL2MP_Player*>(C_HL2MP_Player::GetLocalPlayer()); //RE-enable these. -HairyPotter
	//if (pHL2Player && pHL2Player->m_iStamina < 20)
 	//	(GET_HUDELEMENT( CHintbox ))->UseHint(HINT_STAMINA, 6, DISPLAY_ONCE);
	
	//let paint sort it out
	/*C_BasePlayer *player = C_BasePlayer::GetLocalPlayer();
	if (player)
	{
		if (player->GetHealth() <= 0)
		{
			HideShowAll(false);
		}
	}*/
}

void CHudBG2::Reset( void )
{
	//mapchange, clear indicators. and stuff.
	m_flGivenExpireTime = 0;
	m_flTakenExpireTime = 0;
	m_flLastSwing = 0.5f;
	m_flAFlashEnd = m_flBFlashEnd = 0;
	m_iLastSwingA = m_iLastSwingB = 0;
	HideShowAll( false );
}

void CHudBG2::HideShowAll( bool visible )
{
	m_pLabelAScore->SetVisible(visible && HL2MPRules()->UsingTickets());
	m_pLabelBScore->SetVisible(visible && HL2MPRules()->UsingTickets());
	m_pLabelBTickets->SetVisible(visible);
	m_pLabelATickets->SetVisible(visible);

	extern ConVar mp_rounds;
	m_pLabelCurrentRound->SetVisible(visible && mp_rounds.GetBool());

	m_pLabelWaveTime->SetVisible(visible && !IsLMS());
	m_pLabelRoundTime->SetVisible(visible);
	m_pLabelAmmo->SetVisible(visible);
	//m_pLabelBGVersion->SetVisible(false);	// BP: not used yet as its not subtle enough, m_pLabelBGVersion->SetVisible(ShouldDraw());
	m_pLabelDamageGiven->SetVisible(m_flGivenExpireTime > gpGlobals->curtime);	//always show damage indicator (unless expired)
	m_pLabelDamageTaken->SetVisible(m_flTakenExpireTime > gpGlobals->curtime);	//always show damage indicator (unless expired)
	m_pLabelLMS->SetVisible( visible && IsLMSstrict() && cl_draw_lms_indicator.GetBool() );
}

//BG2 - Tjoppen - cl_hitverif & cl_winmusic && capturesounds & voice comm sounds //HairyPotter
ConVar	cl_hitverif( "cl_hitverif", "1", FCVAR_ARCHIVE, "Display hit verification? 1 For all. 2 For Melee only. 3 For Firearms only." );
ConVar	cl_hitdamageline( "cl_hitdamageline", "0", FCVAR_ARCHIVE, "Display hit damage line? 1 For all. 2 For Melee only. 3 For Firearms only." ); // BG2 - VisualMelon - default to off
ConVar	cl_winmusic( "cl_winmusic", "1", FCVAR_ARCHIVE, "Play win music?" );
ConVar	cl_capturesounds( "cl_capturesounds", "1", FCVAR_ARCHIVE, "Play flag capture sounds?" );
ConVar cl_vcommsounds("cl_vcommsounds", "1", FCVAR_ARCHIVE, "Allow voice comm sounds?" );
//


void CHudBG2::MsgFunc_HitVerif( bf_read &msg )
{
	int attacker, victim, hitgroup;
	int attackType;
	int damage;
	Color ColourWhite( 255, 255, 255, 255 ); // BG2 - These are now both WHITE, these colours are NOT uses
	//Color ColourGreen( 0, 150, 0, 255 ); // BG2 - VisualMelon - made darker (formerly 0, 200, 0, 255)
	//Color ColourYellow( 226, 226, 0, 255 ); // BG2 - VisualMelon - made yellow (formerly red 200, 0, 0, 255)

	attacker	= msg.ReadByte();
	victim		= msg.ReadByte();
	hitgroup	= msg.ReadByte();
	damage		= msg.ReadShort();

	//attack type and hitgroup are packed into the same byte
	attackType = (hitgroup >> 4) & 0xF;
	hitgroup &= 0xF;

	//don't display messages for 0-damage
	if (damage <= 0)
		return;

	C_HL2MP_Player *pAttacker = dynamic_cast<C_HL2MP_Player*>(UTIL_PlayerByIndex( attacker ));

	if( !pAttacker )
		return;

	C_BaseBG2Weapon *pWeapon = dynamic_cast<C_BaseBG2Weapon*>(pAttacker->GetActiveWeapon());

	if( C_BasePlayer::GetLocalPlayer()->entindex() == attacker )
	{
		if ((cl_hitverif.GetInt() == 1) || 
			((attackType == ATTACKTYPE_STAB || attackType == ATTACKTYPE_SLASH) && cl_hitverif.GetInt() == 3) ||
			((attackType == ATTACKTYPE_FIREARM && cl_hitverif.GetInt() == 2)))
		{
			hitVerificationLatency = cl_hitveriflatency.GetFloat();
			hitVerificationHairs = gpGlobals->curtime + hitVerificationLatency;
		}
	}

	if( !pWeapon || !C_BasePlayer::GetLocalPlayer() )
		return;

	//play melee hit sound if attacker's last attack was a melee attack
	//note: hit sound should always be played, regardless of what cl_hitdamageline is set to
	//in other words: don't mess up the order of these tests
	if( attackType == ATTACKTYPE_STAB || attackType == ATTACKTYPE_SLASH )
	{
		//we need play the hit sound like this since WeaponSound() only plays the local player's weapon's sound, not those of the other players
		const char *sound = pWeapon->GetShootSound( MELEE_HIT );

		if( sound && sound[0] )
		{
			CLocalPlayerFilter filter;
			C_BaseEntity::EmitSound( filter, SOUND_FROM_LOCAL_PLAYER, sound );
		}

		//cl_hitdamageline 3 -> don't display melee hit damage lines
		if( cl_hitdamageline.GetInt() == 3 )
			return;
	}

	//cl_hitdamageline 0 -> don't display any hit damage line, while 2 -> don't display shot hit damage lines
	if( cl_hitdamageline.GetInt() == 0 || (cl_hitdamageline.GetInt() == 2 && attackType == ATTACKTYPE_FIREARM) )
		return;

	//Msg( "MsgFunc_HitVerif: %i %i %i %f\n", attacker, victim, hitgroup, damage );

	//accumulate partial HitVerif messages within a small time window
	//this makes buckshot damage display correctly with and without simulated bullets
	std::string message;

	if( C_BasePlayer::GetLocalPlayer()->entindex() == victim )
	{
		//local player is victim ("You were hit...")
		message = m_IsVictimAccumulator.Accumulate( damage, attacker, hitgroup );
		m_pLabelDamageTaken->SetFgColor( ColourWhite ); // BG2 - VisualMelon - formerly ColourYellow

		m_pLabelDamageTaken->SetText( message.c_str() );
		m_flTakenExpireTime = gpGlobals->curtime + 5.0f;
	}
	else if( C_BasePlayer::GetLocalPlayer()->entindex() == attacker )
	{
		//"You hit..."
		message = m_IsAttackerAccumulator.Accumulate( damage, victim, hitgroup );
		m_pLabelDamageGiven->SetFgColor( ColourWhite ); // BG2 - VisualMelon - formerly ColourGreen

		m_pLabelDamageGiven->SetText( message.c_str() );
		m_flGivenExpireTime = gpGlobals->curtime + 5.0f;
	}
	else
	{
		/**
		 * We're neither the attacker nor the victim.
		 * This should rarely happen since only the attacker and victim are added to
		 * this message's recipient filter. However, a user reported the indicator
		 * showing another player's damage, so return in this case just to be safe.
		 */
		return;
	}

	
}

void CHudBG2::MsgFunc_WinMusic( bf_read &msg )
{
	if( !cl_winmusic.GetBool() )
		return;

	int team = msg.ReadByte();

	CLocalPlayerFilter filter;


	switch( team ) //Switches are more efficient.
	{
		case TEAM_AMERICANS:
			C_BaseEntity::EmitSound( filter, SOUND_FROM_LOCAL_PLAYER, "Americans.win" );
			break;
		case TEAM_BRITISH:
			C_BaseEntity::EmitSound( filter, SOUND_FROM_LOCAL_PLAYER, "British.win" );
			break;
	}
}
//Make capture sounds client side. -HairyPotter
void CHudBG2::MsgFunc_CaptureSounds( bf_read &msg ) 
{
	if( !cl_capturesounds.GetBool() )
		return;

	Vector pos;
	char sound[255];

	msg.ReadBitVec3Coord( pos );
	msg.ReadString( sound, sizeof(sound) );
	PlayCaptureSound( pos, sound );
}
void CHudBG2::PlayCaptureSound( const Vector &origin, char sound[255] )
{
	CLocalPlayerFilter filter;

	//Hadto fix this because valve's code doesn't like direct wavs + scripts being played with the same code.
	EmitSound_t params;
	params.m_pSoundName = sound;
	params.m_flSoundTime = 0.0f;
	params.m_pOrigin = &origin;
	/*if ( params.m_flVolume == NULL )
	{
		Msg("The volume params aren't being set right for direct .wav sounds \n");
		params.m_flVolume = 0.8f;
	}*/
	if ( params.m_nChannel == NULL ) //Defualt to CHAN_AUTO for now.
	{
		//Msg("The channel params aren't being set right for direct .wav sounds \n");
		params.m_nChannel = CHAN_AUTO;
	}
	if ( params.m_SoundLevel == NULL )
	{
		//Msg("The soundlevel params aren't being set right for direct .wav sounds \n");
		params.m_SoundLevel = SNDLVL_60dB;
	}


	C_BaseEntity::EmitSound( filter, SOUND_FROM_WORLD, params );
	//
}
//
//Make voice comm sounds client side. -HairyPotter
void CHudBG2::MsgFunc_VCommSounds( bf_read &msg ) 
{
	if ( !cl_vcommsounds.GetBool() )
		return;

	int client	= msg.ReadByte();
	char snd[512];

	msg.ReadString( snd, sizeof(snd) );

	PlayVCommSound( snd, client );
}
void CHudBG2::PlayVCommSound( char snd[512], int playerindex )
{
	C_BasePlayer *pPlayer = UTIL_PlayerByIndex( playerindex );

	if ( pPlayer ) //Just make sure.
		pPlayer->EmitSound( snd );
}
//