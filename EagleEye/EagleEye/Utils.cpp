#include "Utils.h"

void FitAndCenterImage(int areaWidth, 
					   int areaHeight, 
					   int imageWidth, 
					   int imageHeight, 
					   int & x, 
					   int & y, 
					   int & newWidth, 
					   int & newHeight)
{
	float a = (float)areaWidth / imageWidth;
	float b = (float)areaHeight / imageHeight;
	if (b < a)
		a = b;

	newWidth = (int)(imageWidth * a);
	newHeight = (int)(imageHeight * a);
	x = (areaWidth - newWidth) >> 1;
	y = (areaHeight - newHeight) >> 1;
}

void FileTimeToLocalTimeString(FILETIME * fileTime, WCHAR * string, int countMax)
{
	SYSTEMTIME sysTime;
	SYSTEMTIME localTime;
	FileTimeToSystemTime(fileTime, &sysTime);
	SystemTimeToTzSpecificLocalTime(NULL, &sysTime, &localTime);
	swprintf_s(string,  countMax, L"%02d.%02d.%d %02d-%02d-%02d",
			   localTime.wDay, localTime.wMonth, localTime.wYear,
			   localTime.wHour, localTime.wMinute, localTime.wSecond);
}

unsigned int crcTable[256];

void MakeCrcTable(void)
{
	unsigned int c;
	unsigned int n, k;
	for (n = 0; n < 256; n++)
	{
		c = n;
		for (k = 0; k < 8; k++)
		{
			if ( (c & 1) == 1)
			{
				c = 0xedb88320 ^ (c >> 1);
			}
			else
			{
				c = (c >> 1);
			}
		}
		crcTable[n] = c;
	}
}

unsigned int CalcCrc(const unsigned char * pBuffer, unsigned int nLen)
{ 
	unsigned int crc = 0xffffffff;
	for (unsigned int i = 0; i < nLen; i++)
	{        
		crc = crcTable[ (unsigned int) ( (crc ^ (unsigned char)(pBuffer[i])) & 0xff)] ^ (crc >> 8);
	}
	crc ^= 0xffffffffL;
	return crc;
}

unsigned int CalculateUniqueColors(Bitmap * bitmap)
{
	BitmapData bitmapData;
	Rect rect = Rect::Rect(0, 0, bitmap->GetWidth(), bitmap->GetHeight());

	bitmap->LockBits(&rect, ImageLockModeRead, PixelFormat32bppARGB, &bitmapData);
	unsigned int * pixelBuffer = (unsigned int *)bitmapData.Scan0;
	unsigned int numOfColors = 0;
	unsigned int numOfPixels;

	numOfPixels = bitmap->GetWidth() * bitmap->GetHeight();
	unsigned int * colorMap = new unsigned int[0x80000];

	for (unsigned int i = 0; i < 0x80000; ++i)
	{
		*(colorMap + i) = 0;
	}
	for (unsigned int i = numOfPixels; i > 0; --i)
	{
		unsigned int pixel = (*pixelBuffer++) & 0x00FFFFFF;
		unsigned char bit = pixel & 0x1F;
		unsigned int offset = (pixel & 0x00FFFFE0) >> 5;
		if (!((*(colorMap + offset)) & (1 << bit)))
		{
			*(colorMap + offset) = (1 << bit) | (*(colorMap + offset));
			numOfColors++;
		}
	}

	delete colorMap;
	bitmap->UnlockBits(&bitmapData);

	return numOfColors;
}

bool GetVersionInformation(WCHAR * fileName, WCHAR * versionInformation, WCHAR * buffer, int cCount)
{
	DWORD p;
	int size = GetFileVersionInfoSize(fileName, &p);
	if (size)
	{
		void * data = new char[size];
		if (GetFileVersionInfo(fileName, p, size, data))
		{
			UINT len;
			struct LANGANDCODEPAGE 
			{
				WORD wLanguage;
				WORD wCodePage;
			}*lpTranslate;

			VerQueryValue(data, L"\\VarFileInfo\\Translation", (LPVOID*)&lpTranslate, &len);
			if (len)
			{
				void * value;
				WCHAR subBlock[MAX_PATH + 1];
				swprintf_s(subBlock, 
					MAX_PATH,
					L"\\StringFileInfo\\%04x%04x\\",
					lpTranslate->wLanguage,
					lpTranslate->wCodePage);

				wcscat_s(subBlock + wcslen(subBlock), MAX_PATH - wcslen(subBlock), versionInformation);

				VerQueryValue(data, 
					subBlock, 
					&value, 
					&len);

				if (len)
				{
					wcscpy_s(buffer, cCount, (WCHAR*)value);
					delete data;
					return true;
				}
			}
		}
		delete data;
	}

	return false;
}

void GetNearestMonitorWorkAndScreenArea(HWND window, RECT * workArea, RECT * screenArea)
{
	HMONITOR hMonitor = MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST);
	MONITORINFO monInfo;
	monInfo.cbSize = sizeof(MONITORINFO);
	GetMonitorInfo(hMonitor, &monInfo);
	workArea->top = monInfo.rcWork.top;
	workArea->bottom = monInfo.rcWork.bottom;
	workArea->left = monInfo.rcWork.left;
	workArea->right = monInfo.rcWork.right;

	screenArea->top = monInfo.rcMonitor.top;
	screenArea->bottom = monInfo.rcMonitor.bottom;
	screenArea->left = monInfo.rcMonitor.left;
	screenArea->right = monInfo.rcMonitor.right;

	return;
}