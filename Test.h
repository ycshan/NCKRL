//
// Created by ycshan on 2018/9/6.
//

#ifndef NCKRL_TEST_H
#define NCKRL_TEST_H

#include <iostream>
#include <cstring>
#include <cstdio>
#include <map>
#include <vector>
#include <set>
#include <string>
#include <ctime>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <sstream>
#include <fstream>
#include "Config.h"
#include "Utilities.h"

using namespace std;

class Test {
private:
    Data data_set;
    Parameter params;
    unsigned relation_num,entity_num;
    vector<vector<double> > relation_vec,entity_vec;
    vector<pair<pair<int, int>, int> >	data_test; //(headID,tailID,relationID) of test set

    //check whether a triple is already existed in train, valid and test
    map<pair<int,int>, map<int,int> > all_exist;
public:
    Test(Data& p_data, Parameter& p_param, unsigned&rel_num,
    unsigned&ent_num, vector<pair<pair<int, int>, int> >& d_test,
    map<pair<int,int>, map<int,int> >& d_all_exist);
    void link_prediction();
private:
    double calc_sum(int e1,int e2,int rel);
};


#endif //NCKRL_TEST_H
