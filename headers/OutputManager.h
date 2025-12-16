#pragma once
#include "Buffer.h"
#include "Rover.h"
#include "EventCalendar.h"

struct ServiceResults {
	int serviceTime;
	int inBufferAwaitingTime;
};

struct RoversStatistics {
	std::vector<double> busyInTotals;

	RoversStatistics(int roversAmount) {
		busyInTotals.reserve(roversAmount);
		for (int i = 0; i < roversAmount; i++) {
			busyInTotals.push_back(0.0);
		}
	}
};

class OutputManager {
private:
	static const int ROVERS_AMOUNT = 5;
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
	ServiceResults startService(Rover* rover, Request request, int currentTime);
	void printRoversInfo();
	bool isAnyRoverAvailable();

	RoversStatistics roversStats = RoversStatistics(ROVERS_AMOUNT);
};
