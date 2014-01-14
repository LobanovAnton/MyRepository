#ifndef __IC__
#define __IC__

#include "wia.h"
#include "sti.h"
#include "Shlwapi.h"
#include "wchar.h"

HRESULT CreateWiaDeviceManager( IWiaDevMgr **ppWiaDevMgr );

HRESULT SelectCameraDeviceDlg(HWND hwndParent, IWiaDevMgr * pWiaDevMgr, IWiaItem ** ppWiaItem);

HRESULT GetImageDlg(HWND hwndParent, IWiaItem * pItemRoot, IWiaItem *** pppItems, LONG * count);

HRESULT TransferWiaItem( IWiaItem *pWiaItem, WCHAR * fullPath);

#endif // __IC__