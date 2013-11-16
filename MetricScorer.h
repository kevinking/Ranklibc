#ifndef METRIC_SCORER_H
#define METRIC_SCORER_H

#include "DataType.h"
#include <string>
using namespace std;

class MetricScorer
{
public:
    MetricScorer();
    virtual ~MetricScorer();

    void setK(int k);
    double score(RankList **rls, int qsize);

    virtual double score(RankList *rl);
    virtual string name() = 0;
    virtual void swapChange(const RankList* rl, double** &changes);

protected:
    int k;

private:
};

#endif // METRIC_SCORER_H
