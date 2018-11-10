//
// Created by ycshan on 2018/9/6.
//
#pragma once
#ifndef NCKRL_UTILITIES_H
#define NCKRL_UTILITIES_H

#define qNULL (-999)

#include <vector>
#include <cmath>
#include <string>
#include <cstdlib>
#include <random>

using namespace std;

namespace Utilities{

    inline double vec_len(vector<double> &a)
    {
        double res=0;
        for(unsigned i=0; i<a.size(); i++)
            res+=a[i]*a[i];
        return sqrt(res);
    }

    inline double sqr(double x)
    {
        return x*x;
    }

    inline double cmp(pair<int,double> a, pair<int,double> b)
    {
        return a.second<b.second;
    }

    inline int rand_max(unsigned long x)
    {
        random_device rd;
        unsigned long temp = (rd()*rd())%x;
        while(temp < 0)
            temp += x;
        return (int)temp;
    }

}
#endif //NCKRL_UTILITIES_H
