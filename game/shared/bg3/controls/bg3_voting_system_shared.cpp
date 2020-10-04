
#include <map>
#include <iterator>
#include "bg3_voting_system_shared.h"

CVote::CVote(EVoteType vote_type) {
	type = vote_type;
	active = false;
}

void CVote::AddOption(char* option) {
	options.insert(std::make_pair(option, 0));
}

bool CVote::RegisterVote(char* option) {
	std::map<char*, int>::iterator it = options.find(option);

	// key already present in the map
	if (it != options.end()) {
		it->second++;	// increment map's value for key 'c'
		return true;
	}
	else {
		return false;
	}
}

void CVote::StartVote() {
	active = true;
}

void CVote::EndVote() {
	active = false;
	std::map<char*, int>::iterator itr;
	int max = 0;
	for (itr = options.begin(); itr != options.end(); ++itr) {
		if (itr->second > max) {
			max = itr->second;
			result = itr->first;
		}
	}
}

bool CVote::IsActive() {
	return active;
}

char* CVote::GetResult() {
	return result;
}