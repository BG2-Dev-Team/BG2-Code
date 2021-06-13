/*
	The Battle Grounds 2 - A Source modification
	Copyright (C) 2005, The Battle Grounds 2 Team and Contributors

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

	You may also contact the (future) team via the Battle Grounds website and/or forum at:
		www.bgmod.com

	 Note that because of the sheer volume of files in the Source SDK this
	notice cannot be put in all of them, but merely the ones that have any
	changes from the original SDK.
	 In order to facilitate easy searching, all changes are and must be
	commented on the following form:

	//BG2 - <name of contributer>[ - <small description>]
*/

#ifndef USE_ENTITY_BULLET
#ifndef CLIENT_DLL

#define BULLET_SIMULATION_FREQUENCY 500 // scaled down from 1000
#define BULLET_SIMULATION_RWC 10 // number of motions between collision detection

void SpawnServerBullet(const Vector& position, const QAngle& angle, int iDamage, float flConstantDamageRange, float flRelativeDrag, float flMuzzleVelocity, float flDamageDropoffMultiplier, CBasePlayer *pOwner, bool bPenetrateFlesh);
void UpdateBullets();

#endif
#else
#ifndef CLIENT_DLL

class CBullet : public CBaseCombatCharacter
{
	DECLARE_CLASS( CBullet, CBaseCombatCharacter );

public:
	CBullet() { };
	~CBullet();

	Class_T Classify( void ) { return CLASS_NONE; }

public:
	void Spawn( void );
	void Precache( void );
	void BubbleThink( void );
	void BoltTouch( CBaseEntity *pOther );
	bool CreateVPhysics( void );
	unsigned int PhysicsSolidMaskForEntity() const;
	static CBullet *BulletCreate( const Vector &vecOrigin, const QAngle &angAngles, int iDamage, float flConstantDamageRange, float flRelativeDrag, float flMuzzleVelocity, CBasePlayer *pentOwner = NULL, CBaseCombatWeapon *pWeapon = NULL );

protected:

	CHandle<CSpriteTrail>	m_pGlowTrail;
	
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
	DECLARE_SERVERCLASS();
};

#else

class C_Bullet : public C_BaseCombatCharacter
{
	DECLARE_CLASS( C_Bullet, C_BaseCombatCharacter );
	DECLARE_CLIENTCLASS();
public:
	
	C_Bullet( void );

	virtual RenderGroup_t GetRenderGroup( void )
	{
		// We want to draw translucent bits as well as our main model
		return RENDER_GROUP_TWOPASS;
	}

	virtual void	ClientThink( void );

	virtual void	OnDataChanged( DataUpdateType_t updateType );
	virtual int		DrawModel( int flags );

private:

	C_Bullet( const C_Bullet & ); // not defined, not accessible

	Vector	m_vecLastOrigin;
	bool	m_bUpdated;
};

#endif
#endif
