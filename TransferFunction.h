#ifndef TRANSFERFUNCTION_H
#define TRANSFERFUNCTION_H

#include <math.h>

class TransferFunction
{
public:
    TransferFunction();
    virtual ~TransferFunction();

    virtual double compute(double x) = 0;
    virtual double computeDerivative(double x) = 0;
protected:
private:
};

class LogiFunction : public TransferFunction
{
public:
    LogiFunction():TransferFunction()
    {
    }
    virtual ~LogiFunction() {}

    double compute(double x)
    {
        return (double) (1.0 / (1.0 + exp(-x)));
    }

    double computeDerivative(double x)
    {

        double output = (double) (1.0 / (1.0 + exp(-x)));
        return (double) (output * (1.0 - output));
    }
};
#endif // TRANSFERFUNCTION_H
