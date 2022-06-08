#ifndef MODELLISTENER_HPP
#define MODELLISTENER_HPP

#include <gui/model/Model.hpp>
#include "Message.h"

class ModelListener
{
public:
	ModelListener() :
			model(0)
	{
	}

	virtual ~ModelListener()
	{
	}

	void bind(Model *m)
	{
		model = m;
	}

	virtual void notifyDataUpdated(toDisplayMessage msg)
	{
	}
protected:
	Model *model;
};

#endif // MODELLISTENER_HPP
