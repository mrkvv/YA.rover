#include "Buffer.h"

Buffer::Buffer() {
	this->slots.resize(this->CAPACITY);
	this->freeSlotPointer = 0;
}

int Buffer::findFirstFreeSlot() {
    for (int i = 0; i < CAPACITY; i++) {
        if (!slots[i].has_value()) {
            return i;
        }
    }
    return -1;  // Буфер полон
}

std::optional<Request> Buffer::getLowestPriorityRequest() {
    if (this->isEmpty()) return std::nullopt;

    Request r(-1, 0, INT_MAX, "");

    for (int i = 0; i < CAPACITY; i++) {
        if (slots[i].has_value() && slots[i].value().getPriority() >= r.getPriority()) {
            r = slots[i].value();
        }
    }
    return r;
}

int Buffer::getHighestPriorityPackage() {
    if (this->isEmpty()) return -1;

    int temp = 4;
    for (int i = 0; i < CAPACITY; i++) {
        if (slots[i].has_value() && slots[i].value().getPriority() < temp) {
            temp = slots[i].value().getPriority();
        }
    }

    return temp;
}

std::optional<Request> Buffer::getHighestPriorityRequestInPackage(int packageId) {
    if (this->isEmpty()) return std::nullopt;

    Request r(-1, packageId, INT_MAX, "");

    for (int i = 0; i < CAPACITY; i++) {
        if (slots[i].has_value()
            && slots[i].value().getPriority() == packageId
            && slots[i].value().getGenerationTime() <= r.getGenerationTime()
            ) {
            r = slots[i].value();
        }
    }

    if (r.getId() == -1) return std::nullopt;

    return r;
}

bool Buffer::addRequest(Request request) {
    if (this->isFull()) return false;

    // Заносим заявку буквально вот так, руками отыскивая нужное первое свободное место от начала
    slots[this->findFirstFreeSlot()] = request;
    return true;
}

bool Buffer::removeRequest(Request request) {
    if (this->isEmpty()) return true;

    for (int i = 0; i < CAPACITY; i++) {
        if (slots[i].has_value()
            && slots[i].value().getGenerationTime() == request.getGenerationTime()
            && slots[i].value().getDeliveryAdress() == request.getDeliveryAdress()
            ) {
            slots[i] = std::nullopt;
            return true;
        }
    }
    return false;
}

std::optional<Request> Buffer::removeRequest(int index) {
    if (index < 0 || index >= CAPACITY || this->isEmpty()) return std::nullopt;

    std::optional<Request> r = slots[index];

    if (slots[index].has_value()) {
        slots[index] = std::nullopt;
    }
    return r;
}

bool Buffer::isEmpty() {
    for (int i = 0; i < CAPACITY; i++) {
        if (slots[i].has_value()) {
            return false;
        }
    }
    return true;
}

bool Buffer::isFull() {
    for (int i = 0; i < CAPACITY; i++) {
        if (!slots[i].has_value()) {
            return false;
        }
    }
    return true;
}

void Buffer::printBuffer() {
    std::cout << "---BUFFER:\n";
    for (int i = 0; i < CAPACITY; i++) {
        if (slots[i].has_value()) {
            std::cout << "---" << i << ". " << slots[i].value() << "\n";
        }
        else {
            std::cout << "---" << i << ". null\n";
        }
    }
}

