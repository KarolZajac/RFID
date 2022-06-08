#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>

#include "FreeRTOS.h"
#include "queue.h"
#include "main.h"


extern "C" QueueHandle_t toDisplayQueue;

Model::Model() : modelListener(0)
{

}

void Model::tick()
{
	auto result = xQueueReceive(toDisplayQueue, &msg, 0);
	if(result == pdTRUE)
	{
		xprintf("Model received data batch!\n");
		wasUpdated = true;
		modelListener->notifyDataUpdated(msg);
	}
}

bool Model::newDataAvailable()
{
	if(wasUpdated)
	{
		wasUpdated = false;
		return true;
	}
	return false;
}
toDisplayMessage Model::getData() const
{
	return msg;
}
