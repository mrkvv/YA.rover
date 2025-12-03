#pragma once
#include <Windows.h>
#include "InputManager.h"
#include "OutputManager.h"
#include "Source.h"
#include "EventCalendar.h"

class SimulationController {
private:
	static const int START_TIME = 0;
	static const int END_TIME = 20;
	static const int REQUESTS_PER_SOURCE_AMOUNT = 20;

	Buffer buf;
	InputManager im;
	OutputManager om;
	std::vector<Source> sources;
	std::vector<Request> requests;

	int currentTime;
	HANDLE hConsole;

public:
	EventCalendar calendar;

	SimulationController();
	bool makeStep();

	// Метрики для автоматического режима
	int requestsInSum = REQUESTS_PER_SOURCE_AMOUNT * 3;
	int passedRequests = 0;
	int deniedRequests = 0;

	void printAutoModeResults();

private:
	void processIncomingRequest();
	void processRequestsInBuffer();
};
