//HairyPotter - FOR CTF MODE!!

const int CtfFlag_START_DISABLED = 1;		// spawnflag definition

class CtfFlag : public CBaseAnimating
{
	DECLARE_CLASS( CtfFlag, CBaseAnimating );
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();
	DECLARE_DATADESC();

	COutputEvent m_OnPickedUp;
	COutputEvent m_OnDropped;
	COutputEvent m_OnReturned;
	COutputEvent m_OnEnable;
	COutputEvent m_OnDisable;

	CNetworkVar( string_t, n_cFlagName );
	CNetworkVar( bool, m_bIsCarried );
	CNetworkVar( int, m_iForTeam );

	void InputReset( inputdata_t &inputData );
	void InputEnable( inputdata_t &inputData );
	void InputDisable( inputdata_t &inputData );
	void InputToggle( inputdata_t &inputData );

	int		m_iFlagMode = 0,
			m_iTeamBonus,
			iTeam,
			m_iPlayerBonus,
			m_iDropSound,
			m_iReturnSound,
			m_iPickupSound;

	float	m_flPickupRadius, m_fReturnTime, fReturnTime;

	char CTFMsg[512], *m_strName;

	Vector FlagOrigin;    
	QAngle FlagAngle;

public:

	bool m_bFlagIsDropped, m_bActivated;   
	char *cFlagName;
	int m_iFlagWeight;

	void PlaySound( Vector origin, int sound );
	void ResetFlag();
	void Spawn( void );
	void Precache( void );
	void PrintAlert( char *Msg, const char * PlayerName, char * FlagName );
	void PrintAlert( int msg_type, const char * param1 = 0, const char * param2 = 0 );
	void ReturnFlag( void );
	void PlaySound( int iSound );
	void DropFlag( void );
	void Think( void );
	bool SetActiveState(bool bActive);
};
//END CTF FLAG
