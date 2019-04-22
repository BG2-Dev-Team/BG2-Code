#ifndef BG3_PLAYER_LOCATOR_H
#define BG3_PLAYER_LOCATOR_H

class CHL2MP_Player;

//Given a string, returns a player whose name matches the string
void GetPlayersFromString(CHL2MP_Player** pPlayerList, const char* pszString, CHL2MP_Player* pRequester = NULL);

#endif //BG3_PLAYER_LOCATOR_H