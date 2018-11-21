//
// Created by ycshan on 9/6/2018.
//

#ifndef NCKRL_TRAIN_H
#define NCKRL_TRAIN_H

#include <vector>
#include <map>
#include <cstdlib>
#include <cmath>
#include "Config.h"
#include "Utilities.h"

using namespace std;

class Train {
private: //need to be initialized
    Data data_set;
    Parameter params;
    unsigned relation_num,entity_num;
    vector<pair<pair<int, int>, int> >	data_train; //(headID,tailID,relationID) of training set
    map<pair<int,int>, map<int,int> > train_exist; //check whether a triple is already existed
    map<int,double> left_num,right_num;
private:
    double val_loss; //loss function value
    double conf_step = 0.0001; //defined in CKRL
    vector<vector<double> > relation_vec,entity_vec; //relation/entity embedding
    vector<vector<double> > relation_tmp,entity_tmp;
    vector<double> rate_confidence; //local triple confidence
    vector<pair<double,double> > neg_confidence; //negative triple confidence && its quality
    vector<pair<double, double> > neg_exp; //used for soft max function
public:
    Train(Data& p_data, Parameter& p_param, unsigned&rel_num,
            unsigned&ent_num,vector<pair<pair<int, int>, int> >& d_train,
          map<pair<int,int>, map<int,int> >& t_exist,
          map<int,double>& l_num, map<int,double>& r_num);
    void run();
private:
    double norm(vector<double> &a);
    void sgd();
    double calc_sum(int e1,int e2,int rel);
    double expected_value();
    void gradient(int e1_a,int e2_a,int rel_a,int e1_b,int e2_b,int rel_b,double tri_conf);
    void train_kb(int e1_a,int e2_a,int rel_a,int e1_b,int e2_b,int rel_b,int tri_num,bool rel_flag=false);
};

#endif //NCKRL_TRAIN_H
