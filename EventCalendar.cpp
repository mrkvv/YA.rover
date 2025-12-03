#include "EventCalendar.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <iomanip>

void EventCalendar::addEvent(int time, const std::string& type,
    const std::string& description, const std::string& component, double order) {
    events.emplace_back(time, type, description, component, order);
}

void EventCalendar::printCalendar() const {
    std::ofstream file("event_calendar.txt");

    if (!file.is_open()) {
        std::cout << "Ошибка открытия файла для записи календаря!" << std::endl;
        return;
    }

    file << "\n";
    file << "---------------------------------------------------------------------------------------------------\n";
    file << "                                         КАЛЕНДАРЬ СОБЫТИЙ                                         \n";
    file << "----------------------------------------------------------------------------------------------------\n";
    file << "  Время  |              Тип              |        Компонент        |           Описание            \n";
    file << "---------|-------------------------------|-------------------------|-------------------------------\n";

    auto sortedEvents = events;
    std::sort(sortedEvents.begin(), sortedEvents.end(),
        [](const Event& a, const Event& b) { 
            if (a.time != b.time) return a.time < b.time;
            if (a.orderInOutput != b.orderInOutput) return a.orderInOutput < b.orderInOutput;
            return a.type < b.type;
        });

    int prevTime = 0;
    for (const auto& event : sortedEvents) {
        if (prevTime < event.time) {
            file << "---------------------------------------------------------------------------------------------------\n";
            prevTime = event.time;
        }
        file << std::setw(8) << event.time << " |"
            << std::setw(30) << event.type << " |"
            << std::setw(24) << event.component << " | "
            << event.description << std::endl;
    }

    file << "---------------------------------------------------------------------------------\n";
    file << "Всего событий: " << events.size() << std::endl;

    file.close();
}
