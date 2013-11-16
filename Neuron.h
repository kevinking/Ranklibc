#ifndef NEURON_H
#define NEURON_H

#include "TransferFunction.h"
#include "Synapse.h"
#include "PropParameter.h"
#include <vector>
using std::vector;

class Synapse;
class Neuron
{
public:
    Neuron();
    virtual ~Neuron();

    static double momentum;
    static double learningRate;

    TransferFunction *tfunc;
    double output;// sigmoid(wsum)
    vector<double> *outputs;
    double delta_i;
    vector<double>* deltas_j;

    vector<Synapse*> *inLinks;
    vector<Synapse*> *outLinks;

    double getOutput()
    {
        return output;
    }

    double getOutput(int i)
    {
        return outputs->at(i);
    }
    void setOutput(double v)
    {
        output = v;
    }
    void addOutputs(double v)
    {
        outputs->push_back(v);
    }

    void computeOutput();
    void computeOutput(int i);
    void computerDeltaOfOuputLayer(PropParameter *param);
    void updateDelta(PropParameter *param);
    void updateWeight(PropParameter *param);
protected:
private:
};

#endif // NEURON_H
