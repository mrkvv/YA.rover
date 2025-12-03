#pragma once
#include "Buffer.h"
#include "Rover.h"
#include "EventCalendar.h"

class OutputManager {
private:
	static const int ROVERS_AMOUNT = 4;
	Buffer& buf;
	EventCalendar& calendar;
	std::vector<Rover> rovers;
	int currentRoverPointer;
	int highestPriorityPackage;

public:
	OutputManager() = delete;
	OutputManager(Buffer& buf, EventCalendar& cal);

	std::optional<Request> selectRequest();
	int updateRoversState(int currentTime);
	Rover* selectRover();
	void startService(Rover* rover, Request request, int currentTime);
	void printRoversInfo();
	bool isAnyRoverAvailable();
};
