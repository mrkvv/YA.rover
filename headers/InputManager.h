#pragma once
#include "Buffer.h"
#include "EventCalendar.h"

class InputManager {
private:
	Buffer& buf;
	EventCalendar& calendar;
	bool handleBufferFull(Request request);

public:
	InputManager() = delete;
	InputManager(Buffer& buf, EventCalendar& cal);
	bool processRequest(Request request);
	Buffer& getBuffer();
};
