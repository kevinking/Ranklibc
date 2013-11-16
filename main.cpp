#include "DataType.h"
#include "Util.h"
#include "Ranker.h"
#include "RankNet.h"
#include "MetricScorer.h"
#include "DCG_scorer.h"

void getFeatureIDFromFile(char* fn, vector<int> *featureids, map<int, string> &featureNameIdMap)
{
    if(strlen(fn) == 0)
        return ;

    ifstream in(fn);
    if (!in)
    {
        cerr << "cannot open " << string(fn) << endl;
        return;
    }
    string line;
    while(getline(in, line))
    {
        vector<string> tokens;
        splitString(line, tokens, "\t");
        int fid = atoi(tokens[0].c_str());
        string featurename = "NONE";

        if (tokens.size() >= 2)
            featurename = tokens[1];
        featureNameIdMap.insert(make_pair(fid, featurename));
        featureids->push_back(fid);
    }
}

void evaluate(char* trainfile, char* testfile, char* validatefile, char* featurefile, MetricScorer* trainScorer, MetricScorer* testScorer)
{
    DataSet* trainSet   = new DataSet(trainfile);
    DataSet* testSet    = NULL;
    if (strlen(testfile) != 0)
        testSet = new DataSet(testfile);
    DataSet* validateSet= NULL;
    if (strlen(validatefile) != 0)
        validateSet  = new DataSet(validatefile);

    trainSet->init();
   // testSet->init();
   // validateSet->init();

    vector<int> *featureids = new vector<int>();
    map<int, string> featureNameIdMap;
    getFeatureIDFromFile(featurefile, featureids, featureNameIdMap);

    RankNet *rank = new RankNet();
    rank->setTrainMetricScorer(trainScorer);
    rank->setTestMetricScorer(testScorer);
    rank->setTrainDataSet(trainSet);
    rank->setTestDataSet(testSet);
    rank->setValidateDataSet(validateSet);
    rank->setFeaturs(featureids) ;

    rank->init();
    rank->learn();

    delete trainSet;
    delete featureids;
    delete rank;

}

int main()
{
    char trainfile[100] = "/home/kevin/RankLibc/data/train.txt";
    char testfile[100] = "";
    char validatefile[100] = "";
    char featurefile[100] = "/home/kevin/RankLibc/data/featureid.txt";
    DCG_scorer *trainScorer = new DCG_scorer();
    DCG_scorer *testScorer = new DCG_scorer();
    evaluate(trainfile, testfile, validatefile, featurefile, trainScorer, testScorer);

    return 0;
}
