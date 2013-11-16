#include "MetricScorer.h"

MetricScorer::MetricScorer() : k(10)
{

}

MetricScorer::~MetricScorer()
{
    //dtor
}

void MetricScorer::setK(int _k)
{
    k = _k;
}


double MetricScorer::score(RankList **rls, int qsize)
{
    double scores = 0.0;
    for (int i = 0; i < qsize; i++)
        scores += score(rls[i]);
    return scores / qsize;
}

double MetricScorer::score(RankList *rl)
{
    // todo

    return 0;
}

void MetricScorer::swapChange(const RankList* rl, double** &changes)
{

}

