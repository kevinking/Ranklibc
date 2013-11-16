#ifndef RANKER_H
#define RANKER_H

#include "DataType.h"
#include "DCG_scorer.h"

class Ranker
{
public:
    Ranker();
    virtual ~Ranker();

    DataSet *trainDataSet;
    DataSet *testDataSet;
    DataSet *validateDataSet;

    vector<int> *features;
    MetricScorer *trainScorer;
    MetricScorer *testScorer;
    double scoreOnTrainData;
    double bestScoreOnValidationData;

    static bool  verbose;

    virtual void init() = 0;
    virtual void learn() = 0;
    virtual double eval(DataPoint *p) = 0;

    virtual string model() = 0;
    virtual string toString() = 0;
    virtual string name() = 0;
    virtual void printParameters() = 0;

    void setTrainDataSet(DataSet *ds)
    {
        trainDataSet = ds;
    }

    void setTestDataSet(DataSet *ds)
    {
        testDataSet = ds;
    }

    void setValidateDataSet(DataSet *ds)
    {
        validateDataSet = ds;
    }

    void setTrainMetricScorer(MetricScorer *scorer)
    {
        trainScorer = scorer;
    }

    void setTestMetricScorer(MetricScorer *scorer)
    {
        testScorer = scorer;
    }

    void setFeaturs(vector<int> *f)
    {
        features = f;
    }

    double getScoreOnTrainData()
    {
        return scoreOnTrainData;
    }

    double getScoreOnValidationData()
    {
        return bestScoreOnValidationData;
    }

    vector<int>* getFeatures()
    {
        return features;
    }

protected:
private:
};

#endif // RANKER_H
