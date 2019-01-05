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

#ifndef HTML_HIDDEN_LOAD_H
#define HTML_HIDDEN_LOAD_H


#include "cbase.h"

#include <vgui_controls/Frame.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/HTML.h>

#include <game/client/iviewport.h>
#include "shareddefs.h"

//-------------------------------------------------------------------
// Purpose - to specialize the HTML object such that it hides
//   itself while loading a page. This way, when different pages are
//   loaded, the old page doesn't show while the new page is loading.
//TODO reimplement? SetVisible(...) doesn't seem to work here
//-------------------------------------------------------------------
class CHidingHTML : public vgui::HTML {
	DECLARE_CLASS_SIMPLE(CHidingHTML, vgui::HTML);

	CHidingHTML(Panel *parent, const char *name, bool allowJavaScript = false, bool bPopupWindow = false) 
		: BaseClass(parent, name, allowJavaScript, bPopupWindow) {}

	virtual bool OnStartRequest(const char *url, const char *target, const char *pchPostData, bool bIsRedirect) OVERRIDE {
		//SetVisible(false);
		return BaseClass::OnStartRequest(url, target, pchPostData, bIsRedirect);
	}

	virtual void OnFinishRequest(const char *url, const char *pageTitle, const CUtlMap < CUtlString, CUtlString > &headers) OVERRIDE {
		//SetVisible(true);
		return BaseClass::OnFinishRequest(url, pageTitle, headers);
	}
};


#endif //HTML_HIDDEN_LOADH