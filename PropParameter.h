#ifndef PROPPARAMETER_H
#define PROPPARAMETER_H


class PropParameter
{
public:
    PropParameter()
        :current(-1), pairsCnt(0), pairMap(0),pairWeight(0)
    {

    }

    PropParameter(int cur, int pairscnt, int***pairMapAddr)
    {
        current = cur;
        pairMap = *pairMapAddr;
        pairsCnt = pairscnt;
        pairWeight=0;
    }

    PropParameter(int cur, int***pairMapAddr,  float ***pairWeightAddr, float ***targetValueAddr)
    {
        current = cur;
        pairMap = *pairMapAddr;
        pairWeight = *pairWeightAddr;
        targetValue = *targetValueAddr;
    }

    virtual ~PropParameter()
    {

    }

    int current; // index of current data point in the ranked list
    int pairsCnt; // length of pairMap[current]
    int **pairMap;

    float **pairWeight;
    float **targetValue;

    float *labels;
protected:
private:
};

#endif // PROPPARAMETER_H
