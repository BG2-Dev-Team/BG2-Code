#include "cbase.h"
#include "mapentities.h"
#include "UtlSortVector.h"
// These are mostly for the casting in ShouldCreateEntity( ent )
#include "bg2/spawnpoint.h"
#include "weapon_hl2mpbasehlmpcombatweapon.h"
//
 
#ifndef CMAPENTITYFILTER_H
#define CMAPENTITYFILTER_H
 
typedef const char* strptr;
 
static bool StrLessThan(  const strptr &src1,  const strptr &src2, void *pCtx ) 
{
	if( strcmp(src1, src2) >= 0)
		return false;
	else
		return true;
}
class CMapEntityFilter : public IMapEntityFilter
{
public:
	// constructor
	CMapEntityFilter();
	// deconstructor
	~CMapEntityFilter();
	
	// used to check if we should reset an entity or not
	virtual bool ShouldCreateEntity( const char *pClassname );
	//bool ShouldCreateEntity( const char *pClassname, const char *pTargetname );
	bool ShouldCreateEntity( CBaseEntity *pEnt );
	// creates the next entity in our stored list.
	virtual CBaseEntity* CreateNextEntity( const char *pClassname );
	// add an entity to our list
	void AddKeep( const char* );
	//BG2 - Tjoppen - AddTargetnameKeep
	void AddTargetnameKeep( const char* );
 
private:
	// our list of entities to keep
	CUtlVector< const char * >	keepList,
								keepTargetnameList;	//BG2 - Tjoppen - keepTargetnameList
};

//BG2 - Tjoppen - CMapEntityFilterExcluder
class CMapEntityFilterExcluder : public CPointEntity
{
public:
	DECLARE_CLASS( CMapEntityFilterExcluder, CPointEntity );

	DECLARE_DATADESC();

	bool		m_bExcludeByTargetName;
	string_t	m_sClassOrTargetName;
};
 
#endif 