#pragma once

class InputEvent
{

};

class Window;

namespace Input
{
	void AssignBasicCallbacks(Window&);
}

class InputManager
{
	InputManager();
	InputManager(const InputManager&) = delete;
	InputManager(InputManager&&) = delete;
	~InputManager();

public:
	static InputManager& Instance()
	{
		static InputManager instance;
		return instance;
	}
};
