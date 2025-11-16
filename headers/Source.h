#pragma once
#include "Request.h"
#include <vector>

class Source {
private:
	int sourceId;
	int minInterval;
	int maxInterval;
	int requestCounter;

public:
	Source() = delete;
	Source(int id, int minInt, int maxInt);

	int getNextGenerationTime();
	Request generateRequest();

	int getSourceId() const;
	int getRequestCounter();

	std::vector<Request> generatePackOfRequests(int amount);
};
