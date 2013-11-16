#ifndef DCG_SCORER_H
#define DCG_SCORER_H
#include "MetricScorer.h"


class DCG_scorer : public MetricScorer
{
public:
    DCG_scorer() : MetricScorer()
    {

    };
    virtual ~DCG_scorer(){}

    virtual double score(RankList *rl);
    virtual string name();
    virtual void swapChange(RankList* rl, double** &changes);

    double get_dcg(vector<float> &v,  int &k);
protected:
private:
};

#endif // DCG_SCORER_H
