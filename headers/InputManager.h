#pragma once
#include "Buffer.h"

class InputManager {
private:
	Buffer& buf;
	bool handleBufferFull(Request request);

public:
	InputManager() = delete;
	InputManager(Buffer& buf);
	bool processRequest(Request request);
	Buffer& getBuffer();
};
