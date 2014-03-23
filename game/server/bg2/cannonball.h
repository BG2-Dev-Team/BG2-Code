/*
This is pretty much a complete ripoff of Tjoppen's bullet code. 
While I have changed a few things here and there to work better for the cannon, 
he deserves 95% of the credit for this code. -HairyPotter
*/

#include "SpriteTrail.h"

class CCannonBall : public CBaseCombatCharacter
{
	DECLARE_CLASS( CCannonBall, CBaseCombatCharacter );

public:
	CCannonBall() { };
	~CCannonBall();

	Class_T Classify( void ) { return CLASS_NONE; }

public:
	void Spawn( void );
	void Precache( void );
	void BubbleThink( void );
	void BoltTouch( CBaseEntity *pOther );
	bool CreateVPhysics( void );
	unsigned int PhysicsSolidMaskForEntity() const;
	static CCannonBall *Create( const Vector &vecOrigin, const QAngle &angAngles, int iDamage, float flConstantDamageRange, float flRelativeDrag, float flMuzzleVelocity );

protected:

	
	int		m_iDamage;
	float	m_flDyingTime;	//BG2 - Tjoppen - bullets must die after a while. say ten seconds
							//					otherwise they'll lay around the map consuming
							//					bandwidth
	float	m_flConstantDamageRange;	//BG2 - Tjoppen - within this range there is no velocity/range based
										//					damage. helps balancing.
	float	m_flRelativeDrag;			//BG2 - Tjoppen - multiply drag by this amount, to separate
										//					pistols, muskets and rifles in terms of range,
										//					dropoff and other things..
	float	m_flMuzzleVelocity;
	Vector	m_vTrajStart;	//BG2 - Tjoppen - where the bullet started. for range calculations

	CBaseCombatWeapon *firedWeapon;
	CBasePlayer	*pAttacker;

	bool	m_bHasPlayedNearmiss;

	DECLARE_DATADESC();
};