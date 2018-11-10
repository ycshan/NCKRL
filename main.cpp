#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <fstream>
#include <sstream>
#include <cstring>
#include "Config.h"
#include "DataModel.h"
#include "Train.h"
#include "Test.h"

using namespace std;

int arg_pos(char *str, int argc, char **argv) {
    int a;
    for (a = 1; a < argc; a++) if (!strcmp(str, argv[a])) {
            if (a == argc - 1) {
                printf("argument missing for %s\n", str);
                exit(1);
            }
            return a;
        }
    return -1;
}

int main(int argc, char **argv) {
    srand((unsigned) time(nullptr));

    // default parameters
    int epochs = 100;
    unsigned dimension = 50;
    double margin = 3;
    double l_rate = 0.003;
    string method = "bern"; // bern or unif
    string dataset = "FB15K"; // FB15K or WN18
    string noise_rate = "10"; // [10, 20, 40]
    int ng_num = 10; // candidates

    int i;
    if ((i = arg_pos((char *)"-dim", argc, argv)) > 0) dimension = (unsigned)strtol(argv[i + 1], nullptr, 10);
    if ((i = arg_pos((char *)"-margin", argc, argv)) > 0) margin = strtod(argv[i + 1], nullptr);
    if ((i = arg_pos((char *)"-lrate", argc, argv)) > 0) l_rate = strtod(argv[i + 1], nullptr);
    if ((i = arg_pos((char *)"-method", argc, argv)) > 0) method = argv[i + 1];
    if ((i = arg_pos((char *)"-dataset", argc, argv)) > 0) dataset = argv[i + 1];
    if ((i = arg_pos((char *)"-nrate", argc, argv)) > 0) noise_rate = argv[i + 1];
    if ((i = arg_pos((char *)"-nn", argc, argv)) > 0) ng_num = (unsigned)strtol(argv[i + 1], nullptr, 10);

    Data data(dataset,"../data/"+dataset+"/",
            "../results/"+dataset+"/N"+noise_rate+"/",
            "train.txt","neg_train_"+noise_rate+".txt","valid.txt",
            "test.txt","entity2id.txt","relation2id.txt");
    Parameter params(epochs, dimension, l_rate, margin, method, noise_rate, ng_num, true);


    //output
    ofstream fout;
    fout.open((data.report_dir+"/train_details.txt").c_str());
    fout<<"dataset:"<<dataset<<"\tnoise rate:"<<noise_rate<<"\t with pre-"<<noise_rate<<endl;
    cout<<"dimension = "<<dimension<<endl;
    fout<<"dimension = "<<dimension<<endl;
    cout<<"learning rate = "<<l_rate<<endl;
    fout<<"learning rate = "<<l_rate<<endl;
    cout<<"margin = "<<margin<<endl;
    fout<<"margin = "<<margin<<endl;

    cout<<"method = "<<method<<endl;
    fout<<"method = "<<method<<endl;
    fout.close();

    DataModel data_model(data, params);
    data_model.prepare();

    Train training(data,params,data_model.relation_num
            ,data_model.entity_num,data_model.data_train
            ,data_model.train_flg
            ,data_model.left_num,data_model.right_num);
    training.run();

    Test testing(data,params,data_model.relation_num
            ,data_model.entity_num, data_model.data_test
            ,data_model.all_flg);
    testing.link_prediction();

    return 0;
}