//
// BG2 - 
//

#include "cbase.h"
#include "hud.h"
#include "hudelement.h"
#include "hud_macros.h"
#include "hud_numericdisplay.h"
#include "iclientmode.h"
#include "c_basehlplayer.h"
#include "c_hl2mp_player.h"
#include "VGuiMatSurface/IMatSystemSurface.h"
#include "materialsystem/imaterial.h"
#include "materialsystem/imesh.h"
#include "materialsystem/imaterialvar.h"
#include "./bg2/ironsights.h"

#include <vgui/IScheme.h>
#include <vgui/ISurface.h>
#include <KeyValues.h>
#include <vgui_controls/AnimationController.h>

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CHudIronsight : public vgui::Panel, public CHudElement
{
	DECLARE_CLASS_SIMPLE( CHudIronsight, vgui::Panel );

public:
	CHudIronsight( const char *pElementName );
	
	bool	ShouldDraw( void );
	void	Init( void );
	void	LevelInit( void );

protected:
	virtual void ApplySchemeSettings(vgui::IScheme *scheme);
	virtual void Paint( void );

private:

	bool	m_bIronSights;
	bool	m_bPainted;
	float   m_flStartTime;

	CMaterialReference m_Material;
};

DECLARE_HUDELEMENT( CHudIronsight );

using namespace vgui;

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CHudIronsight::CHudIronsight( const char *pElementName ) : CHudElement(pElementName), BaseClass(NULL, "HudIronsights")
{
	vgui::Panel *pParent = g_pClientMode->GetViewport();
	SetParent( pParent );
	
	//SetHiddenBits( HIDEHUD_HEALTH | HIDEHUD_PLAYERDEAD | HIDEHUD_NEEDSUIT );
}

//-----------------------------------------------------------------------------
// Purpose: standard hud element init function
//-----------------------------------------------------------------------------
void CHudIronsight::Init( void )
{
	m_bIronSights = false;
	m_bPainted = false;
	m_flStartTime = -999.0f;
	m_Material.Init( "vgui/zoom", TEXTURE_GROUP_VGUI );
}

//-----------------------------------------------------------------------------
// Purpose: standard hud element init function
//-----------------------------------------------------------------------------
void CHudIronsight::LevelInit( void )
{
	Init();
}

//-----------------------------------------------------------------------------
// Purpose: sets scheme colors
//-----------------------------------------------------------------------------
void CHudIronsight::ApplySchemeSettings( vgui::IScheme *scheme )
{
	BaseClass::ApplySchemeSettings(scheme);

	SetPaintBackgroundEnabled(false);
	SetPaintBorderEnabled(false);
	SetFgColor(scheme->GetColor("ZoomReticleColor", GetFgColor()));

	int screenWide, screenTall;
	GetHudSize(screenWide, screenTall);
	SetBounds(0, 0, screenWide, screenTall);
}

//-----------------------------------------------------------------------------
// Purpose: Save CPU cycles by letting the HUD system early cull
// costly traversal.  Called per frame, return true if thinking and 
// painting need to occur.
//-----------------------------------------------------------------------------
bool CHudIronsight::ShouldDraw( void )
{
	C_BaseHLPlayer *pPlayer = dynamic_cast<C_BaseHLPlayer *>(C_BasePlayer::GetLocalPlayer());
	if ( pPlayer == NULL )
		return false;

	C_BaseCombatWeapon *wpn = pPlayer->GetActiveWeapon();

	//draw while wepon is sighted or until we're done zooming out
	if ( (wpn && wpn->m_bIsIronsighted) || gpGlobals->curtime < m_flStartTime + IRONSIGHTS_FADE_TIME )
		return CHudElement::ShouldDraw();
	else
		return false;
}

//-----------------------------------------------------------------------------
// Purpose: draws the zoom effect
//-----------------------------------------------------------------------------
void CHudIronsight::Paint( void )
{
	m_bPainted = false;

	//C_BaseHLPlayer *pPlayer = dynamic_cast<C_BaseHLPlayer *>(C_BasePlayer::GetLocalPlayer());
	C_HL2MP_Player *pPlayer = dynamic_cast<C_HL2MP_Player*>(C_HL2MP_Player::GetLocalPlayer());
	C_BaseCombatWeapon *wpn = pPlayer->GetActiveWeapon();

	if ( pPlayer == NULL )
		return;

	if ( wpn )
	{
		if ( wpn->m_bIsIronsighted && !m_bIronSights  )
		{
			m_bIronSights = true;
			m_flStartTime = gpGlobals->curtime;
		}
		else if ( !wpn->m_bIsIronsighted && m_bIronSights )
		{
			m_bIronSights = false;
			m_flStartTime = gpGlobals->curtime;
		}
	}

	// draw the appropriately scaled zoom animation
	float deltaTime = ( gpGlobals->curtime - m_flStartTime );
	float alpha = clamp( deltaTime / IRONSIGHTS_FADE_TIME, 0.0f, 1.0f );
	
	if ( !m_bIronSights )
		alpha = 1.0f - alpha;

	Color col = GetFgColor();
	col[3] = alpha * 64;

	surface()->DrawSetColor( col );
	
	// draw zoom circles
	int wide, tall;
	GetSize(wide, tall);

	// draw the darkened edges, with a rotated texture in the four corners
	CMatRenderContextPtr pRenderContext( materials );
	pRenderContext->Bind( m_Material );
	IMesh *pMesh = pRenderContext->GetDynamicMesh( true, NULL, NULL, NULL );

	float x0 = 0.0f, x1 = wide / 2.0f, x2 = wide;
	float y0 = 0.0f, y1 = tall / 2.0f, y2 = tall;

	float uv1 = 1.0f - (1.0f / 255.0f);
	float uv2 = 0.0f + (1.0f / 255.0f);

	struct coord_t
	{
		float x, y;
		float u, v;
	};
	coord_t coords[16] = 
	{
		// top-left
		{ x0, y0, uv1, uv2 },
		{ x1, y0, uv2, uv2 },
		{ x1, y1, uv2, uv1 },
		{ x0, y1, uv1, uv1 },

		// top-right
		{ x1, y0, uv2, uv2 },
		{ x2, y0, uv1, uv2 },
		{ x2, y1, uv1, uv1 },
		{ x1, y1, uv2, uv1 },

		// bottom-right
		{ x1, y1, uv2, uv1 },
		{ x2, y1, uv1, uv1 },
		{ x2, y2, uv1, uv2 },
		{ x1, y2, uv2, uv2 },

		// bottom-left
		{ x0, y1, uv1, uv1 },
		{ x1, y1, uv2, uv1 },
		{ x1, y2, uv2, uv2 },
		{ x0, y2, uv1, uv2 },
	};

	CMeshBuilder meshBuilder;
	meshBuilder.Begin( pMesh, MATERIAL_QUADS, 4 );

	for (int i = 0; i < 16; i++)
	{
		meshBuilder.Color4f( 0.0, 0.0, 0.0, alpha );
		meshBuilder.TexCoord2f( 0, coords[i].u, coords[i].v );
		meshBuilder.Position3f( coords[i].x, coords[i].y, 0.0f );
		meshBuilder.AdvanceVertex();
	}

	meshBuilder.End();
	pMesh->Draw(); 


	m_bPainted = true;
}
