#ifndef DATA_TYPE_H
#define DATA_TYPE_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

#include <stdlib.h>
#include <float.h>
#include <string.h>

#include "Util.h"

using namespace std;

class DataPoint
{
public:
    DataPoint(int max_feature_cnt)
    {
        features_ = new float[max_feature_cnt + 1];
        for (int i = 0; i != max_feature_cnt; i++)
            features_[i] = FLT_MIN;
        max_feature_cnt_ = max_feature_cnt;
    }
    virtual ~DataPoint()
    {
        delete features_;
    }


    float label_;
    int queryid_;
    float cached_;
    double model_score;
    float *features_;
    int max_feature_cnt_;

public:
    void setLabel(float label)
    {
        label_ = label;
    }

    float getLabel()
    {
        return label_;
    }

    void setQueryid(int queryid)
    {
        queryid_ = queryid;
    }

    int getQueryid()
    {
        return queryid_;
    }

    void setCached(float cached)
    {
        cached_ = cached;
    }

    void setModelscore(float cached)
    {
        model_score = cached;
    }

    float getiFeature(int i)
    {
        if (i >= 0 || i <= max_feature_cnt_)
        {
            return features_[i];
        }
        return FLT_MIN;
    }

    void setiFeature(int i, float fval)
    {
        features_[i] = fval;
    }


};



class RankList
{
public:
    RankList()
    {
        dp_cnt_ = 0;
        rl_ = new vector<DataPoint*> ();
    }

    virtual ~RankList()
    {
        // cout << "close RankList\n";
        for (vector<DataPoint*>::iterator it = rl_->begin();
                it != rl_->end();
                it++)
            delete *it;
        delete rl_;
    }

    void add(DataPoint* dp)
    {
        rl_->push_back(dp);
        dp_cnt_++;
    }

    int getDatapointsCnt()
    {
        return dp_cnt_;
    }

    DataPoint* getiDataPoint(int i)
    {
        return (*rl_)[i];
    }
    
    static bool labelCmp(const DataPoint* dp1, const DataPoint* dp2)
    {
        return dp1->label_ < dp2->label_;
    }
    
    void getCorrectRanking(vector<DataPoint*> &dpv)
    {
        dpv.assign(rl_->begin(), rl_->end());
        std::sort(dpv.begin(), dpv.end(), labelCmp);
    }

    static bool modelScoreCmp(const DataPoint*  m1, const DataPoint* m2)
    {
        return m2->model_score < m1->model_score;
    }

    void sortByModelscore()
    {
        std::sort(rl_->begin(), rl_->end(), modelScoreCmp);
    }

    vector<DataPoint*> *rl_;
    int dp_cnt_; // datapoint个数
};

class DataSet
{
public:

    DataSet()
    {
        total_query_cnt  = 100000;
        rls = (RankList**) malloc(100000 * sizeof(RankList*));
        memset(rls, 0, 100000 * sizeof(RankList*));
    }

    DataSet(char* datafile_)
    {
        max_feature_cnt = 0;
        total_query_cnt = 0;
        total_doc_cnt = 0;
        datafile = datafile_;
    }

    void add(int i, RankList* rl)
    {
        rls[i] = rl;
    }

    ~DataSet()
    {
        //cout << "start close dataset\n";
        for (int i = 1; i <= total_query_cnt; i++)
        {
            if (rls[i])
                delete rls[i];
        }
        delete rls;
        //cout << "finish close dataset\n";
    }

    int max_feature_cnt;
    int total_query_cnt;
    int total_doc_cnt;
    char *datafile;
    RankList **rls;


    RankList* getithRankList(int i)
    {
        return rls[i];
    }
    void printDataSetInfo()
    {
        cout << "max_feature_cnt: " << max_feature_cnt << endl;
        cout << "total_query_cnt: " << total_query_cnt << endl;
        cout << "total_doc_cnt: " << total_doc_cnt << endl;
    }

    void init()
    {
        getDatasetInfo();
        open();
        load();
        cout << "finish init" << endl;
        printDataSetInfo();
    }

    void open()
    {
        rls = (RankList**) malloc((total_query_cnt+1) * sizeof(RankList*));
        if (rls == NULL)
        {
            fprintf(stderr, "out of memory\n");
            return;
        }
        memset(rls, 0, (total_query_cnt+1) * sizeof(RankList*));
    }

    /* void close()
     {
         cout << "close dataset\n";
         for (int i = 0; i < total_query_cnt; i++)
         {
             delete rls[i];
         }
         delete rls;
     }
    */
    void getDatasetInfo()
    {
        ifstream in(datafile);
        if (!in)
        {
            cerr << "cannot open " << string(datafile) << endl;
            return;
        }
        string line;
        max_feature_cnt = 0;
        total_query_cnt = 0;
        total_doc_cnt = 0;
        float max_label = -1;
        float min_label = 1;
        int pre_qid = -1;
        while(getline(in, line))
        {
            total_doc_cnt++;
            //0 qid:1 1:1 3:3 #
            vector<string> tokens;
            string::size_type pos = line.find("#");
            string str;
            if (pos != string::npos)
                str = line.substr(0, pos);
            else
                str = line;
            splitString(str, tokens, " ");
            float label = atof(tokens[0].c_str());
            if (label > max_label) max_label = label;
            if (label < min_label) min_label = label;

            int qid = atoi(tokens[1].substr(tokens[1].find(":")+1).c_str());
            if (qid != pre_qid)
            {
                total_query_cnt++;
            }
            pre_qid = qid;

            for (size_t i = 2; i != tokens.size(); i++)
            {
                int featureid = atoi(tokens[i].substr(0, tokens[i].find(":")+1).c_str());
                if (featureid > max_feature_cnt)
                    max_feature_cnt = featureid;
            }
        }

        in.close();
    }

    void load()
    {
        ifstream in(datafile);
        if (!in)
        {
            cerr << "cannot open " << string(datafile) << endl;
            return;
        }
        string line;
        int pre_qid = -1;
        int qid = -1;
        RankList *rl = new RankList();
        while(getline(in, line))
        {
            //cout << "line:" << line << endl;
            //0 qid:1 1:1 3:3 #
            DataPoint *dp = new DataPoint(max_feature_cnt);

            vector<string> tokens;
            string::size_type pos = line.find("#");
            string str;
            if (pos != string::npos)
                str = line.substr(0, pos);
            else
                str = line;
            splitString(str, tokens, " ");
            float label = atof(tokens[0].c_str());
            qid = atoi(tokens[1].substr(tokens[1].find(":")+1).c_str());
            dp->setLabel(label);
            dp->setQueryid(qid);

            if (qid != pre_qid && pre_qid != -1)
            {
                rls[pre_qid] = rl;
                rl = new RankList();
            }
            pre_qid = qid;

            for (size_t i = 2; i != tokens.size(); i++)
            {
                int featureid = atoi(tokens[i].substr(0, tokens[i].find(":")).c_str());
                float val = atof(tokens[i].substr(tokens[i].find(":")+1).c_str());
                dp->setiFeature(featureid, val);
            }
            rl->add(dp);
        }
        rls[qid] = rl;
        cout << "finish load data" << endl;
        in.close();
    }
};


#endif
