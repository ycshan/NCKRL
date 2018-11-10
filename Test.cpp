//
// Created by ycshan on 2018/9/6.
//

#include "Test.h"

double Test::calc_sum(int e1,int e2,int rel)
{
    double sum=0;
    if(params.l1_flag)
        for(int ii=0; ii<params.dim; ii++)
            sum+=-fabs(entity_vec[e2][ii]-entity_vec[e1][ii]-relation_vec[rel][ii]);
    else
        for(int ii=0; ii<params.dim; ii++)
            sum+=-Utilities::sqr(entity_vec[e2][ii]-entity_vec[e1][ii]-relation_vec[rel][ii]);
    return sum;
}

//bool Test::test_exist(const pair<pair<int, int>, int>& triple){
//    for(auto iter = check_all.begin();iter!=check_all.end();++iter)
//        if(triple == *iter)
//            return true;
//    return false;
//}

void Test::link_prediction() {
    FILE* f1 = fopen((data_set.report_dir+"relation2vec."+params.method).c_str(),"r");
    FILE* f3 = fopen((data_set.report_dir+"entity2vec."+params.method).c_str(),"r");
    cout<<relation_num<<' '<<entity_num<<endl;
    unsigned long relation_num_fb=relation_num;
    relation_vec.resize(relation_num_fb);
    for(int i=0; i<relation_num_fb;i++)
    {
        relation_vec[i].resize(params.dim);
        for(int ii=0; ii<params.dim; ii++)
            fscanf(f1,"%lf",&relation_vec[i][ii]);
    }
    entity_vec.resize(entity_num);
    for(int i=0; i<entity_num;i++)
    {
        entity_vec[i].resize(params.dim);
        for(int ii=0; ii<params.dim; ii++)
            fscanf(f3,"%lf",&entity_vec[i][ii]);
        if(Utilities::vec_len(entity_vec[i])-1>1e-3)
            cout<<"wrong_entity"<<i<<' '<<Utilities::vec_len(entity_vec[i])<<endl;
    }
    fclose(f1);
    fclose(f3);
    double left_rank=0,left_rank_filter= 0;
    double left_hit=0,left_hit_filter = 0;  //hit@N
    double left_reciprocal=0,left_reciprocal_filter=0;

    double right_rank = 0,right_rank_filter=0;
    double right_hit=0,right_hit_filter = 0;    //hit@N
    double right_reciprocal=0,right_reciprocal_filter=0;

    double left_first_hit = 0, left_first_hit_filter = 0;   //hit@1
    double right_first_hit = 0, right_first_hit_filter = 0; //hit@1

    double mid_rank = 0,mid_rank_filter=0;
    double mid_hit=0,mid_hit_filter = 0;

    unsigned hit_relation = 1;  //hit@1:no need to consider filter
    unsigned hit_entity = 10;
    unsigned hit_entity_first = 1; //hit@1:no need to consider filter
    unsigned long test_total=data_test.size();
    for(unsigned testid = 0; testid<test_total; testid+=1)
    {
        int h = data_test[testid].first.first;
        int l = data_test[testid].first.second;
        int rel = data_test[testid].second;
        vector<pair<int,double> > a;
        for(int i=0; i<entity_num; i++)		//head
        {
            double sum = calc_sum(i,l,rel);
            a.emplace_back(make_pair(i,sum));
        }
        sort(a.begin(),a.end(),Utilities::cmp);
        unsigned filter = 0;
        for(unsigned long i=a.size()-1; i>=0; i--)
        {
            if(all_exist[make_pair(a[i].first,l)].count(rel)==0)
                filter+=1;
            if(a[i].first ==h)
            {
                left_rank+=a.size()-i;
                left_reciprocal+=1.0/(a.size()-i);
                left_rank_filter+=filter+1;
                left_reciprocal_filter+=1.0/(filter+1);
                if(a.size()-i<=hit_entity)
                    left_hit+=1;
                if(filter+1<=hit_entity)
                    left_hit_filter+=1;
                if(a.size()-i<=hit_entity_first)
                    left_first_hit+=1;
                if(filter+1<=hit_entity_first)
                    left_first_hit_filter+=1;
                break;
            }
        }
        a.clear();
        for(int i=0; i<entity_num; i++)		//tail
        {
            double sum = calc_sum(h,i,rel);
            a.emplace_back(make_pair(i,sum));
        }
        sort(a.begin(),a.end(),Utilities::cmp);
        filter=0;
        for(unsigned long i=a.size()-1; i>=0; i--)
        {
            if(all_exist[make_pair(h,a[i].first)].count(rel)==0)
                filter+=1;
            if(a[i].first==l)
            {
                right_rank+=a.size()-i;
                right_reciprocal+=1.0/(a.size()-i);
                right_rank_filter+=filter+1;
                right_reciprocal_filter+=1.0/(filter+1);
                if(a.size()-i<=hit_entity)
                    right_hit+=1;
                if(filter+1<=hit_entity)
                    right_hit_filter+=1;
                if(a.size()-i<=hit_entity_first)
                    right_first_hit+=1;
                if(filter+1<=hit_entity_first)
                    right_first_hit_filter+=1;
                break;
            }
        }
        a.clear();
        for(int i=0; i<relation_num; i++)		//relation
        {
            double sum = calc_sum(h,l,i);
            a.emplace_back(make_pair(i,sum));
        }
        sort(a.begin(),a.end(),Utilities::cmp);
        filter=0;
        for(unsigned long i=a.size()-1; i>=0; i--)
        {
            if(all_exist[make_pair(h,l)].count(a[i].first)==0)
                filter+=1;
            if(a[i].first==rel)
            {
                mid_rank+=a.size()-i;
                mid_rank_filter+=filter+1;
                if(a.size()-i<=hit_relation)
                    mid_hit+=1;
                if(filter+1<=hit_relation)
                    mid_hit_filter+=1;
                break;
            }
        }
        a.clear();
        if(testid%1000==0) cout<<"testid: "<<testid<<endl;
    }
    // predict head
    double rleft_mean_rank=left_rank/test_total;
    double fleft_mean_rank=left_rank_filter/test_total;
    double rleft_mrr=left_reciprocal/test_total;
    double fleft_mrr=left_reciprocal_filter/test_total;
    double rleft_hit=left_hit/test_total;
    double fleft_hit=left_hit_filter/test_total;

    // predict tail
    double rright_mean_rank=right_rank/test_total;
    double fright_mean_rank=right_rank_filter/test_total;
    double rright_mrr=right_reciprocal/test_total;
    double fright_mrr=right_reciprocal_filter/test_total;
    double rright_hit=right_hit/test_total;
    double fright_hit=right_hit_filter/test_total;

    // predict relation
    double rrel_mean_rank=mid_rank/test_total;
    double frel_mean_rank=mid_rank_filter/test_total;
    double rrel_hit=mid_hit/test_total;
    double frel_hit=mid_hit_filter/test_total;

    // new add: hit@1 on predicting head and tial
    double rleft_first_hit=left_first_hit/test_total;
    double fleft_first_hit=left_first_hit_filter/test_total;
    double rright_first_hit=right_first_hit/test_total;
    double fright_first_hit=right_first_hit_filter/test_total;

    //output
    ofstream fout;
    fout.open((data_set.report_dir+"report.txt").c_str());
    fout<<"LEFT\tmr:"<<rleft_mean_rank<<'\t'<<fleft_mean_rank<<"\tmrr:"<<rleft_mrr;
    fout<<'\t'<<fleft_mrr<<"\thit@10:"<<rleft_hit<<'\t'<<fleft_hit;
    fout<<"\thit@1:"<<rleft_first_hit<<'\t'<<fleft_first_hit<<endl;

    fout<<"RIGHT\tmr:"<<rright_mean_rank<<'\t'<<fright_mean_rank<<"\tmrr:"<<rright_mrr;
    fout<<'\t'<<fright_mrr<<"\thit@10:"<<rright_hit<<'\t'<<fright_hit;
    fout<<"\thit@1:"<<rright_first_hit<<'\t'<<fright_first_hit<<endl;

    fout<<"MEAN\tmr:"<<(rleft_mean_rank+rright_mean_rank)/2<<'\t'<<(fleft_mean_rank+fright_mean_rank)/2;
    fout<<"\tmrr:"<<(rleft_mrr+rright_mrr)/2<<'\t'<<(fleft_mrr+fright_mrr)/2;
    fout<<"\thit@10:"<<(rleft_hit+rright_hit)/2<<'\t'<<(fleft_hit+fright_hit)/2;
    fout<<"\thit@1:"<<(rleft_first_hit+rright_first_hit)/2<<'\t'<<(fleft_first_hit+fright_first_hit)/2<<endl;

    fout<<"REL\tmr:"<<rrel_mean_rank<<'\t'<<frel_mean_rank;
    fout<<"\thit@1:"<<rrel_hit<<'\t'<<frel_hit<<endl;

    fout.close();
}

Test::Test(Data& p_data, Parameter& p_param, unsigned&rel_num,
unsigned&ent_num, vector<pair<pair<int, int>, int> >& d_test,
map<pair<int,int>, map<int,int> >& d_all_exist)
:data_set(p_data),params(p_param)
{
    this->relation_num = rel_num;
    this->entity_num = ent_num;
    this->data_test = d_test;
    this->all_exist = d_all_exist;
}