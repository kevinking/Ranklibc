#ifndef SYNAPSE_H
#define SYNAPSE_H

#include "Neuron.h"

class Neuron;
class Synapse
{
public:
    Synapse()
    {
    }
    Synapse(Neuron *s, Neuron *t)
    {
        source = s;
        target = t;
    }
    virtual ~Synapse()
    {

    }

    double weight;
    double dW; // last weight adjustment
    Neuron *source;
    Neuron *target;

    Neuron* getSource()
    {
        return source;
    }
    Neuron* getTarget()
    {
        return target;
    }
    void setSource(Neuron* s)
    {
        source = s;
    }
    void setTarget(Neuron* t)
    {
        target = t;
    }
    double getWeight()
    {
        return weight;
    }
    void setWeight(double w)
    {
        weight = w;
    }
    double getdw()
    {
        return dW;
    }
    void setdw(double d)
    {
        dW = d;
    }
    
    void setWeightAdjust(double dwa)
    {
        dW =dwa;
    }

    void updateWeight()
    {
        weight += dW;
    }
protected:
private:
};

#endif // SYNAPSE_H
