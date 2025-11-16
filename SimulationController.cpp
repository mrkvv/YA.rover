#include "SimulationController.h"
#include <algorithm>

SimulationController::SimulationController() 
	: buf(), im(buf), om(buf), currentTime(0), hConsole(GetStdHandle(STD_OUTPUT_HANDLE)) 
{
	sources.reserve(3);
	for (int i = 0; i < 3; ++i) {
		sources.emplace_back(i + 1, START_TIME, END_TIME);
	}

	requests.reserve(3 * REQUESTS_PER_SOURCE_AMOUNT);

	// Генерируем заявки (типо симулируем будущее)
	for (int i = 0; i < 3; ++i) {
		auto rs = sources[i].generatePackOfRequests(REQUESTS_PER_SOURCE_AMOUNT);
		requests.insert(requests.end(), rs.begin(), rs.end());
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
	std::cout << "------------ Time: " << currentTime << " units ------------\n";

	processIncomingRequest();
	processRequestsInBuffer();

	currentTime++;
	return (currentTime - 2 > (END_TIME - START_TIME));
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
		im.processRequest(req);
	}
}

void SimulationController::processRequestsInBuffer() {
	// Начинаем обработку заявок в буфере и отправку их на выполнение

	// Актуализируем информацию о Роверах - какие сейчас свободны, а какие заняты.
	SetConsoleTextAttribute(hConsole, 10);
	om.updateRoversState(currentTime);

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
