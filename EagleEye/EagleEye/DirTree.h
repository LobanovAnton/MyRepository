#ifndef __DR__
#define __DR__

#include <shlobj.h>
#include <Shlwapi.h>

struct TreeItem
{
	IShellFolder * isfParent;
	LPITEMIDLIST pidl;
	LPITEMIDLIST pidlFull;
	WCHAR path[MAX_PATH];
	TreeItem * nextTreeItem;
};

void Expand(HWND hwndTree, HTREEITEM treeItem);

void InsertItem(HWND hwndTree, HTREEITEM treeItem, TreeItem * pTreeItem, WCHAR * text);

void ExpandingForPath(HWND hwndTree, WCHAR * path, HTREEITEM treeItem);

void ReleaseDirTree();

#endif //__DR__