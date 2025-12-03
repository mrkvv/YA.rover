#include "SimulationController.h"
#include <algorithm>

SimulationController::SimulationController() 
	: buf(), im(buf, calendar), om(buf, calendar), currentTime(START_TIME), hConsole(GetStdHandle(STD_OUTPUT_HANDLE)), calendar(EventCalendar())
{
	sources.reserve(3);
	for (int i = 0; i < 3; ++i) {
		sources.emplace_back(i + 1, START_TIME, END_TIME);
	}

	requests.reserve(3 * REQUESTS_PER_SOURCE_AMOUNT);

	std::string descriptionForCalendar;

	// Генерируем заявки (типо симулируем будущее)
	for (int i = 0; i < 3; ++i) {
		auto rs = sources[i].generatePackOfRequests(REQUESTS_PER_SOURCE_AMOUNT);
		requests.insert(requests.end(), rs.begin(), rs.end());

		for (Request& r : rs) {
			descriptionForCalendar = "Source#" + std::to_string(i + 1) + " Request#" + std::to_string(r.getId());
			calendar.addEvent(r.getGenerationTime(), "Создание заявки", descriptionForCalendar, "Источник", 0.0);
		}
	}

	std::sort(requests.begin(), requests.end(),
		[](const Request& a, const Request& b) {
			return a.getGenerationTime() < b.getGenerationTime();
		}
	);
}

bool SimulationController::makeStep() {
	// Для читаемости будем менять цвет выводимого текста в консоли на свой
	SetConsoleTextAttribute(hConsole, 15);
	std::cout << "\n------------ Time: " << currentTime << " units ------------\n";

	processIncomingRequest();
	processRequestsInBuffer();

	currentTime++;
	return (currentTime - (int) END_TIME/4 > (END_TIME - START_TIME));
}

void SimulationController::processIncomingRequest() {

	SetConsoleTextAttribute(hConsole, 11);

	std::vector<Request> foundRequests;

	// Получаем все созданные в наше время заявки
	std::copy_if(requests.begin(), requests.end(),
		std::back_inserter(foundRequests),
		[this](const Request& req) {
			return req.getGenerationTime() == this->currentTime;
		});

	// Обрабатываем каждую заявку
	for (const auto& req : foundRequests) {
		if (!im.processRequest(req)) deniedRequests++;
	}
}

void SimulationController::processRequestsInBuffer() {
	// Начинаем обработку заявок в буфере и отправку их на выполнение

	// Актуализируем информацию о Роверах - какие сейчас свободны, а какие заняты.
	SetConsoleTextAttribute(hConsole, 10);
	passedRequests += om.updateRoversState(currentTime);

	while (!buf.isEmpty()) {
		std::optional<Request> req = om.selectRequest(); // Вытаскиваем заявку из буфера
		Rover* selectedRover = om.selectRover(); // По кольцу выбираем первый свободный ровер

		// Если мы получили заявку И есть свободный ровер И мы выяснили какой ровер должен выполять заказ,
		// то выполняем заказ
		SetConsoleTextAttribute(hConsole, 14);
		if (req.has_value() && om.isAnyRoverAvailable() && selectedRover != nullptr) {
			om.startService(selectedRover, req.value(), currentTime);
		}
		else break; // Иначе просто выходим из цикла. Такое скорее всего произойдет из-за занятости всех роверов.
	}
	
	// На каждом шаге будем выводить текущее состояние с нашим крутым цветом)
	SetConsoleTextAttribute(hConsole, 15);
	buf.printBuffer();
	om.printRoversInfo();
}

void SimulationController::printAutoModeResults() {
	std::cout << "\n\n------------------------------------------------------------------------------\n";
	std::cout << "----------------------------AUTO MODE RESULTS---------------------------------\n";
	std::cout << "TOTAL REQUESTS: " << requestsInSum << "\n";
	std::cout << "PASSED REQUESTS: " << passedRequests << "\n";
	std::cout << "DENIED REQUESTS: " << deniedRequests << "\n";
}
