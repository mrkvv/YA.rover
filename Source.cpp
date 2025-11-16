#include "Source.h"
#include <iostream>
#include <string>
#include <algorithm>

Source::Source(int id, int minInt, int maxInt) {
	this->sourceId = id;
	this->minInterval = minInt;
	this->maxInterval = maxInt;
	this->requestCounter = 0;

	srand(time(0));
}

int Source::getNextGenerationTime() {
	return minInterval + 1 + rand() % (maxInterval - minInterval);
}

Request Source::generateRequest() {
	std::string adress = "Source" + std::to_string(this->sourceId) + " #" + std::to_string(this->requestCounter);
	return Request(this->requestCounter++, this->sourceId, getNextGenerationTime(), adress);
}

int Source::getSourceId() const {
	return this->sourceId;
}

int Source::getRequestCounter() {
	return this->requestCounter;
}

std::vector<Request> Source::generatePackOfRequests(int amount) {
    std::vector<Request> requests;
    requests.reserve(amount);

    for (int i = 0; i < amount; ++i) {
		Request req = this->generateRequest();
        requests.push_back(req);
    }

	std::sort(requests.begin(), requests.end(),
		[](const Request& a, const Request& b) {
			return a.getGenerationTime() < b.getGenerationTime();
		}
	);

    return requests;
}
