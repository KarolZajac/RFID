#include <gui/screen_screen/screenView.hpp>
#include <gui/screen_screen/screenPresenter.hpp>

screenPresenter::screenPresenter(screenView& v)
    : view(v)
{

}

void screenPresenter::activate()
{

}

void screenPresenter::deactivate()
{

}

void screenPresenter::notifyDataUpdated(toDisplayMessage msg)
{
	if(msg.imgData)
		view.updateImage(msg.imgData);

	if(msg.textDat)
		view.updateText(msg.textDat);
}
