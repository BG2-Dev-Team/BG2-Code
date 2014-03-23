#include "cbase.h"
#include "hudelement.h"
#include "hud_macros.h"
#include "c_playerresource.h"
#include "clientmode_hl2mpnormal.h"
#include <vgui_controls/Controls.h>
#include <vgui_controls/Panel.h>
#include <vgui/ISurface.h>
#include <vgui/ILocalize.h>
#include <KeyValues.h>
#include <sstream>
#include <set>
#include "c_baseplayer.h"
#include "c_team.h"
#include "c_hl2mp_player.h"
#include "hl2mp_gamerules.h"

class CHudPickup : public CHudElement, public vgui::Panel
{
	DECLARE_CLASS_SIMPLE( CHudPickup, vgui::Panel );
public:
	CHudPickup( const char *pElementName );	
	void Init();
	void VidInit();
	void MsgFunc_WeaponPickup( bf_read &msg );
	void MsgFunc_PickupDelay ( bf_read &msg );
	virtual void ApplySchemeSettings( IScheme *scheme );

	static CHudPickup *GetInstance();


private:
	CHudTexture	*m_PickupIcon;
	CHudTexture *m_PickupProgress;

protected:
	virtual void Paint();

	static CHudPickup *s_pInstance;	

	float m_fPickStart;
	float m_fPickEnd;
	float m_fTouchExpire;

};