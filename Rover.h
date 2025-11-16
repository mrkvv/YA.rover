#pragma once
#include "Request.h"
#include <optional>
#include <random>

class Rover {
private:
	int id;
	bool isBusy;
	std::optional<Request> currentRequest;
	// startTime это время, указанное в request
	int endTime;

	static std::mt19937 gen;
	static double lambda; // Параметр для эксп распределения, по дефолту = 1.0

public:
	Rover() = delete;
	Rover(int id);

	void startService(Request request, int currentTime);
	void completeService();
	bool isAvailable();
	int getId();
	int getEndTime();

	static void setLambda(double newLambda);

private:
	int generateServiceTime();
};
