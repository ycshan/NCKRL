//
// Created by ycshan on 2018/11/23.
//

#ifndef NCKRL_PROGRESSBAR_H
#define NCKRL_PROGRESSBAR_H

#include <chrono>
#include <iostream>

class ProgressBar {
private:
    unsigned ticks = 0;

    const unsigned total_ticks;
    const unsigned bar_width;
    const char complete_char = '=';
    const char incomplete_char = ' ';
    const std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();

public:
    ProgressBar(unsigned total,unsigned width,char complete,
            char incomplete):total_ticks {total},bar_width {width},
            complete_char {complete},incomplete_char {incomplete} {}
    unsigned operator++() {
        return ++ticks;
    }
    inline void display();
    inline void done();
};

void ProgressBar::display() {
    float progress = (float) ticks / total_ticks;
    int pos = (int) (bar_width * progress);

    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
    auto time_elapsed = std::chrono::duration_cast<std::chrono::seconds>(now-start_time).count();
    unsigned hours = 0;
    if (time_elapsed > 3600)
        hours = (unsigned)(time_elapsed % 3600);
    std::cout << "[";

    for (int i = 0; i < bar_width; ++i) {
        if (i < pos) std::cout << complete_char;
        else if (i == pos) std::cout << ">";
        else std::cout << incomplete_char;
    }
    if (hours)
        std::cout << "] " << int(progress * 100.0) << "% " << hours <<"h"<< time_elapsed << "s\r";
    else
        std::cout << "] " << int(progress * 100.0) << "% " << time_elapsed << "s\r";
    std::cout.flush();
}

void ProgressBar::done() {
    display();
    std::cout << std::endl;
}

#endif //NCKRL_PROGRESSBAR_H
