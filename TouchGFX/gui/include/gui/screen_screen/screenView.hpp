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
	void writeRadioCallback() override;
	void readRadioCallback() override;

	void updateImage(uint8_t *img);
	void updateText(const uint8_t *text);

protected:
	//Image dynamicImage;
	BitmapId dynamicBitmapId;
	ScalableImage dynamicImage;

	static constexpr unsigned textAreaBufferSize=256;
	touchgfx::Unicode::UnicodeChar textAreaBuffer[textAreaBufferSize];
};

#endif // SCREENVIEW_HPP
