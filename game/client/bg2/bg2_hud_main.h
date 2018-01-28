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
#include <sstream>
#include <set>
#include "c_baseplayer.h"
#include "c_team.h"
#include "c_hl2mp_player.h"
#include "hl2mp_gamerules.h"

//BG2 - Tjoppen - used for accumulated damage cauased by or inflicted on the player, over a short period of time (buckshot)
#define ACCUMULATE_LIMIT	0.5	//accumulate damages for this long. should be enough for all shot to hit

static const char* HitgroupName( int hitgroup )
{
	//BG2 - Tjoppen - TODO: localize
	switch ( hitgroup )
	{
	case HITGROUP_GENERIC:
		return NULL;
	case HITGROUP_HEAD:
		return "head";
	case HITGROUP_CHEST:
		return "chest";
	case HITGROUP_STOMACH:
		return "stomach";
	case HITGROUP_LEFTARM:
		return "left arm";
	case HITGROUP_RIGHTARM:
		return "right arm";
	case HITGROUP_LEFTLEG:
		return "left leg";
	case HITGROUP_RIGHTLEG:
		return "right leg";
	default:
		return "unknown default case";
	}
}

class CDamagePlayerAccumulator
{

public:

	bool m_bIsVictim;			//governs what kind of message is produced
	float m_flLastAttack;
	int m_iTotalDamage;
	int m_iHitgroup;			//only used if only one player was hit
	std::set<int> m_sPlayers;

	std::string GetPlayerName( int player )
	{
		player_info_t info;

		if( engine->GetPlayerInfo( player, &info ) )
			return info.name;
		else
			return "";
	}

	CDamagePlayerAccumulator( bool isVictim )
	{
		m_bIsVictim = isVictim;
		m_flLastAttack = 0;
		m_iTotalDamage = 0;
	}

	//accumulates the specified damage and returns a proper message
	std::string Accumulate( int damage, int player, int hitgroup )
	{
		if( m_flLastAttack + ACCUMULATE_LIMIT > gpGlobals->curtime )
		{
			//accumulate
			m_iTotalDamage += damage;
			m_sPlayers.insert(player);

			//only override hitgroup if it's a headshot
			if( hitgroup == HITGROUP_HEAD )
				m_iHitgroup = hitgroup;
		}
		else
		{
			//reset
			m_flLastAttack = gpGlobals->curtime;
			m_iTotalDamage = damage;
			m_iHitgroup = hitgroup;
			m_sPlayers.clear();
			m_sPlayers.insert(player);
		}

		/**
		  Generate hit verification message. We have five cases:
		   * we're the victim and were hit by one attacker in no particular hitgroup
		   * we're the victim and were hit by one attacker in some hitgroup (head, chest etc.)
		   * we're the attacker and hit one victim in no particular hitgroup
		   * we're the attacker and hit one victim in some hitgroup
		   * we're the attacker and hit multiple victims
	      
		  This means we don't care about being hit by multiple attackers (just print the first one)
		  or things like headshoting multiple victims
		 */
		//use m_iHitgroup instead of hitgroup so headshots show even though hitgroup might be something else
		const char *hitgroupname = HitgroupName( m_iHitgroup );
		std::ostringstream oss;

		//TODO: localize
		if( m_bIsVictim )
		{
			oss << "You were hit";

			if( hitgroupname )
			{
				//specific hitgroup
				oss << " in the " << hitgroupname;
			}

			oss << " by " << GetPlayerName(player);
		}
		else
		{
			oss << "You hit ";

			if( m_sPlayers.size() == 1 )
			{
				//single victim
				oss << GetPlayerName(player);

				if( hitgroupname )
				{
					//specific hitgroup
					oss << " in the " << hitgroupname;
				}
			}
			else
			{
				//multiple victims
				//desired format: "player1[, player2[, player3...]] and playerN"
				size_t numLeft = m_sPlayers.size() - 1;

				for( std::set<int>::iterator it = m_sPlayers.begin(); it != m_sPlayers.end(); it++, numLeft-- )
				{
					oss << GetPlayerName(*it);

					if( numLeft > 1 )
						oss << ", ";
					else if( numLeft > 0 )
						oss << " and ";
				}
			}
		}

		oss << " for " << m_iTotalDamage << " points of damage";

		return oss.str();
	}
};


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
	virtual void Paint( void );
	virtual void ApplySettings(KeyValues *inResourceData);
	virtual void ApplySchemeSettings( vgui::IScheme *scheme );
	virtual void OnThink();
	void Reset( void );

	void MsgFunc_HitVerif( bf_read &msg );
	void MsgFunc_WinMusic( bf_read &msg );
	//HairyPotter
	void MsgFunc_CaptureSounds( bf_read &msg );
	void PlayCaptureSound ( const Vector &origin, char sound[255] );
	void MsgFunc_VCommSounds( bf_read &msg );
	void PlayVCommSound ( char snd[512], int playerindex );
	//
	
	void HideShowAll( bool visible );

	CDamagePlayerAccumulator m_IsVictimAccumulator;
	CDamagePlayerAccumulator m_IsAttackerAccumulator;

	static CHudBG2 *GetInstance();

private:

	CHudTexture		* m_Base; 
	CHudTexture		* m_AmerHealthBase;
	CHudTexture		* m_AmerHealth;
	CHudTexture		* m_AmerStamina;
	CHudTexture		* m_BritHealthBase;
	CHudTexture		* m_BritHealth;
	CHudTexture		* m_BritStamina;
	CHudTexture		* m_SwingometerRed;
	CHudTexture		* m_SwingometerBlue;

	vgui::Label * m_pLabelBScore; 
	vgui::Label * m_pLabelAScore; 
	vgui::Label * m_pLabelBTickets;
	vgui::Label * m_pLabelATickets;
	vgui::Label * m_pLabelCurrentRound;
	vgui::Label * m_pLabelAmmo; 
	vgui::Label * m_pLabelWaveTime; 
	vgui::Label * m_pLabelRoundTime;
	vgui::Label * m_pLabelDamageTaken;
	vgui::Label * m_pLabelDamageGiven;
	vgui::Label * m_pLabelLMS;		//BG2 - Tjoppen - TODO: remove this when hintbox works correctly

	float m_flTakenExpireTime;
	float m_flGivenExpireTime;
	float m_flLastSwing;	//last frame's swingometer value
	float m_flAFlashEnd;	//american ticket flash timer
	float m_flBFlashEnd;	//british ticket flash timer
	int m_iLastSwingA;		//swinga of last frame
	int m_iLastSwingB;		//swingb of last frame

	int basex, basey, basew, baseh;
	int swingx, swingy, swingw, swingh;
	int staminax, staminay, staminaw, staminah;
	int healthbasex, healthbasey, healthbasew, healthbaseh;
	int healthx, healthy, healthw, healthh;

protected:

	static CHudBG2 *s_pInstance;
};