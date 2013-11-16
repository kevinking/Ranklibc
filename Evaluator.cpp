#include "Evaluator.h"

Evaluator::Evaluator()
{
    //ctor
}

Evaluator::~Evaluator()
{
    //dtor
}

void Evaluator::getFeatureIDFromFile(vector<int> &featureids, map<int, string> &featureNameIdMap )
{
    if(strlen(featurefile) == 0)
        return ;

    ifstream in(featurefile);
    if (!in)
    {
        cerr << "cannot open " << string(featurefile) << endl;
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
        featureids.push_back(fid);
    }
}

void Evaluator::evaluate()
{
    DataSet* trainSet   = new DataSet(trainfile);
    DataSet* testSet    = NULL;
    if (strlen(testfile) != 0)
        testSet = new DataSet(testfile);
    DataSet* validateSet= NULL;
    if (strlen(validatefile) != 0)
        validateSet  = new DataSet(validatefile);

    trainSet->init();
    testSet->init();
    validateSet->init();

    vector<int> *featureids = new vector<int>();
    map<int, string> featureNameIdMap;
    getFeatureIDFromFile(*featureids, featureNameIdMap);

    rank = new RankNet();
    rank->setTrainMetricScorer(trainScorer);
    rank->setTestMetricScorer(testScorer);
    rank->setTrainDataSet(trainSet);
    rank->setTestDataSet(testSet);
    rank->setValidateDataSet(validateSet);
    rank->setFeaturs(featureids);

    rank->init();
    rank->learn();
}
