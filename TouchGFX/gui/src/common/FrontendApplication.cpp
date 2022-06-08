#include <gui/common/FrontendApplication.hpp>

constexpr uint16_t bitmapCacheSize{4096};
uint16_t bitmapCache[bitmapCacheSize/sizeof(uint16_t)];

FrontendApplication::FrontendApplication(Model& m, FrontendHeap& heap)
    : FrontendApplicationBase(m, heap)
{
	Bitmap::setCache(bitmapCache, bitmapCacheSize, 3);
}
