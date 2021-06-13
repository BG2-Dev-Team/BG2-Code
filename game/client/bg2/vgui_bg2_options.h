#include "KeyValues.h"
#include "FileSystem.h"
#include <vgui_controls/Frame.h>
#include <vgui_controls/Slider.h>
#include <vgui_controls/CheckButton.h>
#include "BG3/vgui/bg3_fonts.h"
#include <chrono>

extern ConVar	cl_crosshair,

				cl_crosshair_r,
				cl_crosshair_g,
				cl_crosshair_b,
				cl_crosshair_a,
				cl_crosshair_texture_index,

				cl_dynamic_crosshair,

				//cl_simple_smoke,
				cl_capturesounds,
				hud_showtargetid,
				cl_vcommsounds,

				cl_flagstatusdetail,

				cl_hitverif,
				cl_hitdamageline,
				cl_winmusic,
				hud_takesshots;


class CBG2OptionsPanel : public vgui::Frame
{
	DECLARE_CLASS_SIMPLE( CBG2OptionsPanel, vgui::Frame );

	//BG2 - Tjoppen - don't want any virtual crap.. everything in the class. thus the reason for not using
	//				the MESSAGE_FUNC_PARAMS macro
	_MessageFuncCommon( Command, "Command", 1, vgui::DATATYPE_KEYVALUES, NULL, 0, 0 );

	/* Command
		a command was sent from a button. if it's "close", set cvars
		also send command to baseclass
	*/

	void Command( KeyValues *data )
	{
		const char *command = data->GetString( "command" );

		if( stricmp( command, "close" ) == 0 )
		{
			//closing.. set cvars
			cl_crosshair_r.SetValue( m_pRedCrosshairSlider->GetValue() );
			cl_crosshair_g.SetValue( m_pGreenCrosshairSlider->GetValue() );
			cl_crosshair_b.SetValue( m_pBlueCrosshairSlider->GetValue() );
			cl_crosshair_a.SetValue( m_pAlphaCrosshairSlider->GetValue() );
			cl_crosshair_texture_index.SetValue(m_pCrosshairTextureSlider->GetValue());

			//cl_simple_smoke.SetValue( m_pSimpleSmokeCheckButton->IsSelected() );
			cl_flagstatusdetail.SetValue( m_pSimpleFlagHUDCheckButton->IsSelected() ? "1" : "2" );

			cl_crosshair.SetValue(	(m_pFlag0CrosshairCheckButton->IsSelected() ? 1 : 0) |
									(m_pFlag1CrosshairCheckButton->IsSelected() ? 2 : 0) |
									(m_pFlag2CrosshairCheckButton->IsSelected() ? 4 : 0) |
									(m_pFlag3CrosshairCheckButton->IsSelected() ? 8 : 0) );

			cl_dynamic_crosshair.SetValue(m_pDynamicCrosshairCheckButton->IsSelected());
			extern ConVar r_drawviewmodel;
			r_drawviewmodel.SetValue(!m_pHideViewmodelCheckButton->IsSelected());
			extern ConVar cl_hide_expbottle;
			cl_hide_expbottle.SetValue(m_pHideExpBottleCheckButton->IsSelected());

			if (m_pHitverifCheckButton->IsEnabled()) // BG2 - VisualMelon - So we don't keep resetting it if it's disabled
				cl_hitverif.SetValue( m_pHitverifCheckButton->IsSelected() );
			if (m_pHitdamagelineCheckButton->IsEnabled())
				cl_hitdamageline.SetValue( m_pHitdamagelineCheckButton->IsSelected() );
			cl_winmusic.SetValue( m_pWinmusicCheckButton->IsSelected() );
			cl_capturesounds.SetValue( m_pCaptureSoundsCheckButton->IsSelected() );
			cl_vcommsounds.SetValue( m_pVCommSoundsCheckButton->IsSelected() );
			hud_takesshots.SetValue( m_pScreenShotCheckButton->IsSelected() );
			hud_showtargetid.SetValue( m_pShowNamesCheckButton->IsSelected() );
		}

		BaseClass::OnCommand( command );
	}

	/* SetAdjustors
		sets the various sliders and checkboxes to whatever value they should be
	*/
	void SetAdjustors( void )
	{
		m_pRedCrosshairSlider->SetRange( 0, 255 );
		m_pRedCrosshairSlider->SetValue( cl_crosshair_r.GetInt() );

		m_pGreenCrosshairSlider->SetRange( 0, 255 );
		m_pGreenCrosshairSlider->SetValue( cl_crosshair_g.GetInt() );
		
		m_pBlueCrosshairSlider->SetRange( 0, 255 );
		m_pBlueCrosshairSlider->SetValue( cl_crosshair_b.GetInt() );
		
		m_pAlphaCrosshairSlider->SetRange( 0, 255 );
		m_pAlphaCrosshairSlider->SetValue( cl_crosshair_a.GetInt() );

		m_pCrosshairTextureSlider->SetRange(0, 9);
		m_pCrosshairTextureSlider->SetValue(cl_crosshair_texture_index.GetInt());
		m_iCrosshairImageIndex = cl_crosshair_texture_index.GetInt();
		
		//m_pSimpleSmokeCheckButton->SetSelected( cl_simple_smoke.GetBool() );
		m_pSimpleFlagHUDCheckButton->SetSelected( cl_flagstatusdetail.GetInt() == 1 ? true : false );
		
		m_pFlag0CrosshairCheckButton->SetSelected( (cl_crosshair.GetInt() & 1) ? true : false );
		m_pFlag1CrosshairCheckButton->SetSelected( (cl_crosshair.GetInt() & 2) ? true : false );
		m_pFlag2CrosshairCheckButton->SetSelected( (cl_crosshair.GetInt() & 4) ? true : false );
		m_pFlag3CrosshairCheckButton->SetSelected( (cl_crosshair.GetInt() & 8) ? true : false );

		m_pDynamicCrosshairCheckButton->SetSelected(cl_dynamic_crosshair.GetBool());
		extern ConVar r_drawviewmodel;
		m_pHideViewmodelCheckButton->SetSelected(!r_drawviewmodel.GetBool());
		extern ConVar cl_hide_expbottle;
		m_pHideExpBottleCheckButton->SetSelected(cl_hide_expbottle.GetBool());

		m_pHitverifCheckButton->SetSelected( cl_hitverif.GetBool() );
		m_pHitdamagelineCheckButton->SetSelected( cl_hitdamageline.GetBool() );
		m_pWinmusicCheckButton->SetSelected( cl_winmusic.GetBool() );
		m_pCaptureSoundsCheckButton->SetSelected( cl_capturesounds.GetBool() );
		m_pVCommSoundsCheckButton->SetSelected( cl_vcommsounds.GetBool() );
		m_pScreenShotCheckButton->SetSelected( hud_takesshots.GetBool() );
		m_pShowNamesCheckButton->SetSelected( hud_showtargetid.GetBool() );
	}

	/* CBG2OptionsPanel
		constructor..
	*/
	CBG2OptionsPanel( vgui::VPANEL parent ) : BaseClass( NULL, "BG2OptionsPanel" )
	{
		SetParent( parent );

		vgui::HScheme scheme = vgui::scheme()->LoadSchemeFromFile( "resource/SourceScheme.res", "SourceScheme" );
		SetScheme( scheme );
		
		SetVisible( false );

		m_pCurrentCrosshairImage = NULL;

		m_pRedCrosshairSlider = new vgui::Slider( this, "RedCrosshairSlider" );
		m_pRedCrosshairSlider->AddActionSignalTarget( this );

		m_pGreenCrosshairSlider = new vgui::Slider( this, "GreenCrosshairSlider" );
		m_pGreenCrosshairSlider->AddActionSignalTarget( this );

		m_pBlueCrosshairSlider = new vgui::Slider( this, "BlueCrosshairSlider" );
		m_pBlueCrosshairSlider->AddActionSignalTarget( this );

		m_pAlphaCrosshairSlider = new vgui::Slider( this, "AlphaCrosshairSlider" );
		m_pAlphaCrosshairSlider->AddActionSignalTarget( this );

		m_pCrosshairTextureSlider = new vgui::Slider(this, "CrosshairTextureSlider");
		m_pCrosshairTextureSlider->AddActionSignalTarget(this);

		m_pSimpleSmokeCheckButton = new vgui::CheckButton( this, "SimpleSmokeCheckButton", "" );
		m_pSimpleSmokeCheckButton->AddActionSignalTarget( this );

		m_pSimpleFlagHUDCheckButton = new vgui::CheckButton( this, "SimpleFlagHUDCheckButton", "" );
		m_pSimpleFlagHUDCheckButton->AddActionSignalTarget( this );

		m_pFlag0CrosshairCheckButton = new vgui::CheckButton( this, "Flag0CrosshairCheckButton", "" );
		m_pFlag0CrosshairCheckButton->AddActionSignalTarget( this );

		m_pFlag1CrosshairCheckButton = new vgui::CheckButton( this, "Flag1CrosshairCheckButton", "" );
		m_pFlag1CrosshairCheckButton->AddActionSignalTarget( this );

		m_pFlag2CrosshairCheckButton = new vgui::CheckButton( this, "Flag2CrosshairCheckButton", "" );
		m_pFlag2CrosshairCheckButton->AddActionSignalTarget( this );

		m_pFlag3CrosshairCheckButton = new vgui::CheckButton( this, "Flag3CrosshairCheckButton", "" );
		m_pFlag3CrosshairCheckButton->AddActionSignalTarget( this );

		m_pDynamicCrosshairCheckButton = new vgui::CheckButton(this, "DynamicCrosshairCheckButton", "");
		m_pDynamicCrosshairCheckButton->AddActionSignalTarget(this);

		m_pHideViewmodelCheckButton = new vgui::CheckButton(this, "HideViewmodelCheckButton", "");
		m_pHideViewmodelCheckButton->AddActionSignalTarget(this);

		m_pHideExpBottleCheckButton = new vgui::CheckButton(this, "HideExpBottleCheckButton", "");
		m_pHideExpBottleCheckButton->AddActionSignalTarget(this);

		m_pHitverifCheckButton = new vgui::CheckButton( this, "HitverifCheckButton", "" );
		m_pHitverifCheckButton->AddActionSignalTarget( this );

		m_pHitdamagelineCheckButton = new vgui::CheckButton( this, "HitdamagelineCheckButton", "" );
		m_pHitdamagelineCheckButton->AddActionSignalTarget( this ); // BG2 - VisualMelon

		m_pWinmusicCheckButton = new vgui::CheckButton( this, "WinmusicCheckButton", "" );
		m_pWinmusicCheckButton->AddActionSignalTarget( this );

		m_pCaptureSoundsCheckButton = new vgui::CheckButton( this, "CaptureSoundsCheckButton", "" );
		m_pCaptureSoundsCheckButton->AddActionSignalTarget( this );

		m_pVCommSoundsCheckButton = new vgui::CheckButton( this, "VCommSoundsCheckButton", "" );
		m_pVCommSoundsCheckButton->AddActionSignalTarget( this );

		m_pScreenShotCheckButton = new vgui::CheckButton( this, "ScreenShotCheckButton", "" );
		m_pScreenShotCheckButton->AddActionSignalTarget( this );

		m_pShowNamesCheckButton = new vgui::CheckButton( this, "ShowNamesCheckButton", "" );
		m_pShowNamesCheckButton->AddActionSignalTarget( this );

		//set sliders and checkboxes correctly
		SetAdjustors();

		LoadControlSettings( "resource/ui/BG2OptionsPanel.res" );
	}

	/* OnSetFocus
		called whenever this menu is brough up. makes sure all sliders and checkboxes are valid
	*/
	void OnSetFocus( void )
	{
		//when we're visible, make sure sliders and checkboxes are set correctly
		SetAdjustors();
	}

	void Paint() override {
		BaseClass::Paint();

		using namespace vgui;

		bool bTexture = m_pFlag3CrosshairCheckButton->IsSelected();
		bool bCircle = m_pFlag0CrosshairCheckButton->IsSelected();
		bool bThreeLines = m_pFlag1CrosshairCheckButton->IsSelected();
		bool bDot = m_pFlag2CrosshairCheckButton->IsSelected();
		bool dynamic = m_pDynamicCrosshairCheckButton->IsSelected();

		int cx = 232;
		int cy = 176;

		float radius = 32.f;
		if (dynamic) {
			//using namespace std::chrono;
			//milliseconds ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
			float _time = gpGlobals->curtime; //(uint64)ms.count() + reinterpret_cast<uint64>(&ms) / 1000.f;
			radius -= (sinf(_time) + 1) * 8;
		}
		
		//texture
		if (bTexture) {
			if (!m_pCurrentCrosshairImage || m_iCrosshairImageIndex != m_pCrosshairTextureSlider->GetValue()) {
				m_iCrosshairImageIndex = m_pCrosshairTextureSlider->GetValue();
				char buffer[64];
				Q_snprintf(buffer, sizeof(buffer), "../hud/crosshair%i", m_iCrosshairImageIndex);
				m_pCurrentCrosshairImage = scheme()->GetImage(buffer, false);
			}
			m_pCurrentCrosshairImage->SetPos(cx - radius, cy - radius);
			m_pCurrentCrosshairImage->SetSize(radius * 2 + 1, radius * 2 + 1);

			m_pCurrentCrosshairImage->Paint();
		}

		int r, g, b, a;
		r = m_pRedCrosshairSlider->GetValue();
		b = m_pBlueCrosshairSlider->GetValue();
		g = m_pGreenCrosshairSlider->GetValue();
		a = m_pAlphaCrosshairSlider->GetValue();
		surface()->DrawSetColor(r, g, b, a);

		//dot
		if (bDot) {
			surface()->DrawFilledRect(cx, cy, cx + 1, cy + 1);
		}

		//three lines
		if (bThreeLines) {
			int accuracyShift = (dynamic && !bCircle && !bTexture) ? radius - 3 : 0;
			int space = 2;
			int len = 3;

			//without expansion
			//left
			surface()->DrawFilledRect(cx - space - len - accuracyShift, cy, cx - space - accuracyShift, cy + 1);
			//right
			surface()->DrawFilledRect(cx + space + accuracyShift + 1, cy, cx + space + len + accuracyShift + 1, cy + 1);
			//bottom
			surface()->DrawFilledRect(cx, cy + space, cx + 1, cy + space + len);
		}

		//circle
		if (bCircle) {
			surface()->DrawOutlinedCircle(cx, cy, radius, 32);
		}
	}

	/* ~CBG2OptionsPanel
		destructor
	*/
	~CBG2OptionsPanel()
	{
	}

	vgui::Slider	*m_pRedCrosshairSlider,
					*m_pGreenCrosshairSlider,
					*m_pBlueCrosshairSlider,
					*m_pAlphaCrosshairSlider,
					*m_pCrosshairTextureSlider;

	vgui::CheckButton	*m_pSimpleSmokeCheckButton,
						*m_pSimpleFlagHUDCheckButton,

						*m_pFlag0CrosshairCheckButton,
						*m_pFlag1CrosshairCheckButton,
						*m_pFlag2CrosshairCheckButton,
						*m_pFlag3CrosshairCheckButton,

						*m_pDynamicCrosshairCheckButton,
						*m_pHideViewmodelCheckButton,
						*m_pHideExpBottleCheckButton,

						*m_pHitverifCheckButton,
						*m_pHitdamagelineCheckButton,
						*m_pCaptureSoundsCheckButton,
						*m_pVCommSoundsCheckButton,
						*m_pShowNamesCheckButton,
						*m_pWinmusicCheckButton,
						*m_pScreenShotCheckButton;

	vgui::IImage* m_pCurrentCrosshairImage;
	int	m_iCrosshairImageIndex;

protected:
	//virtual void OnCommand(const char *pCommand);
	/*void OnThink( void )
	{
		
		
	}*/
};

extern CBG2OptionsPanel *bg2options;