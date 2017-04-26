//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef CLASSMENU_H
#define CLASSMENU_H
#ifdef _WIN32
#pragma once
#endif

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

#include <game/client/iviewport.h>

#include "hl2mp_gamerules.h" // BG2 - VisualMelon - moved from classmenu.cpp so we can get the ammo counts

class KeyValues;

namespace vgui
{
	class TextEntry;
	class Button;
	class Panel;
	class ImagePanel;
	class ComboBox;
}

#define BLACK_BAR_COLOR	Color(0, 0, 0, 196)

class IBaseFileSystem;

//-----------------------------------------------------------------------------
// Purpose: the bottom bar panel, this is a seperate panel because it
// wants mouse input and the main window doesn't
//----------------------------------------------------------------------------
class CClassButton : public vgui::Button
{
public:
	DECLARE_CLASS_SIMPLE( CClassButton, vgui::Button);

	CClassButton(Panel *parent, const char *panelName, const char *text) : Button( parent, panelName, text ) { m_iCommand = 0; }

	void SetCommand( int command );
	void OnMousePressed(vgui::MouseCode code);
	void OnCursorEntered( void );
	void OnCursorExited( void ){ m_bMouseOver = false; };
	void PerformCommand( void );
	//Added functionality for images as buttons. -HairyPotter
	virtual void Paint();
	virtual void ApplySchemeSettings( vgui::IScheme *pScheme );
	virtual void ApplySettings( KeyValues *inResourceData )
	{
		Q_strncpy( BritishImage, (const char *)inResourceData->GetString( "BritishImage", "" ), 80 );
		Q_strncpy( AmericanImage, (const char *)inResourceData->GetString( "AmericanImage", "" ), 80 );

		Q_strncpy( BritishMouseoverImage, (const char *)inResourceData->GetString( "BritishMouseoverImage", "" ), 80 );
		Q_strncpy( AmericanMouseoverImage, (const char *)inResourceData->GetString( "AmericanMouseoverImage", "" ), 80 );
		
		BaseClass::ApplySettings( inResourceData );
	}
	//

private:
	int m_iCommand;

	char BritishImage[80],
		 AmericanImage[80],
		 BritishMouseoverImage[80],
		 AmericanMouseoverImage[80];

	bool m_bMouseOver;
};

class CTeamButton : public vgui::Button
{
public:
	DECLARE_CLASS_SIMPLE( CTeamButton, vgui::Button);

	CTeamButton(Panel *parent, const char *panelName, const char *text) : Button( parent, panelName, text ) { m_iCommand = 0; }

	void SetCommand( int command );
	void OnMousePressed(vgui::MouseCode code);
	void OnCursorEntered( void );
	void OnCursorExited( void ){ m_bMouseOver = false; };
	void PerformCommand( void );
	//Added functionality for images as buttons. -HairyPotter
	virtual void ApplySchemeSettings( vgui::IScheme *pScheme );
	virtual void Paint( void );
	virtual void ApplySettings( KeyValues *inResourceData )
	{
		Q_strncpy( TeamImage, (const char *)inResourceData->GetString( "Image", "" ), 80 );
		Q_strncpy( TeamMouseoverImage, (const char *)inResourceData->GetString( "MouseoverImage", "" ), 80 );

		BaseClass::ApplySettings( inResourceData );
	}
	//

private:
	int m_iCommand;

	char TeamImage[80],
		 TeamMouseoverImage[80];

	bool m_bMouseOver;

};

class COkayButton : public vgui::Button
{
public:
	DECLARE_CLASS_SIMPLE( COkayButton, vgui::Button );

	COkayButton(Panel *parent, const char *panelName, const char *text) : Button( parent, panelName, text ) { }

	void SetCommand( int command ){}
	void OnMousePressed(vgui::MouseCode code);
	virtual void ApplySchemeSettings( vgui::IScheme *pScheme );
	void OnCursorEntered( void ){}
	void PerformCommand( void );
};

// BG2 - VisualMelon - hacky little class to make togglebuttons behave
class CLazyToggleButton : public vgui::ToggleButton
{
protected:
	bool acceptClick;

public:
	void DoClick()
	{
		if (acceptClick)
			ToggleButton::DoClick();
		acceptClick = false;
	}

	CLazyToggleButton(Panel *parent, const char *panelName, const char *text) : ToggleButton( parent, panelName, text ) {  }
};

// only giving the weapon buttons the special lazy treatment
class CWeaponButton : public CLazyToggleButton
{
public:
	DECLARE_CLASS_SIMPLE( CWeaponButton, vgui::ToggleButton );

	CWeaponButton(Panel *parent, const char *panelName, const char *text) : CLazyToggleButton( parent, panelName, text ) {  }

	void OnMousePressed(vgui::MouseCode code);
	void OnCursorEntered( void );
	void OnCursorExited( void ){ m_bMouseOver = false; };
	virtual void ApplySchemeSettings(vgui::IScheme *pScheme);
	virtual void Paint( void );
	void GetWeaponImages( char *m_szActiveImage, char *m_szInactiveImage );
	virtual void ApplySettings( KeyValues *inResourceData )
	{
		Q_strncpy( AInf1, (const char *)inResourceData->GetString( "AInfImage1", "" ), 80 );
		Q_strncpy( AInf2, (const char *)inResourceData->GetString( "AInfImage2", "" ), 80 );
		Q_strncpy( AInf3, (const char *)inResourceData->GetString( "AInfImage3", "" ), 80 );
		Q_strncpy( BInf1, (const char *)inResourceData->GetString( "BInfImage1", "" ), 80 );
		Q_strncpy( BInf2, (const char *)inResourceData->GetString( "BInfImage2", "" ), 80 );
		Q_strncpy( ASki1, (const char *)inResourceData->GetString( "ASkiImage1", "" ), 80 );
		Q_strncpy( ASki2, (const char *)inResourceData->GetString( "ASkiImage2", "" ), 80 );
		Q_strncpy( BSki1, (const char *)inResourceData->GetString( "BSkiImage1", "" ), 80 );
		Q_strncpy( BSki2, (const char *)inResourceData->GetString( "BSkiImage2", "" ), 80 );
		Q_strncpy( BLight1, (const char *)inResourceData->GetString( "BLightImage1", "" ), 80 );
		Q_strncpy( AOff1, (const char *)inResourceData->GetString( "AOffImage1", "" ), 80 );
		Q_strncpy( BOff1, (const char *)inResourceData->GetString( "BOffImage1", "" ), 80 );

		//Mouse Overs
		Q_strncpy( AInf1M, (const char *)inResourceData->GetString( "AInfMouseImage1", "" ), 80 );
		Q_strncpy( AInf2M, (const char *)inResourceData->GetString( "AInfMouseImage2", "" ), 80 );
		Q_strncpy( AInf3M, (const char *)inResourceData->GetString( "AInfMouseImage3", "" ), 80 );
		Q_strncpy( BInf1M, (const char *)inResourceData->GetString( "BInfMouseImage1", "" ), 80 );
		Q_strncpy( BInf2M, (const char *)inResourceData->GetString( "BInfMouseImage2", "" ), 80 );
		Q_strncpy( ASki1M, (const char *)inResourceData->GetString( "ASkiMouseImage1", "" ), 80 );
		Q_strncpy( ASki2M, (const char *)inResourceData->GetString( "ASkiMouseImage2", "" ), 80 );
		Q_strncpy( BSki1M, (const char *)inResourceData->GetString( "BSkiMouseImage1", "" ), 80 );
		Q_strncpy( BSki2M, (const char *)inResourceData->GetString( "BSkiMouseImage2", "" ), 80 );
		Q_strncpy( BLight1M, (const char *)inResourceData->GetString( "BLightMouseImage1", "" ), 80 );
		Q_strncpy( AOff1M, (const char *)inResourceData->GetString( "AOffMouseImage1", "" ), 80 );
		Q_strncpy( BOff1M, (const char *)inResourceData->GetString( "BOffMouseImage1", "" ), 80 );
		//

		BaseClass::ApplySettings( inResourceData );
	}

	char BLight1[80], BInf1[80], BInf2[80], AInf1[80], AInf2[80], AInf3[80], ASki1[80], ASki2[80],
		 BSki1[80], BSki2[80], BLight1M[80], BInf1M[80], BInf2M[80], AInf1M[80], AInf2M[80], AInf3M[80], ASki1M[80], ASki2M[80],
		 BSki1M[80], BSki2M[80], AOff1[80], BOff1[80], AOff1M[80], BOff1M[80];

	bool m_bMouseOver;
};

class CAmmoButton : public vgui::ToggleButton
{
public:
	DECLARE_CLASS_SIMPLE( CAmmoButton, vgui::ToggleButton );

	CAmmoButton(Panel *parent, const char *panelName, const char *text) : ToggleButton( parent, panelName, text ) {  }

	void OnMousePressed(vgui::MouseCode code);
	void OnCursorEntered( void );
	void OnCursorExited( void ){ m_bMouseOver = false; };
	virtual void ApplySchemeSettings(vgui::IScheme *pScheme);
	virtual void Paint( void );
	virtual void ApplySettings( KeyValues *inResourceData )
	{
		Q_strncpy( Image, (const char *)inResourceData->GetString( "Image", "" ), 80 );
		Q_strncpy( MouseoverImage, (const char *)inResourceData->GetString( "MouseoverImage", "" ), 80 );
		Q_strncpy( RestrictedImage, (const char *)inResourceData->GetString( "RestrictedImage", "" ), 80 );

		BaseClass::ApplySettings( inResourceData );
	}

	char Image[80],
		MouseoverImage[80],
		RestrictedImage[80];

	bool m_bMouseOver, m_bRestricted;
};

// basicly the same as CAmmoButton
class CSkinButton : public vgui::ToggleButton
{
	DECLARE_CLASS_SIMPLE( CSkinButton, vgui::ToggleButton );

	CSkinButton(Panel *parent, const char *panelName, const char *text) : ToggleButton( parent, panelName, text ) {  }

	void OnMousePressed(vgui::MouseCode code);
	void OnCursorEntered( void );
	void OnCursorExited( void ){ m_bMouseOver = false; };
	virtual void ApplySchemeSettings(vgui::IScheme *pScheme);
	virtual void Paint( void );
	virtual void ApplySettings( KeyValues *inResourceData )
	{
		Q_strncpy( ImageBrit_Inf, (const char *)inResourceData->GetString( "ImageBrit_Inf", "" ), 80 );
		Q_strncpy( MouseoverImageBrit_Inf, (const char *)inResourceData->GetString( "MouseoverImageBrit_Inf", "" ), 80 );
		Q_strncpy( ImageAmer_Inf, (const char *)inResourceData->GetString( "ImageAmer_Inf", "" ), 80 );
		Q_strncpy( MouseoverImageAmer_Inf, (const char *)inResourceData->GetString( "MouseoverImageAmer_Inf", "" ), 80 );
		Q_strncpy( ImageBrit_Off, (const char *)inResourceData->GetString( "ImageBrit_Off", "" ), 80 );
		Q_strncpy( MouseoverImageBrit_Off, (const char *)inResourceData->GetString( "MouseoverImageBrit_Off", "" ), 80 );
		Q_strncpy( ImageAmer_Off, (const char *)inResourceData->GetString( "ImageAmer_Off", "" ), 80 );
		Q_strncpy( MouseoverImageAmer_Off, (const char *)inResourceData->GetString( "MouseoverImageAmer_Off", "" ), 80 );

		BaseClass::ApplySettings( inResourceData );
	}

		char ImageBrit_Inf[80],
		ImageAmer_Inf[80],
		MouseoverImageBrit_Inf[80],
		MouseoverImageAmer_Inf[80],
		ImageBrit_Off[80],
		ImageAmer_Off[80],
		MouseoverImageBrit_Off[80],
		MouseoverImageAmer_Off[80];

	bool m_bMouseOver;
};

// BG2 - VisualMelon
class weaponStat
{
public:
	char img[80];
	int count; // BG2 - VisualMelon - find the count in hl2mp_player.cpp (ever so slightly hard coded)
};

class CStatsButton : public vgui::ToggleButton
{
	DECLARE_CLASS_SIMPLE( CStatsButton, vgui::ToggleButton );

	CStatsButton(Panel *parent, const char *panelName, const char *text) : ToggleButton( parent, panelName, text ) {  }

	void OnMousePressed(vgui::MouseCode code);
	void OnCursorEntered( void ) {}; // don't really need these
	void OnCursorExited( void ) {};
	void GetWeaponImage( weaponStat wepStat );
	virtual void ApplySchemeSettings(vgui::IScheme *pScheme);
	virtual void Paint( void );
	virtual void ApplySettings( KeyValues *inResourceData )
	{
		// BG2 - VisualMelon - stolen from CWeaponButton for good reason
		Q_strncpy( AInf1.img, (const char *)inResourceData->GetString( "AInfImage1", "" ), 80 );
		Q_strncpy( AInf2.img, (const char *)inResourceData->GetString( "AInfImage2", "" ), 80 );
		/*Q_strncpy( AInf3.img, (const char *)inResourceData->GetString( "AInfImage3", "" ), 80 );*/
		Q_strncpy( BInf1.img, (const char *)inResourceData->GetString( "BInfImage1", "" ), 80 );
		Q_strncpy( BInf2.img, (const char *)inResourceData->GetString( "BInfImage2", "" ), 80 );
		Q_strncpy( ASki1.img, (const char *)inResourceData->GetString( "ASkiImage1", "" ), 80 );
		Q_strncpy( ASki1_shot.img, (const char *)inResourceData->GetString( "ASkiImage1_shot", "" ), 80 );
		Q_strncpy( ASki2.img, (const char *)inResourceData->GetString( "ASkiImage2", "" ), 80 );
		Q_strncpy( BSki1.img, (const char *)inResourceData->GetString( "BSkiImage1", "" ), 80 );
		Q_strncpy( BSki2.img, (const char *)inResourceData->GetString( "BSkiImage2", "" ), 80 );
		Q_strncpy( BLight1.img, (const char *)inResourceData->GetString( "BLightImage1", "" ), 80 );
		Q_strncpy( BLight1_shot.img, (const char *)inResourceData->GetString( "BLightImage1_shot", "" ), 80 );
		Q_strncpy( AOff1.img, (const char *)inResourceData->GetString( "AOffImage1", "" ), 80 );
		Q_strncpy( BOff1.img, (const char *)inResourceData->GetString( "BOffImage1", "" ), 80 );

		AInf1.count = HL2MPRules()->getDefaultInfantryAmmo();
		AInf2.count = AInf1.count;
		/*AInf3.count = AInf1.count;*/
		BInf1.count = AInf1.count;
		BInf2.count = AInf1.count;

		ASki1.count = HL2MPRules()->getDefaultSkirmisherAmmo();
		ASki1_shot.count = ASki1.count;
		ASki2.count = ASki1.count;
		BSki1.count = ASki1.count;
		BSki2.count = ASki1.count;

		BLight1.count = HL2MPRules()->getDefaultLightInfantryAmmo();
		BLight1_shot.count = BLight1.count;

		AOff1.count = HL2MPRules()->getDefaultOfficerAmmo();
		BOff1.count = AOff1.count;
		
		BaseClass::ApplySettings( inResourceData );
	}

	// BG2 - VisualMelon - these are stolen from CWeaponButton also
	weaponStat BLight1, BInf1, BInf2, AInf1, AInf2, /*AInf3,*/ ASki1, ASki2, BSki1, BSki2, AOff1, BOff1;
	// BG2 - VisualMelon - additional ones for weapons with shot option
	weaponStat BLight1_shot, ASki1_shot;
};


class CClassMenu : public vgui::Frame, public IViewPortPanel
{
	DECLARE_CLASS_SIMPLE( CClassMenu, vgui::Frame );

public:
	CClassMenu( IViewPort *pViewPort );
	~CClassMenu();

	virtual const char *GetName( void ) { return PANEL_CLASSES; }
	virtual void SetData(KeyValues *data){}
	bool SetScreen( int m_iScreen, bool m_bVisible, bool m_bUpdate = true );
	void SetDefaultWeaponKit( int m_iTeam, int m_iClass, int weapon, int ammo, int skin );
	void UpdateDefaultWeaponKit( int m_iTeam, int m_iClass );
	virtual void Reset( void ) { }//m_pPlayerList->DeleteAllItems(); }
	virtual void Update( void );
	virtual bool NeedsUpdate( void ) { return false; }
	virtual bool HasInputElements( void ) { return true; }
	virtual void ShowPanel( bool bShow );
	virtual void Paint( void );


	// both vgui::Frame and IViewPortPanel define these, so explicitly define them here as passthroughs to vgui
	virtual bool IsVisible() { return BaseClass::IsVisible(); }
	vgui::VPANEL GetVPanel( void ) { return BaseClass::GetVPanel(); }
	virtual void SetParent(vgui::VPANEL parent) { BaseClass::SetParent(parent); }

	CClassButton	*m_pInfantryButton,
					*m_pOfficerButton,
					*m_pSkirmisherButton,
					*m_pSniperButton,
					*m_pLightInfantryButton,
					*m_pGrenadierButton;

	CTeamButton		*m_pBritishButton,
					*m_pAmericanButton,
					*m_pSpectateButton,
					*m_pAutoassignButton;

	CWeaponButton	*m_pWeaponButton1,
					*m_pWeaponButton2,
					*m_pWeaponButton3;

	CStatsButton	*m_pStatsButton;

	CAmmoButton		*m_pAmmoButton1,
					*m_pAmmoButton2;

	CSkinButton		*m_pSkinButton1,
					*m_pSkinButton2,
					*m_pSkinButton3;

	vgui::ImagePanel *m_pWeaponSelection,
					 *m_pAmmoSelection;

	vgui::Label *m_pBritishLabel,
				*m_pAmericanLabel,
				*m_pInfantryLabel,
				*m_pOfficerLabel,
				*m_pRiflemanLabel,
				*m_pSkirmisherLabel,
				*m_pLightInfantryLabel,
				*m_pGrenadierLabel;

	vgui::Label *m_pAmmoCount; // BG2 - VisualMelon - Label to display the number of "cartridges" for the weapon

	vgui::CheckButton		*m_pQuickJoinCheckButton;

	COkayButton		*m_pOK;

	vgui::HTML		*m_pInfoHTML;

	int m_iTeamSelection,
		m_iClassSelection;

	void OnThink();
	void UpdateClassLabelText( vgui::Label *pLabel, int iClass);
//	void UpdateWeaponButtonImages( void ); // BG2 - VisualMelon - this doesn't exist
	void UpdateAmmoButtons( void );
	void UpdateSkinButtons( void );
	void ToggleButtons(int iShowScreen);

	//being in classmenu mode means we must be visible aswell
	bool IsInClassMenu( void ) { return m_pInfantryButton->IsVisible() && IsVisible(); }
	bool IsInTeamMenu( void ) { return m_pBritishButton->IsVisible() && IsVisible(); }
	bool IsInWeaponMenu( void ) { return m_pAmmoButton1->IsVisible() && IsVisible(); }

	void ShowFile( const char *filename );

	//Used for gathering kit info from cvars. -HairyPotter
	struct
	{
		int m_iGun,
			m_iAmmo,
			m_iClassSkin;
	} DefaultKit;
	//

private:
	// VGUI2 overrides
	virtual void OnKeyCodePressed(vgui::KeyCode code);
	virtual void ApplySchemeSettings(vgui::IScheme *pScheme);
	virtual void PerformLayout();

	void SetViewModeText( const char *text ) { }//m_pViewOptions->SetText( text ); }

	void SetPlayerFgColor( Color c1 ) { }//m_pPlayerList->SetFgColor(c1); }

	IViewPort *m_pViewPort;

	int				m_iInfantryKey,
					m_iOfficerKey,
					m_iSniperKey,
					m_iSkirmisherKey,
					m_iLightInfantryKey,
					m_iGrenadierKey,
					m_iSlot6Key,
					m_iOkayKey;

	CClassButton	*m_pCancelButton;

	int				m_iCancelKey,

					teammenu,
					classmenu,
					weaponmenu,
					commmenu,
					commmenu2;
public:
};

extern void PlaySound( const char *m_szSound );

#endif // CLASSMENU_H
