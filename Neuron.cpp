#include "Neuron.h"
#include "Synapse.h"
#include <stdlib.h>

Neuron::Neuron()
{
    tfunc   = new LogiFunction();
    output  = 0.0;// sigmoid(wsum)
    outputs = new vector<double>() ;
    delta_i = 0.0;
    deltas_j = new vector<double>();

    inLinks = new vector<Synapse*>();
    outLinks= new vector<Synapse*>();
}

void Neuron::computeOutput()
{
    Synapse *s;
    double wsum = 0.0;
    for (std::size_t j = 0; j != inLinks->size(); j ++)
    {
        s = inLinks->at(j);
        wsum += s->getSource()->getOutput() * s->getWeight();
    }
    output = tfunc->compute(wsum);
}

void Neuron::computeOutput(int i)
{
    Synapse *s;
    double wsum = 0.0;
    for (std::size_t j = 0; j != inLinks->size(); j ++)
    {
        s = inLinks->at(j);
        wsum += s->getSource()->getOutput(i) * s->getWeight();
    }
    outputs->push_back(tfunc->compute(wsum));
}

void Neuron::computerDeltaOfOuputLayer(PropParameter *param)
{
    int ** pairMap = param->pairMap;
    int current = param->current;

    for (int k = 0; k < param->pairsCnt; k++)
    {
        int j = pairMap[current][k];
        float weight = 1.0f;
        double Pij = 0;
        if (param->pairWeight == NULL) //RankNet
        {
            weight = 1.0f;
            Pij = (double) (1.0 / (1.0 + exp(outputs->at(current)-outputs->at(j))));
            //this is in fact not "pij", but "targetValue-pij": 1 - 1/(1+e^{-o_ij})
        }
        else  // LambdaRank
        {
            weight = param->pairWeight[current][k];
            Pij = (double)( param->targetValue[current][k] - 1.0 / (1.0 + exp(-(outputs->at(current)-outputs->at(j)))));
        }

        double lambda = weight * Pij;
        delta_i += lambda;
        deltas_j->push_back(lambda * tfunc->computeDerivative(outputs->at(j)));
    }

    delta_i *= tfunc->computeDerivative(outputs->at(current));
}

void Neuron::updateDelta(PropParameter *param)
{
    int** pairMap = param->pairMap;
    int current = param->current;
    float** pairWeight = param->pairWeight;
    delta_i = 0;
    for (int k = 0; k < param->pairsCnt; k++)
    {
        int j = pairMap[current][k];
        float weight;
        if (pairWeight == NULL)
            weight = 1.0f;
        else
            weight = pairWeight[current][k];
        double errorSum = 0.0;
        for (std::size_t l = 0; l < outLinks->size(); l++)
        {
            Synapse *s = outLinks->at(l);
            errorSum += s->getTarget()->deltas_j->at(k) * s->weight;
            if (k == 0)
                delta_i += s->getTarget()->delta_i * s->weight;
        }
        if (k == 0)
            delta_i *= weight * tfunc->computeDerivative(outputs->at(current));
        deltas_j->push_back(errorSum * weight * tfunc->computeDerivative(outputs->at(j)));
    }
}

void Neuron::updateWeight(PropParameter *param)
{
   for (std::size_t k = 0; k < inLinks->size(); k++)
   {
        Synapse *s = inLinks->at(k);
        double sum_j = 0.0;
        for (std::size_t l = 0; l < deltas_j->size(); l++)
        {
            sum_j += deltas_j->at(l) * (s->getSource()->getOutput(param->pairMap[param->current][l]));
        }
        double dw = learningRate * (delta_i * s->getSource()->getOutput(param->current) - sum_j);
        s->setWeightAdjust(dw);
        s->updateWeight();
   }
}

Neuron::~Neuron()
{
    //dtor
}
