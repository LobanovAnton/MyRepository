#include "ShellHelper.h"

IContextMenu * CreateContextMenu(HWND hwnd, IShellFolder * isf, UINT count, LPCITEMIDLIST * pidl)
{
	IContextMenu * contextMenu = 0;
	IContextMenu2 * contextMenu2 = 0;

	if (isf && count && pidl)
	{
		isf->GetUIObjectOf(hwnd, count, (LPCITEMIDLIST*)pidl, IID_IContextMenu, NULL, (void**)&contextMenu);
		if (contextMenu)
		{
			contextMenu->QueryInterface(IID_IContextMenu2, (void**)&contextMenu2);
			if (contextMenu2)
			{
				contextMenu->Release();
				return contextMenu2;
			}
			return contextMenu;
		}
	}
	return NULL;
}

int DoContextMenuThing(HWND hwnd, IContextMenu * contextMenu, POINT * pt)
{
	WCHAR buffer[MAX_PATH + 1];

	if (contextMenu && pt)
	{
		HMENU menu = CreatePopupMenu();
		if (menu)
		{
			contextMenu->QueryContextMenu(menu, 0, 1, 0x7fff, CMF_NORMAL);

			int idCmd = TrackPopupMenu(menu, 
				TPM_LEFTALIGN | TPM_RETURNCMD | TPM_RIGHTBUTTON, 
				pt->x, 
				pt->y,
				0,
				hwnd,
				NULL);

			if (idCmd)
			{
				CMINVOKECOMMANDINFO cmi;
				contextMenu->GetCommandString(idCmd - 1, GCS_VERBW, NULL, (LPSTR)buffer, MAX_PATH);
				cmi.cbSize = sizeof(CMINVOKECOMMANDINFO);
				cmi.fMask  = 0;
				cmi.hwnd   = hwnd;
				cmi.lpVerb = MAKEINTRESOURCEA(idCmd - 1);
				cmi.lpParameters = NULL;
				cmi.lpDirectory  = NULL;
				cmi.nShow        = SW_SHOWNORMAL;
				cmi.dwHotKey     = 0;
				cmi.hIcon        = NULL;
				HRESULT hr = contextMenu->InvokeCommand(&cmi);
				if (hr == S_OK && 
					!wcscmp(buffer, L"delete"))
				{
					DestroyMenu(menu);
					return 1;
				}
			}
			DestroyMenu(menu);
		}
	}
	return 0;
}

BOOL CopyingFileDialog(HWND hWnd, WCHAR * fullPathFrom, WCHAR * pathTo)
{
	SHFILEOPSTRUCT fo;
	ZeroMemory((void*)&fo, sizeof(fo));
	fo.hwnd = hWnd;
	fo.pTo = pathTo;
	fo.hNameMappings = NULL;
	fo.fAnyOperationsAborted = false;
	fo.lpszProgressTitle = L"Copying files";
	fo.wFunc = FO_COPY;
	fo.pFrom = fullPathFrom;
	fo.fFlags = FOF_NOERRORUI | FOF_SIMPLEPROGRESS;

	fullPathFrom[wcslen(fullPathFrom) + 1] = 0;

	return (!SHFileOperation(&fo) && !fo.fAnyOperationsAborted);
}

BOOL DeleteFileDialog(HWND hWnd, WCHAR * fullPath)
{
	SHFILEOPSTRUCT fo;
	ZeroMemory((void*)&fo, sizeof(fo));
	fo.hwnd = hWnd;
	fo.pTo = NULL;
	fo.hNameMappings = NULL;
	fo.fAnyOperationsAborted = false;
	fo.lpszProgressTitle = L"Deleting files";
	fo.wFunc = FO_DELETE;
	fo.pFrom = fullPath;
	fo.fFlags = FOF_NOERRORUI | FOF_SIMPLEPROGRESS;

	fullPath[wcslen(fullPath) + 1] = 0;

	return (!SHFileOperation(&fo) && !fo.fAnyOperationsAborted);
}

BOOL BrowseFolderDialog(HWND hDlg, WCHAR * fullPath)
{
	WCHAR displayName[MAX_PATH];
	LPITEMIDLIST pidl;
	BROWSEINFO bi;
	ZeroMemory((void*)&bi, sizeof(bi));
	bi.hwndOwner = hDlg;
	bi.pidlRoot = 0;
	bi.lpszTitle = L"Select folder";
	bi.pszDisplayName = displayName;
	bi.ulFlags = BIF_UAHINT | BIF_USENEWUI;
	bi.lpfn = NULL;
	bi.lParam = NULL;
	bi.iImage = NULL;
	pidl = SHBrowseForFolder(&bi);
	if (pidl)
	{
		SHGetPathFromIDList(pidl, fullPath);
		ILFree(pidl);
		return true;
	}
	return false;
}

IShellFolder * CreateIShellFolder(WCHAR * path)
{
	IShellFolder * isfRoot = NULL;
	IShellFolder * isf = NULL;
	SHGetDesktopFolder(&isfRoot);
	if (isfRoot)
	{
		LPITEMIDLIST pidl = ILCreateFromPath(path);
		isfRoot->BindToObject(pidl, NULL, IID_IShellFolder, (void**)&isf);
		ILFree(pidl);
		isfRoot->Release();
	}
	return isf;
}