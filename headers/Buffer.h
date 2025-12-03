#pragma once
#include "Request.h"
#include <vector>
#include <optional>

class Buffer {
private:
	static const int CAPACITY = 10;
	std::vector<std::optional<Request>> slots;
	int freeSlotPointer;

public:
	Buffer();

	int findFirstFreeSlot();
	std::optional<Request> getLowestPriorityRequest();
	int getHighestPriorityPackage();
	std::optional<Request> getHighestPriorityRequestInPackage(int packageId);
	bool addRequest(Request request);
	bool removeRequest(Request request);
	std::optional<Request> removeRequest(int index);

	bool isFull();
	bool isEmpty();

	void printBuffer();
};
