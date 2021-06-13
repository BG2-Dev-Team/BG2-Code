//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "hud.h"
#include "hud_crosshair.h"
#include "iclientmode.h"
#include "view.h"
#include "vgui_controls/Controls.h"
#include "vgui/ISurface.h"
#include "ivrenderview.h"
#include "materialsystem/imaterialsystem.h"
#include "VGuiMatSurface/IMatSystemSurface.h"
#include "client_virtualreality.h"
#include "sourcevr/isourcevirtualreality.h"

//BG2 - Tjoppen - #includes
#include "../game/shared/bg2/weapon_bg2base.h"
#include "./bg2/ironsights.h"
#include "c_playerresource.h"

//

#ifdef SIXENSE
#include "sixense/in_sixense.h"
#endif

#ifdef PORTAL
#include "c_portal_player.h"
#endif // PORTAL

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

ConVar crosshair( "crosshair", "1", FCVAR_ARCHIVE );
ConVar cl_observercrosshair( "cl_observercrosshair", "1", FCVAR_ARCHIVE );


//BG2 - Tjoppen - cl_crosshair*
ConVar cl_crosshair("cl_crosshair", "14", FCVAR_ARCHIVE, "Bitmask describing how to draw the crosshair\n  1 = dynamic circular\n  2 = static three-lined\n  4 = dot(square) in the middle\n  8 = hud/crosshair.vtf");
ConVar cl_crosshair_scale("cl_crosshair_scale", "1", FCVAR_ARCHIVE, "Scale of cl_crosshairstyle 1, 2 and 4");

ConVar cl_crosshair_r("cl_crosshair_r", "197", FCVAR_ARCHIVE, "Crosshair redness. 0-255");
ConVar cl_crosshair_g("cl_crosshair_g", "149", FCVAR_ARCHIVE, "Crosshair greenness. 0-255");
ConVar cl_crosshair_b("cl_crosshair_b", "105", FCVAR_ARCHIVE, "Crosshair blueness. 0-255");
ConVar cl_crosshair_a("cl_crosshair_a", "167", FCVAR_ARCHIVE, "Crosshair opacity(alpha). 0-255");

ConVar cl_hitverifhairstyle("cl_hitverifhairstyle", "0", FCVAR_ARCHIVE); // BG2 - VisualMelon - Style of hit verification hairs
ConVar cl_hitverifhairlen("cl_hitverifhairlen", "1", FCVAR_ARCHIVE); // BG2 - VisualMelon - Length of hit verification hairs
ConVar cl_crosshair_texture_index("cl_crosshair_texture_index", "0", FCVAR_ARCHIVE, "Which crosshair texture to use", true, 0.0f, true, 9.0f);

using namespace vgui;

int ScreenTransform( const Vector& point, Vector& screen );

#ifdef TF_CLIENT_DLL
// If running TF, we use CHudTFCrosshair instead (which is derived from CHudCrosshair)
#else
DECLARE_HUDELEMENT( CHudCrosshair );
#endif

CHudCrosshair* CHudCrosshair::g_pCrosshair = NULL;
CHudCrosshair::CHudCrosshair( const char *pElementName ) :
		CHudElement( pElementName ), BaseClass( NULL, "HudCrosshair" )
{
	vgui::Panel *pParent = g_pClientMode->GetViewport();
	SetParent( pParent );

	g_pCrosshair = this;

	m_pCrosshair = 0;

	m_clrCrosshair = Color( 0, 0, 0, 0 );

	m_vecCrossHairOffsetAngle.Init();

	m_flMeleeScanEndTime = -FLT_MAX;

	SetHiddenBits( HIDEHUD_PLAYERDEAD | HIDEHUD_CROSSHAIR );
}

CHudCrosshair::~CHudCrosshair()
{
}

void CHudCrosshair::ApplySchemeSettings( IScheme *scheme )
{
	BaseClass::ApplySchemeSettings( scheme );

	//BG2 - Commented -HairyPotter
	//m_pCrosshair = gHUD.GetIcon("hud_crosshair");
	SetCrosshairTextureIndex(cl_crosshair_texture_index.GetInt());
	//m_pDefaultCrosshair = gHUD.GetIcon("crosshair_default");
	//	
	SetPaintBackgroundEnabled( false );

    SetSize( ScreenWidth(), ScreenHeight() );

	SetForceStereoRenderToFrameBuffer( true );
}

//-----------------------------------------------------------------------------
// Purpose: Save CPU cycles by letting the HUD system early cull
// costly traversal.  Called per frame, return true if thinking and 
// painting need to occur.
//-----------------------------------------------------------------------------
bool CHudCrosshair::ShouldDraw( void )
{
	bool bNeedsDraw;

	if ( m_bHideCrosshair )
		return false;

	C_BasePlayer* pPlayer = C_BasePlayer::GetLocalPlayer();
	if ( !pPlayer )
		return false;

	C_BaseCombatWeapon *pWeapon = pPlayer->GetActiveWeapon();
	if ( pWeapon )
	{
#ifdef TWEAK_IRONSIGHTS
		extern ConVar viewmodel_adjust_enabled;
#endif

		// BG2 - VisualMelon - Moved this into the function that draws Iron Sights, as the hit verification cross needs to show up with iron sights
		/*		if ( pWeapon->m_bIsIronsighted ) //No crosshair in Iron Sights. -HairyPotter
		#ifdef TWEAK_IRONSIGHTS
		return viewmodel_adjust_enabled.GetBool();	//BG2 - Tjoppen - unless we're adjusting the sights
		#else
		return false;
		#endif*/

		if (!pWeapon->ShouldDrawCrosshair())
			return false;
}

#ifdef PORTAL
	C_Portal_Player *portalPlayer = ToPortalPlayer(pPlayer);
	if ( portalPlayer && portalPlayer->IsSuppressingCrosshair() )
		return false;
#endif // PORTAL

	/* disabled to avoid assuming it's an HL2 player.
	// suppress crosshair in zoom.
	if ( pPlayer->m_HL2Local.m_bZooming )
		return false;
	*/

	// draw a crosshair only if alive or spectating in eye
	bNeedsDraw = m_pCrosshair &&
		crosshair.GetInt() &&
		!engine->IsDrawingLoadingImage() &&
		!engine->IsPaused() &&
		g_pClientMode->ShouldDrawCrosshair() &&
		!(pPlayer->GetFlags() & FL_FROZEN) &&
		(pPlayer->entindex() == render->GetViewEntity()) &&
		!pPlayer->IsInVGuiInputMode() &&
		(pPlayer->IsAlive() || (pPlayer->GetObserverMode() == OBS_MODE_IN_EYE) || (cl_observercrosshair.GetBool() && pPlayer->GetObserverMode() == OBS_MODE_ROAMING));

	return (bNeedsDraw && CHudElement::ShouldDraw());
}

extern int hitVerificationHairs;
extern int hitVerificationLatency;

void CHudCrosshair::IronPaint(void)
{
	// BG2 - VisualMelon - TODO: find an event or something to reset hitVerificationHairs
	// HORRIBLE HACK
	if (hitVerificationHairs > gpGlobals->curtime + hitVerificationLatency * 2)
		hitVerificationHairs = -1;

	// BG2 - VisualMelon - Hit verification glyth
	if (hitVerificationHairs > gpGlobals->curtime)
	{
		int w = ScreenWidth(),
			h = ScreenHeight();

		float	cx = w / 2 - 0.5f,
			cy = h / 2 - 0.5f;

		//hitVerificationHairs--; // BG2 - VisualMelon - Need a better system than a frame counter

		// BG2 - VisualMelon - Usual Colour and Scale
		float	scale = cl_crosshair_scale.GetFloat() * min(w, h) / 300.f;

		Color hairColor(cl_crosshair_r.GetInt(), cl_crosshair_g.GetInt(),
			cl_crosshair_b.GetInt(), cl_crosshair_a.GetInt() * (((float)(hitVerificationHairs - gpGlobals->curtime)) / hitVerificationLatency));

		float root2 = 1.41;

		if (cl_hitverifhairstyle.GetInt() == 0)
		{
			surface()->DrawSetColor(hairColor);

			//float dim0 = r * root2 * 0.5 * 0.33;
			//float dim1 = r * root2 * 0.5 * 0.66;
			//float dim0 = scale;
			//float dim1 = scale * 3.f;
			float dim0 = scale * root2 * 0.5;
			float dim1 = scale * 3.f * root2 * 0.5 * cl_hitverifhairlen.GetFloat();

			float bx0; float by0; float bx1; float by1;

			float piby2scale = scale * root2 * 0.5;

			int ti = (piby2scale - 0.5f) / 2;
			int li = (int)piby2scale - ti;

			// middle line
			bx0 = cx + dim0; by0 = cy + dim0; bx1 = cx + dim1; by1 = cy + dim1;
			surface()->DrawLine(bx0, by0, bx1, by1);
			bx0 = cx - dim0; by0 = cy + dim0; bx1 = cx - dim1; by1 = cy + dim1;
			surface()->DrawLine(bx0, by0, bx1, by1);
			bx0 = cx - dim0; by0 = cy - dim0; bx1 = cx - dim1; by1 = cy - dim1;
			surface()->DrawLine(bx0, by0, bx1, by1);
			bx0 = cx + dim0; by0 = cy - dim0; bx1 = cx + dim1; by1 = cy - dim1;
			surface()->DrawLine(bx0, by0, bx1, by1);

			// ++
			bx0 = cx + dim0; by0 = cy + dim0; bx1 = cx + dim1; by1 = cy + dim1;
			for (int i = 1; i <= ti; i++)
			{
				if (i % 2)
				{
					bx0++;
					bx1++;
				}
				else
				{
					by0--;
					by1--;
				}
				surface()->DrawLine(bx0, by0, bx1, by1);
			}

			bx0 = cx + dim0; by0 = cy + dim0; bx1 = cx + dim1; by1 = cy + dim1;
			for (int i = 1; i <= li; i++)
			{
				if (i % 2)
				{
					by0++;
					by1++;
				}
				else
				{
					bx0--;
					bx1--;
				}
				surface()->DrawLine(bx0, by0, bx1, by1);
			}

			// --
			bx0 = cx - dim0; by0 = cy - dim0; bx1 = cx - dim1; by1 = cy - dim1;
			for (int i = 1; i <= ti; i++)
			{
				if (i % 2)
				{
					bx0--;
					bx1--;
				}
				else
				{
					by0++;
					by1++;
				}
				surface()->DrawLine(bx0, by0, bx1, by1);
			}

			bx0 = cx - dim0; by0 = cy - dim0; bx1 = cx - dim1; by1 = cy - dim1;
			for (int i = 1; i <= li; i++)
			{
				if (i % 2)
				{
					by0--;
					by1--;
				}
				else
				{
					bx0++;
					bx1++;
				}
				surface()->DrawLine(bx0, by0, bx1, by1);
			}

			// +-
			bx0 = cx + dim0; by0 = cy - dim0; bx1 = cx + dim1; by1 = cy - dim1;
			for (int i = 1; i <= ti; i++)
			{
				if (i % 2)
				{
					bx0++;
					bx1++;
				}
				else
				{
					by0++;
					by1++;
				}
				surface()->DrawLine(bx0, by0, bx1, by1);
			}

			bx0 = cx + dim0; by0 = cy - dim0; bx1 = cx + dim1; by1 = cy - dim1;
			for (int i = 1; i <= li; i++)
			{
				if (i % 2)
				{
					by0--;
					by1--;
				}
				else
				{
					bx0--;
					bx1--;
				}
				surface()->DrawLine(bx0, by0, bx1, by1);
			}

			// -+
			bx0 = cx - dim0; by0 = cy + dim0; bx1 = cx - dim1; by1 = cy + dim1;
			for (int i = 1; i <= ti; i++)
			{
				if (i % 2)
				{
					bx0--;
					bx1--;
				}
				else
				{
					by0--;
					by1--;
				}
				surface()->DrawLine(bx0, by0, bx1, by1);
			}

			bx0 = cx - dim0; by0 = cy + dim0; bx1 = cx - dim1; by1 = cy + dim1;
			for (int i = 1; i <= li; i++)
			{
				if (i % 2)
				{
					by0++;
					by1++;
				}
				else
				{
					bx0++;
					bx1++;
				}
				surface()->DrawLine(bx0, by0, bx1, by1);
			}

			// BG2 - VisualMelon - This only allows single width  lines
			//surface()->DrawLine( cx + dim0, cy + dim0, cx + dim1, cy + dim1 );
			//surface()->DrawLine( cx - dim0, cy + dim0, cx - dim1, cy + dim1 );
			//surface()->DrawLine( cx - dim0, cy - dim0, cx - dim1, cy - dim1 );
			//surface()->DrawLine( cx + dim0, cy - dim0, cx + dim1, cy - dim1 );
		}
		else if (cl_hitverifhairstyle.GetInt() == 1)
		{
			surface()->DrawSetColor(hairColor);

			//float dim0 = r * root2 * 0.33;
			//float dim1 = r * root2 * 0.66;
			//float dim0 = scale;
			//float dim1 = scale * 3.f;
			float dim0 = scale * root2 * 0.5;
			float dim1 = scale * 3.f * root2 * 0.5;

			surface()->DrawLine(cx + dim0, cy + dim0, cx + dim1, cy + dim1);
			surface()->DrawLine(cx - dim0, cy + dim0, cx - dim1, cy + dim1);
			surface()->DrawLine(cx - dim0, cy - dim0, cx - dim1, cy - dim1);
			surface()->DrawLine(cx + dim0, cy - dim0, cx + dim1, cy - dim1);
		}
	}
}

#ifdef TF_CLIENT_DLL
extern ConVar cl_crosshair_red;
extern ConVar cl_crosshair_green;
extern ConVar cl_crosshair_blue;
extern ConVar cl_crosshair_scale;
#endif


void CHudCrosshair::GetDrawPosition ( float *pX, float *pY, bool *pbBehindCamera, QAngle angleCrosshairOffset )
{
	QAngle curViewAngles = CurrentViewAngles();
	Vector curViewOrigin = CurrentViewOrigin();

	int vx, vy, vw, vh;
	vgui::surface()->GetFullscreenViewport( vx, vy, vw, vh );

	float screenWidth = vw;
	float screenHeight = vh;

	float x = screenWidth / 2;
	float y = screenHeight / 2;

	bool bBehindCamera = false;

	C_BasePlayer* pPlayer = C_BasePlayer::GetLocalPlayer();
	if ( ( pPlayer != NULL ) && ( pPlayer->GetObserverMode()==OBS_MODE_NONE ) )
	{
		bool bUseOffset = false;
		
		Vector vecStart;
		Vector vecEnd;

		if ( UseVR() )
		{
			// These are the correct values to use, but they lag the high-speed view data...
			vecStart = pPlayer->Weapon_ShootPosition();
			Vector vecAimDirection = pPlayer->GetAutoaimVector( 1.0f );
			// ...so in some aim modes, they get zapped by something completely up-to-date.
			g_ClientVirtualReality.OverrideWeaponHudAimVectors ( &vecStart, &vecAimDirection );
			vecEnd = vecStart + vecAimDirection * MAX_TRACE_LENGTH;

			bUseOffset = true;
		}

#ifdef SIXENSE
		// TODO: actually test this Sixsense code interaction with things like HMDs & stereo.
        if ( g_pSixenseInput->IsEnabled() && !UseVR() )
		{
			// Never autoaim a predicted weapon (for now)
			vecStart = pPlayer->Weapon_ShootPosition();
			Vector aimVector;
			AngleVectors( CurrentViewAngles() - g_pSixenseInput->GetViewAngleOffset(), &aimVector );
			// calculate where the bullet would go so we can draw the cross appropriately
			vecEnd = vecStart + aimVector * MAX_TRACE_LENGTH;
			bUseOffset = true;
		}
#endif

		if ( bUseOffset )
		{
			trace_t tr;
			UTIL_TraceLine( vecStart, vecEnd, MASK_SHOT, pPlayer, COLLISION_GROUP_NONE, &tr );

			Vector screen;
			screen.Init();
			bBehindCamera = ScreenTransform(tr.endpos, screen) != 0;

			x = 0.5f * ( 1.0f + screen[0] ) * screenWidth + 0.5f;
			y = 0.5f * ( 1.0f - screen[1] ) * screenHeight + 0.5f;
		}
	}

	// MattB - angleCrosshairOffset is the autoaim angle.
	// if we're not using autoaim, just draw in the middle of the 
	// screen
	if ( angleCrosshairOffset != vec3_angle )
	{
		QAngle angles;
		Vector forward;
		Vector point, screen;

		// this code is wrong
		angles = curViewAngles + angleCrosshairOffset;
		AngleVectors( angles, &forward );
		VectorAdd( curViewOrigin, forward, point );
		ScreenTransform( point, screen );

		x += 0.5f * screen[0] * screenWidth + 0.5f;
		y += 0.5f * screen[1] * screenHeight + 0.5f;
	}

	*pX = x;
	*pY = y;
	*pbBehindCamera = bBehindCamera;
}

void CHudCrosshair::RegisterMeleeSwing(CBaseBG2Weapon* pWeapon, int iAttack) {
	//ignore if we already have a scan scheduled
	if (m_flMeleeScanEndTime > gpGlobals->curtime)
		return;

	//estimate scan time based on player's ping
	float ping = g_PR->GetPing(CBasePlayer::GetLocalPlayer()->entindex()) / 1000.f;
	m_flMeleeScanStartTime = gpGlobals->curtime + ping;
	m_flMeleeScanEndTime = m_flMeleeScanStartTime + pWeapon->GetRetraceDuration(iAttack);
}

static const char* g_aCrosshairTexturePaths[] = {
	"hud_crosshair0",
	"hud_crosshair1",
	"hud_crosshair2",
	"hud_crosshair3",
	"hud_crosshair4",
	"hud_crosshair5",
	"hud_crosshair6",
	"hud_crosshair7",
	"hud_crosshair8",
	"hud_crosshair9"
};

void CHudCrosshair::SetCrosshairTextureIndex(int i) {
	m_iCrosshairTextureIndex = i;

	if (i < 0) i = 0;
	if (i > 9) i = 9;
	m_pCrosshair = gHUD.GetIcon(g_aCrosshairTexturePaths[i]);
}

ConVar cl_dynamic_crosshair("cl_dynamic_crosshair", "1", FCVAR_ARCHIVE);

static bool g_bPreviousFrameIronsighted = false;
static float g_flEndCrosshairTime = -FLT_MAX;
ConVar cl_persistent_crosshair("cl_persistent_crosshair", "0", FCVAR_CHEAT);
extern ConVar r_drawviewmodel;
void CHudCrosshair::Paint( void )
{
	//BG2 - found this stuff while porting to 2016 - Awesome
	C_BasePlayer* pPlayer = C_BasePlayer::GetLocalPlayer();
	if (!pPlayer)
		return;
	C_BaseCombatWeapon *pWeapon = pPlayer->GetActiveWeapon();

	if (!pWeapon)
		return;

	bool bViewModel = r_drawviewmodel.GetBool();
	bool bWorseCrosshair = !bViewModel && pWeapon->m_bIsIronsighted && gpGlobals->curtime > g_flEndCrosshairTime;

	if (pWeapon->m_bIsIronsighted && !cl_persistent_crosshair.GetBool() && bViewModel) //No crosshair in Iron Sights. -HairyPotter
	{
		IronPaint();
		if (g_bPreviousFrameIronsighted && (gpGlobals->curtime > g_flEndCrosshairTime)) {
			return;
		}
		else if (!g_bPreviousFrameIronsighted) {
			g_flEndCrosshairTime = gpGlobals->curtime + pWeapon->Def()->m_flIronsightsTime;
		}
	}
	g_bPreviousFrameIronsighted = pWeapon->m_bIsIronsighted;


	if ( !m_pCrosshair )
		return;

	if ( !IsCurrentViewAccessAllowed() )
		return;

	//BG2 - found this missing while porting to 2016 - Awesome
	/*
	C_BasePlayer* pPlayer = C_BasePlayer::GetLocalPlayer();
	if ( !pPlayer )
		return;*/

	float x, y;
	bool bBehindCamera;
	GetDrawPosition ( &x, &y, &bBehindCamera, m_vecCrossHairOffsetAngle );

	if( bBehindCamera )
		return;

	//float flWeaponScale = 1.f;
	//int iTextureW = m_pCrosshair->Width();
	//int iTextureH = m_pCrosshair->Height();
	//BG2 - Same old code slightly tweaked - HairyPotter
	C_BaseBG2Weapon *weapon = dynamic_cast<C_BaseBG2Weapon*>(pPlayer->GetActiveWeapon());

	if (weapon)
	{
		int w = ScreenWidth(),
			h = ScreenHeight();

		bool dynamic = cl_dynamic_crosshair.GetBool();

		float	cx = w / 2,
			cy = h / 2,
			r = max(w, h) / 2;

		//only draw the circle if we're holding a firearm that is ready to fire
		//when no, the circle will fade out. once holding a loaded firearm again, it will fade back in
		bool drawCircle = false;

		static float lastr = 0;
		static float circlealpha = 1;

		float crosshairScale = max((int)bWorseCrosshair, cl_crosshair_scale.GetFloat());

	
		if (weapon->GetAttackType(C_BaseBG2Weapon::ATTACK_PRIMARY) == ATTACKTYPE_FIREARM)
		{
			drawCircle = weapon->m_iClip1 || !dynamic;

			//add both spreads (accuracy and internal ballistics) to give a more accurate circle
			r *= !dynamic ? 2.2f : weapon->GetAccuracy(C_BaseBG2Weapon::ATTACK_PRIMARY) + weapon->GetCurrentAmmoSpread();
			r *= 0.008725f;
		}
		else
			r *= 0.05f;

		//crosshair size changes faster when we're aiming in
		//only decay into new crosshair size here if the new one is bigger than the old one
		//as the accuracy equation already does the opposite for us
		//if (r > lastr)
			//r = lastr = r + (lastr - r) * expf((g_bPreviousFrameIronsighted * -3 - 13.0f) * gpGlobals->frametime);
		


		//lerp circle alpha up or down
		circlealpha += (drawCircle ? 6 : -6) * gpGlobals->frametime;

		//bigger, obstructive circles more transparent
		//Msg("%f ", r);
		circlealpha /= (r / 100);

		if (circlealpha < 0) circlealpha = 0;
		if (circlealpha > 1) circlealpha = 1;

		// BG2 - VisualMelon - paint all the stuff that shows up even in iron sights
		IronPaint();

		//melee hitscan icon
		bool bDrawHitScan = dynamic && gpGlobals->curtime > m_flMeleeScanStartTime && gpGlobals->curtime < m_flMeleeScanEndTime;

		//Msg( "%f %f %f\n", cx, cy, r );
		//center dot
		if (cl_crosshair.GetInt() & 4 || bDrawHitScan)
		{
			float scale = crosshairScale * min(w, h) / 300;
			if (bDrawHitScan || bWorseCrosshair) scale *= 3;

			surface()->DrawSetColor(Color(cl_crosshair_r.GetInt(), cl_crosshair_g.GetInt(),
				cl_crosshair_b.GetInt(), cl_crosshair_a.GetInt()));
			//surface()->DrawFilledRect( cx-1, cy-1, cx, cy );	//dot in the middle

			//dot in the middle
			surface()->DrawFilledRect(cx - scale * 0.25f, cy - scale * 0.25f,
				cx + scale * 0.25f, cy + scale * 0.25f);
		}

		//three lines
		if ((cl_crosshair.GetInt() & 2) && !bWorseCrosshair )
		{
			float	scale = crosshairScale * min(w, h) / 300.f;/*,
																			  expand = r * 0.25f;*/

			surface()->DrawSetColor(Color(cl_crosshair_r.GetInt(), cl_crosshair_g.GetInt(),
				cl_crosshair_b.GetInt(), cl_crosshair_a.GetInt()));

			//BG3 - accuracy indicator for when none other available
			float accuracyShift = 0;
			bool nocircleOtherwise = !(cl_crosshair.GetInt() & (1 | 8));
			if (nocircleOtherwise && drawCircle && dynamic)
				accuracyShift = r;

			//without expansion
			//left
			surface()->DrawFilledRect(cx - scale * 3.f - accuracyShift, cy - scale * 0.25f,
				cx - scale - accuracyShift, cy + scale * 0.25f);
			//right
			surface()->DrawFilledRect(cx + scale + accuracyShift, cy - scale * 0.25f,
				cx + scale * 3.f + accuracyShift, cy + scale * 0.25f);
			//bottom
			surface()->DrawFilledRect(cx - scale * 0.25f, cy + scale,
				cx + scale * 0.25f, cy + scale * 3.f);
		}

		//circle
		if (!bWorseCrosshair &&  circlealpha > 0 && cl_crosshair.GetInt() & 1)
		{
			int step = 10;
			float r2 = r * 0.95f;

			surface()->DrawSetColor(Color((cl_crosshair_r.GetInt() * 2) / 3, (cl_crosshair_g.GetInt() * 2) / 3,
				(cl_crosshair_b.GetInt() * 2) / 3, cl_crosshair_a.GetInt() / 3 * circlealpha));
			for (int dx = -1; dx <= 1; dx++)
				for (int dy = -1; dy <= 1; dy++)
				{
					if (dx == 0 && dy == 0)
						continue;

					int cx2 = cx + dx,
						cy2 = cy + dy;

					int lastx = (int)(cx2 + r2*cosf((float)-step * M_PI / 180.f)),
						lasty = (int)(cy2 + r2*sinf((float)-step * M_PI / 180.f));

					for (int i = 0, j = 0; i < 360; i += step, j++)
					{
						float	a = (float)i * M_PI / 180.f;

						int x = (int)(cx2 + r2*cosf(a)),
							y = (int)(cy2 + r2*sinf(a));

						//surface()->DrawSetColor( Color( (j&1)*255, (j&1)*255, (j&1)*255, 255 ) );

						surface()->DrawLine(lastx, lasty, x, y);

						lastx = x;
						lasty = y;
					}
				}

			int lastx = (int)(cx + r2*cosf((float)-step * M_PI / 180.f)),
				lasty = (int)(cy + r2*sinf((float)-step * M_PI / 180.f));

			surface()->DrawSetColor(Color(cl_crosshair_r.GetInt(), cl_crosshair_g.GetInt(),
				cl_crosshair_b.GetInt(), cl_crosshair_a.GetInt()*circlealpha));

			for (int i = 0, j = 0; i < 360; i += step, j++)
			{
				float	a = (float)i * M_PI / 180.f;

				int x = (int)(cx + r2*cosf(a)),
					y = (int)(cy + r2*sinf(a));

				surface()->DrawLine(lastx, lasty, x, y);

				lastx = x;
				lasty = y;
			}
		}

		//texture
		if (drawCircle && cl_crosshair.GetInt() & 8 && !bWorseCrosshair)
		{
			if (m_iCrosshairTextureIndex != cl_crosshair_texture_index.GetInt()) {
				SetCrosshairTextureIndex(cl_crosshair_texture_index.GetInt());
			}
			Color iconColor(255, 80, 0, 255);

			r *= crosshairScale;

			int x = ScreenWidth() / 2 - r,
				y = ScreenHeight() / 2 - r;

			m_pCrosshair->DrawSelf(x, y, 2 * r, 2 * r, iconColor);
		}
	}
	//
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudCrosshair::SetCrosshairAngle( const QAngle& angle )
{
	VectorCopy( angle, m_vecCrossHairOffsetAngle );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudCrosshair::SetCrosshair( CHudTexture *texture, const Color& clr )
{
	//m_pCrosshair = texture; //BG2 - Commented -HairyPotter
	m_clrCrosshair = clr;
}

//-----------------------------------------------------------------------------
// Purpose: Resets the crosshair back to the default
//-----------------------------------------------------------------------------
void CHudCrosshair::ResetCrosshair()
{
	SetCrosshair( m_pDefaultCrosshair, Color(255, 255, 255, 255) );
}
