#include "InputManager.h"

InputManager::InputManager(Buffer& buffer) : buf(buffer) { }

bool InputManager::handleBufferFull(Request request) {

	// ”дал€ть будем самую низкоприоритетную за€вку из всех
	auto requestForDelete = this->buf.getLowestPriorityRequest();

	if (!requestForDelete.has_value()) {
		std::cout << "REQUEST DENY ERROR: Buffer full but no request to remove!" << std::endl;
		return false;
	}
	else if (this->buf.removeRequest(requestForDelete.value())) {
		std::cout << "REQUEST DENY: deleted " << requestForDelete.value() << "\n";
		std::cout << "REQUEST DENY: added " << request << "\n";
		return this->buf.addRequest(request);
	}
	
	return false;
}

bool InputManager::processRequest(Request request) {
	std::cout << "Recieved " << request << "\n";

	// ≈сли буфер полон, значит нужно произвести выбивание за€вки
	if (this->buf.isFull()) return handleBufferFull(request);

	// ≈сли место есть, значит просто добавл€ем
	return this->buf.addRequest(request);
}

Buffer& InputManager::getBuffer() {
	return this->buf;
}
