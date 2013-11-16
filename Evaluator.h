#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "MetricScorer.h"
#include "Ranker.h"
#include "RankNet.h"

class Evaluator
{
public:
    Evaluator();
    virtual ~Evaluator();

    char *trainfile;
    char *testfile;
    char *validatefile;
    char *featurefile;
    char *modelfile;

    MetricScorer *trainScorer;
    MetricScorer *testScorer;
    Ranker *rank;

    void init();
    void getFeatureIDFromFile(vector<int> &featureids, map<int, string> &featureNameIdMap);
    void evaluate();
    void getFeatureIDFromFile( );

protected:
private:
};

#endif // EVALUATOR_H
