#include "GetPhotosDialog.h"
#include "ShellHelper.h"

IWiaItem ** ppItems = NULL;
LONG count;
WCHAR path[MAX_PATH + 1];
WCHAR fullPath[MAX_PATH + 1];
Options * options = NULL;

HRESULT GetPhotos(HINSTANCE hInst, HWND hWnd, Options * _options, GETPHOTOS * getPhotos)
{	
	HRESULT hr;
	ppItems = NULL;
	count = NULL;
	options = _options;
	IWiaDevMgr * wiadevmgr = NULL;
	IWiaItem * wiaItem = NULL;

	hr = CreateWiaDeviceManager(&wiadevmgr);
	if (S_OK == hr)
	{
		hr = SelectCameraDeviceDlg(hWnd, wiadevmgr, &wiaItem);
		if (S_OK == hr)
		{
			hr = GetImageDlg(hWnd, wiaItem, &ppItems, &count);
			if (S_OK == hr)
			{
				hr = (HRESULT)DialogBox(hInst, (LPCTSTR)IDD_GET_PHOTOS, hWnd, (DLGPROC)&GetPhotosDlgProc);

				getPhotos->gotoPath = false;
				wcscpy_s(getPhotos->path, MAX_PATH, L"");

				if (S_OK == hr)
				{
					if (options->checkOptionsGetPhotos[EGPO_GO_TO_DIRECTORY])
					{
						getPhotos->gotoPath = true;
						wcscpy_s(getPhotos->path, MAX_PATH, path);
					}
				}

				for (int i = 0; i < count; ++i)
				{
					ppItems[i]->Release();
				}
			}
			wiaItem->Release();
		}
		wiadevmgr->Release();
	}

	return hr;
}

HRESULT GetFileName(IWiaItem * pWiaItem, WCHAR * path, WCHAR * fullPath, eStoreFileName storeFileNameCode)
{
	WCHAR buffer[MAX_PATH + 1];
	IWiaPropertyStorage *pWiaPropertyStorage = NULL;
	HRESULT hr = pWiaItem->QueryInterface( IID_IWiaPropertyStorage, (void**)&pWiaPropertyStorage );
	if (SUCCEEDED(hr))
	{
		PROPSPEC PropSpec[1] = {0};
		PROPVARIANT PropVariant[1] = {0};

		wcscpy_s(fullPath, MAX_PATH, path);
		if (storeFileNameCode == ESFN_KEEP_FILENAME)
		{
			PropSpec[0].ulKind = PRSPEC_PROPID;
			PropSpec[0].propid = WIA_IPA_ITEM_NAME;
			hr = pWiaPropertyStorage->ReadMultiple(1, PropSpec, PropVariant);
			if (S_OK == hr)
			{
				PathAppend(fullPath, PropVariant[0].pwszVal);
			}
		}
		else
		{
			SYSTEMTIME time;
			SYSTEMTIME * pTime;
			PropSpec[0].ulKind = PRSPEC_PROPID;
			PropSpec[0].propid = WIA_IPA_ITEM_TIME;
			hr = pWiaPropertyStorage->ReadMultiple(1, PropSpec, PropVariant);
			if (S_OK == hr)
			{
				pTime = *((SYSTEMTIME **)(((unsigned int*)&PropVariant[0].date)+1));
			}
			else
			{
				GetSystemTime(&time);
				SystemTimeToTzSpecificLocalTime(NULL, &time, &time);
				pTime = &time;
			}

			swprintf_s(buffer,  MAX_PATH, L"%02d.%02d.%d %02d-%02d-%02d", 
						pTime->wDay, pTime->wMonth, pTime->wYear, pTime->wHour, pTime->wMinute, pTime->wSecond);
			PathAppend(fullPath, buffer);
			hr = S_OK;
		}

		pWiaPropertyStorage->Release();
		pWiaPropertyStorage = NULL;
	}

	return hr;
}

LRESULT CALLBACK GetPhotosDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HWND hCheck[EGPO_COUNT] = {0};
	static HWND hProgressBar;
	static HWND hEditLocation;
	static HWND hEditSubFolder;

	switch (message)
	{
	case WM_INITDIALOG:
		{
			hProgressBar = GetDlgItem(hDlg, IDC_PROGRESS_PROGRESS);
			hEditLocation = GetDlgItem(hDlg, IDC_EDIT_LOCATION);
			hEditSubFolder = GetDlgItem(hDlg, IDC_EDIT_SUBFOLDER);

			for (int i = 0; i < EGPO_COUNT; ++i)
			{
				hCheck[i] = GetDlgItem(hDlg, IDC_CHECK_DELETE_AFTER + i);
				SendMessage(hCheck[i], BM_SETCHECK, options->checkOptionsGetPhotos[i], 0);
			}
			SendMessage(hProgressBar, PBM_SETMARQUEE, false, 1000);
			SendMessage(hProgressBar, PBM_SETRANGE32, 0, count);
			SendMessage(hProgressBar, PBM_SETSTEP, 1, 0);
			SetDlgItemText(hDlg, IDC_EDIT_LOCATION, options->currentCaptureFolder);
			SetDlgItemText(hDlg, IDC_EDIT_SUBFOLDER, options->currentCaptureSubFolder);
			return TRUE;
		}
		break;

	case WM_COMMAND:
		{
			if (LOWORD(wParam) == IDC_BUTTON_BROWSE)
			{
				if (BrowseFolderDialog(hDlg, path))
					SetDlgItemText(hDlg, IDC_EDIT_LOCATION, path);
			}
			if (LOWORD(wParam) == IDC_BUTTON_GET_DATE)
			{
				SYSTEMTIME sysTime;
				GetSystemTime(&sysTime);
				swprintf_s(path,  MAX_PATH, L"%02d.%02d.%d", 
							sysTime.wDay, sysTime.wMonth, sysTime.wYear);
				SetDlgItemText(hDlg, IDC_EDIT_SUBFOLDER, path);
			}
			if (LOWORD(wParam) == IDOK)
			{
				HRESULT hr;

				for (int i = 0; i < EGPO_COUNT; ++i)
				{
					options->checkOptionsGetPhotos[i] = SendMessage(hCheck[i], BM_GETSTATE, 0, 0) & BST_CHECKED;
				}

				GetDlgItemText(hDlg, IDC_EDIT_LOCATION, options->currentCaptureFolder, MAX_PATH);
				GetDlgItemText(hDlg, IDC_EDIT_SUBFOLDER, options->currentCaptureSubFolder, MAX_PATH);
				wcscpy_s(path, sizeof(path) >> 1, options->currentCaptureFolder);
				PathAppend(path, options->currentCaptureSubFolder);
				SHCreateDirectoryEx(NULL, path, NULL);
				for (LONG i = 0; i < count;i++)
				{
					eStoreFileName code;

					if (options->checkOptionsGetPhotos[EGPO_KEEP_FILES_NAMES])
						code = ESFN_KEEP_FILENAME;
					else
						code = ESFN_DATE_FILENAME;

					SendMessage(hProgressBar, PBM_STEPIT, NULL, NULL);

					hr = GetFileName(ppItems[i], path, fullPath, code);
					if (S_OK == hr)
					{
						hr = TransferWiaItem(ppItems[i], fullPath);
					}
				}

				if (options->checkOptionsGetPhotos[EGPO_DELETE_FILES_AFTER])
				{
					for (LONG i = 0; i < count;i++)
					{
						ppItems[i]->DeleteItem(0);
					}
				}

				EndDialog(hDlg, hr);
			}
			if (LOWORD(wParam) == IDCANCEL) 
			{
				EndDialog(hDlg, IDCANCEL);
			}
		}
		break;

	case WM_DESTROY:
		{

		}
		break;
	}
	return FALSE;
}