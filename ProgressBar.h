//
// Created by ycshan on 2018/11/23.
//

#ifndef NCKRL_PROGRESSBAR_H
#define NCKRL_PROGRESSBAR_H

#include <chrono>
#include <iostream>

using namespace std;

class ProgressBar {
private:
    unsigned ticks = 0;

    const unsigned total_ticks;
    const unsigned bar_width;
    const char complete_char = '=';
    const char incomplete_char = ' ';
    const chrono::steady_clock::time_point start_time = chrono::steady_clock::now();

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
    auto pos = (int) (bar_width * progress);

    chrono::steady_clock::time_point now = chrono::steady_clock::now();
    auto time_elapsed = chrono::duration_cast<chrono::seconds>(now-start_time).count();
    unsigned hours = 0;
    if (time_elapsed > 3600) {
        hours = (unsigned)(time_elapsed / 3600);
        time_elapsed = (time_elapsed % 3600);
    }
    cout << "[";

    for (int i = 0; i < bar_width; ++i) {
        if (i < pos) cout << complete_char;
        else if (i == pos) cout << ">";
        else cout << incomplete_char;
    }
    if (hours)
        cout << "] " << int(progress * 100.0) << "% " << hours <<"h "<< time_elapsed << "s\r";
    else
        cout << "] " << int(progress * 100.0) << "% " << time_elapsed << "s\r";
    cout.flush();
}

void ProgressBar::done() {
    display();
    cout << endl;
}

#endif //NCKRL_PROGRESSBAR_H
