#include <gui/screen_screen/screenView.hpp>
#include <cstring>
#include "main.h"
#include "usb_host.h"
#include "fatfs.h"

extern "C" uint8_t doWrite;

//const uint8_t imageStaticData[] =
//{ 164, 73, 163, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
//		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 36, 28, 237, 36,
//		28, 237, 204, 72, 63, 204, 72, 63, 204, 72, 63, 204, 72, 63, 164, 73,
//		163, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
//		255, 255, 255, 255, 255, 36, 28, 237, 36, 28, 237, 36, 28, 237, 255,
//		255, 255, 204, 72, 63, 204, 72, 63, 204, 72, 63, 204, 72, 63, 164, 73,
//		163, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 36, 28,
//		237, 36, 28, 237, 36, 28, 237, 255, 255, 255, 255, 255, 255, 255, 255,
//		255, 204, 72, 63, 204, 72, 63, 204, 72, 63, 204, 72, 63, 164, 73, 163,
//		255, 255, 255, 36, 28, 237, 36, 28, 237, 36, 28, 237, 255, 255, 255,
//		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
//		255, 204, 72, 63, 204, 72, 63, 204, 72, 63, 204, 72, 63, 164, 73, 163,
//		255, 255, 255, 36, 28, 237, 255, 255, 255, 39, 127, 255, 39, 127, 255,
//		39, 127, 255, 39, 127, 255, 39, 127, 255, 39, 127, 255, 76, 177, 34,
//		204, 72, 63, 204, 72, 63, 204, 72, 63, 204, 72, 63, 164, 73, 163, 39,
//		127, 255, 39, 127, 255, 39, 127, 255, 255, 255, 255, 255, 255, 255, 255,
//		255, 255, 255, 255, 255, 255, 255, 255, 76, 177, 34, 39, 127, 255, 204,
//		72, 63, 204, 72, 63, 204, 72, 63, 204, 72, 63, 164, 73, 163, 39, 127,
//		255, 255, 255, 255, 255, 255, 255, 39, 127, 255, 39, 127, 255, 39, 127,
//		255, 39, 127, 255, 76, 177, 34, 255, 255, 255, 255, 255, 255, 204, 72,
//		63, 204, 72, 63, 204, 72, 63, 204, 72, 63, 164, 73, 163, 39, 127, 255,
//		255, 255, 255, 255, 255, 255, 39, 127, 255, 255, 255, 255, 255, 255,
//		255, 39, 127, 255, 39, 127, 255, 255, 255, 255, 255, 255, 255, 204, 72,
//		63, 204, 72, 63, 204, 72, 63, 204, 72, 63, 164, 73, 163, 36, 28, 237,
//		39, 127, 255, 255, 255, 255, 39, 127, 255, 255, 255, 255, 76, 177, 34,
//		76, 177, 34, 39, 127, 255, 39, 127, 255, 255, 255, 255, 204, 72, 63,
//		204, 72, 63, 204, 72, 63, 204, 72, 63, 164, 73, 163, 255, 255, 255, 39,
//		127, 255, 39, 127, 255, 255, 255, 255, 39, 127, 255, 39, 127, 255, 39,
//		127, 255, 39, 127, 255, 39, 127, 255, 39, 127, 255, 204, 72, 63, 204,
//		72, 63, 204, 72, 63, 204, 72, 63, 164, 73, 163, 255, 255, 255, 255, 255,
//		255, 39, 127, 255, 39, 127, 255, 76, 177, 34, 76, 177, 34, 255, 255,
//		255, 255, 255, 255, 255, 255, 255, 39, 127, 255, 204, 72, 63, 204, 72,
//		63, 204, 72, 63, 204, 72, 63, 164, 73, 163, 255, 255, 255, 255, 255,
//		255, 255, 255, 255, 255, 255, 255, 39, 127, 255, 39, 127, 255, 255, 255,
//		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 204, 72, 63, 204, 72,
//		63, 204, 72, 63, 204, 72, 63, 164, 73, 163, 255, 255, 255, 255, 255,
//		255, 255, 255, 255, 76, 177, 34, 76, 177, 34, 255, 255, 255, 39, 127,
//		255, 39, 127, 255, 39, 127, 255, 255, 255, 255, 204, 72, 63, 204, 72,
//		63, 204, 72, 63, 204, 72, 63, 164, 73, 163, 255, 255, 255, 255, 255,
//		255, 255, 255, 255, 76, 177, 34, 255, 255, 255, 255, 255, 255, 255, 255,
//		255, 255, 255, 255, 255, 255, 255, 39, 127, 255, 204, 72, 63, 204, 72,
//		63, 204, 72, 63, 204, 72, 63, 164, 73, 163, 255, 255, 255, 255, 255,
//		255, 76, 177, 34, 76, 177, 34, 255, 255, 255, 255, 255, 255, 255, 255,
//		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 204, 72, 63, 204, 72,
//		63, 204, 72, 63, 204, 72, 63 };

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
	Unicode::snprintf(textAreaBuffer, textAreaBufferSize,
			reinterpret_cast<const char*>(text));
	textArea1.setWildcard(textAreaBuffer);
	textArea1.invalidate();
}

