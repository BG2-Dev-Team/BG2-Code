#include "cbase.h"
#include "bg3_button_close.h"

using namespace vgui;

CCloseButton::CCloseButton(Panel* pParent, const char* buttonName)
	: Button(pParent, buttonName, "")  {

	SetPaintBackgroundEnabled(false);
	SetPaintBorderEnabled(false);

	IViewPortPanel* pViewport = dynamic_cast<IViewPortPanel*>(pParent);
	if (pViewport) {
		m_pViewportParent = pViewport;
		m_bParentIsViewport = true;
	}
	else {
		m_pPanelParent = pParent;
		m_bParentIsViewport = false;
	}

}

void CCloseButton::OnMousePressed(MouseCode code) {
	if (m_bParentIsViewport) {
		m_pViewportParent->ShowPanel(false);
	}
	else {
		m_pPanelParent->SetVisible(false);
	}
}
