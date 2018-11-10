//
// Created by ycshan on 9/6/2018.
//

#include<iostream>
#include<cstring>
#include<cstdio>
#include<map>
#include<vector>
#include<string>
#include<ctime>
#include<cmath>
#include<cstdlib>
#include<sstream>
#include<fstream>

#include "Train.h"

using namespace std;

Train::Train(Data& p_data, Parameter& p_param, unsigned&rel_num,
             unsigned&ent_num,vector<pair<pair<int, int>, int> >& d_train,
             map<pair<int,int>, map<int,int> >& t_exist,
             map<int,double>& l_num, map<int,double>& r_num)
        :data_set(p_data),params(p_param)
{
    this->relation_num = rel_num;
    this->entity_num = ent_num;
    this->data_train = d_train;
    this->train_exist = t_exist;
    this->left_num = l_num;
    this->right_num = r_num;
}

void Train::run(){
    relation_vec.resize(relation_num);		//relation_vec
    for (unsigned i=0; i<relation_vec.size(); i++)
        relation_vec[i].resize(params.dim);
    entity_vec.resize(entity_num);		//entity_vec
    for (unsigned i=0; i<entity_vec.size(); i++)
        entity_vec[i].resize(params.dim);
    relation_tmp.resize(relation_num);		//relation_tmp
    for (unsigned i=0; i<relation_tmp.size(); i++)
        relation_tmp[i].resize(params.dim);
    entity_tmp.resize(entity_num);		//entity_tmp
    for (unsigned i=0; i<entity_tmp.size(); i++)
        entity_tmp[i].resize(params.dim);
    //initialization, pre-trained TransE
    FILE* f1 = fopen((data_set.base_dir+"preTransE/N"+params.noise_rate
            +"/entity2vec."+params.method).c_str(),"r");
    for (int i=0; i<entity_num; i++)
    {
        for (int ii=0; ii<params.dim; ii++)
            fscanf(f1,"%lf",&entity_vec[i][ii]);
        norm(entity_vec[i]);
    }
    fclose(f1);
    FILE* f2 = fopen((data_set.base_dir+"preTransE/N"+params.noise_rate
            +"/relation2vec."+params.method).c_str(),"r");
    for (int i=0; i<relation_num; i++)
    {
        for (int ii=0; ii<params.dim; ii++)
        {
            fscanf(f2,"%lf",&relation_vec[i][ii]);
            //relation_vec[i+1345][ii] = -relation_vec[i][ii];
        }
    }
    fclose(f2);

    //initialization, random
    // for (int i=0; i<relation_num; i++)
    // {
    //     for (int ii=0; ii<n; ii++)
    //         relation_vec[i][ii] = randn(0,1.0/n,-6/sqrt(n),6/sqrt(n));
    // }
    // for (int i=0; i<entity_num; i++)
    // {
    //     for (int ii=0; ii<n; ii++)
    //         entity_vec[i][ii] = randn(0,1.0/n,-6/sqrt(n),6/sqrt(n));
    //     norm(entity_vec[i]);
    // }

    rate_confidence.resize(data_train.size());		//initialization
    for(unsigned i=0; i<data_train.size(); i++)
    {
        rate_confidence[i] = 1;
    }

    sgd();
}

double Train::norm(vector<double> &a){
    double x = Utilities::vec_len(a);
    if (x>1)
        for (unsigned ii=0; ii<a.size(); ii++)
            a[ii]/=x;
    return 0;
}

void Train::sgd() {
    val_loss = 0;
    int batch_num = 100;
    unsigned long batch_size = data_train.size() / batch_num;        //batch size
    for (int epoch = 0; epoch < params.epoch; epoch++) {
        val_loss = 0;
        for (int batch = 0; batch < batch_num; batch++) {
            relation_tmp = relation_vec;
            entity_tmp = entity_vec;
            for (int k = 0; k < batch_size; k++) {
                int i = Utilities::rand_max(data_train.size());
                int j = 0, candidate;
                double neg_quality = qNULL;
                vector<pair<double, double> > neg_exp;
                double exp_sum = 0;
                double pr = params.method != "bern" ? 500 : 1000 * right_num[data_train[i].second] /
                                                            (right_num[data_train[i].second] +
                                                             left_num[data_train[i].second]);
                random_device rd;
                if (rd() % 1000 < pr) {
                    for (unsigned neg = 0; neg < params.ng_num; neg++) {
                        candidate = Utilities::rand_max(entity_num);

                        while (train_exist[make_pair(data_train[i].first.first, candidate)].count(
                                data_train[i].second) > 0)
                            candidate = Utilities::rand_max(entity_num);
                        double vec_norm = calc_sum(data_train[i].first.first, candidate, data_train[i].second);
                        if (neg_quality == qNULL) {
                            neg_quality = -vec_norm;
                            neg_exp.push_back(make_pair(exp(neg_quality), vec_norm));
                            exp_sum += exp(neg_quality);
                            j = candidate;
                            continue;
                        }
                        exp_sum += exp(-vec_norm);
                        neg_exp.push_back(make_pair(exp(-vec_norm), vec_norm));
                        if (neg_quality < -vec_norm) {
                            neg_quality = -vec_norm;
                            j = candidate;
                        }
                    }
                    for (unsigned l = 0; l < neg_exp.size(); l++)
                        neg_confidence.push_back(make_pair(neg_exp[l].first / exp_sum, neg_exp[l].second));
                    train_kb(data_train[i].first.first, data_train[i].first.second, data_train[i].second,
                             data_train[i].first.first, j, data_train[i].second, i);
                    neg_confidence.clear();
                } else {
                    for (unsigned neg = 0; neg < params.ng_num; neg++) {
                        candidate = Utilities::rand_max(entity_num);
                        while (train_exist[make_pair(candidate,
                                                     data_train[i].first.second)].count(data_train[i].second) > 0)
                            candidate = Utilities::rand_max(entity_num);
                        double vec_norm = calc_sum(candidate, data_train[i].first.second, data_train[i].second);
                        if (neg_quality == qNULL) {
                            neg_quality = -vec_norm;
                            neg_exp.push_back(make_pair(exp(neg_quality), vec_norm));
                            exp_sum += exp(neg_quality);
                            j = candidate;
                            continue;
                        }
                        neg_exp.push_back(make_pair(exp(-vec_norm), vec_norm));
                        exp_sum += exp(-vec_norm);
                        if (neg_quality < -vec_norm) {
                            neg_quality = -vec_norm;
                            j = candidate;
                        }
                    }
                    for (unsigned l = 0; l < neg_exp.size(); l++)
                        neg_confidence.push_back(make_pair(neg_exp[l].first / exp_sum, neg_exp[l].second));
                    train_kb(data_train[i].first.first, data_train[i].first.second, data_train[i].second, j,
                             data_train[i].first.second, data_train[i].second, i);
                    neg_confidence.clear();
                }
                int rel_neg = Utilities::rand_max(relation_num);
                while (train_exist[make_pair(data_train[i].first.first,
                                             data_train[i].first.second)].count(rel_neg) > 0)
                    rel_neg = Utilities::rand_max(relation_num);
                train_kb(data_train[i].first.first, data_train[i].first.second, data_train[i].second,
                         data_train[i].first.first, data_train[i].first.second, rel_neg, i, true);

                norm(relation_tmp[data_train[i].second]);
                norm(relation_tmp[rel_neg]);
                norm(entity_tmp[data_train[i].first.first]);
                norm(entity_tmp[data_train[i].first.second]);
                norm(entity_tmp[j]);
            }
            relation_vec = relation_tmp;
            entity_vec = entity_tmp;
        }
        cout << "epoch:" << epoch << " \t" << val_loss << endl;
        if (epoch % 20 == 0) {
            ofstream fout;
            fout.open((data_set.report_dir + "train_details.txt").c_str(), std::ios::app);
            fout << "epoch:" << epoch << " \t" << val_loss << endl;
            fout.close();
        }
        FILE *f2 = fopen((data_set.report_dir + "relation2vec." + params.method).c_str(), "w");
        FILE *f3 = fopen((data_set.report_dir + "entity2vec." + params.method).c_str(), "w");
        FILE *f4 = fopen((data_set.report_dir + "rate_conf." + params.method).c_str(), "w");
        for (int i = 0; i < relation_num; i++)        //output relation2vec
        {
            for (int ii = 0; ii < params.dim; ii++)
                fprintf(f2, "%.6lf\t", relation_vec[i][ii]);
            fprintf(f2, "\n");
        }
        fclose(f2);

        for (int i = 0; i < entity_num; i++)        //output entity_vec
        {
            for (int ii = 0; ii < params.dim; ii++)
                fprintf(f3, "%.6lf\t", entity_vec[i][ii]);
            fprintf(f3, "\n");
        }
        fclose(f3);

        for (unsigned i = 0; i < data_train.size(); i++)        //output rate_confidence
        {
            fprintf(f4, "%.6lf\n", rate_confidence[i]);
        }
        fclose(f4);
    }
}

double Train::calc_sum(int e1,int e2,int rel){
    double sum=0;
    if (params.l1_flag)
        for (int ii=0; ii<params.dim; ii++)
            sum+=fabs(entity_vec[e2][ii]-entity_vec[e1][ii]-relation_vec[rel][ii]);		//h+r=t
    else
        for (int ii=0; ii<params.dim; ii++)
            sum+=Utilities::sqr(entity_vec[e2][ii]-entity_vec[e1][ii]-relation_vec[rel][ii]);		//h+r=t
    return sum;
}

double Train::expected_value(){
    double ex_val=0;
    double test=0;
    for(unsigned i=0;i<neg_confidence.size();i++)
    {
        ex_val+=neg_confidence[i].first * neg_confidence[i].second;
        test+=neg_confidence[i].first;
    }
    return ex_val;
}

void Train::gradient(int e1_a,int e2_a,int rel_a,int e1_b,int e2_b,int rel_b,double tri_conf){
    for (int ii=0; ii<params.dim; ii++)
    {

        double x = 2*(entity_vec[e2_a][ii]-entity_vec[e1_a][ii]-relation_vec[rel_a][ii]);
        if (params.l1_flag)
        {
            if (x>0) x=1;
            else x=-1;
        }
        x = params.l_rate*x*tri_conf;
        relation_tmp[rel_a][ii]+=x;
        entity_tmp[e1_a][ii]+=x;
        entity_tmp[e2_a][ii]-=x;
        x = 2*(entity_vec[e2_b][ii]-entity_vec[e1_b][ii]-relation_vec[rel_b][ii]);
        if (params.l1_flag)
        {
            if (x>0) x=1;
            else x=-1;
        }
        x = params.l_rate*x*tri_conf;
        relation_tmp[rel_b][ii]-=x;
        entity_tmp[e1_b][ii]-=x;
        entity_tmp[e2_b][ii]+=x;
    }
}

void Train::train_kb(int e1_a,int e2_a,int rel_a,int e1_b,int e2_b,int rel_b,int tri_num,bool rel_flag)
{
    double sum1 = calc_sum(e1_a,e2_a,rel_a);
    double sum2 = calc_sum(e1_b,e2_b,rel_b);
    if(sum1+params.margin>sum2)
    {
        val_loss += params.margin+sum1-sum2;
        double temp_conf = rate_confidence[tri_num];
        // double temp_conf = 1;
        gradient( e1_a, e2_a, rel_a, e1_b, e2_b, rel_b, temp_conf);
    }
    //update rate_confidence
    if(rel_flag)
    {
        if(sum1+params.margin>sum2)
        {
            rate_confidence[tri_num] *= 0.9;
            if(rate_confidence[tri_num] < 0.0)
                rate_confidence[tri_num] = 0.0;
        }
        else
        {
            rate_confidence[tri_num] += conf_step;
            if(rate_confidence[tri_num] > 1.0)
                rate_confidence[tri_num] = 1.0;
        }
    }
    else
    {
        if(sum1+params.margin>expected_value())
        {
            rate_confidence[tri_num] *= 0.9;
            if(rate_confidence[tri_num] < 0.0)
                rate_confidence[tri_num] = 0.0;
        }
        else
        {
            rate_confidence[tri_num] += conf_step;
            if(rate_confidence[tri_num] > 1.0)
                rate_confidence[tri_num] = 1.0;
        }
    }
}
