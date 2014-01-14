#ifndef __SA__
#define __SA__

#include <shlobj.h>
#include <Shlwapi.h>

IContextMenu * CreateContextMenu(HWND hwnd, IShellFolder * isf, UINT count, LPCITEMIDLIST * pidl);

int DoContextMenuThing(HWND hwnd, IContextMenu * contextMenu, POINT * pt);

BOOL DeleteFileDialog(HWND hWnd, WCHAR * fullPath);

BOOL BrowseFolderDialog(HWND hDlg, WCHAR * fullPath);

IShellFolder * CreateIShellFolder(WCHAR * path);

BOOL CopyingFileDialog(HWND hWnd, WCHAR * fullPathFrom, WCHAR * pathTo);

#endif //__SA__