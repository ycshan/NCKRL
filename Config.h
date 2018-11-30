//
// Created by ycshan on 2018/9/5.
//

#ifndef NCKRL_CONFIG_H
#define NCKRL_CONFIG_H

#include <string>

using namespace std;

class Data {
public:
    string base_dir;
    string report_dir;
    string train;
    string train_neg;
    string valid;
    string test;
    string entity2id;
    string relation2id;

public:
    Data(const string& d_base_dir,
         const string& d_report_dir,
         const string& d_train,
         const string& d_train_neg,
         const string& d_valid,
         const string& d_test,
         const string& d_entity2id,
         const string& d_relation2id) {
        this->base_dir = d_base_dir;
        this->report_dir=d_report_dir;
        this->train = d_train;
        this->train_neg = d_train_neg;
        this->valid = d_valid;
        this->test = d_test;
        this->entity2id = d_entity2id;
        this->relation2id = d_relation2id;
    }
};

class Parameter {
public:
    unsigned epoch;
    unsigned dim;
    double l_rate;
    double margin;
    string method;
    string noise_rate;
    int ng_num;
    bool l1_flag;
    bool pre_flag;
    string detail;
    string report;

public:
    Parameter(const unsigned& p_epoch,
              const unsigned& p_dim,
              const double& p_l_rate,
              const double& p_margin,
              const string& p_method,
              const string& p_noise_rate,
              const int& p_ng_num,
              const bool& p_l1_flag,
              const bool& p_pre_flag) {
        this->epoch = p_epoch;
        this->dim = p_dim;
        this->l_rate = p_l_rate;
        this->margin = p_margin;
        this->method = p_method;
        this->noise_rate = p_noise_rate;
        this->ng_num = p_ng_num;
        this->l1_flag = p_l1_flag;
        this->pre_flag = p_pre_flag;
        this->detail = "n"+noise_rate+"-l"+to_string(l_rate)+"-r"+
                to_string(margin)+"-c"+to_string(ng_num);
        this->report = "report_"+detail;
    }
};

#endif //NCKRL_CONFIG_H
