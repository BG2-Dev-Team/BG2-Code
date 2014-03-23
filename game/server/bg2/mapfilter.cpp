#include "cbase.h"
#include "baseentity.h"
#include "mapfilter.h"
 
/*static const char *s_PreserveEnts[] =
{
	"env_soundscape_proxy",
	"env_fog_controller",
	"event_queue_saveload_proxy",
	"info_map_parameters",
	"point_viewcontrol",
	"sky_camera",
	"", // END Marker
};*/

// Constructor
CMapEntityFilter::CMapEntityFilter() 
{
	
	//BG2 - Tjoppen - mapfilter_excluder related logic. also make sure we keep them by adding to the list
	//Just call these once, here.
	AddKeep( "mapfilter_excluder" );
	AddKeep( "hl2mp_gamerules" );
	AddKeep( "worldspawn" );
	AddKeep( "ambient_generic" );
	AddKeep( "env_fog_controller" );
	AddKeep( "func_precipitation" );
	AddKeep( "info_intermission" );
	AddKeep( "player" );
	AddKeep( "soundent" );
	AddKeep( "env_soundscape" );
	AddKeep( "env_soundscape_triggerable" );
	AddKeep( "env_sun" );
	AddKeep( "flag" );
	AddKeep( "ctf_flag" );
	AddKeep( "env_wind" );
	AddKeep( "viewmodel" );
	AddKeep( "predicted_viewmodel" );
	AddKeep( "player_manager" );
	AddKeep( "team_manager" );
	AddKeep( "scene_manager" );
	AddKeep( "trigger_soundscape" );
	AddKeep( "shadow_control" );
	//

	CMapEntityFilterExcluder *pExcluder = NULL;

	while( (pExcluder = dynamic_cast<CMapEntityFilterExcluder*>(
			gEntList.FindEntityByClassname( pExcluder, "mapfilter_excluder") )) != NULL )
	{
		if( pExcluder->m_bExcludeByTargetName )
			AddTargetnameKeep( STRING( pExcluder->m_sClassOrTargetName ) );
		else
			AddKeep( STRING( pExcluder->m_sClassOrTargetName ) );
	}
	
}
 
// Deconstructor
CMapEntityFilter::~CMapEntityFilter() 
{
	keepList.Purge();
	keepTargetnameList.Purge();
}
 
// [bool] ShouldCreateEntity [char]
// Purpose   : Used to check if the passed in entity is on our stored list
// Arguments : The classname of an entity 
// Returns   : Boolean value - if we have it stored, we return false.

bool CMapEntityFilter::ShouldCreateEntity( const char *pClassname ) 
{ 
	//This is used for the mapfilter_excluder added in hammer.
	for ( int x = 0; x < keepList.Count(); x++ )
	{
		if ( !Q_stricmp( keepList[x], pClassname ) )
			return false;
	}

	//This takes care of the spawns and weapons through casting & also handles "pTargetnames"
	CBaseEntity *pEnt = gEntList.FindEntityByClassname( NULL, pClassname );
	if ( pEnt && !ShouldCreateEntity( pEnt ) )
		return false;
	//

	return true;
}

//BG2 - Tjoppen - targetname version
//If it's NOT in the list, we should return true.
/*bool CMapEntityFilter::ShouldCreateEntity( const char *pClassname, const char *pTargetname )  
{ 
	for ( int x = 0; x < keepList.Count(); x++ )
	{
		if ( !Q_stricmp( keepList[x], pClassname ) )
			return false;
	}
	for ( int x = 0; x < keepTargetnameList.Count(); x++ )
	{
		if ( !Q_stricmp( keepTargetnameList[x], pTargetname ) )
			return false;
	}

	return true;
}*/
//Ent version.
bool CMapEntityFilter::ShouldCreateEntity( CBaseEntity *pEnt )  
{ 
	//Skip common castable ents.
	//All Defines.
	CBaseHL2MPCombatWeapon *Weapon = dynamic_cast<CBaseHL2MPCombatWeapon *>( pEnt );
	CSpawnPoint *Spot = dynamic_cast<CSpawnPoint *>( pEnt );

	if ( Spot || Weapon && Weapon->GetPlayerOwner() )
		return false;
	//

	const char *pTargetname = STRING( pEnt->GetEntityName() );

	for ( int x = 0; x < keepTargetnameList.Count(); x++ )
	{
		if ( !Q_stricmp( keepTargetnameList[x], pTargetname ) )
			return false;
	}

	return true;
}
 
// [CBaseEntity] CreateNextEntity [char]
// Purpose   : Creates an entity
// Arguments : The classname of an entity
// Returns   : A pointer to the new entity
 
CBaseEntity* CMapEntityFilter::CreateNextEntity( const char *pClassname ) 
{
	return CreateEntityByName( pClassname);
}
 
// [void] AddKeep [char]
// Purpose   : Adds the passed in value to our list of items to keep
// Arguments : The class name of an entity
// Returns   : Void
 
void CMapEntityFilter::AddKeep( const char *sz) 
{
	keepList.AddToTail(sz);
}

//BG2 - Tjoppen - AddTargetnameKeep. same as AddKeep, but for targetnames
void CMapEntityFilter::AddTargetnameKeep( const char *sz) 
{
	keepTargetnameList.AddToTail(sz);
}

//BG2 - Tjoppen - CMapEntityFilterExcluder
BEGIN_DATADESC( CMapEntityFilterExcluder )
	DEFINE_KEYFIELD( m_bExcludeByTargetName, FIELD_BOOLEAN, "ExcludeByTargetName" ),
	DEFINE_KEYFIELD( m_sClassOrTargetName, FIELD_STRING, "ClassOrTargetName" ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( mapfilter_excluder, CMapEntityFilterExcluder );
