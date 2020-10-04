
#include <string>
#include <iterator> 
#include <map> 

enum class EVoteMessageType {
	SEND_OPTION, //used to fill up the vote menu GUI, bc messages are capped at 256 bytes, we have to send multiple
	CANCEL, //the vote has been canceled
	TOPIC, //used to fill the 'question' part of the vote GUI
	TOPIC_PLAYER, //used to fill the 'question' part with a player's name injected into the localized string
	TOPIC_MAP, //used to fill the 'question' part with a map name injected into the localized string
	RESULTS, //sends the generic winning vote to the GUI so players can see results
	RESULTS_MAP, //
	RESULTS_NEXTMAP, //
};

enum class EVoteType {
	NEXT_MAP_VOTE,
	END_MAP_VOTE,
	MUTE_VOTE,
	KICK_VOTE,
	BAN_VOTE,
};

class CVote {
private:
	std::map <char*, int>options;
	EVoteType type;
	bool active;
	char* result;
public:
		CVote(EVoteType vote_type);
		~CVote();

		void StartVote();
		void EndVote();

		bool IsActive();

		void AddOption(char* option);
		bool RegisterVote(char* option);

		char* GetResult();
};