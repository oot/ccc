#include "StdAfx.h"
#include "GdiplusStarter.h"

#pragma comment(lib, "gdiplus")

GdiplusStarter::GdiplusStarter(void)
{
	Gdiplus::GdiplusStartupInput gpsi;
	isSuccess=(Gdiplus::GdiplusStartup(&_gpToken,&gpsi,NULL) == Gdiplus::Ok);
}

GdiplusStarter::~GdiplusStarter(void)
{
	Gdiplus::GdiplusShutdown(_gpToken);
}
