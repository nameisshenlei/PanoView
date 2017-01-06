#ifndef PANORAMAVIEW_H_INCLUDE
#define PANORAMAVIEW_H_INCLUDE

#include "GLTools.h"	
#include "GLMatrixStack.h"
#include "GLFrame.h"
#include "GLFrustum.h"
#include "GLGeometryTransform.h"

#include <math.h>
#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include "GL/glut.h"
#endif

//#include "UsbCamera.h"
//#include "VideoDecoder.h"

class PanoramaView
{
public:
	enum DISPLAYMODE { PANORAMA, FLAT };
	enum PANO_MODE {NORMAL, FISHEYE, PLANTET, SPHERE};

	PanoramaView(HDC hdc);
	~PanoramaView();
	void init(int w, int h);

	void changeSize(int w, int h);
	void render(unsigned char* buffer, int w, int h);

	void setWidth(int w);
	void setHeight(int h);
	void setTextureWidth(int w);
	void setTextureHeight(int h);
	void setTextureBuffer(unsigned char* buffer);

	void mouseDownHandler(int x, int y);
	void mouseUpHandler(int x, int y);
	void mouseMoveHandler(int x, int y);

	void makeLookAt(M3DVector3f viewPosition, M3DVector3f targetPosition, M3DVector3f up, M3DMatrix44f result);
	void normalize(M3DVector3f src, M3DVector3f result);
	void cross(M3DVector3f x, M3DVector3f y, M3DVector3f result);

	void drawSphere(GLfloat fRadius, GLint iSlices, GLint iStacks);
	void drawRect();

	void setDisplayMode(DISPLAYMODE mode);

	void zoomInOut();
	void setZoomFlag(int flag);
	void zoomInStep();
	void zoomOutStep();

	void resetPositoin();
	void judgePanoMode();
	void resetPanoMode(PANO_MODE mode);
	void runIntertia();
	void switchPanoMode(PANO_MODE mode);
	void switchingPanoMode();

private:
	int width;
	int height;
	int textureWidth;   //纹理图片的宽
	int textureHeight;  //纹理图片的高
	unsigned char* textureBuffer;  //纹理图片的缓存

	float zNear;             //投影近距离
	float zFar;              //投影远距离
	int fov;                 //投影的视觉角度
	int latitudeBands;       //纬度分割的块数
	int longitudeBands;      //经度分割的块数
	float radius;            //球的半径
	float viewDistance;      //视点距离

	GLFrame             viewFrame;         //视点位置
	GLFrustum           viewFrustum;       //投影体	
	GLMatrixStack       modelViewMatrix;   //模型视图矩阵
	GLMatrixStack       projectionMatrix;  //投影矩阵
	GLGeometryTransform transformPipeline; //用于管理矩阵

	GLShaderManager     shaderManager;    //着色器
	GLTriangleBatch     rectDrawer;       //画矩形
	GLTriangleBatch     sphereDrawer;     //画球形

	GLuint	shaderID;
	GLuint  textureID;
	GLint	mvpMatrixUniform;
	GLint   textureUniform;

	HDC renderDC;

	bool isMouseDown;
	int  lastMouseX;
	int  lastMouseY;
	float mouseSensitivity;
    float maxViewAngle;
	float minViewAngle;
	float angleAlpha; //
	float angleBeta;  //

	M3DVector3f  viewPosition;
	M3DVector3f  targetPosition;
	M3DVector3f  upDirection;
	M3DMatrix44f viewTransform;
	M3DMatrix44f cameraWorldTransform;
	M3DMatrix44f unitMatrix;

	DISPLAYMODE displayMode;
	PANO_MODE   panoMode;

	int zoomFlag;  //0:保持； 1:放大； 2:缩小
	
	int fovMax;
	int fovMin;

	bool isInertia;
	float lastDeltaX;
	float lastDeltaY;
	float inertiaSpeed;

	PANO_MODE  newPanoMode;
	bool switchingMode = false;
	M3DVector3f newViewPosition;
	float newNear;
	int   newFov;

	float posStep = 0.1;
	float nearStep = 0.05;
	int fovStep = 5;
};

#endif