#ifndef __EP__
#define __EP__

#include "math.h"
#include <windows.h>
#include <tchar.h>
#include <gdiplus.h>
using namespace Gdiplus;

#define PropertyTagExifExposureMode 0xA402
#define PropertyTagExifWhiteBalance 0xA403
#define PropertyTagExifDigitalZoomRatio 0xA404
#define PropertyTagExifSceneCapturedType 0xA406

enum eCanonPropertyId
{
	NumberOfFields = 0x00010000,
	MacroMode,
	SelfTimer,
	QualityCompressionRate,
	FlashMode,
	DriveMode,
	Fieldunknown1,
	FocusMode,
	Fieldunknown2,
	Fieldunknown3,
	ImageSize,
	EasyShoot,
	DigitalZoom,
	Contrast,
	Saturation,
	Sharpness,
	ISOSpeed,
	MeterMode,
	FocusType,
	AFPoint,
	ExposureProgram,
	Fieldunknown4,
	Fieldunknown5,
	FocalLengthLong,
	FocalLengthShort,
	FocalUnitsPermm,
	Fieldunknown6,
	Fieldunknown7,
	FlashActivity,
	FlashDetails,
	Fieldunknown8,
	LongShutterMode,
	Focus,
	Fieldunknown9,
	Fieldunknown10,
	Fieldunknown11,
	ZoomedResolution,
	ZoomedResolutionBase,
	Fieldunknown12,
	Fieldunknown13,
	CanonPhotoEffect,
	Fieldunknown14,
	ColorTone,
	Fieldunknown15,
	Fieldunknown16,
	Fieldunknown17,

	WhiteBalance = 0x00040007,
	Distance = 0x00040013,
	Imagetype = 0x00060000,
	FirmWare = 0x00070000,
	ImageNumber = 0x00080000
};

WCHAR * GetExifPropertyString(WCHAR * fileName, PROPID propId);

bool ReadCanonMakerNotes(WCHAR * fileName);
void ReleaseData();

WCHAR * GetExifCanonMakerNotesString(eCanonPropertyId canonPropId);


#endif //__EP__