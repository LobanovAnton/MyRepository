#include "ExifProperty.h"

struct CanonProperty
{
	unsigned short tag;
	unsigned short type;
	unsigned long  count;
	unsigned long  offset;
};

struct CanonTag
{
	unsigned short tag;
	unsigned short type;
	unsigned long  count;
	void * data;
};

struct exifItem
{
	unsigned long tag;
	unsigned short value;
	WCHAR * string;
};

const exifItem exifItemArray[] = {
											PropertyTagResolutionUnit,2,L"inch",
											PropertyTagResolutionUnit,3,L"centimeter",
											PropertyTagExifFocalResUnit,2,L"inch",
											PropertyTagExifFocalResUnit,3,L"centimeter",
											PropertyTagOrientation,6,L"Rotate -90",
											PropertyTagOrientation,8,L"Rotate 90",
											PropertyTagOrientation,1,L"Normal",
											PropertyTagExifMeteringMode,1,L"Average",
											PropertyTagExifMeteringMode,2,L"Center weighted average",
											PropertyTagExifMeteringMode,3,L"Spot",
											PropertyTagExifMeteringMode,4,L"MultiSpot",
											PropertyTagExifMeteringMode,5,L"Pattern",
											PropertyTagExifMeteringMode,6,L"Partial",
											PropertyTagExifColorSpace,1,L"sRGB",
											PropertyTagExifColorSpace,0xFFFF,L"Uncalibrated",
											PropertyTagExifSensingMethod,1,L"Not defined",
											PropertyTagExifSensingMethod,2,L"One-chip color area sensor",
											PropertyTagExifSensingMethod,3,L"Two-chip color area sensor",
											PropertyTagExifSensingMethod,4,L"Three-chip color area sensor",
											PropertyTagExifSensingMethod,5,L"Color sequential area sensor",
											PropertyTagExifSensingMethod,7,L"Trilinear sensor",
											PropertyTagExifSensingMethod,8,L"Color sequential linear sensor",
											PropertyTagExifExposureProg,0,L"Not defined",
											PropertyTagExifExposureProg,1,L"Manual",
											PropertyTagExifExposureProg,2,L"Normal program",
											PropertyTagExifExposureProg,3,L"Aperture priority",
											PropertyTagExifExposureProg,4,L"Shutter priority",
											PropertyTagExifExposureProg,5,L"Creative program",
											PropertyTagExifExposureProg,6,L"Action program",
											PropertyTagExifExposureProg,7,L"Portrait mode",
											PropertyTagExifExposureProg,8,L"Landscape mode",
											PropertyTagExifExposureMode,0,L"Auto exposure",
											PropertyTagExifExposureMode,1,L"Manual exposuree",
											PropertyTagExifExposureMode,2,L"Auto bracket",
											PropertyTagExifSceneCapturedType,0,L"Standart",
											PropertyTagExifSceneCapturedType,1,L"Landscape",
											PropertyTagExifSceneCapturedType,2,L"Portrait",
											PropertyTagExifSceneCapturedType,3,L"Night scene",
											PropertyTagExifWhiteBalance,0,L"Auto white balance",
											PropertyTagExifWhiteBalance,1,L"Manual white balance",

};

const exifItem exifCanonItemArray[] = {
														MacroMode, 0, L"Macro EOS",
														MacroMode, 1, L"Macro",
														MacroMode, 2, L"Normal",
														QualityCompressionRate,2,L"Normal",
														QualityCompressionRate,3,L"Fine",
														QualityCompressionRate,5,L"SuperFine",
														ImageSize,0,L"Large",
														ImageSize,1,L"Medium",
														ImageSize,2,L"Small",
														ImageSize,5,L"Medium 1",
														ImageSize,6,L"Medium 2",
														ImageSize,8,L"Postcard",
														ISOSpeed,15,L"Auto ISO",
														ISOSpeed,16,L"ISO 50",
														ISOSpeed,17,L"ISO 100",
														ISOSpeed,18,L"ISO 200",
														ISOSpeed,19,L"ISO 400",
														WhiteBalance,0,L"Auto",
														WhiteBalance,1,L"Sunny",
														WhiteBalance,2,L"Cloudy",
														WhiteBalance,3,L"Tungsten",
														WhiteBalance,4,L"Fluorescent",
														WhiteBalance,5,L"Flash",
														WhiteBalance,6,L"Custom",
														WhiteBalance,7,L"Black & White",
														WhiteBalance,8,L"Shade",
														WhiteBalance,9,L"Manual Temperature",
														WhiteBalance,14,L"Fluorescent H",
														WhiteBalance,-1,L"Not available",
														EasyShoot,0,L"Full auto",
														EasyShoot,1,L"Manual",
														EasyShoot,2,L"Landscape",
														EasyShoot,3,L"Fast shutter",
														EasyShoot,4,L"Slow shutter",
														EasyShoot,5,L"Night scene",
														EasyShoot,6,L"Black&White",
														EasyShoot,7,L"Sepia",
														EasyShoot,8,L"Portrait",
														EasyShoot,9,L"Sports",
														EasyShoot,10,L"Macro/Close-Up",
														EasyShoot,11,L"Pan Focus",
														EasyShoot,12,L"Macro",
														EasyShoot,17,L"Macro",
														EasyShoot,18,L"Foliage",
														EasyShoot,19,L"Party",
														EasyShoot,20,L"Fireworks",
														EasyShoot,21,L"Beach",
														EasyShoot,22,L"Underwater",
														EasyShoot,23,L"Snow",
														EasyShoot,27,L"My colors",
														Contrast,-1,L"Low",
														Contrast,0,L"Normal",
														Contrast,1,L"High",
														Saturation,-1,L"Low",
														Saturation,0,L"Normal",
														Saturation,1,L"High",
														Sharpness,-1,L"Low",
														Sharpness,0,L"Normal",
														Sharpness,1,L"High",
														CanonPhotoEffect,0,L"No effect",
														CanonPhotoEffect,1,L"Vivid",
														CanonPhotoEffect,2,L"Neutral",
														CanonPhotoEffect,3,L"Low Sharpening",
														CanonPhotoEffect,4,L"Sepia",
														CanonPhotoEffect,5,L"Black&White",
														CanonPhotoEffect,6,L"Custom",
														DriveMode,0,L"Single Frame Shooting / Timer Mode",
														DriveMode,1,L"Continuous Shooting",
														DriveMode,3,L"High Speed Continuous Shooting",
														FocusMode,0,L"One Shot",
														FocusMode,1,L"AI Servo",
														FocusMode,2,L"AI Focus",
														FocusMode,3,L"Manual Focus",
														FocusMode,4,L"Single",
														FocusMode,5,L"Continuous",
														FocusMode,6,L"Manual Focus ",
														AFPoint,0,L"Right",
														AFPoint,1,L"Center",
														AFPoint,2,L"Left",
														AFPoint,0x2005,L"Manual AF point selection",
														AFPoint,0x3000,L"None",
														AFPoint,0x3001,L"Auto",
														AFPoint,0x3002,L"Right",
														AFPoint,0x3003,L"Center",
														AFPoint,0x3004,L"Left",
														AFPoint,0x4001,L"Auto AF point selection",
														AFPoint,0x4006,L"Face Detect",
														ExposureProgram,0,L"Easy Shoot",
														ExposureProgram,1,L"Program AE",
														ExposureProgram,2,L"Tv Priority",
														ExposureProgram,3,L"Av Priority",
														ExposureProgram,4,L"Manual",
														ExposureProgram,5,L"Auto-DEP",
														ExposureProgram,6,L"DEP",
														DigitalZoom,0,L"No digital zoom",
														DigitalZoom,1,L"2x",
														DigitalZoom,2,L"4x",
														DigitalZoom,3,L"Enable digital zoom"
};

CanonProperty * canonProperties = 0;

CanonTag * canonTagArray = 0;

WCHAR propertyString[1000];

short countTag = 0;

WCHAR * GetExifPropertyString(WCHAR * fileName, PROPID propId)
{

			PropertyItem * pItem;
			UINT size = 0;
			pItem = 0;
			propertyString[0] = 0;

			Image * image = new Image(fileName);

			size = image->GetPropertyItemSize(propId);

			if (size)
			{
				pItem = (PropertyItem*)new char[size];
				image->GetPropertyItem(propId, size, pItem);

				switch (propId)
				{
				case PropertyTagEquipMake:
				case PropertyTagEquipModel:
				case PropertyTagCopyright:
				case PropertyTagArtist:
				case PropertyTagImageDescription:
				case PropertyTagDateTime:
				case PropertyTagExifDTOrig:
				case PropertyTagExifDTDigitized:

						MultiByteToWideChar(CP_ACP,NULL,(char*)pItem->value, -1, propertyString, 1000);
					break;


				case PropertyTagXResolution:
				case PropertyTagYResolution:
				case PropertyTagExifFocalXRes:
				case PropertyTagExifFocalYRes:

						swprintf_s(propertyString, sizeof(propertyString) >> 1, L"%d/%d",*(long*)pItem->value,*((long*)pItem->value + 1));
					break;

				case PropertyTagExifPixXDim:
				case PropertyTagExifPixYDim:

					swprintf_s(propertyString, sizeof(propertyString) >> 1,L"%d",*((unsigned short*)pItem->value));
					break;

				case PropertyTagExifExposureTime:
					
					swprintf_s(propertyString, sizeof(propertyString) >> 1, L"%d/%d s",*(long*)pItem->value,*((long*)pItem->value + 1));
					break;

				case PropertyTagExifFNumber:
					{
					float f = (float)(*(long*)pItem->value) / (float)(*((long*)pItem->value + 1));
					swprintf_s(propertyString, sizeof(propertyString) >> 1, L"f/%.1f",f);
					}
					break;

				case PropertyTagExifShutterSpeed:
					{
					float f = (float)(*(long*)pItem->value) / (float)(*((long*)pItem->value + 1));
					f = (float)pow((double)0.5, (double)f);
					f = 1 / f;
					swprintf_s(propertyString, sizeof(propertyString) >> 1, L"1/%.0f s",f);
					}
					break;

				case PropertyTagExifAperture:
					{
					float f = (float)(*(long*)pItem->value) / (float)(*((long*)pItem->value + 1));
					f = (float)pow((double)2, (double)(f / 2));
					swprintf_s(propertyString, sizeof(propertyString) >> 1, L"f/%.1f",f);
					}
					break;

				case PropertyTagExifExposureBias:
					{
					float f = (float)(*(long*)pItem->value) / (float)(*((long*)pItem->value + 1));
					swprintf_s(propertyString, sizeof(propertyString) >> 1, L"%.2f EV",f);
					}
					break;

				case PropertyTagExifUserComment:

					if (!strcmp((char*)pItem->value, "UNICODE"))
					{
						wcsncpy_s(propertyString, sizeof(propertyString), (WCHAR*)((char*)pItem->value + 8), (pItem->length - 8) / 2);
						propertyString[(pItem->length - 8) / 2] = 0;
					}else if (!strcmp((char*)pItem->value, "ASCII"))
					{
						MultiByteToWideChar(CP_ACP,NULL,((char*)pItem->value + 8), (pItem->length - 8), propertyString, 1000);
						propertyString[pItem->length - 8] = 0;
					}
					break;

				case PropertyTagExifFocalLength:
					{
					float f = (float)(*(long*)pItem->value) / (float)(*((long*)pItem->value + 1));
					swprintf_s(propertyString, sizeof(propertyString) >> 1, L"%.5f mm",f);
					}
					break;

				case PropertyTagExifMaxAperture:
					{
					float f = (float)(*(long*)pItem->value) / (float)(*((long*)pItem->value + 1));
					f = (float)pow((double)2, (double)(f / 2));
					swprintf_s(propertyString, sizeof(propertyString) >> 1, L"f/%.1f",f);
					}
					break;

				case PropertyTagExifSubjectDist:
					{
					float f = (float)(*(long*)pItem->value) / (float)(*((long*)pItem->value + 1));
					swprintf_s(propertyString, sizeof(propertyString) >> 1, L"%.1f m",f);
					}
					break;

				case PropertyTagExifDigitalZoomRatio:
					{
					float f = (float)(*(long*)pItem->value) / (float)(*((long*)pItem->value + 1));
					swprintf_s(propertyString, sizeof(propertyString) >> 1, L"%.1fx",f);
					}
					break;

				case PropertyTagExifVer:
					{
					char buffer[4];

					buffer[0] = ((char*)pItem->value)[1];
					buffer[1] = '.';
					buffer[2] = ((char*)pItem->value)[2];
					buffer[3] = 0;

					MultiByteToWideChar(CP_ACP,NULL,buffer, -1, propertyString, 1000);
					}
					break;

				case PropertyTagExifCompConfig:
					{
						propertyString[0] = 0;

						long value = *((long*)pItem->value);

						for (int i = 0; i < 4; ++i)
						{
							switch (value & 0xFF)
							{
							case 0:
								value >>= 8;
								break;
							case 1:
								wcscat_s(propertyString, sizeof(propertyString) >>1 , L"Y");
								value >>= 8;
								break;
							case 2:
								wcscat_s(propertyString,  sizeof(propertyString) >>1 , L"Cb");
								value >>= 8;
								break;
							case 3:
								wcscat_s(propertyString,  sizeof(propertyString) >>1 , L"Cr");
								value >>= 8;
								break;
							case 4:
								wcscat_s(propertyString,  sizeof(propertyString) >>1 , L"R");
								value >>= 8;
								break;
							case 5:
								wcscat_s(propertyString,  sizeof(propertyString) >>1 , L"G");
								value >>= 8;
								break;
							case 6:
								wcscat_s(propertyString,  sizeof(propertyString) >>1 , L"B");
								value >>= 8;
								break;
							}
						}
					}
					break;

				case PropertyTagExifFlash :
					{
					unsigned short f = *(unsigned short*)pItem->value;

					if (f & 1)
						wcscpy_s(propertyString, sizeof(propertyString) >> 1, L"Flash fired");
					else
						wcscpy_s(propertyString, sizeof(propertyString) >> 1, L"Flash not fired");

					if ((f & 24) == 24)
						wcscat_s(propertyString, sizeof(propertyString) >> 1, L", Auto mode");
					else
						wcscat_s(propertyString, sizeof(propertyString) >> 1, L", Manual mode");

					if (f & 64)
						wcscat_s(propertyString, sizeof(propertyString) >> 1, L", Red eye reduction");
					}
					break;

				case PropertyTagResolutionUnit:
				case PropertyTagExifFocalResUnit:
				case PropertyTagOrientation:
				case PropertyTagExifMeteringMode:
				case PropertyTagExifColorSpace:
				case PropertyTagExifSensingMethod:
				case PropertyTagExifExposureProg:
				case PropertyTagExifExposureMode:
				case PropertyTagExifSceneCapturedType:
				case PropertyTagExifWhiteBalance:
					{
						unsigned short value = *((unsigned short*)pItem->value);
						for (int count = 0; count < (sizeof(exifItemArray) / sizeof(exifItem)); ++count)
						{
							if ((propId == exifItemArray[count].tag) && (value ==exifItemArray[count].value))
							{
								wcscpy_s(propertyString,  sizeof(propertyString) >> 1, exifItemArray[count].string);	
							}
						}
					}
					break;
				
				}

				delete pItem;
				pItem = 0;
				delete image;
				image = 0;

				return propertyString;
			}

	delete image;
	image = 0;
	return NULL;
}

void ReleaseData()
{
	for (int i = 0; i < countTag; ++i)
	{
		if (canonTagArray[i].data)
		{
			delete[] canonTagArray[i].data;
			canonTagArray[i].data = 0;
		}
	}
	if (canonTagArray)
	{
		delete[] canonTagArray;
		canonTagArray = 0;
	}

	countTag = 0;
}

bool ReadCanonMakerNotes(WCHAR * fileName)
{

	PropertyItem * pItem;
	UINT size;

	Image * image = new Image(fileName);
	WCHAR * string = GetExifPropertyString(fileName, PropertyTagEquipMake);
	size = image->GetPropertyItemSize(PropertyTagExifMakerNote);

	if (size && (!wcscmp(L"Canon",string)))
	{
		pItem = (PropertyItem*)new char[size];
		image->GetPropertyItem(PropertyTagExifMakerNote, size, pItem);

		countTag = *(short*)pItem->value;
		short * pointer = (short*)pItem->value;
		pointer++;

		canonProperties = new CanonProperty[countTag];
		canonTagArray = new CanonTag[countTag];
		memcpy(canonProperties,pointer,sizeof(CanonProperty) * countTag);

		HANDLE file = CreateFile(fileName,FILE_READ_DATA,FILE_SHARE_READ,NULL,OPEN_EXISTING,NULL,NULL);

		DWORD num;
		char buffer[5];
		buffer[4] = 0;

		do
		{
			ReadFile(file,buffer,4,&num,NULL);
			SetFilePointer(file,-3,NULL,FILE_CURRENT);
		}while (!(CSTR_EQUAL == CompareStringA(LOCALE_SYSTEM_DEFAULT,NORM_IGNORECASE,"Exif",-1,buffer,-1)));

		DWORD posExif = SetFilePointer(file,5,NULL,FILE_CURRENT);

		for (int i = 0; i < countTag; ++i)
		{			
			SetFilePointer(file, posExif, NULL, FILE_BEGIN);
			SetFilePointer(file, canonProperties[i].offset, NULL, FILE_CURRENT);

			switch (canonProperties[i].type)
			{

			case 1:
			case 2:

				canonTagArray[i].tag = canonProperties[i].tag;
				canonTagArray[i].type = canonProperties[i].type;
				canonTagArray[i].count = canonProperties[i].count;
				canonTagArray[i].data = new char[canonProperties[i].count];
				if (canonProperties[i].count < 5)
				{
					int shift = 0;
					for (unsigned long j = 0; j < canonProperties[i].count; ++j)
					{
						unsigned char value;
						value = (unsigned char)((canonProperties[i].offset >> shift) & 0x000000FF);
						*(((unsigned char*)(canonTagArray[i].data)) + j) = value;
						shift += 8;
					}
					break;
				}
				ReadFile(file,canonTagArray[i].data,canonProperties[i].count,&num,0);
				break;

			case 3:
				{
					canonTagArray[i].tag = canonProperties[i].tag;
					canonTagArray[i].type = canonProperties[i].type;
					canonTagArray[i].count = canonProperties[i].count;
					canonTagArray[i].data = new short[canonProperties[i].count];

					if (canonTagArray[i].count < 3)
					{
						int shift = 0;
						for (unsigned long j = 0; j < canonProperties[i].count; ++j)
						{
							unsigned short value;
							value = (unsigned short)((canonProperties[i].offset >> shift) & 0x0000FFFF);
							*(((short*)(canonTagArray[i].data)) + j) = value;
							shift += 16;
						}
						break;
					}

					ReadFile(file,canonTagArray[i].data, canonProperties[i].count << 1,&num,0);
				}
				break;

			case 4:

				canonTagArray[i].tag = canonProperties[i].tag;
				canonTagArray[i].type = canonProperties[i].type;
				canonTagArray[i].count = canonProperties[i].count;
				canonTagArray[i].data = new long[canonProperties[i].count];
				if (canonProperties[i].count < 2)
				{
					*((unsigned long*)canonTagArray[i].data) = canonProperties[i].offset;
					break;
				}

				ReadFile(file, canonTagArray[i].data, canonProperties[i].count << 2, &num, 0);
				break;
			}
		}

		CloseHandle(file);

		delete[] canonProperties;
		delete pItem;
		pItem = 0;
		delete image;
		return true;
	}

	delete image;
	return false;
}

WCHAR * GetExifCanonMakerNotesString(eCanonPropertyId canonPropId)
{

	propertyString[0] = 0;

	for (short i = 0; i < countTag; ++i)
	{
		if ((canonPropId >> 16) == canonTagArray[i].tag)
		{
			void * p = canonTagArray[i].data;

			switch (canonTagArray[i].type)
			{
			case 1:

				p =  (void*)(((char*)p) + (canonPropId & 0x0000FFFF));

			case 3:

				p =  (void*)(((short*)p) + (canonPropId & 0x0000FFFF));
				break;

			case 4:

				p =  (void*)(((long*)p) + (canonPropId & 0x0000FFFF));
				break;
			}

			switch (canonPropId)
			{
			case ISOSpeed:
				{
					unsigned short value = *((unsigned short*)p);
					if (value & 0x4000)
					{
						swprintf_s(propertyString,  sizeof(propertyString) >> 1, L"ISO %d", value & 0xFFF);
						break;
					}
				}
			case MacroMode:
			case QualityCompressionRate:
			case ImageSize:
			case WhiteBalance:
			case EasyShoot:
			case Contrast:
			case Saturation:
			case Sharpness:
			case CanonPhotoEffect:
			case DriveMode:
			case FocusMode:
			case AFPoint:
			case ExposureProgram:
			case DigitalZoom:
				{
					unsigned short value = *((unsigned short*)p);
					for (int count = 0; count < (sizeof(exifCanonItemArray) / sizeof(exifItem)); ++count)
					{
						if ((canonPropId == exifCanonItemArray[count].tag) && (value ==exifCanonItemArray[count].value))
						{
							wcscpy_s(propertyString,  sizeof(propertyString) >> 1, exifCanonItemArray[count].string);	
						}
					}
				}
				break;

			case Imagetype:
			case FirmWare:

				MultiByteToWideChar(CP_ACP,NULL,(char*)p, canonTagArray[i].count, propertyString, 1000);
				break;

			case ImageNumber:

				swprintf_s(propertyString,  sizeof(propertyString) >> 1, L"%d-%d", *(long*)canonTagArray[i].data / 1000, *(long*)canonTagArray[i].data % 1000);
				break;

			case Distance:

				swprintf_s(propertyString,  sizeof(propertyString) >> 1, L"%.2f m", ((float)(*(short*)p)) / 100);
				break;
			}

			return propertyString;
		}
	}

	return NULL;
}