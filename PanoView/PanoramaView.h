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
	int textureWidth;   //����ͼƬ�Ŀ�
	int textureHeight;  //����ͼƬ�ĸ�
	unsigned char* textureBuffer;  //����ͼƬ�Ļ���

	float zNear;             //ͶӰ������
	float zFar;              //ͶӰԶ����
	int fov;                 //ͶӰ���Ӿ��Ƕ�
	int latitudeBands;       //γ�ȷָ�Ŀ���
	int longitudeBands;      //���ȷָ�Ŀ���
	float radius;            //��İ뾶
	float viewDistance;      //�ӵ����

	GLFrame             viewFrame;         //�ӵ�λ��
	GLFrustum           viewFrustum;       //ͶӰ��	
	GLMatrixStack       modelViewMatrix;   //ģ����ͼ����
	GLMatrixStack       projectionMatrix;  //ͶӰ����
	GLGeometryTransform transformPipeline; //���ڹ������

	GLShaderManager     shaderManager;    //��ɫ��
	GLTriangleBatch     rectDrawer;       //������
	GLTriangleBatch     sphereDrawer;     //������

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

	int zoomFlag;  //0:���֣� 1:�Ŵ� 2:��С
	
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