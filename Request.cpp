#include "Request.h"

Request::Request(int requestId, int sourceId, int genTime, std::string adress) {
	this->requestId = requestId;
	this->sourceId = sourceId;
	this->generationTime = genTime;
	this->deliveryAdress = adress;
}

int Request::getId() {
	return this->requestId;
}

int Request::getPriority() {
	return this->sourceId;
}

int Request::getGenerationTime() const {
	return this->generationTime;
}

std::string Request::getDeliveryAdress() {
	return this->deliveryAdress;
}

std::ostream& operator<<(std::ostream& os, const Request& request) {
	os << "Request#" << request.requestId
		<< " [Source: " << request.sourceId
		<< ", Time: " << request.generationTime << "]";
	return os;
}
