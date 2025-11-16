#include "OutputManager.h"

OutputManager::OutputManager(Buffer& buffer) : buf(buffer) {
	for (int i = 0; i < ROVERS_AMOUNT; i++) {
		rovers.emplace_back(i);
	}
	this->currentRoverPointer = 0;
	this->highestPriorityPackage = 1; // Изначально ставим самый приоритетный первый пакет
}

std::optional<Request> OutputManager::selectRequest() {
	// Пытаемся получить заявку из текущего самого приоритетного пакета
	std::optional<Request> toDo = buf.getHighestPriorityRequestInPackage(highestPriorityPackage);

	// Если мы не получили эту заявку ИЛИ получили, но не валидную,
	// то это либо ошибка, либо что, скорее всего, в текущем самом приоритетном пакете закончились заявки
	// значит нужно избрать новый самый приоритетный пакет и уже из него вытащить заявку
	if (!toDo.has_value() || toDo.value().getId() == -1) {
		this->highestPriorityPackage = buf.getHighestPriorityPackage(); // Перевыбираем пакет
		toDo = buf.getHighestPriorityRequestInPackage(highestPriorityPackage); // Берем из него самую приоритетную заявку
	}

	// Возвращаем почти всегда гарантированно именно заявку, а не nullopt
	return toDo;
}

void OutputManager::updateRoversState(int currentTime) {
	// Просто периодически проверяем, не настало ли сейчас теоритеческое время окончания
	// Если настало, то помечаем ровера как FREE и выводим в консоль
	for (int i = 0; i < ROVERS_AMOUNT; i++) {
		if (rovers[i].getEndTime() == currentTime) rovers[i].completeService();
	}
}

Rover* OutputManager::selectRover() {
	int checked = 0;

	while (checked < ROVERS_AMOUNT) {
		Rover* rover = &rovers[currentRoverPointer];
		
		if (rover->isAvailable()) {
			currentRoverPointer = (currentRoverPointer + 1) % ROVERS_AMOUNT;
			return rover;
		}
		currentRoverPointer = (currentRoverPointer + 1) % ROVERS_AMOUNT;
		checked++;
	}
	return nullptr; // Не нашли свободных роверов
}

void OutputManager::startService(Rover* rover, Request request, int currentTime) {
	if (rover != nullptr) {
		rover->startService(request, currentTime);
		buf.removeRequest(request);
	}
}

void OutputManager::printRoversInfo() {
	std::cout << "ROVERS INFO (pointer: " << currentRoverPointer << ", highest pr package: " << highestPriorityPackage << "): [";
	for (int i = 0; i < ROVERS_AMOUNT - 1; i++) {
		Rover r = rovers[i];
		std::cout << "Rover#" << r.getId() << "-" << (r.isAvailable() ? "FREE" : "BUSY") << ", ";
	}
	std::cout << "Rover#" << rovers[ROVERS_AMOUNT - 1].getId() << "-" << (rovers[ROVERS_AMOUNT - 1].isAvailable() ? "FREE" : "BUSY") << "]\n";
}

bool OutputManager::isAnyRoverAvailable() {
	for (int i = 0; i < ROVERS_AMOUNT; i++) {
		if (rovers[i].isAvailable()) {
			return true;
		}
	}
	return false;
}
