#ifndef RANKNET_H
#define RANKNET_H

#include "Ranker.h"
#include "Layer.h"
class Neuron;

class RankNet : public Ranker
{
public:
    RankNet();

    virtual ~RankNet();

    virtual void init();
    virtual void learn();
    virtual double eval(DataPoint *p);

    virtual void load(string fn){

    }
    virtual string model();
    virtual string toString();
    virtual string name();
    virtual void printParameters();

    static int nIteration;
    static int nHiddenLayer;
    static int nHiddenNodePerLayer;
    static double learningRate;

    vector<Layer*> *layers;
    Layer *inputLayer;
    Layer *outputLayer;
    vector<Synapse*> *Synapses;
    vector<vector<double> > bestModelOnValidation;

    int totalPairs;
    int misorderedPairs;
    double error;
    double lastError;
    int straightLoss;

    void setInputOuput(int nInput, int nOutput);
    void setInputOuput(int nInput, int nOutput, int nType);
    void addHiddenLayer(int size);
    void wire();
    void connect(int sourceLayer, int sourceNeuron, int targetLayer, int targetNeuron);
    void addInput(DataPoint *p);
    void propagate(int i);
    void batchFeedForward(RankList* &rl, int** &pairMap, int* paircnts);
    void batchBackPropagate(int** pairMap, int pointsCnt, int *pairsCnt);
    void clearNeuronOutputs();
    void computePairWeight(int**pairMap, int pointsCnt, int**pairWeight, int *paircnts, RankList rl);
    void saveBestModelOnValidation();
    void restoreBestModelOnValidation();
    double crossEntropy(double o1, double o2, double targetValue);
    void estimateLoss() ;
    void printNetworkConfig();
    void printWeightVector();



protected:
private:
};

#endif // RANKNET_H
