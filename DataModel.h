//
// Created by ycshan on 2018/9/7.
//

#ifndef NCKRL_DATAMODEL_H
#define NCKRL_DATAMODEL_H

#include <iostream>
#include "Config.h"
#include <map>
#include <set>
#include <vector>
#include <fstream>
#include <sstream>
#include <cstring>

using namespace std;

class DataModel {
private:
    Data data_set;
    Parameter params;

    map<string,int> relation2id,entity2id; //<relation,ID>
    map<int,string> id2entity,id2relation; //<ID,relation>
    map<int,map<int,int> > left_entity,right_entity; //<relationID, <entityID, num>>
public:
    unsigned relation_num,entity_num;
    map<int,double> left_num,right_num;

    vector<pair<pair<int, int>, int> >	data_train; //(headID,tailID,relationID) of training set
    vector<pair<pair<int, int>, int> >	data_test;

    //<<headID,tailID>,<relationID,1>> indicates that (headID,tailID,relationID) exists
    map<pair<int,int>, map<int,int> > train_flg; //for training use only
    map<pair<int,int>, map<int,int> > all_flg; //for testing use only
public:
    DataModel(Data &p_data, Parameter &p_params);
    void prepare();
};

DataModel::DataModel(Data &p_data, Parameter &p_params)
        :data_set(p_data),params(p_params) {
    this->relation_num = 0;
    this->entity_num = 0;
    this->relation2id.clear();
    this->entity2id.clear();
    this->id2entity.clear();
    this->id2relation.clear();
    this->left_entity.clear();
    this->right_entity.clear();
    this->left_num.clear();
    this->right_num.clear();
    this->data_train.clear();
    this->data_test.clear();
    this->train_flg.clear();
    this->all_flg.clear();
}

void DataModel::prepare() {
    string buf;
    ifstream ifs_en((data_set.base_dir+"entity2id.txt").c_str(),std::ios::in);
    if(!ifs_en.is_open())
        cout<<"cannot open entity2id.txt!"<<endl;
    else {
        //build entity2ID and ID2entity map
        while(getline(ifs_en, buf)) {
            int x;
            string st;
            istringstream line(buf);
            line >> st >> x;
            entity2id[st] = x;
            id2entity[x] = st;
            entity_num++;
        }
    }
    ifs_en.close();

    ifstream ifs_rel((data_set.base_dir+"relation2id.txt").c_str(),std::ios::in);
    if(!ifs_rel.is_open())
        cout << "cannot open relation2id.txt!" << endl;
    else {
        //build relation2ID and ID2relation map
        while(getline(ifs_rel, buf)) {
            int x;
            string st;
            istringstream line(buf);
            line >> st >> x;
            relation2id[st] = x;
            id2relation[x] = st;
            relation_num++;
        }
    }
    ifs_rel.close();

    //load train
    ifstream ifs_train((data_set.base_dir+"train.txt").c_str(),std::ios::in);
    if(!ifs_train.is_open())
        cout << "cannot open train.txt!" << endl;
    else {
        while(getline(ifs_train, buf)) {
            string s1,s2,s3;
            istringstream line(buf);
            line >> s1 >> s2 >> s3;
            if (entity2id.count(s1) == 0) {
                cout << "miss entity:" << s1 << endl;
            }
            if (entity2id.count(s2) == 0) {
                cout << "miss entity:" << s2 << endl;
            }
            if (relation2id.count(s3) == 0) {
                relation2id[s3] = relation_num;
                relation_num++;
            }
            int e1 = entity2id[s1];
            int e2 = entity2id[s2];
            int rel = relation2id[s3];

            data_train.emplace_back(make_pair(make_pair(e1,e2),rel));
            train_flg[make_pair(e1,e2)][rel] = 1;
            all_flg[make_pair(e1,e2)][rel] = 1;

            left_entity[rel][e1]++; //<relationID, <entityID, num>>
            right_entity[rel][e2]++; //<relationID, <entityID, num>>
        }
    }
    ifs_train.close();

    //load noisy train
    ifstream ifs_neg((data_set.base_dir+"neg_train_"+params.noise_rate+".txt").c_str(),std::ios::in);
    if(!ifs_neg.is_open())
        cout << "cannot open neg_train_"+params.noise_rate+".txt!" << endl;
    else {
        while(getline(ifs_neg, buf)) {
            string s1,s2,s3;
            istringstream line(buf);
            line >> s1 >> s2 >> s3;
            if (entity2id.count(s1) == 0) {
                cout << "miss entity:" << s1 << endl;
            }
            if (entity2id.count(s2) == 0) {
                cout << "miss entity:" << s2 << endl;
            }
            int e1 = entity2id[s1];
            int e2 = entity2id[s2];
            int rel = relation2id[s3];

            data_train.emplace_back(make_pair(make_pair(e1,e2), rel));
            train_flg[make_pair(e1,e2)][rel] = 1;

            left_entity[rel][e1]++; //<relationID, <entityID, num>>
            right_entity[rel][e2]++; //<relationID, <entityID, num>>
        }
    }
    ifs_neg.close();

    //load valid
    ifstream ifs_valid((data_set.base_dir+"valid.txt").c_str(),std::ios::in);
    if(!ifs_valid.is_open())
        cout << "cannot open valid.txt!" << endl;
    else {
        while(getline(ifs_valid, buf)) {
            string s1,s2,s3;
            istringstream line(buf);
            line >> s1 >> s2 >> s3;
            if(entity2id.count(s1) == 0) {
                cout << "miss entity:" << s1 << endl;
            }
            if(entity2id.count(s2) == 0) {
                cout << "miss entity:" << s2 << endl;
            }
            if(relation2id.count(s3) == 0) {
                relation2id[s3] = relation_num;
                relation_num++;
            }
            int e1 = entity2id[s1];
            int e2 = entity2id[s2];
            int rel = relation2id[s3];
            all_flg[make_pair(e1,e2)][rel] = 1;
        }
    }
    ifs_valid.close();

    //load test
    ifstream ifs_test((data_set.base_dir+"test.txt").c_str(),std::ios::in);
    if(!ifs_test.is_open())
        cout << "cannot open test.txt!" << endl;
    else {
        while(getline(ifs_test, buf)) {
            string s1,s2,s3;
            istringstream line(buf);
            line >> s1 >> s2 >> s3;
            if(entity2id.count(s1) == 0) {
                cout << "miss entity:" << s1 << endl;
            }
            if(entity2id.count(s2) == 0) {
                cout << "miss entity:" << s2 << endl;
            }
            if(relation2id.count(s3) == 0) {
                relation2id[s3] = relation_num;
                relation_num++;
            }
            int e1 = entity2id[s1];
            int e2 = entity2id[s2];
            int rel = relation2id[s3];

            data_test.emplace_back(make_pair(make_pair(e1,e2), rel));
            all_flg[make_pair(e1,e2)][rel] = 1;
        }
    }
    ifs_test.close();

    for (int i = 0; i < relation_num; i++) {
        double sum1 = 0, sum2 = 0;
        for (auto it : left_entity[i]) {
            sum1++;
            sum2 += it.second;
        }
        left_num[i] = sum2/sum1;
    }

    for (int i = 0; i < relation_num; i++) {
        double sum1 = 0, sum2 = 0;
        for (auto it : right_entity[i]) {
            sum1++;
            sum2 += it.second;
        }
        right_num[i] = sum2/sum1;
    }
    //output
    ofstream of;
    of.open((data_set.report_dir+"experiment_"+params.detail).c_str(),std::ios::app);
    of << "relation number: " << relation_num << endl;
    of << "entity number: " << entity_num << endl;
    of.close();
}

#endif //NCKRL_DATAMODEL_H
