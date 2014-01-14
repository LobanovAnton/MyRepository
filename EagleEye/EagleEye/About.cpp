#include "About.h"
#include "Utils.h"
#include "resource.h"
#include "wchar.h"

void * scan0Src;
void * scan0Dst;

unsigned char bumpMap1[408][249];
unsigned char bumpMap2[408][249];

unsigned char * pBumpMap1;
unsigned char * pBumpMap2;

void PutDrop(unsigned short x, unsigned short y)
{
	if (y < 3 || y > 245 || x > 408)
		return;

	unsigned char * pBump = (unsigned char*)(pBumpMap1 + x + (y * 408));
	*(pBump - 407) = 0xFF;
	*(pBump - 408) = 0xFF;
	*(pBump - 409) = 0xFF;
	*(pBump - 1) = 0xFF;
	*(pBump + 1) = 0xFF;
	*(pBump + 407) = 0xFF;
	*(pBump + 408) = 0xFF;
	*(pBump + 409) = 0xFF;
}

void DrawWater()
{
	unsigned char * pBump  = pBumpMap1 + 816 + 2;
	unsigned char * pBump2  = pBumpMap2 + 816 + 2;

	for (unsigned int count = 245 * 408 - 4; count > 0; --count)
	{
		int c = 0;
		c += *(pBump - 818);
		c += *(pBump - 817);
		c += *(pBump - 816);
		c += *(pBump - 815);
		c += *(pBump - 814);

		c += *(pBump - 410);
		c += *(pBump - 409);
		c += *(pBump - 408);
		c += *(pBump - 407);
		c += *(pBump - 406);


		c += *(pBump - 2);
		c += *(pBump - 1);
		//c += *(pBump);
		c += *(pBump + 1);		
		c += *(pBump + 2);


		c += *(pBump + 406);
		c += *(pBump + 407);
		c += *(pBump + 408);
		c += *(pBump + 409);
		c += *(pBump + 410);


		c += *(pBump + 814);
		c += *(pBump + 815);
		c += *(pBump + 816);
		c += *(pBump + 817);
		c += *(pBump + 818);

		c *= 5461;
		c >>= 16;
		c -= *pBump2;
		if (c < 0)
			c = 0;
		*pBump2 = (char)c;
		pBump++;
		pBump2++;
	}


	unsigned long * pSrc = (unsigned long*)scan0Src;
	unsigned long * pDest = (unsigned long*)scan0Dst + 408;
	pBump = pBumpMap1 + 408;

	for (unsigned long yCnt = 1; yCnt < 248; ++yCnt)
	{
		for (unsigned long xCnt = 0; xCnt < 408; ++xCnt)
		{
			unsigned long srcYY = yCnt + ((*(pBump + 408) - *(pBump - 408)) >> 2);
			unsigned long srcXX = xCnt + ((*(pBump + 1) - *(pBump - 1)) >> 2);

			if (srcXX < 408 && srcYY < 249)
				*pDest = *(pSrc + srcYY * 408 + srcXX);

			pDest++;
			pBump++;
		}
	}

	unsigned char * pTemp = pBumpMap1;
	pBumpMap1 = pBumpMap2;
	pBumpMap2 = pTemp;

}

LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static bool captured;
	static LONG width, height;
	static HBITMAP pictureSrc;
	static HBITMAP pictureDst;
	static HDC srcDC;
	static HDC dstDC;

	switch (message)
	{
	case WM_INITDIALOG:
		{
			WCHAR buffer[MAX_PATH + 1];
			WCHAR fileName[MAX_PATH + 1];
			GetModuleFileName((HINSTANCE)lParam, fileName, MAX_PATH);
			GetVersionInformation(fileName, L"ProductName", buffer, MAX_PATH);
			wcscat_s(buffer + wcslen(buffer), MAX_PATH - wcslen(buffer), L"\n\nVersion ");
			GetVersionInformation(fileName, L"ProductVersion", buffer + (int)wcslen(buffer), MAX_PATH - (int)wcslen(buffer));
			wcscat_s(buffer + wcslen(buffer), MAX_PATH - wcslen(buffer), L"\n\n");
			GetVersionInformation(fileName, L"LegalCopyright", buffer + (int)wcslen(buffer), MAX_PATH - (int)wcslen(buffer));
			wcscat_s(buffer + wcslen(buffer), MAX_PATH - wcslen(buffer), L"\n\n");
			GetVersionInformation(fileName, L"Comments", buffer + (int)wcslen(buffer), MAX_PATH - (int)wcslen(buffer));
			SetDlgItemText(hDlg, IDC_STATIC, buffer);

			captured = false;

			width = 408;
			height = 249;

			HDC hDC = GetDC(hDlg);

			srcDC = CreateCompatibleDC(hDC);
			dstDC = CreateCompatibleDC(hDC);

			BITMAPINFO bmi;

			bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bmi.bmiHeader.biWidth = width;
			bmi.bmiHeader.biHeight = -height;
			bmi.bmiHeader.biPlanes = 1;
			bmi.bmiHeader.biBitCount = 32;         // four 8-bit components
			bmi.bmiHeader.biCompression = BI_RGB;
			bmi.bmiHeader.biSizeImage = width * height * 4;

			pictureSrc = CreateDIBSection(srcDC, &bmi, DIB_RGB_COLORS, &scan0Src, NULL, NULL);
			pictureDst = CreateDIBSection(dstDC, &bmi, DIB_RGB_COLORS, &scan0Dst, NULL, NULL);

			ReleaseDC(hDlg, hDC);

			pBumpMap1 = (unsigned char*)bumpMap1;
			pBumpMap2 = (unsigned char*)bumpMap2;

		}
		break;

	case WM_MOUSEMOVE:
		{
			if (!captured)
			{
				HDC hDC = GetDC(hDlg);
				HGDIOBJ oldSrcDC = SelectObject(srcDC, pictureSrc);
				HGDIOBJ oldDstDC = SelectObject(dstDC, pictureDst);
				BitBlt(srcDC, 0, 0, width, height, hDC, 0, 0, SRCCOPY);
				BitBlt(dstDC, 0, 0, width, height, hDC, 0, 0, SRCCOPY);
				SelectObject(srcDC, oldSrcDC);
				SelectObject(dstDC, oldDstDC);

				SetTimer(hDlg, 2, 30, NULL);
				captured = true;
			}
			PutDrop(LOWORD(lParam), HIWORD(lParam));
		}
		break;
	case WM_TIMER:
		{
			DrawWater(); 
			HDC pcDC = GetDC(hDlg);
			HGDIOBJ oldDstDC = SelectObject(dstDC, pictureDst);
			BitBlt(pcDC, 0, 0, width, height, dstDC, 0, 0, SRCCOPY);
			SelectObject(dstDC, oldDstDC);
			ReleaseDC(hDlg, pcDC);
		}
		break;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
		{
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	case WM_DESTROY:

		KillTimer(hDlg, 2);
		DeleteDC(srcDC);
		DeleteDC(dstDC);
		DeleteObject(pictureSrc);
		DeleteObject(pictureDst);
		break;
	}
	return FALSE;
}
