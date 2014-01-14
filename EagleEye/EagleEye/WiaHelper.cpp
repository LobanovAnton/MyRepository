#include "WiaHelper.h"

HRESULT CreateWiaDeviceManager( IWiaDevMgr **ppWiaDevMgr )
{
	if (NULL == ppWiaDevMgr)
	{
		return E_INVALIDARG;
	}

	*ppWiaDevMgr = NULL;

	HRESULT hr = CoCreateInstance( CLSID_WiaDevMgr, NULL, CLSCTX_LOCAL_SERVER, IID_IWiaDevMgr, (void**)ppWiaDevMgr );

	return hr;
}

HRESULT SelectCameraDeviceDlg(HWND hwndParent, IWiaDevMgr * pWiaDevMgr, IWiaItem ** ppWiaItem)
{
	return 
	pWiaDevMgr->SelectDeviceDlg(hwndParent, 
								StiDeviceTypeDefault, 
								WIA_SELECT_DEVICE_NODEFAULT, 
								NULL, 
								ppWiaItem);
}

HRESULT GetImageDlg(HWND hwndParent, IWiaItem * pItemRoot, IWiaItem *** pppItems, LONG * count)
{
	return		pItemRoot->DeviceDlg(hwndParent, 
									  WIA_DEVICE_DIALOG_USE_COMMON_UI, 
									  WIA_INTENT_NONE, 
									  count, 
									  pppItems);
}

HRESULT TransferWiaItem(IWiaItem *pWiaItem, WCHAR * fullPath)
{
	if (NULL == pWiaItem)
	{
		return E_INVALIDARG;
	}

	IWiaPropertyStorage *pWiaPropertyStorage = NULL;
	HRESULT hr = pWiaItem->QueryInterface( IID_IWiaPropertyStorage, (void**)&pWiaPropertyStorage );
	if (SUCCEEDED(hr))
	{
		PROPSPEC PropSpec[3] = {0};
		PROPVARIANT PropVariant[3] = {0};

		GUID guidOutputFormat = WiaImgFmt_JPEG;

		PropSpec[0].ulKind = PRSPEC_PROPID;
		PropSpec[0].propid = WIA_IPA_FORMAT;
		PropSpec[1].ulKind = PRSPEC_PROPID;
		PropSpec[1].propid = WIA_IPA_TYMED;

		PropVariant[0].vt = VT_CLSID;
		PropVariant[0].puuid = &guidOutputFormat;
		PropVariant[1].vt = VT_I4;
		PropVariant[1].lVal = TYMED_FILE;

		hr = pWiaPropertyStorage->WriteMultiple( 2, PropSpec, PropVariant, WIA_IPA_FIRST );
		if (!SUCCEEDED(hr))
		{	
			guidOutputFormat = WiaImgFmt_BMP;
			hr = pWiaPropertyStorage->WriteMultiple( 2, PropSpec, PropVariant, WIA_IPA_FIRST );
			if (!SUCCEEDED(hr))
			{
				pWiaPropertyStorage->Release();
				pWiaPropertyStorage = NULL;
				return hr;
			}
			else
			{
				wcscat_s(fullPath, MAX_PATH, L".bmp");
			}
		}
		else
		{
			wcscat_s(fullPath, MAX_PATH, L".jpg");
		}

		IWiaDataTransfer *pWiaDataTransfer = NULL;
		hr = pWiaItem->QueryInterface( IID_IWiaDataTransfer, (void**)&pWiaDataTransfer );
		if (SUCCEEDED(hr))
		{

			STGMEDIUM stgMedium = {0};
			stgMedium.tymed = TYMED_FILE;
			stgMedium.lpszFileName = fullPath;
			hr = pWiaDataTransfer->idtGetData( &stgMedium, NULL);

			pWiaDataTransfer->Release();
			pWiaDataTransfer = NULL;
		}

		pWiaPropertyStorage->Release();
		pWiaPropertyStorage = NULL;
	}

	return hr;
}

