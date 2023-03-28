#include <fmod.hpp>
#include <fmod_studio.hpp>
#include <map>
#include <string>
#include <vector>

class AudioSystem {
private:
public:
	FMOD_RESULT fmodResult;
	FMOD::Studio::System* fmodSystem = NULL;
	std::map<std::string, FMOD::Studio::EventInstance*> fmodEventInstances;
	std::vector<std::string> bpmNames = {
		"event:/Instruments+FX/BPMs/bpm54_5",
		"event:/Instruments+FX/BPMs/bpm62_5",
		"event:/Instruments+FX/BPMs/bpm75_5",
		"event:/Instruments+FX/BPMs/bpm89",
		"event:/Instruments+FX/BPMs/bpm100",
		"event:/Instruments+FX/BPMs/bpm127",
		"event:/Instruments+FX/BPMs/bpm137",
		"event:/Instruments+FX/BPMs/bpm152",
		"event:/Instruments+FX/BPMs/bpm164",
		"event:/Instruments+FX/BPMs/bpm172",
		"event:/Instruments+FX/BPMs/bpm181",
		"event:/Instruments+FX/BPMs/bpm200"
	};
	std::vector<FMOD::Studio::EventInstance*> bpmInstances;

	AudioSystem(std::vector<std::string> fmodEventNames, bool fmodLiveUpdate = false) {
		fmodResult = FMOD::Studio::System::create(&fmodSystem);
		fmodResult = fmodSystem->initialize(256, fmodLiveUpdate ? FMOD_STUDIO_INIT_LIVEUPDATE : FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, 0);

		FMOD::Studio::Bank* masterBank = NULL;
		fmodSystem->loadBankFile("media/Master.bank", FMOD_STUDIO_LOAD_BANK_NORMAL, &masterBank);

		FMOD::Studio::Bank* stringsBank = NULL;
		fmodSystem->loadBankFile("media/Master.strings.bank", FMOD_STUDIO_LOAD_BANK_NORMAL, &stringsBank);

		FMOD::Studio::Bank* musicandFX = NULL;
		fmodResult = fmodSystem->loadBankFile("media/MusicandFX.bank", FMOD_STUDIO_LOAD_BANK_NORMAL, &musicandFX);

		// populate `this->fmodEventInstances`
		for (auto& eventName : fmodEventNames) {
			FMOD::Studio::EventDescription* eventDescription = NULL;
			fmodSystem->getEvent(("event:/" + eventName).c_str(), &eventDescription);

			FMOD::Studio::EventInstance* eventInstance = NULL;
			eventDescription->createInstance(&eventInstance);

			fmodEventInstances[eventName] = eventInstance;
		}

		// populate `this->bpmInstances`
		bpmInstances.resize(bpmNames.size());
		for (int i = 0; i < bpmNames.size(); i++) {
			FMOD::Studio::EventDescription* tempDescription;
			fmodSystem->getEvent(bpmNames[i].c_str(), &tempDescription);
			tempDescription->createInstance(&bpmInstances[i]);
		}
	}

	~AudioSystem() {
		this->fmodSystem->release();
	}

	void fmodUpdate() {
		fmodResult = fmodSystem->update();
	}

	void startEventInstance(std::string eventName) {
		fmodEventInstances[eventName]->start();
	}

	void stopEventInstance(std::string eventName, bool fadeOut = false) {
		fmodEventInstances[eventName]->stop(fadeOut ? FMOD_STUDIO_STOP_ALLOWFADEOUT : FMOD_STUDIO_STOP_IMMEDIATE);
	}
};