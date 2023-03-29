#pragma once
#include <fmod.hpp>
#include <fmod_studio.hpp>
#include <map>
#include <string>
#include <vector>

class AudioSystem {
private:
public:
	FMOD_RESULT fmodResult = FMOD_OK;
	FMOD::Studio::System* fmodSystem = nullptr;
	std::map<std::string, FMOD::Studio::EventInstance*> fmodEventInstances;

	AudioSystem(bool fmodLiveUpdate = false);
	~AudioSystem();

	void loadBanks();
	void loadEventInstance(std::string eventName);
	void fmodUpdate();
	void startEventInstance(std::string eventName);
	void stopEventInstance(std::string eventName, bool fadeOut = false);
};