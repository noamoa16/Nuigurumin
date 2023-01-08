#include "Timer.h"

void Timer::start(){
    start_t = clock();
    isActive = true;
}
void Timer::stop(){
    stop_t = clock();
    isActive = false;
}
int Timer::toMilisec() const{
    return isActive ? clock() - start_t : stop_t - start_t;
}