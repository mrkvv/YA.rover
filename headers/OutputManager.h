#pragma once
#include "Buffer.h"
#include "Rover.h"

class OutputManager {
private:
	static const int ROVERS_AMOUNT = 4;
	Buffer& buf;
	std::vector<Rover> rovers;
	int currentRoverPointer;
	int highestPriorityPackage;

public:
	OutputManager() = delete;
	OutputManager(Buffer& buf);

	std::optional<Request> selectRequest();
	void updateRoversState(int currentTime);
	Rover* selectRover();
	void startService(Rover* rover, Request request, int currentTime);
	void printRoversInfo();
	bool isAnyRoverAvailable();
};
