#include "cbase.h"
#include "bg3_damage_over_range_menu.h"
#include "bg3_fonts.h"

extern ConVar sv_simulatedbullets_flex;
extern ConVar sv_simulatedbullets_freq;
extern ConVar sv_simulatedbullets_rwc;
extern ConVar sv_simulatedbullets_drag;
extern ConVar sv_simulatedbullets_gravity;
extern ConVar sv_gravity;

using namespace vgui;

CDamageOverRangeMenu* g_pDamageOverRangeMenu = NULL;

#define SELECTED_BUTTON_COLOR Color(20, 50, 20, 255)
#define DESELECTED_BUTTON_COLOR Color(50, 30, 30, 128)
#define ONE_HUNDRED_DAMAGE_OFFSET 400

//-------------------------------------------------------
// Weapon selection buttons
//-------------------------------------------------------

CWeaponGraphSelectionButton::CWeaponGraphSelectionButton(Panel* pParent) : vgui::Button(pParent, "weaponGraphSelectionButton", "") {
	m_bSelected = false;
}

void CWeaponGraphSelectionButton::SetWeapon(const CWeaponDef* pWeapon) {
	char buffer[128];
	Q_snprintf(buffer, sizeof(buffer), "#n_%s", pWeapon->m_pszWeaponDefName);
	SetText(buffer);

	m_pWeapon = pWeapon;
}

void CWeaponGraphSelectionButton::OnMousePressed(MouseCode code) {
	m_bSelected = !m_bSelected;
	g_pDamageOverRangeMenu->ResetWeapons();
	Color c = m_bSelected ? SELECTED_BUTTON_COLOR : DESELECTED_BUTTON_COLOR;
	SetDefaultColor(m_pWeapon->GetGraphColor(), c);
}


//-------------------------------------------------------
// Weapon selection panel
//-------------------------------------------------------
CWeaponGraphSelectionPanel::CWeaponGraphSelectionPanel(vgui::Panel* pParent) : vgui::Panel(pParent, "weaponGraphSelectionPanel") {
	PrepareWeaponData();

	m_ppButtons = new CWeaponGraphSelectionButton*[m_iNumWeapons];
	for (int i = 0; i < m_iNumWeapons; i++) {
		m_ppButtons[i] = new CWeaponGraphSelectionButton(this);
		m_ppButtons[i]->SetWeapon(m_ppWeapons[i]);
	}
}

void CWeaponGraphSelectionPanel::ApplySchemeSettings(vgui::IScheme *pScheme) {
	SetPaintBackgroundEnabled(false);
	BaseClass::ApplySchemeSettings(pScheme);

	for (int i = 0; i < m_iNumWeapons; i++) {
		Color c = m_ppWeapons[i]->GetGraphColor();
		m_ppButtons[i]->SetFgColor(m_ppWeapons[i]->GetGraphColor());
		m_ppButtons[i]->SetDefaultColor(c, DESELECTED_BUTTON_COLOR);
		m_ppButtons[i]->MakeReadyForUse();
	}
}

void CWeaponGraphSelectionPanel::GetSelectedWeapons(CUtlVector<const CWeaponDef*>& weapons) {
	weapons.RemoveAll();
	for (int i = 0; i < m_iNumWeapons; i++) {
		if (m_ppButtons[i]->IsSelected()) {
			weapons.AddToTail(m_ppButtons[i]->GetWeapon());
		}
	}
}

void CWeaponGraphSelectionPanel::PrepareWeaponData() {
	extern CUtlDict<CWeaponDef*> g_dictWeaponDefs;

	//Get firing weapons only
	CUtlVector<const CWeaponDef*> firingWeapons;
	for (int i = 0; i < g_dictWeaponDefs.MaxElement(); i++) {
		if (g_dictWeaponDefs.IsValidIndex(i)) {
			const CWeaponDef* def = g_dictWeaponDefs[i];
			if (def->m_Attackinfos[0].m_iAttacktype == ATTACKTYPE_FIREARM
				&& strstr(def->m_pszWeaponDefName, "nobayo") == NULL) {
				firingWeapons.AddToTail(def);
			}
		}
	}

	m_iNumWeapons = firingWeapons.Count();
	m_ppWeapons = new const CWeaponDef*[m_iNumWeapons];
	for (int i = 0; i < firingWeapons.Count(); i++) {
		m_ppWeapons[i] = firingWeapons.Element(i);
	}
}

void CWeaponGraphSelectionPanel::PerformLayout() {
	int w = GetWide();
	int h = GetTall() / m_iNumWeapons;
	for (int i = 0; i < m_iNumWeapons; i++) {
		m_ppButtons[i]->SetPos(0, h * i);
		m_ppButtons[i]->SetSize(w, h);
	}
}

//-------------------------------------------------------
// Damage over range menu
//-------------------------------------------------------

void CDamageOverRangeMenu::ApplySchemeSettings(IScheme* pScheme) {
	BaseClass::ApplySchemeSettings(pScheme);
	ResetWeapons();
	SetDefaultBG3FontScaled(pScheme, m_pMenuTitle);
	SetDefaultBG3FontScaled(pScheme, m_pSelectWeaponsLabel);
	SetDefaultBG3FontScaled(pScheme, m_pHundredDamageLabel);
	SetDefaultBG3FontScaled(pScheme, m_pFiftyDamageLabel);

	for (int i = 0; i < DAMAGE_OVER_RANGE_GRAPH_NUM_YARD_INTERVALS; i++) {
		SetDefaultBG3FontScaledVertical(pScheme, m_ppRangeLabels[i]);
	}
}

void CDamageOverRangeMenu::Paint() {

	PrepareGraphData();
	PaintGraphLines();
	PaintGraphKey();
}

void CDamageOverRangeMenu::PrepareGraphData() {
	for (int i = 0; i < m_weapons.Count(); i++) {
		SimulateTrajectory(m_weapons.Element(i));
	}
}

void CDamageOverRangeMenu::PaintGraphLines() {
	float xscale = GetWide() / 1024.f;
	float yscale = GetTall() / 768.f;
	int ybase = 700 * yscale;
	int xbase = 250 * xscale;
	int width = 700 * xscale;
	int height = 600 * yscale;

	auto damageToYOffset = [&](int damage) {
		return yscale * ONE_HUNDRED_DAMAGE_OFFSET * (damage / 100.f);
	};
																																															
	//first paint x-axis, y-axis, and important labels
	surface()->DrawSetColor(g_cBG3TextColor);
	surface()->DrawFilledRect(xbase - 3, ybase - height, xbase, ybase + 3); //y
	surface()->DrawFilledRect(xbase, ybase, xbase + width, ybase + 3); //x

	surface()->DrawSetColor(g_cBG3TextTransparentColor);

	//100-damage line
	surface()->DrawFilledRect(xbase, ybase - ONE_HUNDRED_DAMAGE_OFFSET * yscale, xbase + width, ybase - ONE_HUNDRED_DAMAGE_OFFSET * yscale + 3);

	//50-damage line
	surface()->DrawFilledRect(xbase, ybase - ONE_HUNDRED_DAMAGE_OFFSET / 2 * yscale, xbase + width, ybase - ONE_HUNDRED_DAMAGE_OFFSET/2 * yscale + 3);

	//draw vertical lines representing range
	surface()->DrawSetColor(g_cBG3GraphKeylineColor);

	//pixels per yard * 50
	const float PIXELS_PER_YARD = (1.f * width / DAMAGE_OVER_RANGE_GRAPH_RANGE_MAX);
	const float YARD_INCREMENT = PIXELS_PER_YARD * DAMAGE_OVER_RANGE_GRAPH_YARD_INTERVAL;
	for (int i = 0; i < DAMAGE_OVER_RANGE_GRAPH_NUM_YARD_INTERVALS; i++) {
		int x = xbase + YARD_INCREMENT + YARD_INCREMENT * i;
		surface()->DrawLine(x, ybase - height, x, ybase);
	}

	for (int i = 0; i < m_weapons.Count(); i++) {
		const CWeaponDef* w = m_weapons.Element(i);
		surface()->DrawSetColor(w->m_graphColor);

		float xinterval = PIXELS_PER_YARD;
		float linex = xbase;
		float nextlinex = xbase + xinterval;

		for (int j = 0; j < DAMAGE_OVER_RANGE_GRAPH_RANGE_MAX - 1; j++) {
			int damagey = damageToYOffset(w->m_aSimulatedDamageData[j]);
			int nextdamagey = damageToYOffset(w->m_aSimulatedDamageData[j + 1]);
			surface()->DrawLine(linex, ybase - damagey, nextlinex, ybase - nextdamagey);
			//surface()->DrawLine(linex, j, nextlinex, j + 1);
			//surface()->DrawFilledRect(linex, ybase - damagey, linex + 1, ybase - damagey + 1);
			linex = nextlinex;
			nextlinex += xinterval;
		}


	}
}

void CDamageOverRangeMenu::PaintGraphKey() {

}

void CDamageOverRangeMenu::ResetWeapons() {
	/*m_weapons.RemoveAll();
	m_weapons.AddToTail(CWeaponDef::GetDefForWeapon("weapon_brownbess"));
	m_weapons.AddToTail(CWeaponDef::GetDefForWeapon("weapon_light_model_charleville"));
	m_weapons.AddToTail(CWeaponDef::GetDefForWeapon("weapon_oldpattern"));*/
	m_pWeaponSelectionPanel->GetSelectedWeapons(m_weapons);
}

void CDamageOverRangeMenu::PerformLayout() {
	float xscale = GetWide() / 1024.f;
	float yscale = GetTall() / 768.f;
	int ybase = 700 * yscale;
	int xbase = 250 * xscale;
	int width = 700 * xscale;

	m_pFiftyDamageLabel->SetSize(50, 50);
	m_pHundredDamageLabel->SetSize(100, 100);
	m_pFiftyDamageLabel->SetPos(xbase - 50, ybase - ONE_HUNDRED_DAMAGE_OFFSET/2 * yscale - 25);
	m_pHundredDamageLabel->SetPos(xbase - 100, ybase - ONE_HUNDRED_DAMAGE_OFFSET * yscale - 50);

	m_pMenuTitle->SetSize(ScreenWidth() - 20, 100 * yscale);
	m_pMenuTitle->SetPos(10, 0);

	m_pSelectWeaponsLabel->SetSize(240 * xscale, 60 * yscale);
	m_pSelectWeaponsLabel->SetPos(10, 100 * yscale);

	m_pWeaponSelectionPanel->SetSize(150 * xscale, GetTall() - (100 * yscale + 10 + 40 * yscale));
	m_pWeaponSelectionPanel->SetPos(0, 100 * yscale + 10 + 40 * yscale);
	m_pWeaponSelectionPanel->PerformLayout();

	//yard range labels
	const float PIXELS_PER_YARD = (1.f * width / DAMAGE_OVER_RANGE_GRAPH_RANGE_MAX);
	const float YARD_INCREMENT = PIXELS_PER_YARD * DAMAGE_OVER_RANGE_GRAPH_YARD_INTERVAL;
	for (int i = 0; i < DAMAGE_OVER_RANGE_GRAPH_NUM_YARD_INTERVALS; i++) {
		m_ppRangeLabels[i]->SetSize(70 * xscale, 50 * yscale);
		m_ppRangeLabels[i]->SetPos(xbase + (i + .5f) * YARD_INCREMENT, ybase);
	}
}

void CDamageOverRangeMenu::SimulateTrajectory(const CWeaponDef* pWeapon) {
	//don't do more than once for each weapon
	if (pWeapon->m_bDataSimulated)
		return;

	int16* damageOutput = pWeapon->m_aSimulatedDamageData;
	float* dropOutput = pWeapon->m_aSimulatedDropData;

	//initialize memory
	for (int i = 0; i < DAMAGE_OVER_RANGE_GRAPH_RANGE_MAX; i++) {
		damageOutput[i] = -1;
		dropOutput[i] = 0.f;
	}

	Vector2D location(0, 0);
	Vector2D velocity = Vector2D(pWeapon->m_flMuzzleVelocity, 0);
	float muzzleVelocity = pWeapon->m_flMuzzleVelocity;
	int baseDamage = pWeapon->m_Attackinfos[0].m_iDamage;
	float constantDamageRange = pWeapon->m_Attackinfos[0].m_flConstantDamageRange;
	float drag = pWeapon->m_Attackinfos[0].m_flRelativeDrag * sv_simulatedbullets_drag.GetFloat();
	float damageDropoffMultiplier = pWeapon->m_flDamageDropoffMultiplier;
	float gravity = sv_gravity.GetFloat() * sv_simulatedbullets_gravity.GetFloat();
	float dt = DAMAGE_OVER_RANGE_GRAPH_TIME_INTERVAL;

	//copied from bullet code
	auto updateVelocity = [&]() {
		Vector2D vecDir = velocity;
		float speed = vecDir.NormalizeInPlace();
		speed -= drag * speed * speed * dt;
		velocity = vecDir * speed;
		velocity.y -= gravity * dt;
	};

	bool bContinueSimulation = true;
	//float maxTime = 3.f;
	//float accumulatedTime = 0.f;
	while (bContinueSimulation) {
		//check if we need results for current range
		int index = (int)(location.x / 36);
		index = Clamp(index, 0, DAMAGE_OVER_RANGE_GRAPH_RANGE_MAX - 1);

		//determine current results when necessary
		if (damageOutput[index] == -1) {
			dropOutput[index] = location.x;
			float spd = velocity.Length();
			int dmg;
			if (location.Length() < constantDamageRange)
				dmg = baseDamage;
			else {
				dmg = (int)(baseDamage * spd * spd / (muzzleVelocity*muzzleVelocity));
				dmg = baseDamage - (baseDamage - dmg) * damageDropoffMultiplier; //scale damage dropof\f
			}
			damageOutput[index] = dmg;
		}

		//update velocity and position
		updateVelocity();
		location += velocity * dt;
		//accumulatedTime += dt;

		bContinueSimulation = index < DAMAGE_OVER_RANGE_GRAPH_RANGE_MAX - 1;
	}

	pWeapon->m_bDataSimulated = true;
}