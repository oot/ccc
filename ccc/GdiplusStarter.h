#pragma once

#include <gdiplus.h>

class GdiplusStarter
{
public:
	GdiplusStarter(void);
	~GdiplusStarter(void);

	bool isSuccess;

private:
     ULONG_PTR _gpToken;
};
