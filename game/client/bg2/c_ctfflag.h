class C_CtfFlag : public C_BaseAnimating
{
	DECLARE_CLASS( C_CtfFlag, C_BaseAnimating );
public:
	DECLARE_CLIENTCLASS();

					C_CtfFlag();
	virtual			~C_CtfFlag();


public:

	bool	m_bIsCarried;
	int		m_iForTeam;
	
	char	n_cFlagName[256];

};

extern CUtlVector< C_CtfFlag * > g_CtfFlags, g_AmericanFlags, g_BritishFlags;