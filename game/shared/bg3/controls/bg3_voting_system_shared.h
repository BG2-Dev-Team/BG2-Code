

enum class EVoteMessageType {
	NONE = 0, //empty message type, defacto NULL value
	CANCEL, //the vote has been canceled
	SEND_OPTION, //used to fill up the vote menu GUI, bc messages are capped at 256 bytes, we have to send multiple
	RESULT, //sends the generic winning vote to the GUI so players can see results
	VOTE_TALLY, //reports to client of another vote being received by a different player, sent with the index of which option the vote was for
	TOPIC, //used to fill the 'question' part of the vote GUI, while also signalling the start of the vote to the player
	TOPIC_MAPCHOICE, //tells the GUI to prepare for multichoice vote, just like TOPIC but with different GUI layout
	TOPIC_BINARY, //used to fill the binary yes/no 'question' part of the vote GUI, while also signalling the start of the vote to the player
	TOPIC_PLAYER, //used to fill the 'question' part with a player's name injected into the localized string, while also signalling the start of the vote to the player
	TOPIC_MAP, //used to fill the 'question' part with a map name injected into the localized string, while also signalling the start of the vote to the player
	
};

#define MAX_NUM_MULTICHOICE_MAP_OPTIONS 6

#define BG3_OFFICIAL_MAP_LIST \
	"bg_abbey", \
	"bg_ambush",\
	"bg_blackswamp",\
	"bg_canal",\
	"bg_canal_test",\
	"bg_fort_stmathieu",\
	"bg_freemans_farm",\
	"bg_horseshoe",\
	"bg_maricopa",\
	"bg_plateau",\
	"bg_snowlake",\
	"bg_townguard",\
	"bg_trenton",\
	"bg_winterisle",\
	"bg_woodland",\
	"bg_wrecked",\
	"ctf_mill_creek",\
	"ctf_river",\
	"ctf_road",\
	"ctf_stonefort",\
	"lb_alpinepass",\
	"lb_autumn",\
	"lb_battleofconcord",\
	"lb_battleofvalcourisland",\
	"lb_continental",\
	"lb_nativevalley",\
	"lb_princeton",\
	"sg_boston",\
	"sg_deroga",\
	"sg_fall",\
	"sg_ravine",\
	"sg_siege",