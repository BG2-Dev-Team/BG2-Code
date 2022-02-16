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
#include "c_baseplayer.h"
#include "c_team.h"
#include "c_hl2mp_player.h"
#include "hl2mp_gamerules.h"

#include "../bg3/vgui/bg3_hud_damage_accumulator.h"
#include "../bg3/vgui/admin/vgui_adminmenu.h"


void LocalGameMsg(const char* pszToken);
void LocalGameMsg(const wchar* pszText);


//==============================================
// CHudBG2
// Displays flag status
//==============================================
class CHudBG2 : public CHudElement, public vgui::Panel
{
	DECLARE_CLASS_SIMPLE( CHudBG2, vgui::Panel );
public:
	CHudBG2( const char *pElementName );
	void Init( void );
	void VidInit( void ) override;
	virtual bool ShouldDraw( void );

	//Checks whether or not we should draw based on current player/weapon, while also
	//assigning to the given pointers - Awesome
	bool ShouldDrawPlayer(C_HL2MP_Player** ppPlayer, C_BaseCombatWeapon** ppWeapon);

	void PaintSwingometer(/*C_HL2MP_Player* pHL2Player,*/ int swingScoreA, int swingScoreB);
	void PaintBackgroundOnto(Label* pLabel);
	void PaintTopCenterHUD(/*C_HL2MP_Player* */); //time, new swingomemeter, flags, backgrounds, round scores
	void PaintBottomLeftHUD(C_HL2MP_Player*, C_BaseCombatWeapon*); //health, ammo
	void PaintBottomRightHUD(C_HL2MP_Player* pPlayer);
	void PaintDeathMessage();
	void PaintClassCounts();


	virtual void Paint(void);
	virtual void ApplySettings(KeyValues *inResourceData);
	virtual void ApplySchemeSettings( vgui::IScheme *scheme );
	virtual void OnThink();
	void Reset( void );

	void SetGameMsgText(const wchar* pszText);
	void MsgFunc_GameMsg(bf_read& msg);
	void MsgFunc_HitVerif( bf_read &msg );
	void MsgFunc_WinMusic( bf_read &msg );
	//HairyPotter
	void MsgFunc_CaptureSounds( bf_read &msg );
	void PlayCaptureSound ( const Vector &origin, char sound[255] );
	void MsgFunc_VCommSounds( bf_read &msg );
	void MsgFunc_ExpEvent(bf_read &msg);
	void MsgFunc_BG3Election(bf_read &msg);
	void PlayVCommSound ( char snd[512], int playerindex );
	void PlayLevelUpSound();
	//
	
	void HideShowAllTeam( bool visible );
	void HideShowAllPlayer(bool visible); //Awesome - split this so that team/player information can be toggled separately

	CDamagePlayerAccumulator m_IsVictimAccumulator;
	CDamagePlayerAccumulator m_IsAttackerAccumulator;

	static CHudBG2 *GetInstance();

private:

	vgui::Label * m_pLabelBScore; 
	vgui::Label * m_pLabelAScore; 
	vgui::Label * m_pLabelBTickets;
	vgui::Label * m_pLabelATickets;
	vgui::Label * m_pLabelCurrentRound;
	vgui::Label * m_pLabelAmmo; 
	vgui::Label * m_pLabelDeathMessage; 
	vgui::Label * m_pLabelRoundTime;
	vgui::Label * m_pLabelDamageTaken;
	vgui::Label * m_pLabelDamageGiven;
	vgui::Label * m_pLabelLMS;		//BG2 - Tjoppen - TODO: remove this when hintbox works correctly
	vgui::Label * m_pLabelHealth;
	vgui::Label * m_pLabelSpectatedPlayerName;

	vgui::Label * m_pLabelGameMessage;

	CAdminMenu  * m_pAdminMenu;


	//exp system and notifications
	CHudTexture* m_pExpBottleBlue;
	CHudTexture* m_pExpBottleRed;
	CHudTexture* m_pExpBottleOverlay;
	vgui::Label* m_pExpEventLabel;
	vgui::Label* m_pExpWarningLabel;
	float m_flLastExpEventTime;

	float m_flGameMessageExpireTime;
	float m_flTakenExpireTime;
	float m_flGivenExpireTime;
	float m_flLastSwing;	//last frame's swingometer value
	float m_flAFlashEnd;	//american ticket flash timer
	float m_flBFlashEnd;	//british ticket flash timer
	int m_iLastSwingA;		//swinga of last frame
	int m_iLastSwingB;		//swingb of last frame

	bool m_bLocalPlayerAlive;
	float m_flEndClassIconDrawTime;
	IImage*		m_pAmericanClassIcons[TOTAL_AMER_CLASSES];
	IImage*		m_pBritishClassIcons[TOTAL_BRIT_CLASSES];
	Label*		m_pClassCountLabels[6];


	CHudTexture* m_pAmerFlagImage;
	CHudTexture* m_pBritFlagImage;

	IImage*		m_pBlackBackground;
	IImage*		m_pStaminaBarImage;
	IImage*		m_pHealthBarImage[4];
	IImage*		m_pHealthBarOverlayImage;
	IImage*		m_pHealthSymbolImage[4];
	IImage*		m_pBottomLeftBackground;
	IImage*		m_pAmmoBallImage;
	IImage*		m_pSwingometerRedImage;
	IImage*		m_pSwingometerBlueImage;
	float		m_flStartPulseTime; //pulsating health symbol
	float		m_flEndPulseTime;

public:

	CAdminMenu* GetAdminMenu() const { return m_pAdminMenu; }
	
protected:

	static CHudBG2 *s_pInstance;
};