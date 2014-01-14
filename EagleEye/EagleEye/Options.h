#ifndef __IT__
#define __IT__

#include "gdiplus.h"
using namespace Gdiplus;

#define MAX_LOADSTRING 100

enum eCheckOptions
{
	ECO_FULLSCREEN,
	ECO_BILINEAR,
	ECO_FOLDER_CONTEXT,

	ECO_COUNT
};

enum eGetPhotosOptions
{
	EGPO_DELETE_FILES_AFTER,
	EGPO_KEEP_FILES_NAMES,
	EGPO_DATE_FILE_NAMES,
	EGPO_GO_TO_DIRECTORY,

	EGPO_COUNT
};

struct Options
{
	Color backThColor;
	Color borderColor;
	bool checkOptions[ECO_COUNT];
	int thumbnailsSize;
	int splitterSize;
	int treeWidth;
	int treeHeight;
	int thumbWindowWidth;
	int thumbWindowHeight;
	bool checkOptionsGetPhotos[EGPO_COUNT];
	DWORD viewMode;
	WCHAR currentCacheDirectory[MAX_PATH + 1];
	WCHAR currentViewDirectory[MAX_PATH + 1];
	WCHAR currentCaptureFolder[MAX_PATH + 1];
	WCHAR currentCaptureSubFolder[MAX_PATH + 1];
};

#endif //__IT__