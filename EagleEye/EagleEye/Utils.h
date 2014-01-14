#ifndef __UTILS__
#define __UTILS__

#include "windows.h"
#include "wchar.h"
#include "gdiplus.h"

using namespace Gdiplus;

void FitAndCenterImage(int areaWidth, 
					   int areaHeight, 
					   int imageWidth, 
					   int imageHeight, 
					   int & x, 
					   int & y, 
					   int & newWidth, 
					   int & newHeight);

void FileTimeToLocalTimeString(FILETIME * fileTime, WCHAR * string, int countMax);

void MakeCrcTable(void);

unsigned int CalcCrc(const unsigned char * pBuffer, unsigned int nLen);

unsigned int CalculateUniqueColors(Bitmap * bitmap);

bool GetVersionInformation(WCHAR * fileName, WCHAR * versionInformation, WCHAR * buffer, int cCount);

void GetNearestMonitorWorkAndScreenArea(HWND window, RECT * workArea, RECT * screenArea);

#endif //__UTILS__