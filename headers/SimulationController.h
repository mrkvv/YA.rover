#pragma once
#include <Windows.h>
#include "InputManager.h"
#include "OutputManager.h"
#include "Source.h"
#include "EventCalendar.h"

struct AutoModeInfo {
	std::vector<double> serviceTimes1;
	std::vector<double> serviceTimes2;
	std::vector<double> serviceTimes3;

	std::vector<double> bufferAwaitingTimes1;
	std::vector<double> bufferAwaitingTimes2;
	std::vector<double> bufferAwaitingTimes3;

	AutoModeInfo(size_t expectedSize) {
		serviceTimes1.reserve(expectedSize);
		serviceTimes2.reserve(expectedSize);
		serviceTimes3.reserve(expectedSize);
		bufferAwaitingTimes1.reserve(expectedSize);
		bufferAwaitingTimes2.reserve(expectedSize);
		bufferAwaitingTimes3.reserve(expectedSize);
	}
};

class SimulationController {
private:
	static const int START_TIME = 0;
	static const int END_TIME = 20;
	static const int REQUESTS_PER_SOURCE_AMOUNT = 30;
	static const int ACTUAL_TIME_TO_STOP_SIMULATION = 4*END_TIME - START_TIME;

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

	AutoModeInfo info = AutoModeInfo(REQUESTS_PER_SOURCE_AMOUNT);

	void printAutoModeResults();
	void printRoversTableInAutoModeStatistics();
private:
	void processIncomingRequest();
	void processRequestsInBuffer();

	double calculateDispersion(const std::vector<double>& values, double average);
};
