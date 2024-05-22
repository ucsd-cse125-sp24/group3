#pragma once

#include <chrono>
#include <functional>
#include <thread>
#include <iostream>

template< class T, class P >
void setTimer(std::chrono::duration<T,P> duration, std::function<void()> callback) {
    std::thread timer([duration, callback]() {
        std::this_thread::sleep_for(duration);
        callback(); 
    });
    timer.detach();
}
