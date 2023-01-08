#pragma once
#include <ctime>

class Timer{
    public:
    void start();
    void stop();
    int toMilisec() const;
    private:
    clock_t start_t, stop_t;
    bool isActive = false;
};