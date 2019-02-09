/*
The Battle Grounds 3 - A Source modification
Copyright (C) 2017, The Battle Grounds 2 Team and Contributors

The Battle Grounds 3 free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

The Battle Grounds 3 is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

Contact information:
Chel "Awesome" Trunk		mail, in reverse: com . gmail @ latrunkster

You may also contact the (future) team via the Battle Grounds website and/or forum at:
battlegrounds3.com

Note that because of the sheer volume of files in the Source SDK this
notice cannot be put in all of them, but merely the ones that have any
changes from the original SDK.
In order to facilitate easy searching, all changes are and must be
commented on the following form:

//BG3 - <name of contributer>[ - <small description>]
*/

#include "cbase.h"
#include "../shared/types/CDblLkLst.hpp"

//------------------------------------------------------------------------
// Purpose: To provide more consistent application of speed modifiers onto
//   players. Only integer-based offset are considered here (not multipliers)
//   This is to solve issues where a player could have a speed modifier
//   from a weapon, flag, trigger, etc. but is removed when another speed modifier
//   is applied.
//------------------------------------------------------------------------
#ifndef BG3_SPEEDMOD_LIST



enum class ESpeedModID {
	Weapon = 0,
	Flag,
	MapTrigger,
};

class CSpeedModList {
private:
	struct SSpeedMod {
		int8 m_iSpeedMod;
		uint8 m_iUniqueID;
	};

	CDblLkLst<SSpeedMod> m_aMods; //internal list of speed modifiers

public:
	//Constructor initializes list of speed mods to empty
	CSpeedModList() {}

	//-------------------------------------------------------------
	// Purpose: Adds or replaces a speed mod.
	// Returns: If the ID was already in the list, returns the change in the mod value
	//          Otherwise returns the given value mod.
	//-------------------------------------------------------------
	inline int8 AddOrReplace(int8 mod, ESpeedModID id) {
		//determine if we already have this id
		int8 foundIndex = -1;
		for (int i = 0; i < m_aMods.length(); i++) {
			if (m_aMods.getRef(i).m_iUniqueID == (uint8)id) {
				foundIndex = i;
				break;
			}
		}

		

		//two cases depending on whether or not we found it
		if (foundIndex == -1) {
			m_aMods.push(SSpeedMod{ mod, (uint8)id });
			return mod;
		}
		else {
			//determine differnce between old and new value
			int8 diff = 0;
			diff = mod - m_aMods.getRef(foundIndex).m_iSpeedMod;
			m_aMods.getRef(foundIndex).m_iSpeedMod = mod;
			return diff;
		}
	}

	//-------------------------------------------------------------
	// Purpose: Given an ID, removes the given speed mod if it exists
	// Returns: 0 if not removed, the value of the modifier otherwise.
	//-------------------------------------------------------------
	inline int8 Remove(ESpeedModID id) {
		for (int i = 0; i < m_aMods.length(); i++) {
			if (m_aMods.getRef(i).m_iUniqueID == (uint8)id) {
				int8 val = m_aMods.getRef(i).m_iSpeedMod;
				m_aMods.remove(i);
				return val;
			}
		}
		return 0;
	}

	//-------------------------------------------------------------
	// Clears the list
	//-------------------------------------------------------------
	inline void Clear() {
		m_aMods.flush();
	}
};

#endif