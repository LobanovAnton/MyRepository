#include "DirTree.h"

TreeItem * startTreeItem = NULL;
TreeItem * curTreeItem = NULL;

void Expand(HWND hwndTree, HTREEITEM treeItem)
{
	IShellFolder * isf = NULL;
	LPITEMIDLIST pidl;
	LPITEMIDLIST pidlFull;
	STRRET strRet;
	if (!treeItem)
	{
		SHGetDesktopFolder(&isf);
		pidl = NULL;
		pidlFull = NULL;
		SHFILEINFO shFileInfo;
		HIMAGELIST hImageList = (HIMAGELIST)SHGetFileInfo((LPCWSTR)L"C:\\", 
											0,
											&shFileInfo, 
											sizeof(shFileInfo), 
											SHGFI_SYSICONINDEX | SHGFI_SMALLICON);
		TreeView_SetImageList(hwndTree, hImageList, TVSIL_NORMAL);
		SHGetFolderLocation(hwndTree, CSIDL_DESKTOP, NULL, NULL, &pidl);
		isf->GetDisplayNameOf(pidl, SHGDN_NORMAL, &strRet);
		TreeItem  * pTreeItem = new TreeItem;
		pTreeItem->nextTreeItem = NULL;
		startTreeItem = pTreeItem;
		curTreeItem = pTreeItem;
		pTreeItem->isfParent = isf;
		isf->AddRef();
		pTreeItem->pidlFull = pidl;
		pTreeItem->pidl = NULL;
		SHGetPathFromIDList(pTreeItem->pidlFull, pTreeItem->path);
		InsertItem(hwndTree, treeItem, pTreeItem, strRet.pOleStr);
		isf->Release();
		return;
	}
	else
	{
		TVITEMEX tvItem;
		tvItem.mask = TVIF_PARAM | TVIF_CHILDREN;
		tvItem.hItem = treeItem;
		TreeView_GetItem(hwndTree, &tvItem);
		TreeItem * pTreeItem = (TreeItem*)tvItem.lParam;
		pidl = pTreeItem->pidl;
		pidlFull = pTreeItem->pidlFull;
		if (pidl)
			pTreeItem->isfParent->BindToObject(pidl, NULL, IID_IShellFolder, (void**)&isf);
		else
		{
			isf = pTreeItem->isfParent;
			pTreeItem->isfParent->AddRef();
		}

		if (isf)
		{
			IEnumIDList * eidList;
			HRESULT hr = isf->EnumObjects(hwndTree, SHCONTF_FOLDERS, &eidList);
			if (S_OK == hr)
			{
				LPITEMIDLIST pidl2;
				ULONG preFetch;
				STRRET strRet;
				bool hasSubFolder = false;

				while ( S_FALSE != eidList->Next(1, &pidl2, &preFetch) && preFetch)
				{
					isf->GetDisplayNameOf(pidl2, SHGDN_NORMAL, &strRet);
					TreeItem  * pTreeItem = new TreeItem;
					pTreeItem->nextTreeItem = NULL;
					curTreeItem->nextTreeItem = pTreeItem;
					curTreeItem = pTreeItem;

					pTreeItem->isfParent = isf;
					isf->AddRef();
					pTreeItem->pidlFull = ILCombine(pidlFull, pidl2);
					pTreeItem->pidl = pidl2;
					SHGetPathFromIDList(pTreeItem->pidlFull, pTreeItem->path);
					InsertItem(hwndTree, treeItem, pTreeItem, strRet.pOleStr);
					hasSubFolder = true;
				}
				if (!hasSubFolder)
				{
					TVITEMEX tvItem;
					tvItem.mask = TVIF_CHILDREN;
					tvItem.cChildren = 0;
					tvItem.hItem = treeItem;
					TreeView_SetItem(hwndTree, &tvItem);
				}

				eidList->Release();
			}
			isf->Release();
		}
	}
}

void InsertItem(HWND hwndTree, HTREEITEM treeItem, TreeItem * pTreeItem, WCHAR * text)
{
	TVINSERTSTRUCT insStruct;
	TVITEMEX tvItem;
	SHFILEINFO shFileInfo;

	SHGetFileInfo((LPCWSTR)pTreeItem->pidlFull, NULL, &shFileInfo, sizeof(shFileInfo), SHGFI_PIDL | SHGFI_SYSICONINDEX | SHGFI_SMALLICON);

	tvItem.mask = TVIF_TEXT | TVIF_PARAM | TVIF_HANDLE | TVIF_CHILDREN | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	tvItem.iImage = shFileInfo.iIcon;
	tvItem.iSelectedImage = shFileInfo.iIcon;
	tvItem.hItem = treeItem;
	tvItem.pszText = text;
	tvItem.cChildren = 1;
	tvItem.lParam = (LPARAM)pTreeItem;

	insStruct.hParent = treeItem;
	insStruct.hInsertAfter = TVI_LAST;
	insStruct.itemex = tvItem;

	treeItem = TreeView_InsertItem(hwndTree,&insStruct);
}

void ExpandingForPath(HWND hwndTree, WCHAR * path, HTREEITEM treeItem)
{
	LPITEMIDLIST pidl2;
	LPITEMIDLIST pidl = ILCreateFromPath(path);
	HTREEITEM item = TreeView_GetChild(hwndTree, treeItem);

	TVITEMEX tvItem;
	tvItem.mask = TVIF_PARAM | TVIF_CHILDREN;

	do
	{
		tvItem.hItem = item;
		TreeView_GetItem(hwndTree, &tvItem);
		TreeItem * pTreeItem = (TreeItem*)tvItem.lParam;
		pidl2 = ILFindChild(pTreeItem->pidlFull, pidl);
		if (pidl2)
		{
			if (ILIsEqual(pidl, pTreeItem->pidlFull))
			{
				TreeView_Select(hwndTree, item, TVGN_CARET);
				break;
			}

			TreeView_Expand(hwndTree, item, TVE_EXPAND);
			ExpandingForPath(hwndTree, path, item);
		}
		item = TreeView_GetNextSibling(hwndTree, item);
	}while(item);

	ILFree(pidl);
}

void ReleaseDirTree()
{
	if (startTreeItem)
	{
		TreeItem * treeItem;
		treeItem = startTreeItem;
		do 
		{
			if (treeItem->pidl)
				ILFree(treeItem->pidl);
			if (treeItem->pidlFull)
				ILFree(treeItem->pidlFull);
			treeItem->isfParent->Release();
			TreeItem * ti = treeItem->nextTreeItem;
			delete treeItem;
			treeItem = ti;
		}while(treeItem);
	}

	curTreeItem = NULL;
	startTreeItem = NULL;
}