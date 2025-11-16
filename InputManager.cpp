#include "InputManager.h"

InputManager::InputManager(Buffer& buffer) : buf(buffer) { }

bool InputManager::handleBufferFull(Request request) {

	// Удалять будем самую низкоприоритетную заявку из всех
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

	// Если буфер полон, значит нужно произвести выбивание заявки
	if (this->buf.isFull()) return handleBufferFull(request);

	// Если место есть, значит просто добавляем
	return this->buf.addRequest(request);
}

Buffer& InputManager::getBuffer() {
	return this->buf;
}
