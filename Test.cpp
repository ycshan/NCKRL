//
// Created by ycshan on 2018/9/6.
//

#include "Test.h"

double Test::calc_sum(int e1,int e2,int rel) {
    double sum = 0;
    if(params.l1_flag)
        for(int ii = 0; ii < params.dim; ii++)
            sum += -fabs(entity_vec[e2][ii]-entity_vec[e1][ii]-relation_vec[rel][ii]);
    else
        for(int ii = 0; ii < params.dim; ii++)
            sum += -Utilities::sqr(entity_vec[e2][ii]-entity_vec[e1][ii]-relation_vec[rel][ii]);
    return sum;
}

void Test::link_prediction() {
    FILE* f1 = fopen((data_set.report_dir+"relation2vec_"+params.detail).c_str(),"r");
    FILE* f2 = fopen((data_set.report_dir+"entity2vec_"+params.detail).c_str(),"r");
    relation_vec.resize(relation_num);
    for (auto &vec : relation_vec) {
        vec.resize(params.dim);
        for (double &val : vec)
            fscanf(f1, "%lf", &val);
    }
    fclose(f1);
    entity_vec.resize(entity_num);
    for (auto &vec : entity_vec) {
        vec.resize(params.dim);
        for (double &val : vec)
            fscanf(f2, "%lf", &val);
    }
    fclose(f2);
    double left_rank = 0,left_rank_filter = 0;
    double left_hit = 0,left_hit_filter = 0; //hit@N
    double left_reciprocal = 0,left_reciprocal_filter = 0;

    double right_rank = 0,right_rank_filter = 0;
    double right_hit = 0,right_hit_filter = 0; //hit@N
    double right_reciprocal = 0,right_reciprocal_filter = 0;

    double left_first_hit = 0, left_first_hit_filter = 0; //hit@1
    double right_first_hit = 0, right_first_hit_filter = 0; //hit@1

    double mid_rank = 0,mid_rank_filter = 0;
    double mid_hit = 0,mid_hit_filter = 0;

    unsigned hit_relation = 1; //hit@1:no need to consider filter
    unsigned hit_entity = 10;
    unsigned hit_entity_first = 1; //hit@1:no need to consider filter
    unsigned long test_total = data_test.size();
    ProgressBar progress_test((unsigned)test_total,70,'#','-');
    progress_test.display();
    for(unsigned test_id = 0; test_id < test_total; test_id +=1) {
        int h = data_test[test_id].first.first;
        int l = data_test[test_id].first.second;
        int rel = data_test[test_id].second;
        vector<pair<int,double> > a;
        for(int i = 0; i < entity_num; i++) { //head
            double sum = calc_sum(i,l,rel);
            a.emplace_back(make_pair(i,sum));
        }
        sort(a.begin(),a.end(),Utilities::cmp);
        unsigned filter = 0;
        for(unsigned long i = a.size()-1; i >= 0; i--) {
            if(all_exist[make_pair(a[i].first,l)].count(rel) == 0)
                filter += 1;
            if(a[i].first == h) {
                left_rank += a.size()-i;
                left_reciprocal += 1.0/(a.size()-i);
                left_rank_filter += filter+1;
                left_reciprocal_filter += 1.0/(filter+1);
                if(a.size()-i <= hit_entity)
                    left_hit += 1;
                if(filter+1 <= hit_entity)
                    left_hit_filter += 1;
                if(a.size()-i <= hit_entity_first)
                    left_first_hit += 1;
                if(filter+1 <= hit_entity_first)
                    left_first_hit_filter += 1;
                break;
            }
        }
        a.clear();
        for(int i = 0; i < entity_num; i++) { //tail
            double sum = calc_sum(h,i,rel);
            a.emplace_back(make_pair(i,sum));
        }
        sort(a.begin(),a.end(),Utilities::cmp);
        filter=0;
        for(unsigned long i = a.size()-1; i >= 0; i--) {
            if(all_exist[make_pair(h,a[i].first)].count(rel) == 0)
                filter += 1;
            if(a[i].first == l) {
                right_rank += a.size()-i;
                right_reciprocal += 1.0/(a.size()-i);
                right_rank_filter += filter+1;
                right_reciprocal_filter += 1.0/(filter+1);
                if(a.size()-i <= hit_entity)
                    right_hit += 1;
                if(filter+1 <= hit_entity)
                    right_hit_filter += 1;
                if(a.size()-i <= hit_entity_first)
                    right_first_hit += 1;
                if(filter+1 <= hit_entity_first)
                    right_first_hit_filter +=1 ;
                break;
            }
        }
        a.clear();
        for(int i = 0; i < relation_num; i++) { //relation
            double sum = calc_sum(h,l,i);
            a.emplace_back(make_pair(i,sum));
        }
        sort(a.begin(),a.end(),Utilities::cmp);
        filter = 0;
        for(unsigned long i = a.size()-1; i >= 0; i--) {
            if(all_exist[make_pair(h,l)].count(a[i].first) == 0)
                filter += 1;
            if(a[i].first == rel) {
                mid_rank += a.size()-i;
                mid_rank_filter += filter+1;
                if(a.size()-i <= hit_relation)
                    mid_hit += 1;
                if(filter+1 <= hit_relation)
                    mid_hit_filter += 1;
                break;
            }
        }
        a.clear();
        ++progress_test;
        progress_test.display();
    }
    progress_test.done();
    //predict head
    double rleft_mean_rank = left_rank/test_total;
    double fleft_mean_rank = left_rank_filter/test_total;
    double rleft_mrr = left_reciprocal/test_total;
    double fleft_mrr = left_reciprocal_filter/test_total;
    double rleft_hit = left_hit/test_total;
    double fleft_hit = left_hit_filter/test_total;

    //predict tail
    double rright_mean_rank = right_rank/test_total;
    double fright_mean_rank = right_rank_filter/test_total;
    double rright_mrr = right_reciprocal/test_total;
    double fright_mrr = right_reciprocal_filter/test_total;
    double rright_hit = right_hit/test_total;
    double fright_hit = right_hit_filter/test_total;

    //predict relation
    double rrel_mean_rank = mid_rank/test_total;
    double frel_mean_rank = mid_rank_filter/test_total;
    double rrel_hit = mid_hit/test_total;
    double frel_hit = mid_hit_filter/test_total;

    //new add: hit@1 on predicting head and tial
    double rleft_first_hit = left_first_hit/test_total;
    double fleft_first_hit = left_first_hit_filter/test_total;
    double rright_first_hit = right_first_hit/test_total;
    double fright_first_hit = right_first_hit_filter/test_total;

    //output
    ofstream of;
    of.open((data_set.report_dir+params.report).c_str(),std::ios::app);
    of<<"\nLEFT\tmr:"<<rleft_mean_rank<<'\t'<<fleft_mean_rank<<"\tmrr:"<<rleft_mrr;
    of<<'\t'<<fleft_mrr<<"\thit@10:"<<rleft_hit<<'\t'<<fleft_hit;
    of<<"\thit@1:"<<rleft_first_hit<<'\t'<<fleft_first_hit<<endl;

    of<<"RIGHT\tmr:"<<rright_mean_rank<<'\t'<<fright_mean_rank<<"\tmrr:"<<rright_mrr;
    of<<'\t'<<fright_mrr<<"\thit@10:"<<rright_hit<<'\t'<<fright_hit;
    of<<"\thit@1:"<<rright_first_hit<<'\t'<<fright_first_hit<<endl;

    of<<"MEAN\tmr:"<<(rleft_mean_rank+rright_mean_rank)/2<<'\t'<<(fleft_mean_rank+fright_mean_rank)/2;
    of<<"\tmrr:"<<(rleft_mrr+rright_mrr)/2<<'\t'<<(fleft_mrr+fright_mrr)/2;
    of<<"\thit@10:"<<(rleft_hit+rright_hit)/2<<'\t'<<(fleft_hit+fright_hit)/2;
    of<<"\thit@1:"<<(rleft_first_hit+rright_first_hit)/2<<'\t'<<(fleft_first_hit+fright_first_hit)/2<<endl;

    of<<"REL\tmr:"<<rrel_mean_rank<<'\t'<<frel_mean_rank;
    of<<"\thit@1:"<<rrel_hit<<'\t'<<frel_hit<<endl;

    of.close();
}

Test::Test(Data& p_data, Parameter& p_param, unsigned&rel_num,
unsigned&ent_num, vector<pair<pair<int, int>, int> >& d_test,
map<pair<int,int>, map<int,int> >& d_all_exist)
:data_set(p_data),params(p_param) {
    this->relation_num = rel_num;
    this->entity_num = ent_num;
    this->data_test = d_test;
    this->all_exist = d_all_exist;
}