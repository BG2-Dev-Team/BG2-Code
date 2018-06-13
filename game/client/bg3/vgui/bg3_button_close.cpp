#include "cbase.h"
#include "bg3_button_close.h"

using namespace vgui;
extern IImage* g_pSelectionIcon;

CCloseButton::CCloseButton(Panel* pParent, const char* buttonName, const char* pszImgName)
	: Button(pParent, buttonName, ""), m_bMouseOver(false)  {

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

	//Get image
	m_pImage = scheme()->GetImage(pszImgName, false);
}

void CCloseButton::OnMousePressed(MouseCode code) {
	if (m_bParentIsViewport) {
		m_pViewportParent->ShowPanel(false);
	}
	else {
		m_pPanelParent->SetVisible(false);
	}
}

void CCloseButton::Paint() {
	int w, h;
	GetSize(w, h);
	m_pImage->SetSize(w, h);
	m_pImage->SetPos(0, 0);
	m_pImage->Paint();

	if (m_bMouseOver) {
		g_pSelectionIcon->SetPos(0, 0);
		g_pSelectionIcon->SetSize(w, h);
		g_pSelectionIcon->Paint();
	}
}
