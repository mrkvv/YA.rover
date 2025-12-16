#include "InputManager.h"

InputManager::InputManager(Buffer& buffer, EventCalendar& cal) : buf(buffer), calendar(cal) { }

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

		std::string description = "Была удалена заявка от source" + std::to_string(requestForDelete.value().getPriority())
			+ " на время " + std::to_string(requestForDelete.value().getGenerationTime());
		calendar.addEvent(request.getGenerationTime(), "Выбивание заявки", description, "Диспетчер постановки", 2.0);

		// Пишем для статистики в сумму отказанных заявок источника
		deniesCounter[requestForDelete.value().getPriority() - 1]++;

		return this->buf.addRequest(request);
	}
	
	return false;
}

bool InputManager::processRequest(Request request) {
	std::cout << "Recieved " << request << "\n";

	// Если буфер полон, значит нужно произвести выбивание заявки
	if (this->buf.isFull()) {
		handleBufferFull(request);
		return false;
	}

	// Если место есть, значит просто добавляем
	std::string description = "Добавлена заявка Source#" + std::to_string(request.getPriority())
		+ " Request#" + std::to_string(request.getId());
	calendar.addEvent(request.getGenerationTime(), "Добавление заявки в буфер", description, "Диспетчер постановки", 1.0);
	this->buf.addRequest(request);
	return true;
}

Buffer& InputManager::getBuffer() {
	return this->buf;
}
