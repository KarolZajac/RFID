#include <gui/screen_screen/screenView.hpp>
#include <cstring>
#include "main.h"
#include "usb_host.h"
#include "fatfs.h"

const uint8_t imageStaticData[] =
{ 164, 73, 163, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 36, 28, 237, 36,
		28, 237, 204, 72, 63, 204, 72, 63, 204, 72, 63, 204, 72, 63, 164, 73,
		163, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 36, 28, 237, 36, 28, 237, 36, 28, 237, 255,
		255, 255, 204, 72, 63, 204, 72, 63, 204, 72, 63, 204, 72, 63, 164, 73,
		163, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 36, 28,
		237, 36, 28, 237, 36, 28, 237, 255, 255, 255, 255, 255, 255, 255, 255,
		255, 204, 72, 63, 204, 72, 63, 204, 72, 63, 204, 72, 63, 164, 73, 163,
		255, 255, 255, 36, 28, 237, 36, 28, 237, 36, 28, 237, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		255, 204, 72, 63, 204, 72, 63, 204, 72, 63, 204, 72, 63, 164, 73, 163,
		255, 255, 255, 36, 28, 237, 255, 255, 255, 39, 127, 255, 39, 127, 255,
		39, 127, 255, 39, 127, 255, 39, 127, 255, 39, 127, 255, 76, 177, 34,
		204, 72, 63, 204, 72, 63, 204, 72, 63, 204, 72, 63, 164, 73, 163, 39,
		127, 255, 39, 127, 255, 39, 127, 255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 76, 177, 34, 39, 127, 255, 204,
		72, 63, 204, 72, 63, 204, 72, 63, 204, 72, 63, 164, 73, 163, 39, 127,
		255, 255, 255, 255, 255, 255, 255, 39, 127, 255, 39, 127, 255, 39, 127,
		255, 39, 127, 255, 76, 177, 34, 255, 255, 255, 255, 255, 255, 204, 72,
		63, 204, 72, 63, 204, 72, 63, 204, 72, 63, 164, 73, 163, 39, 127, 255,
		255, 255, 255, 255, 255, 255, 39, 127, 255, 255, 255, 255, 255, 255,
		255, 39, 127, 255, 39, 127, 255, 255, 255, 255, 255, 255, 255, 204, 72,
		63, 204, 72, 63, 204, 72, 63, 204, 72, 63, 164, 73, 163, 36, 28, 237,
		39, 127, 255, 255, 255, 255, 39, 127, 255, 255, 255, 255, 76, 177, 34,
		76, 177, 34, 39, 127, 255, 39, 127, 255, 255, 255, 255, 204, 72, 63,
		204, 72, 63, 204, 72, 63, 204, 72, 63, 164, 73, 163, 255, 255, 255, 39,
		127, 255, 39, 127, 255, 255, 255, 255, 39, 127, 255, 39, 127, 255, 39,
		127, 255, 39, 127, 255, 39, 127, 255, 39, 127, 255, 204, 72, 63, 204,
		72, 63, 204, 72, 63, 204, 72, 63, 164, 73, 163, 255, 255, 255, 255, 255,
		255, 39, 127, 255, 39, 127, 255, 76, 177, 34, 76, 177, 34, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 39, 127, 255, 204, 72, 63, 204, 72,
		63, 204, 72, 63, 204, 72, 63, 164, 73, 163, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 39, 127, 255, 39, 127, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 204, 72, 63, 204, 72,
		63, 204, 72, 63, 204, 72, 63, 164, 73, 163, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 76, 177, 34, 76, 177, 34, 255, 255, 255, 39, 127,
		255, 39, 127, 255, 39, 127, 255, 255, 255, 255, 204, 72, 63, 204, 72,
		63, 204, 72, 63, 204, 72, 63, 164, 73, 163, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 76, 177, 34, 255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 39, 127, 255, 204, 72, 63, 204, 72,
		63, 204, 72, 63, 204, 72, 63, 164, 73, 163, 255, 255, 255, 255, 255,
		255, 76, 177, 34, 76, 177, 34, 255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 204, 72, 63, 204, 72,
		63, 204, 72, 63, 204, 72, 63 };

constexpr uint8_t imgWidth = 15, imgHeight = 15;
uint8_t *dynamicImageBuffer
{ nullptr };

screenView::screenView()
{
	dynamicBitmapId = Bitmap::dynamicBitmapCreate(imgWidth, imgHeight,
			Bitmap::RGB888);
	Bitmap dynamicBitmap = Bitmap(dynamicBitmapId);
	uint8_t *dynBuffer = Bitmap::dynamicBitmapGetAddress(dynamicBitmapId);

	std::memcpy(dynBuffer, imageStaticData, sizeof(imageStaticData)/2);
	dynamicImage.setBitmap(dynamicBitmap);
	dynamicImage.setXY(85, 165);
	dynamicImage.setScalingAlgorithm(ScalableImage::NEAREST_NEIGHBOR);
	dynamicImage.setWidthHeight(150, 150);
	add(dynamicImage);
}
extern ApplicationTypeDef Appli_state;
void screenView::buttonPressedCallback()
{
	if (Appli_state == APPLICATION_READY)
	{
		FIL f;
		if (f_open(&f, "0:/img.bin", FA_READ) != FR_OK)
		{
			xprintf("Failed to open file!\n");
			return;
		}
		int sumDumped = 0;

		char buffer[17];
		buffer[16] = 0;
		unsigned numread = 0;

		uint8_t *imgBuffer = Bitmap::dynamicBitmapGetAddress(dynamicBitmapId);
		while (f_read(&f, buffer, 16, &numread) == FR_OK && numread != 0 && sumDumped <= imgWidth*imgHeight*3)
		{
			memcpy(&imgBuffer[sumDumped], buffer, numread);
			sumDumped += numread;//xprintf("Read chunk: %s\n", buffer);
		}
		xprintf("Read %d bytes from file.\n", sumDumped);
		f_close(&f);
		dynamicImage.invalidate();
	}
}

void screenView::setupScreen()
{
	screenViewBase::setupScreen();
}

void screenView::tearDownScreen()
{
	screenViewBase::tearDownScreen();
}
