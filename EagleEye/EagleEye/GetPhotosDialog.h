#ifndef __GFD__
#define __GFD__

#define _WIN32_IE 0x0600
#define _WIN32_WINNT 0x501

#include "windows.h"
#include "wchar.h"
#include "commctrl.h"
#include "shlobj.h"
#include "resource.h"
#include "Options.h"
#include "WiaHelper.h"

enum eStoreFileName
{
	ESFN_KEEP_FILENAME,
	ESFN_DATE_FILENAME,

	ESFN_COUNT
};

struct GETPHOTOS
{
	bool gotoPath;
	WCHAR path[MAX_PATH + 1];
};

HRESULT GetPhotos(HINSTANCE hInst, HWND hwnd, Options * _options, GETPHOTOS * getPhotos);

LRESULT CALLBACK GetPhotosDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);


#endif // __GFD__