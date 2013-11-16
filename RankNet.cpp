#include "RankNet.h"

int RankNet::nIteration = 100;
int RankNet::nHiddenLayer = 1;
int RankNet::nHiddenNodePerLayer = 3;
double RankNet::learningRate = 0.00005;
double Neuron::learningRate = 0.00005;


RankNet::RankNet()
{
    layers = new vector<Layer*>();
    inputLayer = new Layer();
    outputLayer = new Layer();
    Synapses =  new vector<Synapse*>();

    lastError = FLT_MAX;
    totalPairs = 0;
    misorderedPairs = 0;
    error = 0.0;
    straightLoss = 0;
}

RankNet::~RankNet()
{
    if (layers)
    {
        for (std::size_t i = 0; i < layers->size(); i++)
            if ((*layers)[i]) delete (*layers)[i];

        delete layers;
    }
    if (Synapses)
    {
        for (std::size_t i = 0; i < Synapses->size(); i++)
            if ((*Synapses)[i]) delete (*Synapses)[i];

        delete Synapses;
    }

}

void RankNet::setInputOuput(int nInput, int nOutput)
{
    inputLayer = new Layer(nInput + 1); // plus bias
    outputLayer = new Layer(nOutput);
    layers->clear();
    layers->push_back(inputLayer);
    layers->push_back(outputLayer);
}

void RankNet::addHiddenLayer(int size)
{
    layers->insert(layers->end() - 1, new Layer(size));
}

void RankNet::wire()
{
    // wire the input layer to the first hidden layer
    for (int  i = 0; i < inputLayer->size() - 1; i++)
        for (std::size_t  j = 0; j < layers->at(1)->size(); j++)
            connect(0, i, 1, j);

    // wire one layer to the next, starting from the first hidden layer
    for (std::size_t  i = 1; i < layers->size()-1; i++)
        for (int j = 0; j < layers->at(i)->size(); j++)
            for(int  k = 0 ; k < layers->at(i+1)->size(); k++)
                connect(i, j, i+1, k);
    //wire the "bias" neuron to all others of all layers
    for (std::size_t  i = 1; i < layers->size(); i++)
        for (int  j = 0; j < layers->at(i)->size(); j++)
            connect(0, inputLayer->size() - 1, i, j);

// TODO (kevin#1#): initialize weights
}


void RankNet::connect(int sourceLayer, int sourceNeuron, int targetLayer, int targetNeuron)
{
    Synapses->push_back(new Synapse(layers->at(sourceLayer)->get(sourceNeuron), layers->at(targetLayer)->get(targetNeuron)));
}

void RankNet::addInput(DataPoint *p)
{
    for (int k = 0; k < inputLayer->size()-1; k++)
        inputLayer->get(k)->addOutputs(p->getiFeature(features->at(k)));
    // and now the bias node with a fix "1.0"
    inputLayer->get(inputLayer->size()-1)->addOutputs(1.0f);
}

void RankNet::propagate(int i)
{
    for (std::size_t  k = 1; k < layers->size(); k++)
        layers->at(k)->computeOutput(i);
}

void RankNet::batchFeedForward(RankList* &rl, int** &pairMap, int* paircnts)
{
    for (int i = 0; i < rl->getDatapointsCnt(); i++)
    {
        addInput(rl->getiDataPoint(i));
        propagate(i);

        int count = 0;
        for (int j = 0; j < rl->getDatapointsCnt(); j++)
            if (rl->getiDataPoint(i)->getLabel() > rl->getiDataPoint(j)->getLabel())
                count++;
        pairMap[i] = new int[count];
        paircnts[i] = count;
        int k = 0;
        for (int j = 0; j < rl->getDatapointsCnt(); j++)
            if (rl->getiDataPoint(i)->getLabel() > rl->getiDataPoint(j)->getLabel())
                pairMap[i][k++] = j;
    }
}

/**
* @parameter pairMap[i][k]= j
* @parameter paircnt[i]
*/
void RankNet::batchBackPropagate(int** pairMap, int pointsCnt, int *pairsCnt)
{
    for (int i = 0; i < pointsCnt; i++)
    {
        // back propagate
        PropParameter *p = new PropParameter(i, pairsCnt[i], &pairMap);
        // starting from output layer
        outputLayer->computerDeltaForOutputlayer(p);
        // back propagete to the first hidden layer
        for (int j = layers->size() - 2; j >= 1; j--)
            layers->at(j)->updateDelta(p);

        // weight update
        outputLayer->updateWeight(p);
        for (int j = layers->size() - 2; j >= 1; j--)
            layers->at(j)->updateWeight(p);
        delete p;
    }
}


void RankNet::init()
{
    //Set up the network
    setInputOuput(features->size(), 1);
    for (int i = 0; i < nHiddenLayer; i++)
        addHiddenLayer(nHiddenNodePerLayer);
    wire();

    totalPairs = 0;
    for(int i = 0; i < trainDataSet->total_query_cnt; i++)
    {
       /* RankList *rl = trainDataSet->getithRankList(i+1);
        //trainDataSet->getithRankList(i+1)->getCorrectRanking(rl);
        for(int j = 0; j < rl->dp_cnt_ - 1; j++)
            for (int k = j+1; k < rl->dp_cnt_; k++)
                if (rl->getiDataPoint(j)->getLabel() > rl->getiDataPoint(k)->getLabel())
                    totalPairs++;
        */
        vector<DataPoint*> dpv;
        trainDataSet->getithRankList(i+1)->getCorrectRanking(dpv);
        for (size_t j = 0; j != dpv.size() - 1; j++)
            for (int k = j+1; k != dpv.size(); k++)
                if (dpv[j]->getLabel() > dpv[k]->getLabel())
                    totalPairs++;
    }
    if (validateDataSet != NULL)
    {
        for (std::size_t  i = 0; i != layers->size(); i++)
        {
            vector<double> v;
            bestModelOnValidation.push_back(v);
        }
    }
}


void RankNet::learn()
{
    for (int i = 0; i < nIteration; i++)
    {
        cout << "iteration: "<< i <<endl;
        for (int j = 0; j < trainDataSet->total_query_cnt; j++)
        {
            cout << "queryid: " << j << endl;
            RankList * rl = trainDataSet->getithRankList(j+1);
            int dpcnt = rl->getDatapointsCnt();
            int *paircnts = new int[dpcnt];
            int **pairMap = new int*[dpcnt];

            batchFeedForward(rl, pairMap, paircnts);
            batchBackPropagate(pairMap, dpcnt, paircnts);

            // 后续优化
            delete paircnts;
            for (int k = 0; k < dpcnt; k++)
            {
                delete pairMap[k];
                pairMap[k] = NULL;
            }
            delete pairMap;
            pairMap = NULL;
            paircnts = NULL;
            // clearNeuronOutputs???
        }

        for (int j = 0; j < trainDataSet->total_query_cnt; j++)
        {
            RankList * rl = trainDataSet->getithRankList(j+1);
            for (int i = 0; i < rl->getDatapointsCnt(); i++)
            {
                double cur_val = eval(rl->getiDataPoint(i));
                rl->getiDataPoint(i)->setModelscore(cur_val);
            }
            // sort rl's datapoint by cached.
            // todo
            rl->sortByModelscore();
            scoreOnTrainData += trainScorer->score(rl);
        }
        scoreOnTrainData /= trainDataSet->total_query_cnt;
        cout << "scoreOnTrainData: " << scoreOnTrainData<<endl;
        estimateLoss();
    }
}




double RankNet::crossEntropy(double o1, double o2, double targetValue)
{
    double oij = o1 - o2;
    double ce = -targetValue * oij + log(1+exp(oij))/log(2);
    return  ce;
}
void RankNet::estimateLoss()
{
    misorderedPairs = 0;
    error = 0.0;
    for (int j = 0; j < trainDataSet->total_query_cnt; j++)
    {
        RankList *rl = trainDataSet->getithRankList(j+1);
        for (int k = 0; k < rl->getDatapointsCnt(); k++)
        {
            double o1 = eval(rl->getiDataPoint(k));
            for (int l = k + 1; l < rl->getDatapointsCnt(); l++)
            {
                if (rl->getiDataPoint(k)->getLabel() > rl->getiDataPoint(l)->getLabel())
                {
                    double o2 = eval(rl->getiDataPoint(l));
                    error += crossEntropy(o1, o2, 1.0f);
                    if (o1 < o2)
                        misorderedPairs++;
                }
            }
        }
    }
    error = 1.0 * error/totalPairs;
    lastError = error;
    cout << "error: " << error << "  misorderedPairs: " <<misorderedPairs<< endl;
}
double RankNet::eval(DataPoint *p)
{
    double val = 0.0;
    for (int  k = 0; k < inputLayer->size() - 1; k++)
        inputLayer->get(k)->setOutput(p->getiFeature(k));
    inputLayer->get(inputLayer->size() - 1)->setOutput(1.0f);

    for (std::size_t  k = 1; k < layers->size(); k++)
        layers->at(k)->computeOutput();
    val = outputLayer->get(0)->getOutput();

    return val;
}

string RankNet::model()
{
    string m;
    return m;
}
string RankNet::toString()
{
    string m;
    return m;
}
string RankNet::name()
{
    string m;
    return m;
}
void RankNet::printParameters()
{


}
