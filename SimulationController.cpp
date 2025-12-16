#include "SimulationController.h"
#include <algorithm>
#include <numeric>
#include <iomanip>

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
	return currentTime > ACTUAL_TIME_TO_STOP_SIMULATION;
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
		!im.processRequest(req);
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

			ServiceResults serviceResults = om.startService(selectedRover, req.value(), currentTime);
			int serviceTime = serviceResults.serviceTime;
			int inBufferAwaitingTime = serviceResults.inBufferAwaitingTime;

			// Статистика для автоматического режима
			if (req.value().getPriority() == 1) {
				this->info.serviceTimes1.push_back(serviceTime);
				this->info.bufferAwaitingTimes1.push_back(inBufferAwaitingTime);
			}
			else if (req.value().getPriority() == 2) {
				this->info.serviceTimes2.push_back(serviceTime);
				this->info.bufferAwaitingTimes2.push_back(inBufferAwaitingTime);
			}
			else if (req.value().getPriority() == 3) {
				this->info.serviceTimes3.push_back(serviceTime);
				this->info.bufferAwaitingTimes3.push_back(inBufferAwaitingTime);
			}
		}
		else break; // Иначе просто выходим из цикла. Такое скорее всего произойдет из-за занятости всех роверов.
	}
	
	// На каждом шаге будем выводить текущее состояние с нашим крутым цветом)
	SetConsoleTextAttribute(hConsole, 15);
	buf.printBuffer();
	om.printRoversInfo();
}

void SimulationController::printAutoModeResults() {
	std::cout << "\n\n------------------------------------------------------------------------------------------------------------------\n";
	std::cout << "----------------------------------------------AUTO MODE RESULTS---------------------------------------------------\n";

	int totalRequests = REQUESTS_PER_SOURCE_AMOUNT;

	double p1 = im.deniesCounter[0] / totalRequests;
	double p2 = im.deniesCounter[1] / totalRequests;
	double p3 = im.deniesCounter[2] / totalRequests;

	double avgServiceTime1 = std::accumulate(this->info.serviceTimes1.begin(), this->info.serviceTimes1.end(), 0.0) / totalRequests;
	double avgServiceTime2 = std::accumulate(this->info.serviceTimes2.begin(), this->info.serviceTimes2.end(), 0.0) / totalRequests;
	double avgServiceTime3 = std::accumulate(this->info.serviceTimes3.begin(), this->info.serviceTimes3.end(), 0.0) / totalRequests;

	double avgInBufferAwaitingTime1 = 
		std::accumulate(this->info.bufferAwaitingTimes1.begin(), this->info.bufferAwaitingTimes1.end(), 0.0) / totalRequests;
	double avgInBufferAwaitingTime2 =
		std::accumulate(this->info.bufferAwaitingTimes2.begin(), this->info.bufferAwaitingTimes2.end(), 0.0) / totalRequests;
	double avgInBufferAwaitingTime3 =
		std::accumulate(this->info.bufferAwaitingTimes3.begin(), this->info.bufferAwaitingTimes3.end(), 0.0) / totalRequests;

	double Tservice1 = avgServiceTime1 + avgInBufferAwaitingTime1;
	double Tservice2 = avgServiceTime2 + avgInBufferAwaitingTime2;
	double Tservice3 = avgServiceTime3 + avgInBufferAwaitingTime3;

	double dispServiceTime1 = calculateDispersion(info.serviceTimes1, avgServiceTime1);
	double dispServiceTime2 = calculateDispersion(info.serviceTimes2, avgServiceTime2);
	double dispServiceTime3 = calculateDispersion(info.serviceTimes3, avgServiceTime3);

	double dispBufferAwaitingTime1 = calculateDispersion(info.bufferAwaitingTimes1, avgInBufferAwaitingTime1);
	double dispBufferAwaitingTime2 = calculateDispersion(info.bufferAwaitingTimes2, avgInBufferAwaitingTime2);
	double dispBufferAwaitingTime3 = calculateDispersion(info.bufferAwaitingTimes3, avgInBufferAwaitingTime3);

	// Заголовок таблицы
	std::cout << std::left
		<< std::setw(10) << "Source#"
		<< std::setw(12) << "Req amount"
		<< std::setw(12) << "P_deny"
		<< std::setw(12) << "T_in"
		<< std::setw(18) << "T_in_buffer"
		<< std::setw(15) << "T_in_service"
		<< std::setw(20) << "Disp_in_buffer"
		<< std::setw(18) << "Disp_in_service"
		<< "\n";

	// Строки таблицы
	std::cout << std::left
		<< std::setw(10) << "Source1"
		<< std::setw(12) << totalRequests
		<< std::setw(12) << std::fixed << std::setprecision(4) << p1
		<< std::setw(12) << std::fixed << std::setprecision(2) << Tservice1
		<< std::setw(18) << std::fixed << std::setprecision(2) << avgInBufferAwaitingTime1
		<< std::setw(15) << std::fixed << std::setprecision(2) << avgServiceTime1
		<< std::setw(20) << std::fixed << std::setprecision(2) << dispBufferAwaitingTime1
		<< std::setw(18) << std::fixed << std::setprecision(2) << dispServiceTime1
		<< "\n";

	std::cout << std::left
		<< std::setw(10) << "Source2"
		<< std::setw(12) << totalRequests
		<< std::setw(12) << std::fixed << std::setprecision(4) << p2
		<< std::setw(12) << std::fixed << std::setprecision(2) << Tservice2
		<< std::setw(18) << std::fixed << std::setprecision(2) << avgInBufferAwaitingTime2
		<< std::setw(15) << std::fixed << std::setprecision(2) << avgServiceTime2
		<< std::setw(20) << std::fixed << std::setprecision(2) << dispBufferAwaitingTime2
		<< std::setw(18) << std::fixed << std::setprecision(2) << dispServiceTime2
		<< "\n";

	std::cout << std::left
		<< std::setw(10) << "Source3"
		<< std::setw(12) << totalRequests
		<< std::setw(12) << std::fixed << std::setprecision(4) << p3
		<< std::setw(12) << std::fixed << std::setprecision(2) << Tservice3
		<< std::setw(18) << std::fixed << std::setprecision(2) << avgInBufferAwaitingTime3
		<< std::setw(15) << std::fixed << std::setprecision(2) << avgServiceTime3
		<< std::setw(20) << std::fixed << std::setprecision(2) << dispBufferAwaitingTime3
		<< std::setw(18) << std::fixed << std::setprecision(2) << dispServiceTime3
		<< "\n";

	printRoversTableInAutoModeStatistics();
	std::cout << "P_deny in total: " << (p1 + p2 + p3) / 3 << "\n";
	std::cout << "T_in in total: " << (Tservice1 + Tservice2 + Tservice3) / 3 << "\n";
}

double SimulationController::calculateDispersion(const std::vector<double>& values, double average) {
	if (values.empty() || values.size() < 2) {
		return 0.0;
	}

	double sumOfSquares = 0.0;
	for (double value : values) {
		double dif = value - average;
		sumOfSquares += dif * dif;
	}

	// Несмещенная оценка дисперсии
	return sumOfSquares / (values.size() - 1);
}

void SimulationController::printRoversTableInAutoModeStatistics() {
	std::cout << "\n\n------------------------------------------------------------------------------------------------------------------\n";
	std::cout << "-------------------------------------------ROVERS CHARACTERISTICS-------------------------------------------------\n";

	std::cout << std::left
		<< std::setw(15) << "Rover #"
		<< std::setw(25) << "Load coefficient"
		<< "\n";

	double avgK_load = 0;

	for (size_t i = 0; i < om.roversStats.busyInTotals.size(); i++) {
		double loadCoefficient = (om.roversStats.busyInTotals[i] / (END_TIME + END_TIME/4)) > 1.0 ?
			1 : (om.roversStats.busyInTotals[i] / (END_TIME + END_TIME / 4));

		avgK_load += loadCoefficient;

		std::cout << std::left
			<< std::setw(15) << i + 1
			<< std::setw(25) << loadCoefficient
			<< "\n";
	}
	avgK_load /= om.roversStats.busyInTotals.size();
	std::cout << "\nAverage load coefficient - " << avgK_load << "\n";
}
