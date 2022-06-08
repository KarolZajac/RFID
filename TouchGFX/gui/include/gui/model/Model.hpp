#ifndef MODEL_HPP
#define MODEL_HPP

#include "Message.h"

class ModelListener;

class Model
{
public:
    Model();

    void bind(ModelListener* listener)
    {
        modelListener = listener;
    }

    bool newDataAvailable();
    toDisplayMessage getData() const;

    void tick();
protected:
    ModelListener* modelListener;
    bool wasUpdated=false;
    toDisplayMessage msg;
};

#endif // MODEL_HPP
