// EagleEye.cpp : Defines the entry point for the application.
//
#include "EagleEye.h"
#include "crtdbg.h"

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);


int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.
	MSG msg;

	hInst = hInstance;

	MakeCrcTable();

	int argCount;
	LPWSTR cmdLine = GetCommandLine();
	LPWSTR * cmdLineArray = CommandLineToArgvW(cmdLine, &argCount);
	wcscpy_s(currentProgrammDirectory, sizeof(currentProgrammDirectory) >> 1, cmdLineArray[0]);
	wcscpy_s(currentProgrammFullPath, sizeof(currentProgrammFullPath) >> 1, cmdLineArray[0]);
	PathQuoteSpaces(currentProgrammFullPath);
	PathRemoveFileSpec(currentProgrammDirectory);

	PathUnquoteSpaces(lpCmdLine);

	LoadSetting();

	wcscpy_s(stringBuffer, sizeof(stringBuffer) >> 1, lpCmdLine);

	if (PathFileExists(stringBuffer))
	{
		if (!PathIsDirectory(stringBuffer))
			PathRemoveFileSpec(stringBuffer);
		SetCurrentDirectory(stringBuffer);
	}
	else
	{
		SetCurrentDirectory(options.currentViewDirectory);
	}

	PathStripPath(lpCmdLine);

	GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR           gdiplusToken;

	// Initialize GDI+.
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	OleInitialize(NULL);

	LoadResource();

	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	filesInDirectory = ScanDirectory(&cFileNode);
	FileNode * fileNode = FindFileNode(cFileNode, lpCmdLine);

	if (!fileNode)
	{
		if (DialogBox(hInst, (LPCTSTR)IDD_THUMBNAILS, hWnd, (DLGPROC)Thumbnails) == IDCANCEL)
		{
			msg.message = 0;
			DestroyWindow(hWnd);
			goto Exit;
		}
	}
	else
	{
		cFileNode = fileNode;
		LoadImage(cFileNode->fileName);
	}

	AdjustWindowToFitImage();

	if (options.checkOptions[ECO_FULLSCREEN])
		SendMessage(hWnd, WM_KEYDOWN, VK_RETURN, 0);
	if (options.checkOptions[ECO_BILINEAR])
		SendMessage(hWnd,WM_KEYDOWN,0x42,0);

	ShowWindow(hWnd, SW_SHOW);

	// Main message loop:
	while(GetMessage(&msg,NULL,0,0))
	{
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

Exit:

	DeleteAllFileNode(cFileNode);

	if (image)
	{
		delete image;
		image = 0;
	}

	OleUninitialize();

	GdiplusShutdown(gdiplusToken);

	DestroyMenu(hMenu);
	DestroyMenu(hMenuThumb);

	SaveSetting();

	return msg.message;
}

void LoadResource()
{
	LoadString(hInst, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInst, IDC_EAGLEEYE, szWindowClass, MAX_LOADSTRING);

	LoadString(hInst, IDS_STRING_FULLSCREEN, szFullScreen, MAX_LOADSTRING);
	LoadString(hInst, IDS_STRING_WINDOW, szWindow, MAX_LOADSTRING);
	LoadString(hInst, IDS_STRING_BILINEAR, szBilinear, MAX_LOADSTRING);
	LoadString(hInst, IDS_STRING_NEAREST, szNearest, MAX_LOADSTRING);
	LoadString(hInst, IDS_STRING_PIXEL, szPixel, MAX_LOADSTRING);
	LoadString(hInst, IDS_STRING_UNKNOWN, szUnknownFileFormat, MAX_LOADSTRING);
	LoadString(hInst, IDS_STRING_4INDEXED, sz4Indexed, MAX_LOADSTRING);
	LoadString(hInst, IDS_STRING_8INDEXED, sz8Indexed, MAX_LOADSTRING);
	LoadString(hInst, IDS_STRING_16RGB, sz16RGB, MAX_LOADSTRING);
	LoadString(hInst, IDS_STRING_24RGB, sz24RGB, MAX_LOADSTRING);
	LoadString(hInst, IDS_STRING_32RGB, sz32RGB, MAX_LOADSTRING);
	LoadString(hInst, IDS_STRING_32ARGB, sz32ARGB, MAX_LOADSTRING);
	LoadString(hInst, IDS_STRING_48RGB, sz48RGB, MAX_LOADSTRING);
	LoadString(hInst, IDS_STRING_64ARGB, sz64ARGB, MAX_LOADSTRING);
	LoadString(hInst, IDS_STRING_FILEINFO, szFileInfo, MAX_LOADSTRING);
	LoadString(hInst, IDS_STRING_ZOOMED, szZoomed, MAX_LOADSTRING);
	LoadString(hInst, IDS_STRING_FILES, szFiles, MAX_LOADSTRING);
	LoadString(hInst, IDS_STRING_SIZE, szSize, MAX_LOADSTRING);
	LoadString(hInst, IDS_STRING_MB, szMb, MAX_LOADSTRING);
	LoadString(hInst, IDS_STRING_TAG, szTag, MAX_LOADSTRING);
	LoadString(hInst, IDS_STRING_VALUE, szValue, MAX_LOADSTRING);
	LoadString(hInst, IDS_STRING_CANONMN, szCanonMN, MAX_LOADSTRING);
	LoadString(hInst, IDS_STRING_EXIFNOTAVAILABLE, szExifNotAvailable, MAX_LOADSTRING);
	LoadString(hInst, IDS_STRING_BPP, szBPP, MAX_LOADSTRING);
	LoadString(hInst, IDS_STRING_PIXELS, szPixels, MAX_LOADSTRING);
	LoadString(hInst, IDS_STRING_MPIXELS, szMPixels, MAX_LOADSTRING);
	LoadString(hInst, IDS_STRING_BYTES, szBytes, MAX_LOADSTRING);

	for (int i = 0; i < (IDS_EXT15 - IDS_EXT1) + 1; ++i)
	{
		LoadString(hInst, i + IDS_EXT1, szExt[i], MAX_LOADSTRING);
	}

	icon = LoadIcon(hInst, (LPCTSTR)IDI_EAGLEEYE);
	cursor = LoadCursor(NULL, IDC_ARROW);
	handCursor = LoadCursor(NULL, IDC_HAND);
	arrowHCursor = LoadCursor(NULL, IDC_SIZEWE);
	arrowVCursor = LoadCursor(NULL, IDC_SIZENS);

	hMenu = LoadMenu(hInst,MAKEINTRESOURCE(IDR_MENU1));
	hMenuThumb = LoadMenu(hInst,MAKEINTRESOURCE(IDR_MENU2));

	for (int i = 0; i < (IDI_ICON_ICO - IDI_ICON_JPG) + 1; ++i)
	{
		fileIcons[i] = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON_JPG + i));
	}

	//accelerator = LoadAccelerators(hInst, MAKEINTRESOURCE(IDR_ACCELERATOR));
}
void LoadSetting()
{
	int argCount;
	LPWSTR cmdLine = GetCommandLine();
	LPWSTR * cmdLineArray = CommandLineToArgvW(cmdLine, &argCount);
	wcscpy_s(stringBuffer, sizeof(stringBuffer) >> 1, cmdLineArray[0]);
	PathRemoveExtension(stringBuffer);
	wcscat_s(stringBuffer, sizeof(stringBuffer) >>1,  L".ini");

	HANDLE file = CreateFile(stringBuffer,FILE_READ_DATA,FILE_SHARE_READ,NULL,OPEN_EXISTING,NULL,NULL);
	if (file != INVALID_HANDLE_VALUE)
	{
		DWORD bytesCount = 0;
		ReadFile(file, &options, sizeof(options), &bytesCount, NULL);
		CloseHandle(file);
	}
	if (!PathIsDirectory(options.currentCacheDirectory))
		wcscpy_s(options.currentCacheDirectory, sizeof(options.currentCacheDirectory) >> 1, currentProgrammDirectory);
	if (!PathIsDirectory(options.currentViewDirectory))
		wcscpy_s(options.currentViewDirectory, sizeof(options.currentViewDirectory) >> 1, currentProgrammDirectory);
	if (!PathIsDirectory(options.currentCaptureFolder))
		wcscpy_s(options.currentCaptureFolder, sizeof(options.currentCaptureFolder) >> 1, currentProgrammDirectory);
}

void SaveSetting()
{
	int argCount;
	LPWSTR cmdLine = GetCommandLine();
	LPWSTR * cmdLineArray = CommandLineToArgvW(cmdLine, &argCount);
	wcscpy_s(stringBuffer, sizeof(stringBuffer) >> 1, cmdLineArray[0]);
	PathRemoveExtension(stringBuffer);
	wcscat_s(stringBuffer, sizeof(stringBuffer) >>1, L".ini");

	GetCurrentDirectory(MAX_PATH, options.currentViewDirectory);

	HANDLE file = CreateFile(stringBuffer,FILE_WRITE_DATA,NULL,NULL,CREATE_ALWAYS,NULL,NULL);
	if (file != INVALID_HANDLE_VALUE)
	{
		DWORD bytesCount = 0;
		WriteFile(file, &options, sizeof(options), &bytesCount, NULL);

		CloseHandle(file);
	}
}

void DeleteFileNode(FileNode * fileNode)
{
	FileNode * rfileNode = fileNode;
	while (rfileNode->fileCount < rfileNode->pNextNode->fileCount)
	{
		rfileNode->fileCount--;
		rfileNode = rfileNode->pNextNode;
	}

	rfileNode->fileCount--;

	if (fileNode != fileNode->pNextNode)
	{
		fileNode->pPrevNode->pNextNode = fileNode->pNextNode;
		fileNode->pNextNode->pPrevNode = fileNode->pPrevNode;
		delete fileNode;
	}
}

void DeleteAllFileNode(FileNode * fileNode)
{
	FileNode * fn;

	if (fileNode)
	{
		fn = fileNode->pPrevNode;
		while (fileNode != fn)
		{
			fileNode = fileNode->pNextNode;
			delete fileNode->pPrevNode;
			fileNode->pPrevNode = 0;
		}

		delete fileNode;
		fileNode = 0;
	}
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= icon;
	wcex.hCursor		= 0;
	wcex.hbrBackground	= 0;
	wcex.lpszMenuName	= 0;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= icon;

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, 
      CW_USEDEFAULT, CW_USEDEFAULT, width, height, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   DragAcceptFiles(hWnd,true);

   InitCommonControls();

   return TRUE;
}

#include "stdio.h"

//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static WCHAR fileName[MAX_PATH + 1];
	static WCHAR curDir[MAX_PATH + 1];
	static WCHAR newDir[MAX_PATH + 1];
	static int aWidths [4];
	static int xMouse;
	static int yMouse;
	static unsigned char a = 0;

	switch (message) 
	{

	case WM_CREATE:
		hWndStatus = CreateWindowEx ( 
		0,                           
		STATUSCLASSNAME,               
		L"",                            
		WS_CHILD | WS_BORDER | WS_VISIBLE | SBS_SIZEGRIP,   
		0, 0, 0, 0,                    
		hWnd,                           
		(HMENU)ID_STATUSBAR,            
		hInst,                      
		NULL);

		SendMessage (hWndStatus, SB_GETRECT, 0, (LPARAM)&statusArea);

		GetNearestMonitorWorkAndScreenArea(hWnd, &workArea, &screenArea);

	break;

	case WM_TIMER:

		SetCursor(NULL);
		KillTimer(hWnd, 2);

		break;

	case WM_DROPFILES:
		{
			DragQueryFile((HDROP)wParam, 0, fileName, MAX_PATH);

			GetCurrentDirectory(MAX_PATH, curDir);
			wcscpy_s(newDir, sizeof(newDir) >> 1, fileName);
			PathRemoveFileSpec(newDir);
			SetCurrentDirectory(newDir);
			PathStripPath(fileName);

			FileNode * fileNode;
			FileNode * fileNodeCurr;
			unsigned int filesInDir = ScanDirectory(&fileNode);
			fileNodeCurr = FindFileNode(fileNode, fileName);
			if (fileNodeCurr)
			{
				DeleteAllFileNode(cFileNode);
				cFileNode = fileNodeCurr;
				filesInDirectory = filesInDir;
				LoadImage(cFileNode->fileName);
				scaleToFit = true;
				UpdateScrollInfo();
				RedrawWindow(hWnd,0,0,RDW_INVALIDATE);
			}
			else
			{
				DeleteAllFileNode(fileNode);
				SetCurrentDirectory(curDir);
			}
			DragFinish((HDROP)wParam);
		}
		break;

	  case WM_CONTEXTMENU:

		  {
		    KillTimer(hWnd, 2);

			HMENU hmenuTrackPopup = GetSubMenu(hMenu, 0); 
 
			int item = TrackPopupMenuEx(hmenuTrackPopup, 
            TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RETURNCMD ,
            GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), hWnd, 0);

			SetTimer(hWnd, 2, 2000, NULL);

			switch (item)
			{
			case ID_FIT:

				SendMessage(hWnd,WM_KEYDOWN,0x41,0);

				break;
			case ID_FULL:

				SendMessage(hWnd,WM_KEYDOWN,0x46,0);

				break;
			case ID_DELETEFILE:

				SendMessage(hWnd,WM_KEYUP,VK_DELETE,0);
				break;
			case ID_FULLSCREEN:

				SendMessage(hWnd,WM_KEYDOWN,VK_RETURN,0);
				break;
			case ID_BILINEARMODE:

				SendMessage(hWnd,WM_KEYDOWN,0x42,0);
				break;
			case ID_ROTATE90:

				SendMessage(hWnd,WM_KEYDOWN,0x52,0);
				break;
			case ID_HORIZONTALFLIP:

				SendMessage(hWnd,WM_KEYDOWN,0x48,0);
				break;
			case ID_VERTICALFLIP:

				SendMessage(hWnd,WM_KEYDOWN,0x56,0);
				break;
			case ID_RCOMPONENT:
				{
				rComponent = CheckMenuItem(hMenu,ID_RCOMPONENT,MF_BYCOMMAND | rComponent);
				RedrawWindow(hWnd,0,0,RDW_INVALIDATE);
				}
				break;
			case ID_GCOMPONENT:
				{
				gComponent = CheckMenuItem(hMenu,ID_GCOMPONENT,MF_BYCOMMAND | gComponent);
				RedrawWindow(hWnd,0,0,RDW_INVALIDATE);
				}
				break;
			case ID_BCOMPONENT:
				{
				bComponent = CheckMenuItem(hMenu,ID_BCOMPONENT,MF_BYCOMMAND | bComponent);
				RedrawWindow(hWnd,0,0,RDW_INVALIDATE);
				}
				break;
			case ID_ACOMPONENT:
				{
				aComponent = CheckMenuItem(hMenu,ID_ACOMPONENT,MF_BYCOMMAND | aComponent);
				RedrawWindow(hWnd,0,0,RDW_INVALIDATE);
				}
				break;
			case ID_GRAYSCALE:
				{
				grayScale = CheckMenuItem(hMenu,ID_GRAYSCALE,MF_BYCOMMAND | grayScale);
				RedrawWindow(hWnd,0,0,RDW_INVALIDATE);
				}
				break;
			case ID_SAVEFILE:

				SendMessage(hWnd,WM_KEYDOWN,0x53,0);
				break;
			case ID_OPENFILE:
				{
					DWORD code;
					do
					{
						code = OpenFileDialog();
						if (code == ERC_CANCEL)
							break;
						if (code == ERC_OK)
						{
							scaleToFit = true;
							UpdateScrollInfo();
						}
					}while (code != ERC_OK);
					RedrawWindow(hWnd,0,0,RDW_INVALIDATE);
				}
				break;
			case ID_SAVEFILEAS:

				SaveFileDialog();
				break;

			case ID_PROPERTYEXIF:

				SendMessage(hWnd,WM_KEYDOWN,0x50,0);
				break;

			case ID_THUMBNAILS:

				SendMessage(hWnd,WM_KEYDOWN,0x54,0);
				break;

			case ID_MENU_OPTIONS:

				DialogBox(hInst, (LPCTSTR)IDD_OPTION, hWnd, (DLGPROC)Options);
				break;

			case ID_MENU_PROPFILE:

				DialogBox(hInst, (LPCTSTR)IDD_PROPFILE, hWnd, (DLGPROC)PropertyFile);
				break;

			case ID_WALLPAPER_CENTERED:
			case ID_WALLPAPER_TILED:
			case ID_WALLPAPER_STRETCH:
				{
					WALLPAPEROPT wpo;
					wpo.dwSize = sizeof(WALLPAPEROPT);
					switch (item)
					{
						case ID_WALLPAPER_CENTERED:
							wpo.dwStyle = WPSTYLE_CENTER;
							break;
						case ID_WALLPAPER_TILED:
							wpo.dwStyle = WPSTYLE_TILE;
							break;
						case ID_WALLPAPER_STRETCH:
							wpo.dwStyle = WPSTYLE_STRETCH;
							break;
					}

					HRESULT hr;
					IActiveDesktop *pActiveDesktop;

					CoCreateInstance(CLSID_ActiveDesktop, NULL, CLSCTX_INPROC_SERVER,
										  IID_IActiveDesktop, (void**)&pActiveDesktop);

					GetCurrentDirectory(1023, stringBuffer);
					PathAppend(stringBuffer, cFileNode->fileName);
					hr = pActiveDesktop->SetWallpaper(stringBuffer, NULL);
					pActiveDesktop->SetWallpaperOptions(&wpo, NULL);
					pActiveDesktop->ApplyChanges(AD_APPLY_REFRESH | AD_APPLY_SAVE);

					pActiveDesktop->Release();
				}
				break;

			case ID_EDIT_COPY:
				{
					IDataObject * data;
					FORMATETC fmtetc = { CF_BITMAP, 0, DVASPECT_CONTENT, -1, TYMED_GDI};
					STGMEDIUM stgmed = { TYMED_GDI, {0}, 0 };
					HBITMAP bitmap;
					image->GetHBITMAP(Color(255,255,255,255), &bitmap);
					stgmed.hBitmap = bitmap;
					CreateDataObject(&fmtetc, &stgmed, 1, &data);
					OleSetClipboard(data);
					OleFlushClipboard();
					data->Release();
					DeleteObject(bitmap);
				}
				break;
			case ID_FILE_COPY:
				{	
					GetCurrentDirectory(1024, stringBuffer);
					IShellFolder * isf = CreateIShellFolder(stringBuffer);
					if (isf)
					{
						PathAppend(stringBuffer, cFileNode->fileName);
						LPITEMIDLIST pidl = ILCreateFromPath(stringBuffer);
						if (pidl)
						{
							LPITEMIDLIST filePidl = ILFindLastID(pidl);
							IDataObject * data = NULL;
							isf->GetUIObjectOf(hWnd, 1, (LPCITEMIDLIST*)&filePidl, IID_IDataObject, NULL, (void**)&data);
							if (data)
							{
								OleSetClipboard(data);
								data->Release();
							}
						}
					}
				}
				break;
			case ID_FILE_COPYTO:
				{
					WCHAR buffer[MAX_PATH + 1];
					if (BrowseFolderDialog(hWnd, stringBuffer))
					{
						GetCurrentDirectory(MAX_PATH, buffer);
						PathAppend(buffer, cFileNode->fileName);
						CopyingFileDialog(hWnd, buffer, stringBuffer);
					}
				}
				break;
			}

		  }

		break;

	  case WM_SIZE:
		  {
			MoveWindow (hWndStatus, 0, HIWORD(lParam) - 10, LOWORD(lParam),
						HIWORD(lParam), false);
			aWidths [0] = LOWORD(lParam) / 4;
			aWidths [1] = LOWORD(lParam) / 2;
			aWidths [2] = aWidths [1] + LOWORD(lParam) / 4;
			aWidths [3] = -1;

			SendMessage (hWndStatus, SB_SETPARTS, 4, (LPARAM)aWidths);
			SendMessage (hWndStatus, SB_GETRECT, 0, (LPARAM)&statusArea);

			UpdateScrollInfo();
		  }
      break;

	  case WM_MOVE:
		  {
			  GetNearestMonitorWorkAndScreenArea(hWnd, &workArea, &screenArea);
		  }
		  break;

	  case WM_HSCROLL:

		  if (LOWORD(wParam) == SB_THUMBTRACK || LOWORD(wParam) == SB_THUMBPOSITION)
		  {
			  RECT rect;
			  GetClientRect(hWnd,&rect);
			  rect.bottom -= statusArea.bottom + 1;

			  SCROLLINFO scrollInfo;
			  scrollInfo.cbSize = sizeof(SCROLLINFO);
			  scrollInfo.fMask = SIF_TRACKPOS;
			  GetScrollInfo(hWnd, SB_HORZ, &scrollInfo);

			  int xOffset2 = xOffset;
			  xOffset = -scrollInfo.nTrackPos;
			  UpdateScrollInfo();
			  ScrollWindowEx(hWnd, xOffset - xOffset2,0,&rect,&rect,0,&rect,SW_INVALIDATE);
		  }
		  if (LOWORD(wParam) == SB_LINELEFT)
		  {
			  SendMessage(hWnd,WM_KEYDOWN,VK_LEFT,0);
		  }
		  if (LOWORD(wParam) == SB_LINERIGHT)
		  {
			  SendMessage(hWnd,WM_KEYDOWN,VK_RIGHT,0);
		  }
		  break;
	  case WM_VSCROLL:

		  if (LOWORD(wParam) == SB_THUMBTRACK)
		  {
			  RECT rect;
			  GetClientRect(hWnd,&rect);
			  rect.bottom -= statusArea.bottom + 1;

			  SCROLLINFO scrollInfo;
			  scrollInfo.cbSize = sizeof(SCROLLINFO);
			  scrollInfo.fMask = SIF_TRACKPOS;
			  GetScrollInfo(hWnd, SB_VERT, &scrollInfo);

			  int yOffset2 = yOffset;
			  yOffset = -scrollInfo.nTrackPos;
			  UpdateScrollInfo();
			  ScrollWindowEx(hWnd, 0, yOffset - yOffset2,&rect,&rect,0,&rect,SW_INVALIDATE);
		  }
		  if (LOWORD(wParam) == SB_LINEUP)
		  {
			  SendMessage(hWnd,WM_KEYDOWN,VK_UP,0);
		  }
		  if (LOWORD(wParam) == SB_LINEDOWN)
		  {
			  SendMessage(hWnd,WM_KEYDOWN,VK_DOWN,0);
		  }
		  break;

	  case WM_KEYUP:

		if (wParam == VK_DELETE)
		{
			if (filesInDirectory)
			{
				if (image)
				{
					delete image;
					image = 0;
				}
				ZeroMemory(stringBuffer, sizeof(stringBuffer));
				GetCurrentDirectory(1024, stringBuffer);
				PathAppend(stringBuffer, cFileNode->fileName);
				if (DeleteFileDialog(hWnd, stringBuffer))
				{
					cFileNode = cFileNode->pNextNode;
					DeleteFileNode(cFileNode->pPrevNode);
					filesInDirectory--;
				}
				LoadImage(cFileNode->fileName);
				scaleToFit = true;
			}
			UpdateScrollInfo();
			RedrawWindow(hWnd,0,0,RDW_INVALIDATE);
		}
		break;

	  case WM_MOUSEWHEEL:


		  if (GET_WHEEL_DELTA_WPARAM(wParam) < 0)
		  {
			  if (LOWORD(wParam) == MK_CONTROL)
				SendMessage(hWnd,WM_KEYDOWN,VK_SUBTRACT,0);
			  else
				SendMessage(hWnd,WM_KEYDOWN,VK_SPACE,0);
		  }
		  else
		  {
			  if (LOWORD(wParam) == MK_CONTROL)
				SendMessage(hWnd,WM_KEYDOWN,VK_ADD,0);
			  else
				SendMessage(hWnd,WM_KEYDOWN,VK_BACK,0);
		  }
		  break;

	  case WM_MOUSEMOVE:

		  if (wParam == MK_LBUTTON)
		  {
			  SetCursor(handCursor);

			  int xOffset2 = xOffset;
			  int yOffset2 = yOffset;
			  xOffset -= (xMouse - GET_X_LPARAM(lParam));
			  yOffset -= (yMouse - GET_Y_LPARAM(lParam));

			  RECT rect;
			  GetClientRect(hWnd,&rect);
			  rect.bottom -= statusArea.bottom + 1;

			  UpdateScrollInfo();

			  ScrollWindowEx(hWnd,xOffset - xOffset2,yOffset - yOffset2,&rect,&rect,0,&rect,SW_INVALIDATE);
		  }
		  else
		  {
			SetCursor(cursor);
		  }

		  KillTimer(hWnd, 2);
		  SetTimer(hWnd, 2, 2000, NULL);

		  xMouse = GET_X_LPARAM(lParam);
		  yMouse = GET_Y_LPARAM(lParam);

		  break;

		case WM_LBUTTONDBLCLK:

		  SendMessage(hWnd,WM_KEYDOWN,VK_RETURN,0);
		  break;

		case WM_KEYDOWN:

		if (wParam == VK_RETURN)
		{
			currentfullScreenMode = CheckMenuItem(hMenu,ID_FULLSCREEN,MF_BYCOMMAND | currentfullScreenMode);
			if (currentfullScreenMode == MF_CHECKED)
			{
				SetWindowLongPtr(hWnd,GWL_STYLE,WS_OVERLAPPEDWINDOW);
				ShowWindow(hWnd, true);

				if (windowWidth > (workArea.right - workArea.left))
					windowWidth = workArea.right - workArea.left;
				if (windowHeight > (workArea.bottom - workArea.top))
					windowHeight = workArea.bottom - workArea.top;

				MoveWindow(hWnd,
							workArea.left + ((workArea.right - workArea.left) - windowWidth)/2,
							workArea.top + ((workArea.bottom  - workArea.top) - windowHeight)/2,
							windowWidth,windowHeight,true);
				
			}
			else
			{
				RECT rect;
				GetWindowRect(hWnd,&rect);
				windowWidth = rect.right - rect.left;
				windowHeight = rect.bottom - rect.top;
				SetWindowLongPtr(hWnd,GWL_STYLE,WS_POPUP);
				ShowWindow(hWnd, true);
				MoveWindow(hWnd, screenArea.left, screenArea.top, screenArea.right - screenArea.left, 
																screenArea.bottom - screenArea.top, 
																true);
			}
			UpdateScrollInfo();
			RedrawWindow(hWnd,0,0,RDW_INVALIDATE);
		 }

		if (wParam == VK_ESCAPE)
		{
			DestroyWindow(hWnd);
		}
		if (wParam == VK_SPACE)
		{
			cFileNode = cFileNode->pNextNode;
			LoadImage(cFileNode->fileName);
			scaleToFit = true;
			UpdateScrollInfo();
			RedrawWindow(hWnd,0,0,RDW_INVALIDATE);
		}
		if (wParam == VK_BACK)
		{
			cFileNode = cFileNode->pPrevNode;
			LoadImage(cFileNode->fileName);
			scaleToFit = true;
			UpdateScrollInfo();
			RedrawWindow(hWnd,0,0,RDW_INVALIDATE);
		}
		if (wParam == VK_ADD)
		{
			scaleToFit = false;
			cScaleFactor += (cScaleFactor * 0.05f);
			if (cScaleFactor > 100.0f)
				cScaleFactor = 100.0f;
			UpdateScrollInfo();
			RedrawWindow(hWnd,0,0,RDW_INVALIDATE);
		}

		if (wParam == VK_SUBTRACT)
		{
			scaleToFit = false;
			cScaleFactor -= (cScaleFactor * 0.05f);
			if (cScaleFactor < 0.01f)
				cScaleFactor = 0.01f;
			UpdateScrollInfo();
			RedrawWindow(hWnd,0,0,RDW_INVALIDATE);
		}

		if (wParam == VK_RIGHT)
		{
			RECT rect;
			GetClientRect(hWnd,&rect);
			rect.bottom -= statusArea.bottom + 1;
			int xOffset2 = xOffset;
			xOffset-=10;
			UpdateScrollInfo();
			ScrollWindowEx(hWnd, xOffset - xOffset2,0,&rect,&rect,0,&rect,SW_INVALIDATE);
		}
		if (wParam == VK_LEFT)
		{
			RECT rect;
			GetClientRect(hWnd,&rect);
			rect.bottom -= statusArea.bottom + 1;
			int xOffset2 = xOffset;
			xOffset+=10;
			UpdateScrollInfo();
			ScrollWindowEx(hWnd, xOffset - xOffset2,0,&rect,&rect,0,&rect,SW_INVALIDATE);
		}
		if (wParam == VK_DOWN)
		{
			RECT rect;
			GetClientRect(hWnd,&rect);
			rect.bottom -= statusArea.bottom + 1;
			int yOffset2 = yOffset;
			yOffset-=10;
			UpdateScrollInfo();
			ScrollWindowEx(hWnd, 0, yOffset - yOffset2,&rect,&rect,0,&rect,SW_INVALIDATE);		
		}
		if (wParam == VK_UP)
		{
			RECT rect;
			GetClientRect(hWnd,&rect);
			rect.bottom -= statusArea.bottom + 1;
			int yOffset2 = yOffset;
			yOffset+=10;
			UpdateScrollInfo();
			ScrollWindowEx(hWnd, 0, yOffset - yOffset2,&rect,&rect,0,&rect,SW_INVALIDATE);				
		}
		if (wParam == 0x41)
		{
			ShowScrollBar(hWnd,SB_BOTH,false);
			scaleToFit = true;
			UpdateScrollInfo();
			RedrawWindow(hWnd,0,0,RDW_INVALIDATE);
		}

		if (wParam == 0x42)
		{
			currentBilinearMode = CheckMenuItem(hMenu,ID_BILINEARMODE,MF_BYCOMMAND | currentBilinearMode);
			RedrawWindow(hWnd,0,0,RDW_INVALIDATE);
		}

		if (wParam == 0x46)
		{
			cScaleFactor = 1;
			scaleToFit = false;
			UpdateScrollInfo();
			RedrawWindow(hWnd,0,0,RDW_INVALIDATE);
		}

		if (wParam == 0x53)
		{
			image->Save(L"temp",&clsId,NULL);
			if (image)
			{
				delete image;
				image = 0;
			}
			CopyFileEx(L"temp", cFileNode->fileName, NULL, NULL, false, NULL);
			DeleteFile(L"temp");
			LoadImage(cFileNode->fileName);
			scaleToFit = true;
			UpdateScrollInfo();
			RedrawWindow(hWnd,0,0,RDW_INVALIDATE);
		}

		if (wParam == 0x52)
		{
			image->RotateFlip(Rotate90FlipNone);
			int temp = width;
			width = height;
			height = temp;
			UpdateScrollInfo();
			RedrawWindow(hWnd,0,0,RDW_INVALIDATE);
		}

		if (wParam == 0x48)
		{
			image->RotateFlip(RotateNoneFlipX);
			RedrawWindow(hWnd,0,0,RDW_INVALIDATE);
		}

		if (wParam == 0x56)
		{
			image->RotateFlip(RotateNoneFlipY);
			RedrawWindow(hWnd,0,0,RDW_INVALIDATE);
		}

		if (wParam == 0x50)
		{
			DialogBox(hInst, (LPCTSTR)IDD_PROPEXIF, hWnd, (DLGPROC)PropertyEXIF);
		}
		if (wParam == 0x54)
		{
			if (image)
			{
				delete image;
				image = 0;
			}
			ShowWindow(hWnd, SW_HIDE);
			KillTimer(hWnd, 2);
			if (DialogBox(hInst, (LPCTSTR)IDD_THUMBNAILS, hWnd, (DLGPROC)Thumbnails) == IDCANCEL)
			{
				DestroyWindow(hWnd);
			}
			else
			{
				scaleToFit = true;
				if (currentfullScreenMode == MF_CHECKED)
				{
					AdjustWindowToFitImage();
					
				}
				else
				{
					MoveWindow(hWnd, screenArea.left, screenArea.top, screenArea.right - screenArea.left, 
																	screenArea.bottom - screenArea.top, 
																	true);
				}
				UpdateScrollInfo();
				ShowWindow(hWnd, true);
			}
			SetTimer(hWnd, 2, 2000, NULL);
		}

		case WM_PAINT:
		{

			PAINTSTRUCT ps;
			HDC hdcMain = BeginPaint(hWnd, &ps);
			if (hdcMain)
			{
				RECT rect;
				GetClientRect(hWnd, &rect);
				rect.bottom -= statusArea.bottom + 1;
				if (ps.rcPaint.bottom > rect.bottom)
					ps.rcPaint.bottom = rect.bottom;

				if (image)
				{
					HDC chdc = CreateCompatibleDC(hdcMain);

					graphics = new Graphics(chdc);

					graphics->DrawImage(image, 0, 0, width, height);

					if (graphics)
					{
						delete graphics;
						graphics = 0;
					}

					if (chdc)
					{
						DeleteDC(chdc);
						chdc = 0;
					}

					graphics = new Graphics(hdcMain);

					DrawWindow(&ps.rcPaint);

					if (graphics)
					{
						delete graphics;
						graphics = 0;
					}
				}
				EndPaint(hWnd, &ps);
			}
		}
		break;
	case WM_DESTROY:
		KillTimer(hWnd, 2);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void FeelListView(HWND & hwndList, HIMAGELIST & hLarge, HIMAGELIST & hSmall, int filesInDir, FileNode * fileNode)
{
	hLarge = ImageList_Create(options.thumbnailsSize,options.thumbnailsSize,ILC_COLOR32,filesInDir,filesInDir);
	hSmall = ImageList_Create(16, 16, ILC_COLOR32, filesInDir, filesInDir);

	ListView_SetImageList(hwndList,hLarge,LVSIL_NORMAL);
	ListView_SetImageList(hwndList,hSmall,LVSIL_SMALL);

	LVITEM lvI;
	UINT array[] = {1, 2, 3};

	lvI.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_COLUMNS; 
	lvI.state = 0; 
	lvI.stateMask = 0; 
	lvI.iSubItem = 0;
	lvI.lParam = 0;
	lvI.cColumns = 3;
	lvI.puColumns = array;

	for (int indexItem = 0; indexItem < filesInDir; ++indexItem)
	{
		lvI.iImage = indexItem;
		lvI.iItem = indexItem;
		lvI.pszText = fileNode->fileName;
		ListView_InsertItem(hwndList, &lvI);

		fileNode = fileNode->pNextNode;
	}

	if (ListView_GetView(hwndList) == LV_VIEW_DETAILS)
	{
		ListView_SetColumnWidth(hwndList, 0, LVSCW_AUTOSIZE_USEHEADER);
	}
}

LRESULT CALLBACK Options(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HWND checkHwnd[ECO_COUNT] = {0, 0, 0};
	static HWND hwndSlider = 0;
	static HWND hwndCombo = 0;
	static HWND hwndBorderColor = 0;
	static HWND hwndBackThColor = 0;
	static DWORD rgbBorder = options.borderColor.ToCOLORREF();
	static DWORD rgbBackTh = options.backThColor.ToCOLORREF();
	static HBRUSH hBorderColor = 0;
	static HBRUSH hBackThColor = 0;

	switch (message)
	{
	case WM_INITDIALOG:
		{

			hwndSlider = GetDlgItem(hDlg, IDC_SLIDER1);

			SendMessage(hwndSlider, TBM_SETRANGE, 
						(WPARAM) TRUE,                   // redraw flag 
						(LPARAM) MAKELONG(1, 10));  // min. & max. positions 

			SendMessage(hwndSlider, TBM_SETPAGESIZE, 
						0, (LPARAM) 1);                  // new page size

			SendMessage(hwndSlider, TBM_SETPOS, true, options.splitterSize);

			swprintf_s(stringBuffer,  sizeof(stringBuffer) >> 1, L"%d %s", options.splitterSize, szPixel);
			SetDlgItemText(hDlg, IDC_SPLITTER_SIZE_TEXT, stringBuffer);

			for (int i = 0; i < ECO_COUNT; ++i)
			{
				checkHwnd[i] = GetDlgItem(hDlg, i + IDC_FULLSCREEN);
				SendMessage(checkHwnd[i], BM_SETCHECK, options.checkOptions[i], 0);
			}

			WCHAR buffer[MAX_LOADSTRING];
			WCHAR fileTypeDecs[MAX_PATH];

			for (int i = 0; i < (IDS_EXT15 - IDS_EXT1) + 1; ++i)
			{
				DWORD type;
				DWORD len;
				DWORD flag = BST_UNCHECKED;
				HWND hDlgItem = GetDlgItem(hDlg, i + IDC_CHECK_EXT1);
				SetDlgItemText(hDlg, i + IDC_CHECK_EXT1, szExt[i]);
				wcscpy_s(fileTypeDecs, MAX_PATH, szTitle);
				wcscat_s(fileTypeDecs, MAX_PATH, szExt[i]);
				wcscpy_s(stringBuffer, sizeof(stringBuffer) >> 1, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.");
				wcscat_s(stringBuffer, sizeof(stringBuffer) >> 1, szExt[i]);
				if (SHGetValue(HKEY_CURRENT_USER, stringBuffer, L"ProgId", &type , buffer, &len) == ERROR_SUCCESS)
				{
					if (!wcscmp(buffer, fileTypeDecs))
					{
						flag = BST_CHECKED;
					}
				}
				else
				{
					wcscpy_s(stringBuffer, sizeof(stringBuffer) >> 1, L".");
					wcscat_s(stringBuffer, sizeof(stringBuffer) >> 1, szExt[i]);
					if (SHGetValue(HKEY_CLASSES_ROOT, stringBuffer, L"", &type , buffer, &len) == ERROR_SUCCESS)
					{
						if (!wcscmp(buffer, fileTypeDecs))
						{
							flag = BST_CHECKED;
						}
					}
				}
				SendMessage(hDlgItem, BM_SETCHECK, flag, 0);
			}

			if (options.checkOptions[ECO_FULLSCREEN])
				SetDlgItemText(hDlg, IDC_FULLSCREEN, szFullScreen);
			else
				SetDlgItemText(hDlg, IDC_FULLSCREEN, szWindow);
			
			if (options.checkOptions[ECO_BILINEAR])
				SetDlgItemText(hDlg, IDC_BILINEAR, szBilinear);
			else
				SetDlgItemText(hDlg, IDC_BILINEAR, szNearest);

			hwndCombo = GetDlgItem(hDlg, IDC_COMBO1);

			SendMessage(hwndCombo, CB_ADDSTRING, 0, (LPARAM)L"20x20");
			SendMessage(hwndCombo, CB_ADDSTRING, 0, (LPARAM)L"40x40");
			SendMessage(hwndCombo, CB_ADDSTRING, 0, (LPARAM)L"60x60");
			SendMessage(hwndCombo, CB_ADDSTRING, 0, (LPARAM)L"80x80");
			SendMessage(hwndCombo, CB_ADDSTRING, 0, (LPARAM)L"100x100");
			SendMessage(hwndCombo, CB_ADDSTRING, 0, (LPARAM)L"120x120");
			SendMessage(hwndCombo, CB_ADDSTRING, 0, (LPARAM)L"140x140");
			SendMessage(hwndCombo, CB_ADDSTRING, 0, (LPARAM)L"160x160");
			SendMessage(hwndCombo, CB_ADDSTRING, 0, (LPARAM)L"180x180");
			SendMessage(hwndCombo, CB_ADDSTRING, 0, (LPARAM)L"200x200");

			SendMessage(hwndCombo, CB_SETCURSEL, (options.thumbnailsSize / 20) - 1, 0);

			SetDlgItemText(hDlg,IDC_CUR_CACHE_DIR, options.currentCacheDirectory);

			hwndBorderColor = GetDlgItem(hDlg, IDC_BORDER_COLOR);
			hwndBackThColor = GetDlgItem(hDlg, IDC_BACKTH_COLOR);
			return TRUE;
		}
		break;
	case WM_CTLCOLORSTATIC:

		if (GetDlgCtrlID((HWND)lParam) == IDC_BORDER_COLOR)
		{
			if (hBorderColor)
				DeleteObject(hBorderColor);
			hBorderColor = CreateSolidBrush(rgbBorder);
			return (LRESULT)hBorderColor;	
		}
		if (GetDlgCtrlID((HWND)lParam) == IDC_BACKTH_COLOR)
		{
			if (hBackThColor)
				DeleteObject(hBackThColor);
			hBackThColor = CreateSolidBrush(rgbBackTh);
			return (LRESULT)hBackThColor;	
		}
		break;
	case WM_COMMAND:

		if (LOWORD(wParam) == IDC_SEL_FOLDER)
		{
			if (BrowseFolderDialog(hDlg, options.currentCacheDirectory))
				SetDlgItemText(hDlg,IDC_CUR_CACHE_DIR, options.currentCacheDirectory);
		}
		if (LOWORD(wParam) == IDC_CLEAN_FOLDER)
		{
			ZeroMemory(stringBuffer, sizeof(stringBuffer));
			wcscpy_s(stringBuffer, sizeof(stringBuffer) >> 1, options.currentCacheDirectory);
			PathAppend(stringBuffer, L"Thumbnails");
			DeleteFileDialog(hDlg, stringBuffer);
		}
		if (LOWORD(wParam) == IDC_BORDER_COLOR_BUTTON) 
		{
			CHOOSECOLOR cc;
			COLORREF acrCustClr[16];
			

			ZeroMemory(&cc, sizeof(cc));
			cc.lStructSize = sizeof(cc);
			cc.hwndOwner = hDlg;
			cc.lpCustColors = (LPDWORD) acrCustClr;
			cc.rgbResult = rgbBorder;
			cc.Flags = CC_FULLOPEN | CC_RGBINIT;
			 
			if (ChooseColor(&cc)==TRUE) 
			{
				rgbBorder = cc.rgbResult;
				RedrawWindow(hwndBorderColor, 0, 0, RDW_INVALIDATE);
			}
		}
		if (LOWORD(wParam) == IDC_BACKTH_COLOR_BUTTON) 
		{
			CHOOSECOLOR cc;
			COLORREF acrCustClr[16];

			ZeroMemory(&cc, sizeof(cc));
			cc.lStructSize = sizeof(cc);
			cc.hwndOwner = hDlg;
			cc.lpCustColors = (LPDWORD) acrCustClr;
			cc.rgbResult = rgbBackTh;
			cc.Flags = CC_FULLOPEN | CC_RGBINIT;
			 
			if (ChooseColor(&cc)==TRUE) 
			{
				rgbBackTh = cc.rgbResult;
				RedrawWindow(hwndBackThColor, 0, 0, RDW_INVALIDATE);
			}
		}

		if (LOWORD(wParam) == IDOK) 
		{

			for (int i = 0; i < ECO_COUNT; ++i)
			{
				options.checkOptions[i] = SendMessage(checkHwnd[i], BM_GETSTATE, 0, 0) & BST_CHECKED;
			}

			WCHAR buffer[MAX_PATH + 1];
			WCHAR path[MAX_PATH + 1];

			for (int i = 0; i < (IDS_EXT15 - IDS_EXT1) + 1; ++i)
			{
				wcscpy_s(buffer, MAX_PATH, szTitle);
				wcscat_s(buffer, MAX_PATH, szExt[i]);
				HWND hDlgItem = GetDlgItem(hDlg, i + IDC_CHECK_EXT1);
				if (SendMessage(hDlgItem, BM_GETSTATE, 0, 0) & BST_CHECKED)
				{
					wcscpy_s(stringBuffer, sizeof(stringBuffer) >> 1, L".");
					wcscat_s(stringBuffer, sizeof(stringBuffer) >> 1, szExt[i]);
					SHSetValue(HKEY_CLASSES_ROOT, stringBuffer, L"", REG_SZ , buffer, (DWORD)wcslen(buffer) << 1);
					wcscpy_s(stringBuffer, sizeof(stringBuffer) >> 1, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.");
					wcscat_s(stringBuffer, sizeof(stringBuffer) >> 1, szExt[i]);
					SHSetValue(HKEY_CURRENT_USER, stringBuffer, L"ProgId", REG_SZ , buffer, (DWORD)wcslen(buffer) << 1);

					SHSetValue(HKEY_CLASSES_ROOT, buffer, L"", REG_SZ , L"EagleEye Image", 34);
					wcscpy_s(stringBuffer, sizeof(stringBuffer) >> 1, buffer);
					wcscat_s(stringBuffer, sizeof(stringBuffer) >> 1, L"\\shell\\open");
					SHSetValue(HKEY_CLASSES_ROOT, stringBuffer, L"", REG_SZ , L"Open with EagleEye", 46);

					wcscpy_s(stringBuffer, sizeof(stringBuffer) >> 1, buffer);
					wcscat_s(stringBuffer, sizeof(stringBuffer) >> 1, L"\\DefaultIcon");
					wcscpy_s(path, MAX_PATH, currentProgrammFullPath);
					PathUnquoteSpaces(path);
					swprintf_s(path + wcslen(path), MAX_PATH - wcslen(path), L",%d", iconIndex[i]);
					SHSetValue(HKEY_CLASSES_ROOT, stringBuffer, L"", REG_SZ , path, (DWORD)wcslen(path) << 1);

					wcscpy_s(stringBuffer, sizeof(stringBuffer) >> 1, buffer);
					wcscat_s(stringBuffer, sizeof(stringBuffer) >> 1, L"\\shell\\open\\command");
					wcscpy_s(path, MAX_PATH, currentProgrammFullPath);
					wcscat_s(path, MAX_PATH, L" \"%1\"");
					SHSetValue(HKEY_CLASSES_ROOT, stringBuffer, L"", REG_SZ , path, (DWORD)wcslen(path) << 1);
				}
			}

			if (options.checkOptions[ECO_FOLDER_CONTEXT])
			{
				SHSetValue(HKEY_CLASSES_ROOT, L"Directory\\shell\\EEBrowser", L"", REG_SZ , L"Browse with EagleEye", 46);
				SHSetValue(HKEY_CLASSES_ROOT, L"Directory\\shell\\EEBrowser\\command", L"", REG_SZ , path, (DWORD)wcslen(path) << 1);
			}
			else
				SHDeleteKey(HKEY_CLASSES_ROOT, L"Directory\\shell\\EEBrowser");

			options.borderColor.SetFromCOLORREF(rgbBorder);
			options.backThColor.SetFromCOLORREF(rgbBackTh);
			options.splitterSize = (int)SendMessage(hwndSlider, TBM_GETPOS, 0, 0);
			options.thumbnailsSize = ((int)SendMessage(hwndCombo, CB_GETCURSEL, 0, 0) + 1) * 20;

			SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);

			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		if (LOWORD(wParam) == IDCANCEL) 
		{
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}

		if (LOWORD(wParam) == IDC_FULLSCREEN && HIWORD(wParam) == BN_CLICKED)
		{
			if (SendMessage(checkHwnd[ECO_FULLSCREEN], BM_GETSTATE, 0, 0) & BST_CHECKED)
				SetDlgItemText(hDlg, IDC_FULLSCREEN, szFullScreen);
			else
				SetDlgItemText(hDlg, IDC_FULLSCREEN, szWindow);		
		}

		if (LOWORD(wParam) == IDC_BILINEAR && HIWORD(wParam) == BN_CLICKED)
		{
			if (SendMessage(checkHwnd[ECO_BILINEAR], BM_GETSTATE, 0, 0) & BST_CHECKED)
				SetDlgItemText(hDlg, IDC_BILINEAR, szBilinear);
			else
				SetDlgItemText(hDlg, IDC_BILINEAR, szNearest);
		}

		break;

		case WM_HSCROLL:
			{
				int p = (int)SendMessage(hwndSlider, TBM_GETPOS, 0, 0);
				swprintf_s(stringBuffer, sizeof(stringBuffer) >> 1, L"%d %s", p, szPixel);
				SetDlgItemText(hDlg, IDC_SPLITTER_SIZE_TEXT, stringBuffer);
			}
		break;

		case WM_DESTROY:
		
			KillTimer(hWnd, 2);
			DeleteObject(hBorderColor);
			DeleteObject(hBackThColor);
		break;
	}
	return FALSE;
}

Status CreateThumbnailImage(FileNode * fileNode, HBITMAP & bitmapThumb)
{
	unsigned c = 0;
	WCHAR buffer[MAX_PATH];
	WCHAR path[MAX_PATH];
	WCHAR driveNum[3];
	bool isCreate = false;
	Bitmap * bitmapTh;
	Status s;

	wcscpy_s(stringBuffer, sizeof(stringBuffer) >> 1, options.currentCacheDirectory);
	PathAppend(stringBuffer, L"Thumbnails");
	GetCurrentDirectory(MAX_PATH, path);
	WCHAR * p = PathSkipRoot(path);
	swprintf_s(driveNum, sizeof(driveNum) >> 1, L"%d", PathGetDriveNumber(path));
	PathAppend(stringBuffer, driveNum);
	PathAppend(stringBuffer, p);
	SHCreateDirectoryEx(NULL, stringBuffer, NULL);

	wcscpy_s(path, sizeof(path) >> 1, fileNode->fileName);
	swprintf_s(buffer, sizeof(buffer) >> 1, L"%lu", fileNode->size);
	wcscat_s(path, sizeof(path) >> 1, buffer);
	swprintf_s(buffer, sizeof(buffer) >> 1, L"%d", options.thumbnailsSize);
	wcscat_s(path, sizeof(path) >> 1, buffer);
	swprintf_s(buffer, sizeof(buffer) >> 1, L"%u", options.backThColor.ToCOLORREF());
	wcscat_s(path, sizeof(path) >> 1, buffer);

	unsigned int crc = CalcCrc((unsigned char *)path, (unsigned int)(wcslen(path) << 1));

	swprintf_s(path, sizeof(path) >> 1, L"%u", crc);

	PathAppend(stringBuffer, path);
	wcscat_s(stringBuffer, sizeof(stringBuffer) >>1, L".ivthumb");	

	bitmapTh = new Bitmap(stringBuffer);
	s = bitmapTh->GetLastStatus();

	if (s != Ok)
	{
		delete bitmapTh;

		bitmapTh = new Bitmap(options.thumbnailsSize,options.thumbnailsSize,PixelFormat24bppRGB);
		Image * image = new Bitmap(fileNode->fileName);
		Graphics * graphics = new Graphics(bitmapTh);
		
		INT thWidth, thHeight, x, y;
		FitAndCenterImage(options.thumbnailsSize,
						  options.thumbnailsSize,
						  image->GetWidth(),
						  image->GetHeight(),
						  x,
						  y,
						  thWidth,
						  thHeight);

		Image * thumbImage = image->GetThumbnailImage(thWidth, thHeight, NULL, NULL); 
		graphics->Clear(options.backThColor);
		graphics->DrawImage(thumbImage, x, y, thWidth, thHeight);

		CLSID clsId;
		GetEncoderClsid(L"image/jpeg",&clsId);
		bitmapTh->SetResolution((REAL)image->GetWidth(), (REAL)image->GetHeight());
		bitmapTh->Save(stringBuffer, &clsId);

		delete thumbImage;
		delete graphics;
		delete image;
	}

	fileNode->width = (UINT)bitmapTh->GetHorizontalResolution();
	fileNode->height = (UINT)bitmapTh->GetVerticalResolution();
	bitmapTh->GetHBITMAP(0, &bitmapThumb);

	delete bitmapTh;

	return s;
}

LRESULT CALLBACK Thumbnails(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HWND hwndToolbar;
	static HWND hwndList;
	static HWND hwndTree;
	static HWND hwndImage;
	static HIMAGELIST hImageList;
	static HIMAGELIST hLarge;
	static HIMAGELIST hSmall;
	static int countFiles; 
	static FileNode * fileNode;
	static int filesInDir;
	static bool trackX = false;
	static bool trackY = false;
	static IContextMenu * contextMenu = 0;
	IDragSourceHelper *m_pdsh = 0;
	static Bitmap * curImage = 0;
	static float a = 0;
	static float b = 0;
	static int toolBarHeight;

	switch (message)
	{
	case WM_INITMENUPOPUP:
		{
			void * t;
			if (contextMenu && !contextMenu->QueryInterface(IID_IContextMenu2, &t))
				((IContextMenu2*)contextMenu)->HandleMenuMsg(message, wParam, lParam);
		}
			break;
	case WM_DRAWITEM:
		{
			void * t;
			if (!wParam && contextMenu && !contextMenu->QueryInterface(IID_IContextMenu2, &t))
				((IContextMenu2*)contextMenu)->HandleMenuMsg(message, wParam, lParam);

			if (wParam == IDC_IMAGE_CONTROL)
			{
				DRAWITEMSTRUCT * dis = (DRAWITEMSTRUCT*)lParam;
				graphics = new Graphics(dis->hDC);

				if (curImage)
				{
					int thWidth, thHeight, X, Y;
					FitAndCenterImage(options.treeWidth - options.splitterSize,
									options.thumbWindowHeight - options.treeHeight - (options.splitterSize << 1) - toolBarHeight,
									curImage->GetWidth(),
									curImage->GetHeight(),
									X,
									Y,
									thWidth,
									thHeight);

					graphics->SetCompositingMode(CompositingModeSourceCopy);
					graphics->SetInterpolationMode(InterpolationModeLowQuality);
					graphics->SetPixelOffsetMode(PixelOffsetModeHalf);
					graphics->DrawImage(curImage, X, Y, thWidth, thHeight);
					Rect clipRect;
					clipRect.X = X;
					clipRect.Y = Y;
					clipRect.Width = thWidth;
					clipRect.Height = thHeight;
					graphics->SetClip(clipRect, CombineModeExclude);
				}

				graphics->Clear(options.backThColor);

				delete graphics;
			}
		}
			break;
	case WM_MEASUREITEM:
		{
			void * t;
			if (contextMenu && !contextMenu->QueryInterface(IID_IContextMenu2, &t))
				((IContextMenu2*)contextMenu)->HandleMenuMsg(message, wParam, lParam);
		}
			break;

	case WM_INITDIALOG:
		{
		   GetNearestMonitorWorkAndScreenArea(hDlg, &workArea, &screenArea);

		   TBBUTTON tbButtons[] = 
		   {
				{ 0, IDM_GETPHOTOS, TBSTATE_ENABLED, TBSTYLE_BUTTON | BTNS_SHOWTEXT , 0, 0, 0, 0},
				{ 0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0L, 0},
				{ 1, IDM_VIEW_LIST, TBSTATE_ENABLED, TBSTYLE_BUTTON | BTNS_SHOWTEXT , 0, 0, 0, 1},
				{ 2, IDM_VIEW_ICON, TBSTATE_ENABLED, TBSTYLE_BUTTON | BTNS_SHOWTEXT ,  0, 0, 0, 2},
				{ 3, IDM_VIEW_DETAILS, TBSTATE_ENABLED, TBSTYLE_BUTTON | BTNS_SHOWTEXT , 0, 0, 0, 3},
				{ 4, IDM_VIEW_TILES, TBSTATE_ENABLED, TBSTYLE_BUTTON | BTNS_SHOWTEXT ,  0, 0, 0, 4},
				{ 0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0L, 0},
				{ 5, IDM_OPTIONS, TBSTATE_ENABLED, TBSTYLE_BUTTON | BTNS_SHOWTEXT , 0, 0, 0, 5},
				{ 0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0L, 0},
				{ 6, IDM_ABOUT, TBSTATE_ENABLED, TBSTYLE_BUTTON | BTNS_SHOWTEXT,  0, 0, 0, 6}
			};

			hwndToolbar = CreateWindowEx(0, TOOLBARCLASSNAME, (LPWSTR) NULL, 
										WS_CHILD | WS_VISIBLE | TBSTYLE_LIST | TBSTYLE_TOOLTIPS , 0, 0, 0, 0, hDlg, 
										(HMENU) ID_TOOLBAR, hInst, NULL); 

			SendMessage(hwndToolbar, TB_BUTTONSTRUCTSIZE, (WPARAM) sizeof(TBBUTTON), 0); 

			SendMessage(hwndToolbar, TB_ADDBUTTONS, 10, (LPARAM)tbButtons);

			hImageList = ImageList_Create(16, 16,ILC_COLOR32, 7, 7);

			
			for (int i = 0; i < 7; ++i)
			{
				LoadString(hInst, IDS_GET_PHOTOS + i, stringBuffer, MAX_LOADSTRING - 1);
				stringBuffer[wcslen(stringBuffer) + 1] = 0;
				SendMessage(hwndToolbar, TB_ADDSTRING, (WPARAM)NULL, (LPARAM)stringBuffer);
				HICON icon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_GET_PHOTOS + i));
				ImageList_AddIcon(hImageList, icon);
			}

			SendMessage(hwndToolbar, TB_SETIMAGELIST, NULL, (LPARAM)hImageList);

			SendMessage(hwndToolbar, TB_SETEXTENDEDSTYLE, 0, TBSTYLE_EX_DOUBLEBUFFER | TBSTYLE_EX_MIXEDBUTTONS);

			RECT r;
			GetWindowRect(hwndToolbar, &r);
			toolBarHeight = r.bottom - r.top;

			hwndImage = GetDlgItem(hDlg, IDC_IMAGE_CONTROL);

			hwndList = GetDlgItem(hDlg, IDC_LIST1);

			ListView_SetExtendedListViewStyle(hwndList, LVS_EX_DOUBLEBUFFER | 
														LVS_EX_INFOTIP | 
														LVS_EX_GRIDLINES | 
														LVS_EX_FULLROWSELECT | 
														LVS_EX_GRIDLINES);
			ListView_SetView(hwndList, options.viewMode);

			LVCOLUMN lvcolumn;

			lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
			lvcolumn.fmt = LVCFMT_CENTER;
			lvcolumn.cx = (options.thumbWindowWidth - options.treeWidth - (options.splitterSize << 1)) >> 2;
			lvcolumn.pszText = L"Filename";
			lvcolumn.iSubItem = 0;
			ListView_InsertColumn(hwndList,0,&lvcolumn);
			lvcolumn.pszText = L"Date / Time";
			lvcolumn.iSubItem = 1;
			ListView_InsertColumn(hwndList,1,&lvcolumn);
			lvcolumn.pszText = L"File size";
			lvcolumn.iSubItem = 2;
			ListView_InsertColumn(hwndList,2,&lvcolumn);
			lvcolumn.pszText = L"Resolution";
			lvcolumn.iSubItem = 3;
			ListView_InsertColumn(hwndList,3,&lvcolumn);

			int swidth = ListView_GetStringWidth(hwndList, L"00.00.0000 00-00-00");

			SIZE sz = {options.thumbnailsSize + swidth + 10, options.thumbnailsSize + 5};
			LVTILEVIEWINFO lvtvi;
			lvtvi.cbSize = sizeof(LVTILEVIEWINFO);
			lvtvi.dwMask = LVTVIM_COLUMNS | LVTVIM_TILESIZE;
			lvtvi.dwFlags = LVTVIF_FIXEDSIZE;
			lvtvi.sizeTile = sz;
			lvtvi.cLines = options.thumbnailsSize / 20 - 1;
			ListView_SetTileViewInfo(hwndList, &lvtvi);

			hwndTree = GetDlgItem(hDlg,IDC_TREE1);

			GetCurrentDirectory(MAX_PATH, stringBuffer);

			Expand(hwndTree, NULL);
			ExpandingForPath(hwndTree, stringBuffer, NULL);

			RECT rect = {0,0,options.thumbWindowWidth, options.thumbWindowHeight};

			long style = GetWindowLong(hDlg,GWL_STYLE);
			long exStyle = GetWindowLong(hDlg,GWL_EXSTYLE);
			AdjustWindowRectEx(&rect, style, false, exStyle);
			rect.right -= rect.left;
			rect.bottom -= rect.top;
			rect.bottom += GetSystemMetrics(SM_CYMENU);

			if (rect.right > (workArea.right - workArea.left))
				rect.right = workArea.right - workArea.left;
			if (rect.bottom > (workArea.bottom - workArea.top))
				rect.bottom = workArea.bottom - workArea.top;

			a = (float)options.treeWidth / options.thumbWindowWidth;
			b = (float)options.treeHeight / (options.thumbWindowHeight - toolBarHeight);

			MoveWindow (hDlg, workArea.left + (((workArea.right - workArea.left) - rect.right) >> 1), 
								workArea.top + (((workArea.bottom - workArea.top) - rect.bottom) >> 1), 
							rect.right, rect.bottom, true);


			SendMessage(hDlg,WM_SETICON,ICON_SMALL, (LPARAM)icon);

			ListView_SetItemState(hwndList, 0, LVIS_FOCUSED, LVIS_FOCUSED);

			return TRUE;
		}
		break;

	case WM_TIMER:
		{
			int cF = countFiles;
			Status s = (Status)-1;
			do
			{
				if (countFiles == filesInDir)
				{
					if (ListView_GetView(hwndList) == LV_VIEW_DETAILS)
					{
						ListView_SetColumnWidth(hwndList, 1, LVSCW_AUTOSIZE_USEHEADER);
						ListView_SetColumnWidth(hwndList, 2, LVSCW_AUTOSIZE_USEHEADER);
						ListView_SetColumnWidth(hwndList, 3, LVSCW_AUTOSIZE);
					}
					KillTimer(hDlg,1);
					break;
				}
				
				HBITMAP hBitmap = 0;

				s = CreateThumbnailImage(fileNode, hBitmap);

				ImageList_Add(hLarge,hBitmap,0);

				WCHAR * pStr = PathFindExtension(fileNode->fileName);
				pStr++;

				for (int j = 0; j < (IDS_EXT15 - IDS_EXT1) + 1; ++j)
				{
					if (CompareString(LOCALE_SYSTEM_DEFAULT,NORM_IGNORECASE,pStr,-1, szExt[j],-1) == CSTR_EQUAL)
					{
						ImageList_AddIcon(hSmall, fileIcons[iconIndex[j]]);
						break;
					}
				}

				FileTimeToLocalTimeString(&fileNode->fileTime, stringBuffer, sizeof(stringBuffer) >> 1);
				ListView_SetItemText(hwndList, countFiles, 1, stringBuffer);
				swprintf_s(stringBuffer, sizeof(stringBuffer) >> 1, L"%0.2f Mb", (float)fileNode->size / (1024 * 1024));
				ListView_SetItemText(hwndList, countFiles, 2, stringBuffer);
				swprintf_s(stringBuffer, sizeof(stringBuffer) >> 1, L"%dx%d", fileNode->width, fileNode->height);
				ListView_SetItemText(hwndList, countFiles, 3, stringBuffer);
				
				fileNode = fileNode->pNextNode;
				
				DeleteObject(hBitmap);

				countFiles++;

			}while((s == Ok) && (countFiles - cF < 10));

			ListView_RedrawItems(hwndList,cF,countFiles - 1);
		}
		break;

	case WM_NOTIFY:

		LPNMITEMACTIVATE nms;
		nms = (LPNMITEMACTIVATE)lParam;
		
		if (nms->hdr.idFrom == IDC_LIST1)
		{
			if (nms->hdr.code == LVN_GETINFOTIP)
			{
				LPNMLVGETINFOTIP lpit = (LPNMLVGETINFOTIP)nms;
				int index = lpit->iItem;

				FileNode * fileNode = cFileNode;
				while (index != fileNode->fileCount - 1)
				{
					fileNode = fileNode->pNextNode;
				}

				wcscpy_s(lpit->pszText, lpit->cchTextMax, fileNode->fileName);

				stringBuffer[0] = L'\n';
				FileTimeToLocalTimeString(&fileNode->fileTime, stringBuffer + 1, sizeof(stringBuffer) >> 1);
				wcscat_s(lpit->pszText, lpit->cchTextMax, stringBuffer);
				swprintf_s(stringBuffer, sizeof(stringBuffer) >> 1, L"\nFile size: %0.2f Mb", (float)fileNode->size / (1024 * 1024));
				wcscat_s(lpit->pszText, lpit->cchTextMax, stringBuffer);
				swprintf_s(stringBuffer, sizeof(stringBuffer) >> 1, L"\nResolution: %dx%d", fileNode->width, fileNode->height);
				wcscat_s(lpit->pszText, lpit->cchTextMax, stringBuffer);

				for (int indexItem = 0; indexItem < 37; ++indexItem)
				{
					WCHAR * string = GetExifPropertyString(fileNode->fileName, propertiesList[indexItem].tag);
					if (string)
					{
						wcscat_s(lpit->pszText, lpit->cchTextMax, L"\n");
						wcscat_s(lpit->pszText, lpit->cchTextMax, propertiesList[indexItem].string);
						wcscat_s(lpit->pszText, lpit->cchTextMax, L": ");
						wcscat_s(lpit->pszText, lpit->cchTextMax, string);
					}
				}
			}
			if (nms->hdr.code == LVN_ITEMCHANGED && (nms->uNewState & LVIS_FOCUSED))
			{
				long index = nms->iItem;

				while (index != cFileNode->fileCount - 1)
				{
					cFileNode = cFileNode->pNextNode;
				}
				if (curImage)
				{
					delete curImage;
					curImage = 0;
				}
				Bitmap * image = new Bitmap(cFileNode->fileName);
				INT thWidth, thHeight, x, y;
				FitAndCenterImage(options.treeWidth - options.splitterSize,
								  options.thumbWindowHeight - options.treeHeight - (options.splitterSize << 1),
								  image->GetWidth(),
								  image->GetHeight(),
								  x,
								  y,
								  thWidth,
								  thHeight);
				
				curImage = new Bitmap(thWidth, thHeight, PixelFormat24bppRGB);
				Graphics * graphics = new Graphics(curImage);
				graphics->DrawImage(image, 0, 0, thWidth, thHeight);
				delete graphics;
				delete image;
				RedrawWindow(hwndImage,0,0,RDW_INVALIDATE);
			}

			if (nms->hdr.code == NM_DBLCLK && nms->iItem >= 0)
			{
				SendMessage(hDlg,WM_COMMAND,IDOK,0);
			}

			if (nms->hdr.code == LVN_BEGINDRAG)
			{
				UINT count = ListView_GetItemCount(hwndList);
				UINT selCount = ListView_GetSelectedCount(hwndList);
				LPITEMIDLIST * p = new LPITEMIDLIST[selCount];

				GetCurrentDirectory(1024, stringBuffer);
				IShellFolder * isf = CreateIShellFolder(stringBuffer);

				UINT indexSel = 0;
				FileNode * fileNode = cFileNode;

				for (UINT i = 0; i < count; ++i)
				{
					if (ListView_GetItemState(hwndList, fileNode->fileCount - 1, LVIS_SELECTED))
					{
						GetCurrentDirectory(1024, stringBuffer);
						PathAppend(stringBuffer, fileNode->fileName);
						LPITEMIDLIST pidl = ILCreateFromPath(stringBuffer);
						p[indexSel++] = ILFindLastID(pidl);
					}
					fileNode = fileNode->pNextNode;
				}

				IDataObject * dataObject;
				isf->GetUIObjectOf(hwndList, selCount, (LPCITEMIDLIST*)p, IID_IDataObject, NULL, (void**)&dataObject);

				LPDROPSOURCE dropSource = new CDropSource();

				DWORD de = 0;
				DoDragDrop(dataObject,
							dropSource,
							DROPEFFECT_COPY | DROPEFFECT_MOVE | DROPEFFECT_LINK,
							&de);

				delete dropSource;
				isf->Release();
				delete[] p;

			}
			if (nms->hdr.code == NM_RCLICK && nms->iItem >= 0)
			{
				UINT count = ListView_GetItemCount(hwndList);
				UINT selCount = ListView_GetSelectedCount(hwndList);

				LPITEMIDLIST * p = new LPITEMIDLIST[selCount];
				FileNode ** indexItem = new FileNode*[selCount];

				GetCurrentDirectory(1024, stringBuffer);
				IShellFolder * isf = CreateIShellFolder(stringBuffer);

				FileNode * fileNode = cFileNode;

				UINT indexSel = 0;
				for (UINT i = 0; i < count; ++i)
				{
					if (ListView_GetItemState(hwndList, fileNode->fileCount - 1, LVIS_SELECTED))
					{
						GetCurrentDirectory(1024, stringBuffer);
						PathAppend(stringBuffer, fileNode->fileName);
						LPITEMIDLIST pidl = ILCreateFromPath(stringBuffer);
						p[indexSel] = ILFindLastID(pidl);
						indexItem[indexSel] = fileNode;
						indexSel++;
					}
					fileNode = fileNode->pNextNode;
				}

				contextMenu = CreateContextMenu(hDlg, isf, selCount, (LPCITEMIDLIST*)p);

				if (contextMenu)
				{
					POINT pt;
					GetCursorPos((LPPOINT)&pt);

					if (DoContextMenuThing(hDlg, contextMenu, &pt))
					{
						FileNode * fileNode;
						for (UINT i = 0; i < selCount; ++i)
						{
							fileNode = indexItem[i];
							if (fileNode == cFileNode)
								cFileNode = cFileNode->pNextNode;
							ListView_DeleteItem(hwndList, fileNode->fileCount - 1);
							DeleteFileNode(fileNode);
							filesInDir--;
						}
						ListView_Arrange(hwndList, LVA_DEFAULT);
					}

					contextMenu->Release();
					contextMenu = 0;
				}

				isf->Release();
				delete[] p;
				delete[] indexItem;
			}
		}
		
		NMTREEVIEW * nmTreeView;
		NMHDR * pHdr;
		pHdr = (NMHDR*)lParam;
		nmTreeView = (NMTREEVIEW*)lParam;
		if (pHdr->idFrom == IDC_TREE1)
		{
			if (pHdr->code == TVN_ITEMEXPANDING && nmTreeView->action == TVE_EXPAND)
			{
				HTREEITEM pTreeItem = nmTreeView->itemNew.hItem;
				Expand(hwndTree, pTreeItem);
			}
			if (pHdr->code == TVN_ITEMEXPANDING && nmTreeView->action == TVE_COLLAPSE)
			{

				TreeView_Expand(hwndTree, nmTreeView->itemNew.hItem, TVE_COLLAPSE | TVE_COLLAPSERESET);
			}

			if (pHdr->code == NM_RCLICK)
			{
				TVITEMEX tvItem;
				HTREEITEM htreeItem = TreeView_GetDropHilight(hwndTree);
				if (!htreeItem)
					htreeItem = TreeView_GetSelection(hwndTree);
				tvItem.mask = TVIF_PARAM | TVIF_CHILDREN;
				tvItem.hItem = htreeItem;
				TreeView_GetItem(hwndTree, &tvItem);

				TreeItem * treeItem = (TreeItem*)tvItem.lParam;
				IShellFolder * isf2 = treeItem->isfParent;

				contextMenu = CreateContextMenu(hDlg, isf2, 1, (LPCITEMIDLIST*)&treeItem->pidl);

				if (contextMenu)
				{
					POINT pt;
					GetCursorPos((LPPOINT)&pt);

					if (DoContextMenuThing(hDlg, contextMenu, &pt))
					{
						TreeView_DeleteItem(hwndTree, htreeItem);
					}
					else
					{
						TreeView_Expand(hwndTree, htreeItem, TVE_COLLAPSE | TVE_COLLAPSERESET);
						TreeView_Expand(hwndTree, htreeItem, TVE_EXPAND);
					}

					contextMenu->Release();
					contextMenu = 0;
				}
			}

			if (nmTreeView->hdr.code == TVN_BEGINDRAG)
			{
				TVITEMEX tvItem;
				HTREEITEM htreeItem = TreeView_GetDropHilight(hwndTree);
				if (!htreeItem)
					htreeItem = TreeView_GetSelection(hwndTree);
				tvItem.mask = TVIF_PARAM | TVIF_CHILDREN;
				tvItem.hItem = htreeItem;
				TreeView_GetItem(hwndTree, &tvItem);

				TreeItem * treeItem = (TreeItem*)tvItem.lParam;
				IShellFolder * isf = treeItem->isfParent;

				IDataObject * dataObject;
				isf->GetUIObjectOf(hwndTree, 1, (LPCITEMIDLIST*)&treeItem->pidl, IID_IDataObject, NULL, (void**)&dataObject);

				LPDROPSOURCE dropSource = new CDropSource();

				DWORD de = 0;
				DoDragDrop(dataObject,
							dropSource,
							DROPEFFECT_COPY | DROPEFFECT_MOVE | DROPEFFECT_LINK,
							&de);

				delete dropSource;
			}

			if (pHdr->code == TVN_SELCHANGING)
			{
				HTREEITEM pTreeItem;
				TVITEMEX tvItem;

				pTreeItem = nmTreeView->itemNew.hItem;

				tvItem.mask = TVIF_PARAM | TVIF_CHILDREN;
				tvItem.hItem = pTreeItem;
				TreeView_GetItem(hwndTree, &tvItem);
				TreeItem * treeItem = (TreeItem*)tvItem.lParam;

				if (curImage)
				{
					delete curImage;
					curImage = 0;
				}
				if (hLarge)
				{
					ImageList_Destroy(hLarge);
					hLarge = 0;
				}
				if (hSmall)
				{
					ImageList_Destroy(hSmall);
					hSmall = 0;
				}
				ListView_DeleteAllItems(hwndList);

				KillTimer(hDlg, 1);

				DeleteAllFileNode(cFileNode);
				cFileNode = 0;

				filesInDir = 0;
				if (SetCurrentDirectory(treeItem->path))
				{
					filesInDir = ScanDirectory(&cFileNode);
					fileNode = cFileNode;
					if (filesInDir)
					{
						FeelListView(hwndList, hLarge, hSmall, filesInDir, cFileNode);
						ListView_SetItemState(hwndList, 0, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
						countFiles = 0;
						SetTimer(hDlg,1,0,0);
					}
					else
					{
						RedrawWindow(hwndImage,0,0,RDW_INVALIDATE);
					}
				}
			}
		}
		break;
	case WM_SIZE:
		{
			options.thumbWindowHeight = HIWORD(lParam);
			options.thumbWindowWidth = LOWORD(lParam);
			options.treeWidth = (int)(a * options.thumbWindowWidth);
			options.treeHeight = (int)(b * (options.thumbWindowHeight - toolBarHeight));
			MoveWindow(hwndToolbar, 0, 0, options.thumbWindowWidth, toolBarHeight, false);
			MoveWindow(hwndImage, 0, options.treeHeight + (options.splitterSize << 1) + toolBarHeight, options.treeWidth - options.splitterSize,  options.thumbWindowHeight - options.treeHeight - (options.splitterSize << 1) - toolBarHeight, false);
			RedrawWindow(hwndImage,0,0,RDW_INVALIDATE | RDW_FRAME);
			RECT r = {options.treeWidth - options.splitterSize, toolBarHeight, options.treeWidth + options.splitterSize, options.thumbWindowHeight};
			RedrawWindow(hDlg, &r, 0, RDW_INVALIDATE);
			RECT r2 = {0, options.treeHeight + toolBarHeight, options.treeWidth, options.treeHeight + (options.splitterSize << 1) + toolBarHeight};
			RedrawWindow(hDlg, &r2, 0, RDW_INVALIDATE);
			MoveWindow (hwndList, options.treeWidth + options.splitterSize, toolBarHeight, options.thumbWindowWidth - (options.treeWidth + options.splitterSize), options.thumbWindowHeight - toolBarHeight, true);
			MoveWindow (hwndTree, 0, toolBarHeight, options.treeWidth - options.splitterSize, options.treeHeight, true);
			ListView_Arrange(hwndList,LVA_ALIGNTOP);
		}
		break;

	case WM_MOVE:
		  {
			GetNearestMonitorWorkAndScreenArea(hDlg, &workArea, &screenArea);
		  }
		  break;

	case WM_MOUSEMOVE:

		if (GET_X_LPARAM(lParam) >= options.treeWidth - options.splitterSize)
		{
			SetCursor(arrowHCursor);
		}
		else if (GET_Y_LPARAM(lParam) >= (options.treeHeight  + toolBarHeight) && 
				GET_Y_LPARAM(lParam) <= options.treeHeight + toolBarHeight + (options.splitterSize << 1))
		{
			SetCursor(arrowVCursor);
		}

		if ((trackX || trackY) && wParam != MK_LBUTTON)
		{
			ReleaseCapture();
			trackX = false;
			trackY = false;
		}
		
		if (!(trackX || trackY))
			break;

		if (trackX)
		{			
			options.treeWidth = GET_X_LPARAM(lParam);
			if (options.treeWidth < options.splitterSize)
				options.treeWidth = options.splitterSize;
			if (options.treeWidth > options.thumbWindowWidth - options.splitterSize)
				options.treeWidth = options.thumbWindowWidth - options.splitterSize;
			a = (float)options.treeWidth / options.thumbWindowWidth;
			RECT r = {options.treeWidth - options.splitterSize, toolBarHeight, options.treeWidth + options.splitterSize, options.thumbWindowHeight};
			RedrawWindow(hDlg, &r, 0, RDW_INVALIDATE);	
			MoveWindow (hwndList, options.treeWidth + options.splitterSize, toolBarHeight, options.thumbWindowWidth - (options.treeWidth + options.splitterSize), options.thumbWindowHeight - toolBarHeight, true);
			ListView_Arrange(hwndList,LVA_ALIGNTOP);
			
		}
		else if (trackY)
		{
			options.treeHeight = GET_Y_LPARAM(lParam) - toolBarHeight;
			if (options.treeHeight < 0)
				options.treeHeight = 0;
			if (options.treeHeight > options.thumbWindowHeight - (options.splitterSize << 1) - toolBarHeight)
				options.treeHeight = options.thumbWindowHeight - (options.splitterSize << 1) - toolBarHeight;
			b = (float)options.treeHeight / (options.thumbWindowHeight - toolBarHeight);
			RECT r = {0, options.treeHeight + toolBarHeight, options.treeWidth, options.treeHeight + (options.splitterSize << 1) + toolBarHeight};
			RedrawWindow(hDlg, &r, 0, RDW_INVALIDATE);
		}
		
		MoveWindow(hwndImage, 0, options.treeHeight + (options.splitterSize << 1) + toolBarHeight, options.treeWidth - options.splitterSize,  options.thumbWindowHeight - options.treeHeight - (options.splitterSize << 1) - toolBarHeight, false);
		MoveWindow (hwndTree, 0, toolBarHeight, options.treeWidth - options.splitterSize, options.treeHeight, true);
		RedrawWindow(hwndImage, 0, 0,RDW_INVALIDATE | RDW_FRAME);
		break;

	case WM_LBUTTONDOWN:

		if (GET_X_LPARAM(lParam) >= options.treeWidth - options.splitterSize)
		{
			SetCursor(arrowHCursor);
			SetCapture(hDlg);
			trackX = true;
		}
		else if (GET_Y_LPARAM(lParam) >= (options.treeHeight + toolBarHeight) && 
				GET_Y_LPARAM(lParam) <= (options.treeHeight + toolBarHeight) + (options.splitterSize << 1))
		{
			SetCursor(arrowVCursor);
			SetCapture(hDlg);
			trackY = true;
		}
		break;

	case WM_COMMAND:
		if(LOWORD(wParam) == ID_VIEWMODE_LIST || LOWORD(wParam) == IDM_VIEW_LIST)
		{
			ListView_SetView(hwndList,LV_VIEW_LIST);
		}
		if(LOWORD(wParam) == ID_VIEWMODE_ICON || LOWORD(wParam) == IDM_VIEW_ICON)
		{
			ListView_SetView(hwndList,LV_VIEW_ICON);
		}
		if(LOWORD(wParam) == ID_VIEWMODE_DETAILS || LOWORD(wParam) == IDM_VIEW_DETAILS)
		{
			ListView_SetView(hwndList,LV_VIEW_DETAILS);
			ListView_SetColumnWidth(hwndList, 0, LVSCW_AUTOSIZE_USEHEADER);
			ListView_SetColumnWidth(hwndList, 1, LVSCW_AUTOSIZE_USEHEADER);
			ListView_SetColumnWidth(hwndList, 2, LVSCW_AUTOSIZE_USEHEADER);
			ListView_SetColumnWidth(hwndList, 3, LVSCW_AUTOSIZE);
		}
		if(LOWORD(wParam) == ID_VIEWMODE_TILES || LOWORD(wParam) == IDM_VIEW_TILES)
		{
			ListView_SetView(hwndList,LV_VIEW_TILE);
		}
		if(LOWORD(wParam) == ID_MENU_OPTIONS || LOWORD(wParam) == IDM_OPTIONS)
		{
			int oldThumbSize = options.thumbnailsSize;
			DialogBox(hInst, (LPCTSTR)IDD_OPTION, hDlg, (DLGPROC)Options);
			MoveWindow(hwndToolbar, 0, 0, options.thumbWindowWidth, toolBarHeight, false);
			MoveWindow(hwndImage, 0, options.treeHeight + (options.splitterSize << 1) + toolBarHeight, options.treeWidth - options.splitterSize,  options.thumbWindowHeight - options.treeHeight - (options.splitterSize << 1) - toolBarHeight, false);
			RedrawWindow(hwndImage,0,0,RDW_INVALIDATE | RDW_FRAME);
			RECT r = {options.treeWidth - options.splitterSize, toolBarHeight, options.treeWidth + options.splitterSize, options.thumbWindowHeight};
			RedrawWindow(hDlg, &r, 0, RDW_INVALIDATE);
			RECT r2 = {0, options.treeHeight + toolBarHeight, options.treeWidth, options.treeHeight + (options.splitterSize << 1) + toolBarHeight};
			RedrawWindow(hDlg, &r2, 0, RDW_INVALIDATE);
			MoveWindow (hwndList, options.treeWidth + options.splitterSize, toolBarHeight, options.thumbWindowWidth - (options.treeWidth + options.splitterSize), options.thumbWindowHeight - toolBarHeight, true);
			MoveWindow (hwndTree, 0, toolBarHeight, options.treeWidth - options.splitterSize, options.treeHeight, true);
			int swidth = ListView_GetStringWidth(hwndList, L"00.00.0000 00-00-00");
			SIZE sz = {options.thumbnailsSize + swidth + 10, options.thumbnailsSize + 5};
			LVTILEVIEWINFO lvtvi;
			lvtvi.cbSize = sizeof(LVTILEVIEWINFO);
			lvtvi.dwMask = LVTVIM_COLUMNS | LVTVIM_TILESIZE;
			lvtvi.dwFlags = LVTVIF_FIXEDSIZE;
			lvtvi.sizeTile = sz;
			lvtvi.cLines = options.thumbnailsSize / 20 - 1;
			ListView_SetTileViewInfo(hwndList, &lvtvi);
			ListView_Arrange(hwndList,LVA_ALIGNTOP);
			if (oldThumbSize != options.thumbnailsSize)
			{
				HTREEITEM treeItem = TreeView_GetSelection(hwndTree);
				NMTREEVIEW treeViewStruct;
				treeViewStruct.itemNew.hItem = treeItem;
				treeViewStruct.hdr.code = TVN_SELCHANGING;
				treeViewStruct.hdr.idFrom = IDC_TREE1;
				SendMessage(hDlg, WM_NOTIFY, NULL, (LPARAM)&treeViewStruct);
			}
		}
		if(LOWORD(wParam) == ID_MENU_GETPICTURE || LOWORD(wParam) == IDM_GETPHOTOS)
		{
			GETPHOTOS getPhotos;
			HRESULT hr = GetPhotos(hInst, hDlg, &options, &getPhotos);
			if (S_OK == hr)
			{
				if (getPhotos.gotoPath)
				{
					TreeView_DeleteAllItems(hwndTree);
					ReleaseDirTree();
					Expand(hwndTree, NULL);
					ExpandingForPath(hwndTree, getPhotos.path, NULL);
				}
			}
		}
		if(LOWORD(wParam) == ID_MENU_ABOUT || LOWORD(wParam) == IDM_ABOUT)
		{
			DialogBoxParam(hInst, (LPCTSTR)IDD_ABOUT, hDlg, (DLGPROC)&About,  (LPARAM)hInst);
		}
		if (LOWORD(wParam) == IDOK)
		{
			filesInDirectory = filesInDir;
			LoadImage(cFileNode->fileName);
			EndDialog(hDlg, LOWORD(wParam));
		}
		if (LOWORD(wParam) == IDCANCEL || LOWORD(wParam) == ID_MENU_EXIT) 
		{
			EndDialog(hDlg, IDCANCEL);
		}
		break;

	case WM_DESTROY:

		KillTimer(hDlg,1);

		options.viewMode = ListView_GetView(hwndList);

		if (hImageList)
		{
			ImageList_Destroy(hImageList);
			hLarge = 0;
		}
		if (hLarge)
		{
			ImageList_Destroy(hLarge);
			hLarge = 0;
		}
		if (hSmall)
		{
			ImageList_Destroy(hSmall);
			hSmall = 0;
		}

		ReleaseDirTree();

		if (curImage)
		{
			delete curImage;
			curImage = NULL;
		}
		break;
	}
	return FALSE;
}

LRESULT CALLBACK PropertyFile(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	unsigned int uc = 0;

	switch (message)
	{
	case WM_INITDIALOG:

		SetDlgItemText(hDlg,IDC_PROP_FILENAME,cFileNode->fileName);
		GetCurrentDirectory(1023, stringBuffer);
		SetDlgItemText(hDlg,IDC_PROP_DIRECTORY, stringBuffer);
		PathAddBackslash(stringBuffer);
		wcscat_s(stringBuffer, sizeof(stringBuffer) >> 1, cFileNode->fileName);
		SetDlgItemText(hDlg,IDC_PROP_PATH, stringBuffer);

		if (guid == ImageFormatJPEG)
		{
			SetDlgItemText(hDlg,IDC_PROP_FORMAT, L"JPEG");
		}
		if (guid == ImageFormatBMP)
		{
			SetDlgItemText(hDlg,IDC_PROP_FORMAT, L"BMP");
		}
		if (guid == ImageFormatTIFF)
		{
			SetDlgItemText(hDlg,IDC_PROP_FORMAT, L"TIFF");
		}
		if (guid == ImageFormatGIF)
		{
			SetDlgItemText(hDlg,IDC_PROP_FORMAT, L"GIF");
		}
		if (guid == ImageFormatPNG)
		{
			SetDlgItemText(hDlg,IDC_PROP_FORMAT, L"PNG");
		}
		if (guid == ImageFormatEXIF)
		{
			SetDlgItemText(hDlg,IDC_PROP_FORMAT, L"EXIF");
		}
		if (guid == ImageFormatEMF)
		{
			SetDlgItemText(hDlg,IDC_PROP_FORMAT, L"EMF");
		}
		if (guid == ImageFormatWMF)
		{
			SetDlgItemText(hDlg,IDC_PROP_FORMAT, L"WMF");
		}

		swprintf_s(stringBuffer, sizeof(stringBuffer) >> 1, L"%d x %d %s (%.2f %s)", width,height, szPixels,((float)(width * height)) / 1000000, szMPixels);
		SetDlgItemText(hDlg,IDC_PROP_SIZE, stringBuffer);

		switch(pixelFormat)
		{
		case PixelFormat4bppIndexed:
			wcscpy_s(stringBuffer, sizeof(stringBuffer) >> 1, L"4 ");
			break;
		case PixelFormat8bppIndexed:
			wcscpy_s(stringBuffer, sizeof(stringBuffer) >> 1, L"8 ");
			break;
		case PixelFormat16bppRGB565:
		case PixelFormat16bppRGB555: 
			wcscpy_s(stringBuffer, sizeof(stringBuffer) >> 1, L"16 ");
			break;
		case PixelFormat24bppRGB:
			wcscpy_s(stringBuffer, sizeof(stringBuffer) >> 1, L"24 ");
			break;
		case PixelFormat32bppRGB:
		case PixelFormat32bppARGB:
			wcscpy_s(stringBuffer, sizeof(stringBuffer) >> 1, L"32 ");
			break;
		case PixelFormat48bppRGB:
			wcscpy_s(stringBuffer, sizeof(stringBuffer) >> 1, L"48 ");
			break;
		case PixelFormat64bppARGB:
			wcscpy_s(stringBuffer, sizeof(stringBuffer) >> 1, L"64 ");
			break;
		}

		wcscat_s(stringBuffer, sizeof(stringBuffer) >> 1, szBPP);
		SetDlgItemText(hDlg,IDC_PROP_COLORS, stringBuffer);

		swprintf_s(stringBuffer,  sizeof(stringBuffer) >> 1, L"%.2f %s (%I64u %s)", (float)cFileNode->size / (1024 * 1024), 
				 szMb, 
				 cFileNode->size, 
				 szBytes);

		SetDlgItemText(hDlg,IDC_PROP_DISKSIZE, stringBuffer);

		swprintf_s(stringBuffer,  sizeof(stringBuffer) >> 1, L"%.2f %s (%I64u %s)", 
				 (float)((float)width * (float)height * 4) / (1024 * 1024), 
				 szMb, 
				 (unsigned __int64)width * (unsigned __int64)height * 4,
				 szBytes);

		SetDlgItemText(hDlg,IDC_PROP_MEMSIZE, stringBuffer);

		FileTimeToLocalTimeString(&cFileNode->fileTime, stringBuffer, sizeof(stringBuffer) >> 1);
		SetDlgItemText(hDlg,IDC_PROP_DATE, stringBuffer);

		swprintf_s(stringBuffer,  sizeof(stringBuffer) >> 1, L"%d / %d", cFileNode->fileCount, filesInDirectory);
		SetDlgItemText(hDlg,IDC_PROP_DIRINDEX, stringBuffer);

		uc = CalculateUniqueColors(image);
		swprintf_s(stringBuffer,  sizeof(stringBuffer) >> 1, L"%d", uc);
		SetDlgItemText(hDlg,IDC_PROP_UNCOLORS, stringBuffer);

		return TRUE;
		break;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK) 
		{
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		if (LOWORD(wParam) == IDCANCEL) 
		{
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
	return FALSE;
}

LRESULT CALLBACK PropertyEXIF(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HWND hwndList;

	switch (message)
	{
	case WM_INITDIALOG:

		{
			RECT rect;
			GetClientRect(hDlg, &rect);

			hwndList = GetDlgItem(hDlg, IDC_LIST1);
			ListView_SetExtendedListViewStyleEx(hwndList,LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT,
												LVS_EX_GRIDLINES);

			LVCOLUMN lvcolumn;

			lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
			lvcolumn.fmt = LVCFMT_CENTER;
			lvcolumn.cx = 250;
			lvcolumn.pszText = szTag;
			lvcolumn.iSubItem = 0;
			int a = ListView_InsertColumn(hwndList,0,&lvcolumn);
			lvcolumn.pszText = szValue;
			lvcolumn.iSubItem = 1;
			a = ListView_InsertColumn(hwndList,1,&lvcolumn);

			LVITEM lvI;

			lvI.mask = LVIF_TEXT; 
			lvI.state = 0; 
			lvI.stateMask = 0; 
			lvI.iImage = 0;
			lvI.iSubItem = 0;
			lvI.lParam = 0;

			int indexItem = 0;
			int indexinList = 0;
			for (indexItem = 0; indexItem < 37; ++indexItem)
			{
				WCHAR * string = GetExifPropertyString(cFileNode->fileName, propertiesList[indexItem].tag);
				if (string)
				{
					lvI.iItem = indexinList++;
					lvI.pszText = propertiesList[indexItem].string;
					ListView_InsertItem(hwndList, &lvI);
					ListView_SetItemText(hwndList, lvI.iItem, 1, string);
				}
			}

			if (ReadCanonMakerNotes(cFileNode->fileName))
			{

				lvI.iItem = indexinList++;
				lvI.pszText = L"";
				ListView_InsertItem(hwndList, &lvI);
				lvI.iItem = indexinList++;
				lvI.pszText = szCanonMN;
				ListView_InsertItem(hwndList, &lvI);
				lvI.iItem = indexinList++;
				lvI.pszText = L"";
				ListView_InsertItem(hwndList, &lvI);

				for (; indexItem < 56; ++indexItem)
				{
					WCHAR * string = GetExifCanonMakerNotesString((eCanonPropertyId)propertiesList[indexItem].tag);
					if (string)
					{
						lvI.iItem = indexinList++;
						lvI.pszText = propertiesList[indexItem].string;
						ListView_InsertItem(hwndList, &lvI);
						ListView_SetItemText(hwndList, lvI.iItem, 1, string);
					}
				}

			}

			ReleaseData();

			if (!indexinList)
			{
				lvI.iItem = indexinList++;
				lvI.pszText = szExifNotAvailable;
				ListView_InsertItem(hwndList, &lvI);
			}

			DWORD size = ListView_ApproximateViewRect(hwndList, -1, -1, -1);
			rect.top = 0;
			rect.left = 0;
			rect.right = LOWORD(size) + 4;
			rect.bottom = HIWORD(size);
			LONG style = GetWindowLong(hDlg,GWL_STYLE);
			LONG exStyle = GetWindowLong(hDlg,GWL_EXSTYLE);
			AdjustWindowRectEx(&rect, style, false, exStyle);
			rect.right -= rect.left;
			rect.bottom -= rect.top;
			if (rect.right > (workArea.right - workArea.left))
				rect.right = workArea.right - workArea.left;
			if (rect.bottom > (workArea.bottom - workArea.top))
				rect.bottom = workArea.bottom - workArea.top;
			MoveWindow (hDlg, workArea.left + (((workArea.right - workArea.left) - rect.right) >> 1), 
							  workArea.top + (((workArea.bottom - workArea.top) - rect.bottom) >> 1), 
							rect.right, rect.bottom, true);

			return TRUE;
		}
		break;

	case WM_SIZE:

		MoveWindow (hwndList, 0, 0, LOWORD(lParam), HIWORD(lParam), true);
		break;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK) 
		{
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		if (LOWORD(wParam) == IDCANCEL) 
		{
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
	return FALSE;
}

FileNode * FindFileNode(FileNode * fileNode, WCHAR * fileName)
{
	if (fileNode)
	{
		FileNode * tempFileNode = fileNode;
		do 
		{
			if (CompareString(LOCALE_SYSTEM_DEFAULT,NORM_IGNORECASE,fileNode->fileName,-1,fileName,-1) == CSTR_EQUAL)
				return fileNode;
			fileNode = fileNode->pNextNode;
		} while(fileNode != tempFileNode);
	}

	return NULL;
}

unsigned int ScanDirectory(FileNode ** cFileNode)
{
	HANDLE handle = 0;
	WIN32_FIND_DATAW findData;

	FileNode * tempFileNode = 0;
	FileNode * fileNode = 0;
	handle = FindFirstFile(L"*.*",&findData);
	unsigned int filesInDirectory = 0;

	do
	{
		if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			WCHAR * pStr = PathFindExtension(findData.cFileName);

			if (*pStr)
			{
				pStr++;
				for (int i = 0; i < (IDS_EXT15 - IDS_EXT1) + 1; ++i)
				{
					if (CompareString(LOCALE_SYSTEM_DEFAULT,NORM_IGNORECASE,pStr,-1, szExt[i],-1) == CSTR_EQUAL)
					{
						if (!fileNode)
						{
							fileNode = new FileNode;
							fileNode->pNextNode = 0;
							tempFileNode = fileNode;
							int len = (int)wcslen(findData.cFileName);
							wcscpy_s(fileNode->fileName, sizeof(fileNode->fileName) >> 1, findData.cFileName);
							fileNode->fileCount = 1;
							fileNode->size = ((long long)findData.nFileSizeHigh << 32) | findData.nFileSizeLow;
							fileNode->fileTime = findData.ftCreationTime;
						}
						else
						{
							tempFileNode->pNextNode = new FileNode;
							tempFileNode->pNextNode->fileCount = tempFileNode->fileCount + 1;
							tempFileNode->pNextNode->pPrevNode = tempFileNode;
							tempFileNode = tempFileNode->pNextNode;
							tempFileNode->pNextNode = 0;
							int len = (int)wcslen(findData.cFileName);
							wcscpy_s(tempFileNode->fileName, sizeof(tempFileNode->fileName) >> 1, findData.cFileName);
							tempFileNode->size = ((long long)findData.nFileSizeHigh << 32) | findData.nFileSizeLow;
							tempFileNode->fileTime = findData.ftCreationTime;
						}
						break;
					}
				}
			}
		}
	}while (FindNextFile(handle,&findData));

	FindClose(handle);

	if (fileNode)
	{
		filesInDirectory = tempFileNode->fileCount;
		fileNode->pPrevNode = tempFileNode;
		tempFileNode->pNextNode = fileNode;
	}
	*cFileNode = fileNode;
	return filesInDirectory;
}

DWORD OpenFileDialog()
{
	OPENFILENAME openfilename;
	WCHAR filename[MAX_PATH + 1];
	WCHAR currDir[MAX_PATH + 1];
	WCHAR fileTitle[MAX_PATH + 1];

	ZeroMemory(&openfilename, sizeof(OPENFILENAME));
	filename[0] = L'\0';
	GetCurrentDirectory(MAX_PATH, currDir);

	openfilename.lStructSize = sizeof(OPENFILENAME);
	openfilename.hwndOwner = hWnd;
	openfilename.hInstance = NULL;
	openfilename.lpstrFilter = L"Picture\0*.jpeg;*.jpg;*.bmp;*.gif;*.tif;*.tiff;*.png\0\0";
	openfilename.nFilterIndex = 1;
	openfilename.lpstrFile = filename;
	openfilename.nMaxFile = MAX_PATH;
	openfilename.lpstrInitialDir = currDir;
	openfilename.lpstrFileTitle = fileTitle;
	openfilename.nMaxFileTitle = MAX_PATH;
	openfilename.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST;

	if (GetOpenFileName(&openfilename))
	{
		WCHAR curDir[MAX_PATH + 1];

		GetCurrentDirectory(MAX_PATH, curDir);
		PathRemoveFileSpec(openfilename.lpstrFile);
		SetCurrentDirectory(openfilename.lpstrFile);
		int filesInDir;
		FileNode * fileNode;
		FileNode * fileNodeCurr;
		filesInDir = ScanDirectory(&fileNode);
		fileNodeCurr = FindFileNode(fileNode, openfilename.lpstrFileTitle);
		if (fileNodeCurr)
		{
			filesInDirectory = filesInDir;
			DeleteAllFileNode(cFileNode);
			cFileNode = fileNodeCurr;
			LoadImage(cFileNode->fileName);
			return ERC_OK;
		}
		DeleteAllFileNode(fileNode);
		SetCurrentDirectory(curDir);
		return ERC_NOIMAGE;
	}
	return ERC_CANCEL;
}

DWORD SaveFileDialog()
{
	OPENFILENAME openfilename;
	WCHAR filename[MAX_PATH + 1];
	WCHAR currDir[MAX_PATH + 1];
	WCHAR fileTitle[MAX_PATH + 1];

	ZeroMemory(&openfilename, sizeof(OPENFILENAME));
	wcscpy_s(filename, sizeof(filename) >> 1, cFileNode->fileName);
	GetCurrentDirectory(MAX_PATH, currDir);

	openfilename.lStructSize = sizeof(OPENFILENAME);
	openfilename.hwndOwner = hWnd;
	openfilename.hInstance = NULL;
	openfilename.lpstrFilter = L"Picture\0*.jpeg;*.jpg;*.bmp;*.gif;*.tif;*.tiff;*.png\0";
	openfilename.nFilterIndex = 1;
	openfilename.lpstrFile = filename;
	openfilename.nMaxFile = MAX_PATH;
	openfilename.lpstrInitialDir = currDir;
	openfilename.lpstrFileTitle = fileTitle;
	openfilename.nMaxFileTitle = MAX_PATH;
	openfilename.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST;

	if (GetSaveFileName(&openfilename))
	{
		image->Save(openfilename.lpstrFile,&clsId,NULL);
	}
	SetCurrentDirectory(currDir);
	return ERC_OK;
}

bool LoadImage(WCHAR * fileName)
{
	if (image)
	{
		delete image;
		image = 0;
	}

	image = new Bitmap(fileName);

	Status s = image->GetLastStatus();

	if (s != Ok)
	{
		delete image;
		image = new Bitmap(800,600,PixelFormat24bppRGB);
		width = image->GetWidth();
		height = image->GetHeight();
		pixelFormat = 0;
		return false;
	}

	image->GetRawFormat(&guid);

	if (guid == ImageFormatJPEG)
	{
		GetEncoderClsid(L"image/jpeg",&clsId);

		UINT size = image->GetPropertyItemSize(PropertyTagOrientation);

		if (size)
		{
			PropertyItem* pItem = (PropertyItem*)malloc(size);

			Status s = image->GetPropertyItem(PropertyTagOrientation , size, pItem);

			if (*(short*)(pItem->value) == 6)
			{
				image->RotateFlip(Rotate90FlipNone);
			}
			if (*(short*)(pItem->value) == 8)
			{
				image->RotateFlip(Rotate90FlipXY);
			}
		
			*(short*)(pItem->value) = 1;

			image->SetPropertyItem(pItem);

			free(pItem);
			pItem = 0;
		}
	}
	if (guid == ImageFormatBMP)
	{
		GetEncoderClsid(L"image/bmp",&clsId);
	}
	if (guid == ImageFormatTIFF)
	{
		GetEncoderClsid(L"image/tiff",&clsId);
	}
	if (guid == ImageFormatGIF)
	{
		GetEncoderClsid(L"image/gif",&clsId);
	}
	if (guid == ImageFormatPNG)
	{
		GetEncoderClsid(L"image/png",&clsId);
	}
	if (guid == ImageFormatEXIF)
	{
		GetEncoderClsid(L"image/exif",&clsId);
	}
	if (guid == ImageFormatEMF)
	{
		GetEncoderClsid(L"image/emf",&clsId);
	}
	if (guid == ImageFormatWMF)
	{
		GetEncoderClsid(L"image/wmf",&clsId);
	}

	width = image->GetWidth();
	height = image->GetHeight();
	pixelFormat = image->GetPixelFormat();

	return true;
}

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
   UINT  num = 0;          // number of image encoders
   UINT  size = 0;         // size of the image encoder array in bytes

   ImageCodecInfo* pImageCodecInfo = NULL;

   GetImageEncodersSize(&num, &size);
   if(size == 0)
      return -1;  // Failure

   pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
   if(pImageCodecInfo == NULL)
      return -1;  // Failure

   GetImageEncoders(num, size, pImageCodecInfo);

   for(UINT j = 0; j < num; ++j)
   {
      if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
      {
         *pClsid = pImageCodecInfo[j].Clsid;
         free(pImageCodecInfo);
         return j;  // Success
      }    
   }

   free(pImageCodecInfo);
   return -1;  // Failure
}

void AdjustWindowToFitImage()
{
	RECT window = {0, 0, width, height};
	RECT rect = window;

	long style = GetWindowLong(hWnd,GWL_STYLE);
	long exStyle = GetWindowLong(hWnd,GWL_EXSTYLE);
	AdjustWindowRectEx(&window, style, false, exStyle);
	window.right = (window.right - window.left);
	window.left = 0;
	window.bottom = (window.bottom - window.top);
	window.top = 0;

	window.right -= rect.right;
	window.bottom -= rect.bottom;
	window.bottom += statusArea.bottom + 1;

	cScaleFactor = 1;
	xOffset = 0;
	yOffset = 0;
	scaleToFit = true;

	if (((workArea.right - workArea.left) - window.right) < rect.right || 
		((workArea.bottom - workArea.top) - window.bottom) < rect.bottom)
	{
		float aw = (float)((workArea.right - workArea.left) - window.right) / (float)rect.right;
		float ah = (float)((workArea.bottom - workArea.top) - window.bottom) / (float)rect.bottom;

		if (aw > ah)
			aw = ah;

		cScaleFactor = aw;

		rect.right = (DWORD)((float)rect.right * aw);
		rect.bottom = (DWORD)((float)rect.bottom * aw);
	}

	rect.right += window.right;
	rect.bottom += window.bottom;

	w = rect.right;
	h = rect.bottom;

	MoveWindow(hWnd, workArea.left + ((workArea.right - workArea.left) - rect.right)/2,
					 workArea.top + ((workArea.bottom -workArea.top) - rect.bottom)/2,
														rect.right,rect.bottom,true);
}

void UpdateScrollInfo()
{
	RECT rec;
	SCROLLINFO scrInfo;

	GetClientRect(hWnd,&rec);

	rec.bottom -= statusArea.bottom + 1;

	if (scaleToFit)
	{
		cScaleFactor = ((float)rec.right / (float)width);
		if (cScaleFactor > (((float)rec.bottom) / (float)height))
			cScaleFactor = ((float)rec.bottom) / (float)height;
			
		xOffset = 0;
		yOffset = 0;

		if (cScaleFactor > 1)
		{
			cScaleFactor = 1;
		}
	}

	int w2 = w;
	int h2 = h;
	w = (int)((float)width * cScaleFactor);
	h = (int)((float)height * cScaleFactor);

	x = (rec.right - w) >> 1;
	y = (rec.bottom - h) >> 1;

	float sf = (float)w / (float)w2;
	xOffset = (int)(xOffset * sf);
	sf = (float)h / (float)h2;
	yOffset = (int)(yOffset * sf);

	if (w <= rec.right)
	{
		xOffset = 0;
	}
	else
	{
		if (x + xOffset > 0)
		{
			xOffset = -x; 
		}
		if (x + xOffset + w < rec.right)
		{
			xOffset = -x - w + rec.right;
		}
	}

	if (h <= rec.bottom)
	{
		yOffset = 0;
	}
	else
	{
		if (y + yOffset + h < rec.bottom)
		{
			yOffset = -y - h + rec.bottom;
		}
		if (y + yOffset > 0)
		{
			yOffset = -y; 
		}
	}

	scrInfo.cbSize = sizeof(SCROLLINFO);
	scrInfo.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
	scrInfo.nMax = x + w - 1;
	scrInfo.nMin = x;
	scrInfo.nPage = rec.right;
	scrInfo.nPos = -xOffset;

	SetScrollInfo(hWnd,SB_HORZ,&scrInfo,true);

	scrInfo.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
	scrInfo.nMax = y + h - 1;
	scrInfo.nMin = y;
	scrInfo.nPage = rec.bottom;
	scrInfo.nPos = -yOffset;

	SetScrollInfo(hWnd,SB_VERT,&scrInfo,true);
}

void DrawWindow(RECT * rect)
{
	WCHAR * strPF = L"";

	Rect clipRect(rect->left,rect->top,(rect->right - rect->left) + 1,(rect->bottom - rect->top) + 1);

	graphics->SetClip(clipRect);

	graphics->SetCompositingMode(CompositingModeSourceCopy);

	if (currentBilinearMode == MF_UNCHECKED)
		graphics->SetInterpolationMode(InterpolationModeBilinear);
	else
		graphics->SetInterpolationMode(InterpolationModeNearestNeighbor);

	graphics->SetPixelOffsetMode(PixelOffsetModeHalf);

	if ((rComponent == MF_UNCHECKED) &
		(gComponent == MF_UNCHECKED) &
		(bComponent == MF_UNCHECKED) &
		(aComponent == MF_CHECKED) &
		(grayScale == MF_CHECKED))
	{
		graphics->DrawImage(image, x + xOffset, y + yOffset, w, h);
	}
	else
	{
		ColorMatrix colorMatrix = { (float)(rComponent == 0), 0.0f, 0.0f, 0.0f, 0.0f,
									0.0f, (float)(gComponent == 0), 0.0f, 0.0f, 0.0f,
									0.0f, 0.0f, (float)(bComponent == 0), 0.0f, 0.0f,
									0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
									0.0f, 0.0f, 0.0f, 0.0f, 1.0f};

		if (grayScale == MF_UNCHECKED)
		{
		colorMatrix.m[0][0] *= 0.2125f;
		colorMatrix.m[0][1]  = colorMatrix.m[0][0];
		colorMatrix.m[0][2]  = colorMatrix.m[0][0];

		colorMatrix.m[1][1] *= 0.7154f;
		colorMatrix.m[1][0]  = colorMatrix.m[1][1];
		colorMatrix.m[1][2]  = colorMatrix.m[1][1];

		colorMatrix.m[2][2] *= 0.0721f;
		colorMatrix.m[2][0]  = colorMatrix.m[2][2];
		colorMatrix.m[2][1]  = colorMatrix.m[2][2];
		}

		if (aComponent == MF_UNCHECKED)
		{
			colorMatrix.m[0][0] = 0;
			colorMatrix.m[0][1] = 0;
			colorMatrix.m[0][2] = 0;
			colorMatrix.m[1][1] = 0;
			colorMatrix.m[1][0] = 0;
			colorMatrix.m[1][2] = 0;
			colorMatrix.m[2][2] = 0;
			colorMatrix.m[2][0] = 0;
			colorMatrix.m[2][1] = 0;
			colorMatrix.m[3][0] = colorMatrix.m[3][3];
			colorMatrix.m[3][1] = colorMatrix.m[3][3];
			colorMatrix.m[3][2] = colorMatrix.m[3][3];
		}

		ImageAttributes imageAttributes;   
		imageAttributes.SetColorMatrix(&colorMatrix, ColorMatrixFlagsDefault, ColorAdjustTypeBitmap);

		graphics->DrawImage(image, Rect(x + xOffset, y + yOffset, w, h), 0, 0, width, height ,UnitPixel,&imageAttributes,0,0);
	}

	RECT clearRect;
	GetClientRect(hWnd,&clearRect);

	clipRect.X = clearRect.left;
	clipRect.Y = clearRect.top;
	clipRect.Width = clearRect.right;
	clipRect.Height = clearRect.bottom  - (statusArea.bottom + 1);

	graphics->SetClip(clipRect,CombineModeReplace);

	clipRect.X = x + xOffset;
	clipRect.Y = y + yOffset;
	clipRect.Width = w;
	clipRect.Height = h;

	graphics->SetClip(clipRect, CombineModeExclude);

	graphics->Clear(options.borderColor);


	switch(pixelFormat)
	{
	case 0:
		strPF = szUnknownFileFormat;
		break;
	case PixelFormat4bppIndexed:
		strPF = sz4Indexed;
		break;
	case PixelFormat8bppIndexed:
		strPF = sz8Indexed;
		break;
	case PixelFormat16bppRGB565:
	case PixelFormat16bppRGB555: 
		strPF = sz16RGB;
		break;
	case PixelFormat24bppRGB:
		strPF = sz24RGB;
		break;
	case PixelFormat32bppRGB:
		strPF = sz32RGB;
		break;
	case PixelFormat32bppARGB:
		strPF = sz32ARGB;
		break;
	case PixelFormat48bppRGB:
		strPF = sz48RGB;
		break;
	case PixelFormat64bppARGB:
		strPF = sz64ARGB;
		break;
	default:
		strPF = L"";
		break;
	}

	SetWindowText(hWnd,cFileNode->fileName);

	if (pixelFormat)
		swprintf_s(stringBuffer, sizeof(stringBuffer) >> 1, L"%s %dx%d %s", szFileInfo,width,height,strPF);
	else
		swprintf_s(stringBuffer, sizeof(stringBuffer) >> 1, L"%s %s", szFileInfo,strPF);

	SendMessage (hWndStatus, SB_SETTEXT, 0, (LPARAM)stringBuffer);

	swprintf_s(stringBuffer, sizeof(stringBuffer) >> 1, L"%s %d%s", szZoomed, (int)(cScaleFactor * 100), L"%");

	SendMessage (hWndStatus, SB_SETTEXT, 1, (LPARAM)stringBuffer);

	swprintf_s(stringBuffer, sizeof(stringBuffer) >> 1, L"%s %d%s%d", szFiles, cFileNode->fileCount, L"/", filesInDirectory);

	SendMessage (hWndStatus, SB_SETTEXT, 2, (LPARAM)stringBuffer);

	float size = (float)((float)width * (float)height * 4) / (1024 * 1024);
	float cSize = (float)cFileNode->size / (1024 * 1024);

	swprintf_s(stringBuffer, sizeof(stringBuffer) >> 1, L"%s %.2f %s / %.2f %s", szSize, cSize, szMb, size, szMb);

	SendMessage (hWndStatus, SB_SETTEXT, 3, (LPARAM)stringBuffer);
}
