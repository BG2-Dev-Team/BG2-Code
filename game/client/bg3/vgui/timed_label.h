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
www.bg2mod.com

Note that because of the sheer volume of files in the Source SDK this
notice cannot be put in all of them, but merely the ones that have any
changes from the original SDK.
In order to facilitate easy searching, all changes are and must be
commented on the following form:

//BG3 - <name of contributer>[ - <small description>]
*/

#ifndef BG3_TIMED_LABEL_H
#define BG3_TIMED_LABEL_H

#include "cbase.h"
#include <game/client/iviewport.h>
#include <vgui_controls/Frame.h>
#include "baseviewport.h"
#include <vgui_controls/TextEntry.h>
#include <vgui_controls/Panel.h>
#include <vgui_controls/ImagePanel.h>
#include <vgui_controls/Menu.h>

namespace vgui {
	class TimedLabel : public Label {
	public:
		DECLARE_CLASS_SIMPLE(TimedLabel, Label);
		
		TimedLabel(Panel* pParent, const char* pszName, const char* text) : BaseClass(pParent, pszName, text) {}

		void Paint() override {
			m_bShouldDraw = gpGlobals->curtime < m_flEndDrawTime;
			BaseClass::Paint();
		}

		bool IsVisible() override {
			return m_bShouldDraw && BaseClass::IsVisible();
		}

		void Activate(float flDuration) {
			SetVisible(true);
			m_bShouldDraw = true;
			m_flEndDrawTime = gpGlobals->curtime + flDuration;
		}

	private:
		bool m_bShouldDraw = false;
		float m_flEndDrawTime = -1.f;
	};
}

#endif //BG3_TIMED_LABEL_H