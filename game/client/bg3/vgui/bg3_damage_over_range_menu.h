
#ifndef BG3_DAMAGE_OVER_RANGE_MENU_H
#define BG3_DAMAGE_OVER_RANGE_MENU_H

#include <vgui/IScheme.h>
#include <vgui/KeyCode.h>
#include <vgui/MouseCode.h>
#include <vgui_controls/HTML.h>
#include <vgui_controls/Frame.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/ComboBox.h>
#include <vgui_controls/ToggleButton.h>
#include <vgui_controls/CheckButton.h>
#include "vgui_bitmapimage.h"
#include "vgui_bitmapbutton.h"
#include <vgui_controls/ImagePanel.h>
#include <imagemouseoverbutton.h>
#include "../shared/bg3/bg3_weapon_def.h"
#include "../shared/bg3/bg3_class.h"

#define DAMAGE_OVER_RANGE_GRAPH_TIME_INTERVAL 0.001f
#define DAMAGE_OVER_RANGE_GRAPH_YARD_INTERVAL 50
#define DAMAGE_OVER_RANGE_GRAPH_NUM_YARD_INTERVALS (DAMAGE_OVER_RANGE_GRAPH_RANGE_MAX / DAMAGE_OVER_RANGE_GRAPH_YARD_INTERVAL)

class CDamageOverRangeMenu;
extern CDamageOverRangeMenu* g_pDamageOverRangeMenu;



class CWeaponGraphSelectionButton : public vgui::Button {
public:
	DECLARE_CLASS_SIMPLE(CWeaponGraphSelectionButton, vgui::Button);
	CWeaponGraphSelectionButton(vgui::Panel* pParent);

	void SetWeapon(const CWeaponDef* pWeapon);
	void OnMousePressed(vgui::MouseCode code) override;
	const CWeaponDef* GetWeapon() const { return m_pWeapon; }
	bool IsSelected() const { return m_bSelected; }

private:
	bool m_bSelected;
	const CWeaponDef* m_pWeapon;
};

class CWeaponGraphSelectionPanel : public vgui::Panel {
	DECLARE_CLASS_SIMPLE(CWeaponGraphSelectionPanel, vgui::Panel);

	CWeaponGraphSelectionPanel(vgui::Panel* pParent);
	void ApplySchemeSettings(vgui::IScheme *pScheme) override;

	//void Paint();

	void GetSelectedWeapons(CUtlVector<const CWeaponDef*>& weapons);
	void PrepareWeaponData();

	void PerformLayout();
private:
	const CWeaponDef** m_ppWeapons;
	CWeaponGraphSelectionButton** m_ppButtons;
	int m_iNumWeapons;
};


class CDamageOverRangeMenu : public vgui::Panel {
public:
	DECLARE_CLASS_SIMPLE(CDamageOverRangeMenu, vgui::Panel);

	CDamageOverRangeMenu(vgui::Panel* pParent) : vgui::Panel(pParent, "damageOverRangeMenu") {
		g_pDamageOverRangeMenu = this;
		m_pMenuTitle = new vgui::Label(this, "title", "#BG3_Damage_Over_Range_Menu_Title");
		m_pSelectWeaponsLabel = new vgui::Label(this, "selectionLabel", "#BG3_Damage_Over_Range_Menu_Tip");
		m_pWeaponSelectionPanel = new CWeaponGraphSelectionPanel(this);

		m_pHundredDamageLabel = new vgui::Label(this, "damageLabel", "100");
		m_pFiftyDamageLabel = new vgui::Label(this, "damageLabel", "50");

		char buffer[5];
		for (int i = 0; i < DAMAGE_OVER_RANGE_GRAPH_NUM_YARD_INTERVALS; i++) {
			Q_snprintf(buffer, sizeof(buffer), "%i", DAMAGE_OVER_RANGE_GRAPH_YARD_INTERVAL * (i + 1));
			m_ppRangeLabels[i] = new vgui::Label(this, "rangeLabel", buffer);
			m_ppRangeLabels[i]->SetContentAlignment(vgui::Label::Alignment::a_center);
		}
	}
	void ApplySchemeSettings(vgui::IScheme *pScheme) override;

	void Paint();
	void PrepareGraphData();
	void PaintGraphLines();
	void PaintGraphKey();

	void ResetWeapons();
	void PerformLayout();
private:
	void SimulateTrajectory(const CWeaponDef* pWeapon);
	CUtlVector<const CWeaponDef*>  m_weapons;

	vgui::Label* m_pMenuTitle;
	vgui::Label* m_pSelectWeaponsLabel;

	vgui::Label* m_pHundredDamageLabel;
	vgui::Label* m_pFiftyDamageLabel;

	vgui::Label* m_ppRangeLabels[DAMAGE_OVER_RANGE_GRAPH_NUM_YARD_INTERVALS];

	CWeaponGraphSelectionPanel* m_pWeaponSelectionPanel;
};
#endif //BG3_DAMAGE_OVER_RANGE_MENU_H