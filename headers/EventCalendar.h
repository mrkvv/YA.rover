#pragma once
#include <vector>
#include <string>

struct Event {
    int time;
    std::string type;
    std::string description;
    std::string component;  // Кто инициировал событие
    double orderInOutput;

    Event(int t, const std::string& ty, const std::string& desc, const std::string& comp, double order)
        : time(t), type(ty), description(desc), component(comp), orderInOutput(order) {
    }
};

class EventCalendar {
private:
    std::vector<Event> events;

public:
    void addEvent(int time, const std::string& type,
        const std::string& description, const std::string& component, double order);
    void printCalendar() const;
};