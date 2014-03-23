/*
	The Battle Grounds 2 - A Source modification
	Copyright (C) 206, The Battle Grounds 2 Team and Contributors

	The Battle Grounds 2 free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	The Battle Grounds 2 is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

	Contact information:
		Tomas "Tjoppen" Härdin		mail, in reverse: se . gamedev @ tjoppen
		Jason "Draco" Houston		iamlorddraco@gmail.com

	You may also contact us via the Battle Grounds website and/or forum at:
		www.bgmod.com
*/
#define NUM_HINTS	12

enum //hint displaymodes
{
	DISPLAY_ONCE = 0,
	DISPLAY_ALWAYS,
	OVERRIDE_ALL,
};

enum 
{
	HINT_CAMPING, //implemented
	HINT_CROUCH,
	HINT_CROUCH2,
	HINT_CROUCHSTAB,
	HINT_HINT,
	HINT_SCORE, //implemented
	HINT_MUSKET, //implemented
	HINT_JUMP,
	HINT_STAMINA, //implemented
	HINT_MELEE, //implemented
	HINT_RELOAD, //implemented
	HINT_RELOAD2, 
};

extern char *pHints[NUM_HINTS];

class CHint
{
public:
	CHint(char *input);
	char* GetText() { return m_text; };
	bool Shown() { return m_shown; };
	void SetShown(bool shown) { m_shown = shown; };
private:
	bool m_shown;
	char m_text[512];
};

class CHintbox : public CHudElement , public vgui::Panel
{
	DECLARE_CLASS_SIMPLE( CHintbox, vgui::Panel );

public:
	CHintbox( const char *pElementName );
	virtual void Init( void );	
	virtual void VidInit( void );	
	void Reset();
	virtual void OnThink(void);
	void MsgFunc_Hintbox( bf_read &msg );
	bool SetHint( char* text, int displaytime, int displaymode ); // for custom messages
	bool UseHint( int textpreset, int displaytime, int displaymode ); // for predefined messages

private:
	vgui::HFont m_hHintFont;
	CHint *m_hint[NUM_HINTS];
	Vector m_textpos;
	float m_hintshowtime;
	bool m_hidden;
	vgui::HScheme scheme;
	vgui::Label * m_label; 
	int m_preset;
	float m_showcountdown;
	int m_showgeneralhint;
	float m_initialhintdelay;

protected:
	virtual void ApplySchemeSettings( vgui::IScheme *pScheme );
};
