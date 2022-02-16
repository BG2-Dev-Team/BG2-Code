/*
The Battle Grounds 3 - A Source modification
Copyright (C) 2017, The Battle Grounds 3 Team and Contributors

The Battle Grounds 3 free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

The Battle Grounds 3 is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

Contact information:
Chel "Awesome" Trunk		mail, in reverse: com . gmail @ latrunkster

You may also contact the (future) team via the Battle Grounds website and/or forum at:
battlegrounds3.com

Note that because of the sheer volume of files in the Source SDK this
notice cannot be put in all of them, but merely the ones that have any
changes from the original SDK.
In order to facilitate easy searching, all changes are and must be
commented on the following form:

//BG3 - <name of contributer>[ - <small description>]

Outside contributer - Tingtom - assisted Ricochet with floating flag icon hud

*/

#include "cbase.h"
#include "hudelement.h"
#include <vgui_controls/Panel.h>

#define MAX_FLAGS	12

class CHudFloatingIcon : public CHudElement, public vgui::Panel
{
	DECLARE_CLASS_SIMPLE(CHudFloatingIcon, vgui::Panel);

public:
	CHudFloatingIcon(const char *pElementName);
	void Init(void);
	void VidInit(void);
	virtual bool ShouldDraw(void);
	virtual void Paint(void);
	virtual void ApplySchemeSettings(vgui::IScheme *scheme);
	//void MsgFunc_flagstatus( bf_read &msg );

private:


	/*CHudTexture		*m_pIconBlank,
		*m_pIconRed,
		*m_pIconBlue;*/

	vgui::Label * m_pLabelFlag[MAX_FLAGS];

	CHudTexture* m_teamToIcon[3]; //0 is unassigned, 1 is brit, 2 is amer
};

using namespace vgui;
