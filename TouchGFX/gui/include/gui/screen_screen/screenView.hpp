#ifndef SCREENVIEW_HPP
#define SCREENVIEW_HPP

#include <gui_generated/screen_screen/screenViewBase.hpp>
#include <gui/screen_screen/screenPresenter.hpp>
#include "touchgfx/widgets/ScalableImage.hpp"

class screenView: public screenViewBase
{
public:
	screenView();
	virtual ~screenView()
	{
	}
	virtual void setupScreen();
	virtual void tearDownScreen();
	void buttonPressedCallback() override;
protected:
	//Image dynamicImage;
	BitmapId dynamicBitmapId;
	ScalableImage dynamicImage;
};

#endif // SCREENVIEW_HPP
