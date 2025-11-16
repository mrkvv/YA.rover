#include "Rover.h"

std::mt19937 Rover::gen(std::random_device{}());
double Rover::lambda = 1.0;

Rover::Rover(int id) {
	this->id = id;
	this->isBusy = false;
	this->currentRequest = std::nullopt;
	this->endTime = -1;
}

void Rover::startService(Request request, int currentTime) {
	this->isBusy = true;
	this->currentRequest = request;

	this->endTime = currentTime + generateServiceTime();
	std::cout << "Rover#" << this->id << " started " << request << " until " << this->endTime << "\n";
}

void Rover::completeService() {
	this->isBusy = false;
	
	std::cout << "Rover#" << this->id << " ended " << this->currentRequest.value() << " at " << this->endTime << "\n";
	this->currentRequest = std::nullopt;
	this->endTime = -1;
}

bool Rover::isAvailable() {
	return !(this->isBusy);
}

int Rover::getId() {
	return this->id;
}

int Rover::getEndTime() {
	return this->endTime;
}

void Rover::setLambda(double newLambda) {
	if (newLambda > 0) {
		lambda = newLambda;
	}
}

int Rover::generateServiceTime() {
	std::exponential_distribution<double> expDist(lambda);

	double serviceTime = expDist(gen);
	int intServiceTime = static_cast<int>(std::ceil(serviceTime));

	// Гарантируем, что время обслуживания хотя бы 1 единица
	return std::max(1, std::min(intServiceTime, 10));  // Ограничиваем 1-10 единиц
}
