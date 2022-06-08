#include <gui/screen_screen/screenView.hpp>
#include <cstring>
#include "main.h"
#include "usb_host.h"
#include "fatfs.h"

extern "C" uint8_t doWrite;

constexpr uint8_t imgWidth = 15, imgHeight = 15;
constexpr uint16_t dynamicBitmapSize = imgWidth * imgHeight * 3;

screenView::screenView()
{
	textArea1.setWideTextAction(touchgfx::WIDE_TEXT_WORDWRAP);

	dynamicBitmapId = Bitmap::dynamicBitmapCreate(imgWidth, imgHeight,
			Bitmap::RGB888);
	Bitmap dynamicBitmap = Bitmap(dynamicBitmapId);
	uint8_t *dynBuffer = Bitmap::dynamicBitmapGetAddress(dynamicBitmapId);

	//std::memcpy(dynBuffer, imageStaticData, sizeof(imageStaticData) / 2);
	dynamicImage.setBitmap(dynamicBitmap);
	dynamicImage.setXY(85, 165);
	dynamicImage.setScalingAlgorithm(ScalableImage::NEAREST_NEIGHBOR);
	dynamicImage.setWidthHeight(150, 150);
	add(dynamicImage);
}
extern ApplicationTypeDef Appli_state;

void screenView::setupScreen()
{
	screenViewBase::setupScreen();
}

void screenView::tearDownScreen()
{
	screenViewBase::tearDownScreen();
}

void screenView::writeRadioCallback()
{
	Unicode::snprintf(textAreaBuffer, textAreaBufferSize, "Writing mode");
	textArea1.setWildcard(textAreaBuffer);
	textArea1.invalidate();
	uint8_t *dynBuffer = Bitmap::dynamicBitmapGetAddress(dynamicBitmapId);
	//std::memcpy(dynBuffer, img, 15 * 15 * 3);
	std::memset(dynBuffer, 0, dynamicBitmapSize);
	dynamicImage.invalidate();
	doWrite = 1;
}

void screenView::readRadioCallback()
{
	Unicode::snprintf(textAreaBuffer, textAreaBufferSize, "Reading mode");
	textArea1.setWildcard(textAreaBuffer);
	textArea1.invalidate();
	doWrite = 0;
}

void screenView::updateImage(uint8_t *img)
{
	uint8_t *dynBuffer = Bitmap::dynamicBitmapGetAddress(dynamicBitmapId);
	std::memcpy(dynBuffer, img, 15 * 15 * 3);
	dynamicImage.invalidate();
}

void screenView::updateText(const uint8_t *text)
{
	const auto textToDisplay = reinterpret_cast<const char*>(text);
	xprintf("Displaying text of length:%d\n", strlen(textToDisplay));

	Unicode::snprintf(textAreaBuffer, textAreaBufferSize, textToDisplay);
	textArea1.setWildcard(textAreaBuffer);
	textArea1.invalidate();
}

