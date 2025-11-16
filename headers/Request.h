#pragma once
#include <iostream>

class Request {
private:
	int requestId;
	int sourceId;
	int generationTime;
	std::string deliveryAdress;

public:
	Request() = delete;
	Request(int requestId, int sourceId, int genTime, std::string adress);

	int getId();
	int getPriority();
	int getGenerationTime() const;
	std::string getDeliveryAdress();

	friend std::ostream& operator<<(std::ostream& os, const Request& request);
};
