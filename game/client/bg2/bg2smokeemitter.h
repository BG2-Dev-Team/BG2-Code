//BG2 - Tjoppen - CBG2SmokeEmitter

//velocity will converge to around (E(|GetDrift()|) / -DROPOFF) for small timeDeltas
//where E(x) is the expected value of x, since it's brownian
//extern ConVar cl_smoke_speed_dropoff;

class CBG2SmokeEmitter : public CSimpleEmitter
{
public:
	CBG2SmokeEmitter( const char *pDebugName );
	static CSmartPtr<CBG2SmokeEmitter>	Create( const char *pDebugName );
	virtual void SimulateParticles( CParticleSimulateIterator *pIterator ); //BG2 - Skillet
	virtual void UpdateVelocity( SimpleParticle *pParticle, float timeDelta );
	virtual	float UpdateRoll( SimpleParticle *pParticle, float timeDelta );

private:
	Vector GetDrift();
	CBG2SmokeEmitter( const CBG2SmokeEmitter & );
};

CBG2SmokeEmitter::CBG2SmokeEmitter( const char *pDebugName ) : CSimpleEmitter( pDebugName ) {}

Vector CBG2SmokeEmitter::GetDrift( void )
{
	//Brownian drift
	return Vector(	random->RandomFloat( -300, 300 ),
					random->RandomFloat( -300, 300 ),
					random->RandomFloat( 50, 600 ) );
}

CSmartPtr<CBG2SmokeEmitter> CBG2SmokeEmitter::Create( const char *pDebugName )
{
	return new CBG2SmokeEmitter( pDebugName );
}

void CBG2SmokeEmitter::SimulateParticles( CParticleSimulateIterator *pIterator )
{
	float timeDelta = pIterator->GetTimeDelta();

	SimpleParticle *pParticle = (SimpleParticle*)pIterator->GetFirst();
	while ( pParticle )
	{
		//Update velocity
		UpdateVelocity( pParticle, timeDelta );
		pParticle->m_Pos += pParticle->m_vecVelocity * timeDelta;

		//Should this particle die?
		pParticle->m_flLifetime += timeDelta;
		UpdateRoll( pParticle, timeDelta );

		if ( pParticle->m_flLifetime >= pParticle->m_flDieTime )
			pIterator->RemoveParticle( pParticle );

		pParticle = (SimpleParticle*)pIterator->GetNext();
	}
}

#define REFLECTION_DAMPENING 0.1f
void CBG2SmokeEmitter::UpdateVelocity( SimpleParticle *pParticle, float timeDelta )
{
	//BG2 - Skillet - Collision Detection
	Vector vecDir = pParticle->m_vecVelocity;
	VectorNormalize( vecDir );

	trace_t tr;
	UTIL_TraceLine( pParticle->m_Pos, pParticle->m_Pos + ( vecDir * (3 + CSimpleEmitter::UpdateScale( pParticle )) ), MASK_SOLID_BRUSHONLY, NULL, &tr );
	if ( tr.DidHit() && !(tr.surface.flags & SURF_SKY) )
	{
		if( !tr.m_pEnt || (tr.m_pEnt && !tr.m_pEnt->IsPlayer()) )
		{
			float proj = pParticle->m_vecVelocity.Dot( tr.plane.normal );
			pParticle->m_vecVelocity += tr.plane.normal * (-proj*2.0f); //Reflection
			pParticle->m_vecVelocity *= random->RandomFloat( (REFLECTION_DAMPENING-0.1f), (REFLECTION_DAMPENING+0.1f) ); //Dampening
		}
	}

	//slow down...
	pParticle->m_vecVelocity *= expf( -999999/*cl_smoke_speed_dropoff.GetFloat()*/ * timeDelta );

	//..and drift upwards and sideways as dictated by GetDrift
	pParticle->m_vecVelocity += GetDrift() * timeDelta;
}

float CBG2SmokeEmitter::UpdateRoll( SimpleParticle *pParticle, float timeDelta )
{
	pParticle->m_flRollDelta += RandomFloat( -1, 1 ) * timeDelta;

	return pParticle->m_flRoll;
}