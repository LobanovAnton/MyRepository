#ifndef	__IV__
#define __IV__

#define GDIPVER 0x0100
#define _WIN32_IE 0x0600
#define _WIN32_WINNT 0x501

#define MAX_LOADSTRING 100

#define THUMBNAILS_SIZE 80
#define SPLITTER_SIZE 2
#define TREE_WIDTH 200
#define TREE_HEIGHT 300

#include "windows.h"
#include "windowsx.h"
#include "commctrl.h"
#include "math.h"
#include "wininet.h"
#include "shlobj.h"
#include "Shlwapi.h"
#include "gdiplus.h"
#include "resource.h"
#include "About.h"
#include "ShellHelper.h"
#include "Utils.h"
#include "Options.h"
#include "ExifProperty.h"
#include "DirTree.h"
#include "DataObject.h"
#include "DRAGDROP.h"
#include "GetPhotosDialog.h"
using namespace Gdiplus;

struct FileNode
{
	WCHAR fileName[MAX_PATH];
	FileNode * pNextNode;
	FileNode * pPrevNode;
	int fileCount;
	unsigned __int64 size;
	FILETIME fileTime;
	UINT width;
	UINT height;
};

enum eReturnCode
{
	ERC_CANCEL,
	ERC_NOIMAGE,
	ERC_OK
};

struct sProperties
{
	WCHAR * string;
	unsigned long tag;
};

const sProperties propertiesList[] = {
									L"Equip Make",PropertyTagEquipMake,
									L"Equip Model",PropertyTagEquipModel,
									L"Copyright",PropertyTagCopyright,
									L"Artist",PropertyTagArtist,
									L"Image description",PropertyTagImageDescription,
									L"User comment",PropertyTagExifUserComment,
									L"Pixel X dimension",PropertyTagExifPixXDim,
									L"Pixel Y dimension",PropertyTagExifPixYDim,
									L"Date/time",PropertyTagDateTime,
									L"Date/time original",PropertyTagExifDTOrig,
									L"Date/time digitized",PropertyTagExifDTDigitized,
									L"Orientation",PropertyTagOrientation,
									L"X Resolution",PropertyTagXResolution,
									L"Y Resolution",PropertyTagYResolution,
									L"Resolution unit",PropertyTagResolutionUnit,									
									L"F number",PropertyTagExifFNumber,
									L"Exposure time",PropertyTagExifExposureTime,
									L"Exposure bias value",PropertyTagExifExposureBias,
									L"Exposure programm",PropertyTagExifExposureProg,
									L"Exposure mode",PropertyTagExifExposureMode,
									L"White balance",PropertyTagExifWhiteBalance,
									L"Shutter speed value",PropertyTagExifShutterSpeed,
									L"Aperture value",PropertyTagExifAperture,
									L"Max aperture value",PropertyTagExifMaxAperture,
									L"Flash",PropertyTagExifFlash,
									L"Metering mode",PropertyTagExifMeteringMode,
									L"Focal length",PropertyTagExifFocalLength,
									L"Focal plane X res.",PropertyTagExifFocalXRes,
									L"Focal plane Y res.",PropertyTagExifFocalYRes,
									L"Focal plane res. unit",PropertyTagExifFocalResUnit,
									L"Subject distance",PropertyTagExifSubjectDist,
									L"Digital zoom ratio",PropertyTagExifDigitalZoomRatio,
									L"Scene captured type", PropertyTagExifSceneCapturedType,
									L"Sensing method",PropertyTagExifSensingMethod,
									L"Colorspace",PropertyTagExifColorSpace,
									L"Component config",PropertyTagExifCompConfig,
									L"EXIF version",PropertyTagExifVer,
									

									L"Macro mode", MacroMode,
									L"Quality compression rate", QualityCompressionRate,
									L"Drive mode", DriveMode,
									L"Focus mode", FocusMode,
									L"Easy shooting mode", EasyShoot,
									L"White Balance", WhiteBalance,
									L"Contrast", Contrast,
									L"Saturation", Saturation,
									L"Sharpness", Sharpness,
									L"Canon photo effect", CanonPhotoEffect,
									L"ISO speed", ISOSpeed,
									L"AF point selected", AFPoint,
									L"Exposure program", ExposureProgram,
									L"Distance", Distance,
									L"Digital zoom", DigitalZoom,
									L"Image size", ImageSize,
									L"Image type", Imagetype,
									L"Image number", ImageNumber,
									L"FirmWare", FirmWare
									};

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
WCHAR szFullScreen[MAX_LOADSTRING];
WCHAR szWindow[MAX_LOADSTRING];
WCHAR szBilinear[MAX_LOADSTRING];
WCHAR szNearest[MAX_LOADSTRING];
WCHAR szPixel[MAX_LOADSTRING];
WCHAR szUnknownFileFormat[MAX_LOADSTRING];
WCHAR sz4Indexed[MAX_LOADSTRING];
WCHAR sz8Indexed[MAX_LOADSTRING];
WCHAR sz16RGB[MAX_LOADSTRING];
WCHAR sz24RGB[MAX_LOADSTRING];
WCHAR sz32RGB[MAX_LOADSTRING];
WCHAR sz32ARGB[MAX_LOADSTRING];
WCHAR sz48RGB[MAX_LOADSTRING];
WCHAR sz64ARGB[MAX_LOADSTRING];
WCHAR szFileInfo[MAX_LOADSTRING];
WCHAR szZoomed[MAX_LOADSTRING];
WCHAR szFiles[MAX_LOADSTRING];
WCHAR szSize[MAX_LOADSTRING];
WCHAR szMb[MAX_LOADSTRING];
WCHAR szTag[MAX_LOADSTRING];
WCHAR szValue[MAX_LOADSTRING];
WCHAR szCanonMN[MAX_LOADSTRING];
WCHAR szExifNotAvailable[MAX_LOADSTRING];
WCHAR szBPP[MAX_LOADSTRING];
WCHAR szPixels[MAX_LOADSTRING];
WCHAR szMPixels[MAX_LOADSTRING];
WCHAR szBytes[MAX_LOADSTRING];
WCHAR szExt[(IDS_EXT15 - IDS_EXT1) + 1][MAX_LOADSTRING];

WCHAR currentProgrammDirectory[MAX_PATH + 1];
WCHAR currentProgrammFullPath[MAX_PATH + 1];

WCHAR stringBuffer[1024];

HACCEL accelerator;

// Handle to main window
HWND hWnd = 0;
// Handle to status bar
HWND hWndStatus = 0;

// Handle to "hand" cursor
HCURSOR handCursor = 0;
// Handle to main cursor
HCURSOR cursor = 0;

HCURSOR arrowHCursor = 0;
HCURSOR arrowVCursor = 0;

HICON icon = 0;

int iconIndex[IDS_EXT15 - IDS_EXT1 + 1] = {1,1,1,1,2,2,3,4,5,6,7,8,9,0,0};
HICON fileIcons[(IDI_ICON_ICO - IDI_ICON_JPG) + 1];

// Handle to context menu
HMENU hMenu;
HMENU hMenuThumb;

Graphics * graphics;

//Size and position of main window before full screen
int windowWidth;
int windowHeight;

Bitmap * image = 0;

GUID guid;

CLSID clsId;

//Size of loaded image
int width;
int height;
unsigned int uniqueColors = 0;

PixelFormat pixelFormat;

//Size of screen area desktop
RECT screenArea = {0,0,0,0};
//Size of work area desktop (screen area - system taskbar)
RECT workArea = {0,0,0,0};
//Size of statusbar
RECT statusArea = {0,0,0,0};

//Status of fullscreen
DWORD currentfullScreenMode = MF_CHECKED;
//Status of bilinear mode
DWORD currentBilinearMode = MF_CHECKED;

DWORD rComponent = MF_UNCHECKED;
DWORD gComponent = MF_UNCHECKED;
DWORD bComponent = MF_UNCHECKED;
DWORD aComponent = MF_CHECKED;
DWORD grayScale = MF_CHECKED;

Options options = {Color(255,0,0,0),
					Color(255,0,0,0),
					true, false, false,
					THUMBNAILS_SIZE,
					SPLITTER_SIZE,
					TREE_WIDTH,
					TREE_HEIGHT,
					640,
					480,
					true, false, false, false,
					0
					};

//Scale of displayed image
float cScaleFactor;

int x,y;
//Size of displayed image
int w,h;

//Offsets of displayed image
int xOffset, yOffset;

//Status of fit image to window
bool scaleToFit;

FileNode * cFileNode = 0;
int filesInDirectory;

DWORD OpenFileDialog();

DWORD SaveFileDialog();

bool LoadImage(WCHAR * fileName);

FileNode * FindFileNode(FileNode * fileNode, WCHAR * fileName);

unsigned int ScanDirectory(FileNode ** cFileNode);

void DrawWindow(RECT * rect);

void AdjustWindowToFitImage();

void UpdateScrollInfo();

void DeleteFileNode(FileNode * fileNode);

void DeleteAllFileNode(FileNode * fileNode);

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);

void LoadSetting();

void SaveSetting();

void LoadResource();

LRESULT CALLBACK PropertyFile(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK PropertyEXIF(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK Thumbnails(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK Options(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

#endif //__IV__
