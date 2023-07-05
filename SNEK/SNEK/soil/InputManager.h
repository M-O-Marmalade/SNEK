// The InputManager class spawns its own thread which monitors keyboard/mouse button input.
// This allows console/terminal games to read multiple inputs at once, whereas consoles are typically limited to a single input at a time.

#pragma once
#include <thread>
#include <vector>
#include <string>
#include <mutex>

namespace Soil {
	class InputManager {
	private:
		std::vector<char> keysToCheck = std::vector<char>();
		std::mutex keysToCheckMutex;
		std::vector<bool> inputBuffer = std::vector<bool>();
		std::vector<bool> inputAccessibleBuffer = std::vector<bool>();
		std::thread inputThread;
		bool alive = true;

		void inputThreadLoop();

	public:
		InputManager(std::string keysToCheck = std::string());
		~InputManager();
		void addKeys(std::string keysToCheck);
		void removeKeys(std::string keysToCheck);
		void getBufferState();
		bool isKeyPressed(char key);
	};
}
