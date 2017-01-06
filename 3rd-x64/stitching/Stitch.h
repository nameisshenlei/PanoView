#pragma once
#include <cstdint>

#ifdef _WIN32
	#define DLL_EXPORT __declspec(dllexport)
#else
	#define DLL_EXPORT
#endif

struct ImageParam
{
	uint64_t nCam;		// camera amount
	uint64_t nCamHeight;	// camera image height
	uint64_t nCamWidth;	// camera image width
	uint64_t nCamSize;	// camera image size (height * width)

	uint64_t nFinalHeight;	// final image height
	uint64_t nFinalWidth;		// final image width
	uint64_t nFinalSize;		// final image size (height * width)
};

#ifdef __cplusplus
extern "C"
{
#endif

	DLL_EXPORT int GetNvidiaDeviceCount();

	DLL_EXPORT const char* GetStitchingVer();

	/////////////////////////////////////////////////////////////
	// Parameter: char * ParamFolderPath	---- Parameter files path
	// Remarks:   initialize handle parameter and buffer
	/////////////////////////////////////////////////////////////
	DLL_EXPORT bool StitchInit(char* ParamFilePath, ImageParam* pIP, uint64_t ccFrequency = 5, uint64_t vcFrequency = 5, uint64_t ccInitialCounter = 0, uint64_t vcInitialCounter = 0);
	
	/////////////////////////////////////////////////////////////
	// Returns:   void
	// Remarks:   free buffer
	/////////////////////////////////////////////////////////////
	DLL_EXPORT void StitchUninit();

	/////////////////////////////////////////////////////////////
	// Parameter: uint8_t * pImg1		---- input image 1 data
	// Parameter: uint8_t * pImg2		---- input image 2 data
	// Parameter: uint8_t * pImg3		---- input image 3 data
	// Parameter: uint8_t * pImg4		---- input image 4 data
	// Parameter: uint8_t * pFinal	---- ouput image data
	// Returns:   int				---- process result
	// Remarks:   stitch specified amount image into 1 full image
	/////////////////////////////////////////////////////////////
	DLL_EXPORT int StitchProcess(uint8_t* pImg1, uint8_t* pImg2, uint8_t* pImg3, uint8_t* pImg4, uint8_t* pFinal);

#ifdef __cplusplus
}
#endif