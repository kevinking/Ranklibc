#ifndef LAYER_H
#define LAYER_H
#include <vector>
#include <iostream>
using std::vector;
using std::cout;
#include "Neuron.h"

class Layer
{
public:
    Layer() { }
    Layer(int n)
    {
        size_ = n;
        neurons = new  vector<Neuron*>();
        for (int i = 0; i < n; i++)
            neurons->push_back(new Neuron());
    }
    virtual ~Layer()
    {
        if (neurons)
        {
            for (std::size_t i = 0; i < neurons->size(); i++)
                if(neurons->at(i))
                    delete neurons->at(i);
        }
    }


    vector<Neuron*> *neurons;
    int size_;

    Neuron* get(int k)
    {
        return neurons->at(k);
    }

    int size()
    {
        return size_;
    }
    //
    void computerDeltaForOutputlayer(PropParameter *param)
    {
        for (std::size_t i = 0; i < neurons->size(); i++)
        {
            neurons->at(i)->computerDeltaOfOuputLayer(param);
        }
    }

    void updateDelta(PropParameter *param)
    {
        for (std::size_t i = 0; i < neurons->size(); i++)
            neurons->at(i)->updateDelta(param);
    }

    void updateWeight(PropParameter *param)
    {
        for (std::size_t i = 0; i < neurons->size(); i++)
            neurons->at(i)->updateWeight(param);
    }

    void computeOutput()
    {
        for (vector<Neuron*>::iterator it = neurons->begin(); it != neurons->end(); it++)
        {
            (*it)->computeOutput();
        }
    }

    void computeOutput(int i)
    {
        for (vector<Neuron*>::iterator it = neurons->begin(); it != neurons->end(); it++)
        {
            (*it)->computeOutput(i);
        }
    }
protected:
private:
};

#endif // LAYER_H
