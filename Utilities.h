//
// Created by ycshan on 2018/9/6.
//
#pragma once
#ifndef NCKRL_UTILITIES_H
#define NCKRL_UTILITIES_H

#define qNULL (-999)
#define pi 3.14159265358979

#include <vector>
#include <cmath>
#include <string>
#include <cstdlib>
#include <random>

using namespace std;

namespace Utilities {

    inline double vec_len(vector<double> &a) {
        double res = 0;
        for (double val : a)
            res += val*val;
        return sqrt(res);
    }

    inline double sqr(double x) {
        return x*x;
    }

    inline double cmp(pair<int,double> a, pair<int,double> b) {
        return a.second < b.second;
    }

    inline int rand_max(unsigned long x) {
        random_device rd;
        unsigned long temp = rd()%x;
        return (int)temp;
    }

    //rand of uniform distribution
    inline double rand_uniform(double min, double max) {
        random_device rd;
        return min+(max-min)*rd()/(random_device::max()+1.0);
    }

    inline double normal(double x, double miu, double sigma)
    {
        return 1.0/sqrt(2*pi)/sigma*exp(-1*(x-miu)*(x-miu)/(2*sigma*sigma));
    }

    //rand of normal distribution
    inline double rand_normal(double miu, double sigma, double min, double max) {
        double x,y,scope;
        do {
            x = rand_uniform(min, max);
            y = normal(x, miu, sigma);
            scope = rand_uniform(0.0, normal(miu, miu, sigma));
        } while(scope > y);
        return x;
    }
}
#endif //NCKRL_UTILITIES_H
