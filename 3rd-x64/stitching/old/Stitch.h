#ifndef _STITCH_H
#define _STITCH_H

#ifdef _WIN32
	#define DLL_EXPORT __declspec(dllexport)
#else
	#define DLL_EXPORT
#endif

struct ImageParam
{
	size_t nCam;		// camera amount
	size_t nCamHeight;	// camera image height
	size_t nCamWidth;	// camera image width
	size_t nCamSize;	// camera image size (height * width)

	size_t nFinalHeight;	// final image height
	size_t nFinalWidth;		// final image width
	size_t nFinalSize;		// final image size (height * width)
};

enum
{
	STITCH_COARSE = 0,
	STITCH_REFINED = 1
};

DLL_EXPORT const char* GetStitchingVer();

DLL_EXPORT int GetNvidiaDeviceCount();

/////////////////////////////////////////////////////////////
// Parameter: char * ParamFolderPath	---- Parameter files path
// Remarks:   initialize handle parameter and buffer
/////////////////////////////////////////////////////////////
DLL_EXPORT bool StitchInit(char* ParamFilePath, ImageParam* pIP, unsigned int ccFrequency = 5, unsigned int vcFrequency = 5, unsigned int ccInitialCounter = 0, unsigned int vcInitialCounter = 0, int flags = STITCH_COARSE);


/////////////////////////////////////////////////////////////
// Returns:   void
// Remarks:   free buffer
/////////////////////////////////////////////////////////////
DLL_EXPORT void StitchUninit();

/////////////////////////////////////////////////////////////
// Parameter: UINT8 * pImg1		---- input image 1 data
// Parameter: UINT8 * pImg2		---- input image 2 data
// Parameter: UINT8 * pImg3		---- input image 3 data
// Parameter: UINT8 * pImg4		---- input image 4 data
// Parameter: UINT8 * pFinal	---- ouput image data
// Returns:   int				---- process result
// Remarks:   stitch specified amount image into 1 full image
/////////////////////////////////////////////////////////////
DLL_EXPORT int StitchProcess(unsigned char* pImg1, unsigned char* pImg2, unsigned char* pImg3, unsigned char* pImg4, unsigned char* pFinal);

#endif