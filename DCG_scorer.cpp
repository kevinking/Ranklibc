#include "DCG_scorer.h"
#include "math.h"

double DCG_scorer::score(RankList* rl)
{
    vector<float> rel;
    int m = rl->getDatapointsCnt();
    for (int i = 0; i < m; i++)
    {
        rel.push_back(rl->getiDataPoint(i)->getLabel());
    }

    if (rel.size() == 0)
        return -1.0;

    return get_dcg(rel, k);
}

double DCG_scorer::get_dcg( vector<float> &v,  int &k)
{
    int kk = k;
    if (k > (int)v.size() || k <= 0)
        kk = (int)v.size();

    double dcg = 0.0;
    for (int i = 1 ; i <= kk; i++)
    {
        dcg += (pow(2.0, v.at(i-1)) - 1.0) / (log(i+1)/log(2.0));
    }
    return dcg;
}

void DCG_scorer::swapChange(RankList* rl, double** &changes)
{
    int m = rl->getDatapointsCnt();
    int size = (m > k) ? k : m;
    for (int i = 0; i < size; i++)
    {
        int p1 = i + 1;
        for (int j = i + 1; j < m; j++)
        {
            int p2 = j + 1;
            changes[j][i] = changes[i][j] = (1.0 / (log(p1+1)/log(2)) - 1.0 / (log(p2+1)/log(2))) \
                                            * (pow(2.0, rl->getiDataPoint(i)->getLabel()) - pow(2.0,  rl->getiDataPoint(j)->getLabel()));
        }
    }
}

string DCG_scorer::name()
{
    return "DCG@" + k;

}
