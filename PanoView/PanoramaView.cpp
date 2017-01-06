
#include "stdafx.h"
#include "PanoramaView.h"

PanoramaView::PanoramaView(HDC hdc) :
    mouseSensitivity(1),
    maxViewAngle(0.95),
    minViewAngle(-0.95)
{
	renderDC = hdc;

	textureWidth = 1920;
	textureHeight = 1080;
	width  = 1920;
	height = 1080;

	zNear = 0.1;           //投影近距离
	zFar = 100;            //投影远距离
	fov = 60;              //投影的视觉角度
	latitudeBands = 50;    //纬度分割的块数
	longitudeBands = 50;   //经度分割的块数
	radius = 1.0;          //球的半径
	viewDistance = 1.0;    //视点距离

	textureBuffer = NULL;

	isMouseDown = false;
	lastMouseX = 0;
	lastMouseY = 0;

	viewPosition[0] = 0;
	viewPosition[1] = 0;
	viewPosition[2] = 0;
	targetPosition[0] = 0;
	targetPosition[1] = 0;
	targetPosition[2] = 0;
	upDirection[0] = 0;
	upDirection[1] = 1;
	upDirection[2] = 0;

	angleAlpha = 0;
	angleBeta = 0;

	displayMode = PANORAMA;
	unitMatrix[0] = 1.0;
	unitMatrix[1] = 0.0;
	unitMatrix[2] = 0.0;
	unitMatrix[3] = 0.0;

	unitMatrix[4] = 0.0;
	unitMatrix[5] = 1.0;
	unitMatrix[6] = 0.0;
	unitMatrix[7] = 0.0;

	unitMatrix[8] = 0.0;
	unitMatrix[9] = 0.0;
	unitMatrix[10] = 1.0;
	unitMatrix[11] = 0.0;

	unitMatrix[12] = 0.0;
	unitMatrix[13] = 0.0;
	unitMatrix[14] = 0.0;
	unitMatrix[15] = 1.0;

	zoomFlag = 0;

	panoMode = PANO_MODE::NORMAL;
	fovMax = 170;
	fovMin = 40;

	isInertia = false;
	lastDeltaX = 0.0;
	lastDeltaY = 0.0;
	inertiaSpeed = 7;
}

PanoramaView::~PanoramaView()
{
	if (textureBuffer != NULL)
		delete textureBuffer;
}

void PanoramaView::init(int w, int h)
{
	width = w;
	height = h;
	
	int argc = 1;
	char* argv[] = { "PanoramaView" };
	glutInit(&argc, argv);   //初始化glut实用工具库
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);  //启用双缓冲，深度测试

	GLenum err = glewInit();  //初始化扩展库
	if (GLEW_OK != err) {
		fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
		return;
	}

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);

	shaderManager.InitializeStockShaders();  //初始化着色器
	//加载着色器文件
	shaderID = gltLoadShaderPairWithAttributes("panoramaview.vp", "panoramaview.fp", 2, GLT_ATTRIBUTE_VERTEX, "vVertex", GLT_ATTRIBUTE_TEXTURE0, "vTexture0");

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	mvpMatrixUniform = glGetUniformLocation(shaderID, "mvpMatrix");
	textureUniform = glGetUniformLocation(shaderID, "colorMap");

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	drawRect();   //画矩形
	drawSphere(radius, latitudeBands, longitudeBands); //画球

	viewFrame.MoveForward(viewDistance);
	//viewPosition[0] = 0;
	//viewPosition[1] = 0;
	//viewPosition[2] = 1;
	resetPanoMode(PANO_MODE::PLANTET);

	makeLookAt(viewPosition, targetPosition, upDirection, cameraWorldTransform);
	m3dInvertMatrix44(viewTransform, cameraWorldTransform);

	viewFrustum.SetPerspective(fov, (float)width / (float)height, zNear, zFar);

	projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());

	transformPipeline.SetMatrixStacks(modelViewMatrix, projectionMatrix);
	//modelViewMatrix.PushMatrix(viewFrame);
	modelViewMatrix.PushMatrix(viewTransform);
	//modelViewMatrix.Rotate(180.0f, 0.0f, 1.0f, 0.0f);
}

void PanoramaView::changeSize(int w, int h)
{
	if (h == 0 || w == 0)
		return;

	width = w;
	height = h;

	glViewport(0, 0, w, h);
	viewFrustum.SetPerspective(fov, float(w) / float(h), zNear, zFar);
	projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());

}

void PanoramaView::render(unsigned char* buffer, int w, int h)
{
	textureBuffer = buffer;
	textureWidth = w;
	textureHeight = h;

	runIntertia();

	//if (textureBuffer == NULL)
	//return;
	switchingPanoMode();

	zoomInOut();

	modelViewMatrix.PushMatrix(viewTransform);

	modelViewMatrix.Rotate(10, 0.0f, 1.0f, 0.0f);
	
	if (textureBuffer != NULL)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, textureBuffer);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glUseProgram(shaderID);

	//glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, transformPipeline.GetModelViewProjectionMatrix());
	//glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, unitMatrix);

	glUniform1i(textureUniform, 0);

	if (displayMode == DISPLAYMODE::PANORAMA) {
		glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, transformPipeline.GetModelViewProjectionMatrix());
		sphereDrawer.Draw();
	}
	else{
		glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, unitMatrix);
		rectDrawer.Draw();
	}

	//rectDrawer.Draw();
	//sphereDrawer.Draw();

	modelViewMatrix.PopMatrix();

	SwapBuffers(renderDC);
}

void PanoramaView::setTextureWidth(int w)
{
	textureWidth = w;
}

void PanoramaView::setTextureHeight(int h)
{
	textureHeight = h;
}

void PanoramaView::setTextureBuffer(unsigned char* buffer)
{
	textureBuffer = buffer;
}

void PanoramaView::mouseDownHandler(int x, int y)
{
	isMouseDown = true;
	lastMouseX = x;
	lastMouseY = y;
}

void PanoramaView::mouseUpHandler(int x, int y)
{
	isMouseDown = false;
	isInertia = true;
}

void PanoramaView::mouseMoveHandler(int x, int y)
{	
	if (isMouseDown == false)
		return;

	int newX = x;
	int newY = y;
	float deltaX = newX - lastMouseX;
	float deltaY = newY - lastMouseY;
	lastDeltaX = deltaX;
	lastDeltaY = deltaY;

	//TRACE("deltaX is %04f. \n", deltaX);
	//TRACE("deltaY is %04f. \n", deltaY);

	lastMouseX = newX;
	lastMouseY = newY;

	//if (panoMode == PANO_MODE::SPHERE || panoMode == PANO_MODE::NORMAL) {
		angleAlpha += deltaX / (float)width * 3.1415926 * mouseSensitivity;
		angleBeta -= deltaY / (float)height * 3.1415926 * mouseSensitivity;
	//}
	//else if (panoMode == PANO_MODE::PLANTET) {
		//angleBeta += deltaX / (float)width * 3.1415926 * mouseSensitivity;
		//angleAlpha -= deltaY / (float)height * 3.1415926 * mouseSensitivity;
	//}

	if (angleBeta > maxViewAngle)
		angleBeta = maxViewAngle;
	if (angleBeta < minViewAngle)
		angleBeta = minViewAngle;

	judgePanoMode();

	viewPosition[0] = viewDistance * cos(angleBeta) * sin(angleAlpha);
	viewPosition[1] = viewDistance * sin(angleBeta);
	viewPosition[2] = viewDistance * cos(angleBeta) * cos(angleAlpha);

	//if (panoMode == PANO_MODE::SPHERE || panoMode == PANO_MODE::NORMAL) {
		makeLookAt(viewPosition, targetPosition, upDirection, cameraWorldTransform);
		m3dInvertMatrix44(viewTransform, cameraWorldTransform);
		projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
	//}
	//else if (panoMode == PANO_MODE::PLANTET) {
		//makeLookAt(targetPosition, viewPosition,upDirection, cameraWorldTransform);
		//modelViewMatrix.Rotate(10, 0.0f, 1.0f, 0.0f);
	//}

}

void PanoramaView::makeLookAt(M3DVector3f viewPosition, M3DVector3f targetPosition, M3DVector3f up, M3DMatrix44f result)
{
	M3DVector3f zAxis;
	M3DVector3f sub;
	sub[0] = viewPosition[0] - targetPosition[0];
	sub[1] = viewPosition[1] - targetPosition[0];
	sub[2] = viewPosition[2] - targetPosition[0];

	normalize(sub, zAxis);

	M3DVector3f xAxis = { 1, 0, 0 };
	M3DVector3f yAxis = { 0, 1, 0 };

	M3DVector3f xAxis2 = { 1, 0, 0 };
	M3DVector3f yAxis2 = { 0, 1, 0 };

	if ((zAxis[0] == 0) && (zAxis[1] == 0) && (zAxis[2] == 0)) {
		zAxis[0] = 0;
		zAxis[1] = 0;
		zAxis[2] = 1;
	}
	else if ((zAxis[0] == 0) && (fabs(zAxis[1]) == 1) && (zAxis[2] == 0)) {
		cross(zAxis, xAxis, yAxis);
	}
	else {
		cross(up, zAxis, xAxis);
		cross(zAxis, xAxis, yAxis);
	}

	normalize(xAxis, xAxis2);
	normalize(yAxis, yAxis2);

	result[0] = xAxis2[0];
	result[1] = xAxis2[1];
	result[2] = xAxis2[2];
	result[3] = 0;

	result[4] = yAxis2[0];
	result[5] = yAxis2[1];
	result[6] = yAxis2[2];
	result[7] = 0;

	result[8]  = zAxis[0];
	result[9]  = zAxis[1];
	result[10] = zAxis[2];
	result[11] = 0;

	result[12] = viewPosition[0];
	result[13] = viewPosition[1];
	result[14] = viewPosition[2];
	result[15] = 1;
}

void PanoramaView::normalize(M3DVector3f x, M3DVector3f result)
{
	float length = sqrt(x[0] * x[0] + x[1] * x[1] + x[2] * x[2]);
	if (length > 0.00001) {
		result[0] = x[0] / length;
		result[1] = x[1] / length;
		result[2] = x[2] / length;
	} 
	else {
		result[0] = 0;
		result[1] = 0;
		result[2] = 0;
	}
}

void PanoramaView::cross(M3DVector3f x, M3DVector3f y, M3DVector3f result)
{
	result[0] = x[1] * y[2] - x[2] * y[1];
	result[1] = x[2] * y[0] - x[0] * y[2];
	result[2] = x[0] * y[1] - x[1] * y[0];
}

void PanoramaView::drawSphere(GLfloat fRadius, GLint iSlices, GLint iStacks)
{
	GLfloat drho = (GLfloat)(3.141592653589) / (GLfloat)iStacks;
	GLfloat dtheta = 2.0f * (GLfloat)(3.141592653589) / (GLfloat)iSlices;
	GLfloat ds = 1.0f / (GLfloat)iSlices;
	GLfloat dt = 1.0f / (GLfloat)iStacks;
	GLfloat t = 0.0f;
	GLfloat s = 0.0f;
	GLint i, j;

	sphereDrawer.BeginMesh(iSlices * iStacks * 6);
	for (i = 0; i < iStacks; i++)
	{
		GLfloat rho = (GLfloat)i * drho;
		GLfloat srho = (GLfloat)(sin(rho));
		GLfloat crho = (GLfloat)(cos(rho));
		GLfloat srhodrho = (GLfloat)(sin(rho + drho));
		GLfloat crhodrho = (GLfloat)(cos(rho + drho));

		s = 0.0f;
		M3DVector3f vVertex[4];
		M3DVector3f vNormal[4];
		M3DVector2f vTexture[4];

		for (j = 0; j < iSlices; j++)
		{
			GLfloat theta = (j == iSlices) ? 0.0f : j * dtheta;
			GLfloat stheta = (GLfloat)(-sin(theta));
			GLfloat ctheta = (GLfloat)(cos(theta));

			GLfloat x = stheta * srho;
			GLfloat z = ctheta * srho;
			GLfloat y = crho;

			vTexture[0][0] = s;
			vTexture[0][1] = t;
			vNormal[0][0] = x;
			vNormal[0][1] = y;
			vNormal[0][2] = z;
			vVertex[0][0] = x * fRadius;
			vVertex[0][1] = y * fRadius;
			vVertex[0][2] = z * fRadius;

			x = stheta * srhodrho;
			z = ctheta * srhodrho;
			y = crhodrho;

			vTexture[1][0] = s;
			vTexture[1][1] = t + dt;
			vNormal[1][0] = x;
			vNormal[1][1] = y;
			vNormal[1][2] = z;
			vVertex[1][0] = x * fRadius;
			vVertex[1][1] = y * fRadius;
			vVertex[1][2] = z * fRadius;

			theta = ((j + 1) == iSlices) ? 0.0f : (j + 1) * dtheta;
			stheta = (GLfloat)(-sin(theta));
			ctheta = (GLfloat)(cos(theta));

			x = stheta * srho;
			z = ctheta * srho;
			y = crho;

			s += ds;
			vTexture[2][0] = s;
			vTexture[2][1] = t;
			vNormal[2][0] = x;
			vNormal[2][1] = y;
			vNormal[2][2] = z;
			vVertex[2][0] = x * fRadius;
			vVertex[2][1] = y * fRadius;
			vVertex[2][2] = z * fRadius;

			x = stheta * srhodrho;
			z = ctheta * srhodrho;
			y = crhodrho;

			vTexture[3][0] = s;
			vTexture[3][1] = t + dt;
			vNormal[3][0] = x;
			vNormal[3][1] = y;
			vNormal[3][2] = z;
			vVertex[3][0] = x * fRadius;
			vVertex[3][1] = y * fRadius;
			vVertex[3][2] = z * fRadius;

			sphereDrawer.AddTriangle(vVertex, vNormal, vTexture);

			// Rearrange for next triangle
			memcpy(vVertex[0], vVertex[1], sizeof(M3DVector3f));
			memcpy(vNormal[0], vNormal[1], sizeof(M3DVector3f));
			memcpy(vTexture[0], vTexture[1], sizeof(M3DVector2f));

			memcpy(vVertex[1], vVertex[3], sizeof(M3DVector3f));
			memcpy(vNormal[1], vNormal[3], sizeof(M3DVector3f));
			memcpy(vTexture[1], vTexture[3], sizeof(M3DVector2f));

			sphereDrawer.AddTriangle(vVertex, vNormal, vTexture);
		}
		t += dt;
	}
	sphereDrawer.End();
}

void PanoramaView::drawRect()
{
	rectDrawer.BeginMesh(6);
	static const GLfloat vertexVertices[] = {
		-1.0f, -1.0f,
		1.0f, -1.0f,
		-1.0f, 1.0f,
		1.0f, 1.0f,
	};

	static const GLfloat textureVertices[] = {
		0.0f, 1.0f,
		1.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,
	};

	M3DVector3f vVertex[3];
	M3DVector3f vNormal[3];
	M3DVector2f vTexture[3];

	vVertex[0][0] = -1;   //first ad
	vVertex[0][1] = 1;
	vVertex[0][2] = 0;

	vVertex[1][0] = 1;
	vVertex[1][1] = 1;
	vVertex[1][2] = 0;

	vVertex[2][0] = -1;
	vVertex[2][1] = -1;
	vVertex[2][2] = 0;

	vNormal[0][0] = 0;
	vNormal[0][1] = 0;
	vNormal[0][2] = 1;
	vNormal[1][0] = 0;
	vNormal[1][1] = 0;
	vNormal[1][2] = 1;
	vNormal[2][0] = 0;
	vNormal[2][1] = 0;
	vNormal[2][2] = 1;

	//vTexture[0][0] = 0;
	//vTexture[0][1] = 1;
	//vTexture[1][0] = 1;
	//vTexture[1][1] = 1;
	//vTexture[2][0] = 0;
	//vTexture[2][1] = 0;

	vTexture[0][0] = 0;
	vTexture[0][1] = 0;
	vTexture[1][0] = 1;
	vTexture[1][1] = 0;
	vTexture[2][0] = 0;
	vTexture[2][1] = 1;

	rectDrawer.AddTriangle(vVertex, vNormal, vTexture);

	/////////////////
	vVertex[0][0] = -1;   //second ad
	vVertex[0][1] = -1;
	vVertex[0][2] = 0;

	vVertex[1][0] = 1;
	vVertex[1][1] = 1;
	vVertex[1][2] = 0;

	vVertex[2][0] = 1;
	vVertex[2][1] = -1;
	vVertex[2][2] = 0;

	vNormal[0][0] = 0;
	vNormal[0][1] = 0;
	vNormal[0][2] = 1;
	vNormal[1][0] = 0;
	vNormal[1][1] = 0;
	vNormal[1][2] = 1;
	vNormal[2][0] = 0;
	vNormal[2][1] = 0;
	vNormal[2][2] = 1;

	//vTexture[0][0] = 0;
	//vTexture[0][1] = 0;
	//vTexture[1][0] = 1;
	//vTexture[1][1] = 1;
	//vTexture[2][0] = 1;
	//vTexture[2][1] = 0;
	vTexture[0][0] = 0;
	vTexture[0][1] = 1;
	vTexture[1][0] = 1;
	vTexture[1][1] = 0;
	vTexture[2][0] = 1;
	vTexture[2][1] = 1;

	rectDrawer.AddTriangle(vVertex, vNormal, vTexture);

	rectDrawer.End();
}

void PanoramaView::setDisplayMode(DISPLAYMODE mode)
{
	displayMode = mode;
}

void PanoramaView::zoomInOut()
{
	judgePanoMode();

	if (zoomFlag == 1) {       //放大
		if (fov <= fovMax)
			fov += 10;
	}
	else if (zoomFlag == 2) {  //缩小
		if (fov >= fovMin)
			fov -= 10;
	}
	else
		return;

	//viewPosition[0] = viewDistance * cos(angleBeta) * sin(angleAlpha);
	//viewPosition[1] = viewDistance * sin(angleBeta);
	//viewPosition[2] = viewDistance * cos(angleBeta) * cos(angleAlpha);

	makeLookAt(viewPosition, targetPosition, upDirection, cameraWorldTransform);
	m3dInvertMatrix44(viewTransform, cameraWorldTransform);

	viewFrustum.SetPerspective(fov, (float)width / (float)height, zNear, zFar);
	projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
}

void PanoramaView::setZoomFlag(int flag)
{
	zoomFlag = flag;
}

void PanoramaView::zoomInStep()
{
	judgePanoMode();   

	if (fov <= fovMax)
		fov += 5;
	else
		return;

	//viewPosition[0] = viewDistance * cos(angleBeta) * sin(angleAlpha);
	//viewPosition[1] = viewDistance * sin(angleBeta);
	//viewPosition[2] = viewDistance * cos(angleBeta) * cos(angleAlpha);

	makeLookAt(viewPosition, targetPosition, upDirection, cameraWorldTransform);
	m3dInvertMatrix44(viewTransform, cameraWorldTransform);

	viewFrustum.SetPerspective(fov, (float)width / (float)height, zNear, zFar);
	projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
}

void PanoramaView::zoomOutStep()
{	
	judgePanoMode();

	if (fov >= fovMin)
		fov -= 5;
	else
		return;

	//viewPosition[0] = viewDistance * cos(angleBeta) * sin(angleAlpha);
	//viewPosition[1] = viewDistance * sin(angleBeta);
	//viewPosition[2] = viewDistance * cos(angleBeta) * cos(angleAlpha);

	makeLookAt(viewPosition, targetPosition, upDirection, cameraWorldTransform);
	m3dInvertMatrix44(viewTransform, cameraWorldTransform);

	viewFrustum.SetPerspective(fov, (float)width / (float)height, zNear, zFar);
	projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
}

void PanoramaView::resetPositoin()
{
	angleAlpha = 0;
	angleBeta = 0;
	viewDistance = 0.8;
	
	viewPosition[0] = 0;
	viewPosition[1] = 0;
	viewPosition[2] = 0.8;

	makeLookAt(viewPosition, targetPosition, upDirection, cameraWorldTransform);
	m3dInvertMatrix44(viewTransform, cameraWorldTransform);
}

void PanoramaView::judgePanoMode()
{
	if (panoMode == PANO_MODE::NORMAL) {
		fovMax = 90;
		fovMin = 40;
		maxViewAngle = 1.2;
		minViewAngle = -1.2;
		mouseSensitivity = 1.5;
	}
	else if (panoMode == PANO_MODE::PLANTET) {
		fovMax = 120;
		fovMin = 50;

		maxViewAngle = 1.5;
		minViewAngle = -1.5;
		mouseSensitivity = 2;
	}
	else if (panoMode == PANO_MODE::SPHERE) {
		fovMax = 160;
		fovMin = 50;

		maxViewAngle = 1.2;
		minViewAngle = -1.2;
		mouseSensitivity = 2.5;
	}
}

void PanoramaView::resetPanoMode(PANO_MODE mode)
{
	panoMode = mode;

	if (panoMode == PANO_MODE::NORMAL) {
		fovMax = 90;
		fovMin = 40;
		maxViewAngle = 1.2;
		minViewAngle = -1.2;

		viewPosition[0] = 0;
		viewPosition[1] = 0;
		viewPosition[2] = 1;
		fov = 60;

		mouseSensitivity = 1;

		angleAlpha = 0;
		angleBeta = 0;
		zNear = 0.1;
	}
	else if (panoMode == PANO_MODE::PLANTET) {
		fovMax = 120;
		fovMin = 50;
		maxViewAngle = 1.5;
		minViewAngle = -1.5;

		viewPosition[0] = 0.1;
		viewPosition[1] = 0.99;
		viewPosition[2] = 0.1;
		fov = 100;

		mouseSensitivity = 2;

		angleAlpha = 0.785;
		angleBeta = 1.428;

		zNear = 0.5;
	}
	else if (panoMode == PANO_MODE::SPHERE) {
		fovMax = 160;
		fovMin = 50;
		maxViewAngle = 1.2;
		minViewAngle = -1.2;

		viewPosition[0] = 0;
		viewPosition[1] = 0;
		viewPosition[2] = 1;
		fov = 130;

		mouseSensitivity = 2.5;

		angleAlpha = 0;
		angleBeta = 0;
		zNear = 0.5;
	}
}

void PanoramaView::runIntertia()
{
	if (isInertia == true) {

		float deltaX = lastDeltaX;
		float deltaY = lastDeltaY;
		
		if (fabs(deltaX) > inertiaSpeed || fabs(deltaY) > inertiaSpeed) {

			//TRACE("XXXXXXXXXXXX x is %f \n", deltaX);
			//TRACE("XXXXXXXXXXXX y is %f \n", deltaY);

			angleAlpha += deltaX / (float)width * 3.1415926 * mouseSensitivity;
			angleBeta -= deltaY / (float)height * 3.1415926 * mouseSensitivity;

			if (angleBeta > maxViewAngle)
				angleBeta = maxViewAngle;
			if (angleBeta < minViewAngle)
				angleBeta = minViewAngle;

			judgePanoMode();

			viewPosition[0] = viewDistance * cos(angleBeta) * sin(angleAlpha);
			viewPosition[1] = viewDistance * sin(angleBeta);
			viewPosition[2] = viewDistance * cos(angleBeta) * cos(angleAlpha);

			//if (panoMode == PANO_MODE::SPHERE || panoMode == PANO_MODE::NORMAL) {
			makeLookAt(viewPosition, targetPosition, upDirection, cameraWorldTransform);
			m3dInvertMatrix44(viewTransform, cameraWorldTransform);
			projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());

			if (lastDeltaX >= 0) {
				lastDeltaX -= inertiaSpeed;
			}
			else {
				lastDeltaX += inertiaSpeed;
			}

			if (lastDeltaY >= 0) {
				lastDeltaY -= inertiaSpeed;
			}
			else {
				lastDeltaY += inertiaSpeed;
			}
		}
		else {
			isInertia = false;
		}
	}
}

void PanoramaView::switchPanoMode(PANO_MODE mode)
{
	newPanoMode = mode;
	if (newPanoMode == PANO_MODE::NORMAL) {
		newViewPosition[0] = 0;
		newViewPosition[1] = 0;
		newViewPosition[2] = 1;
		newFov = 60;
		newNear = 0.1;
	}
	else if (newPanoMode == PANO_MODE::PLANTET) {
		newViewPosition[0] = 0.1;
		newViewPosition[1] = 0.99;
		newViewPosition[2] = 0.1;
		newFov = 100;
		newNear = 0.1;
	}
	else if (newPanoMode == PANO_MODE::SPHERE) {
		newViewPosition[0] = 0;
		newViewPosition[1] = 0;
		newViewPosition[2] = 1;
		newFov = 130;
		newNear = 0.5;
	}
	switchingMode = true;
}

void PanoramaView::switchingPanoMode()
{
	if (switchingMode == true) {
		if (fabs(viewPosition[0] - newViewPosition[0]) >= posStep) {
			if (viewPosition[0] - newViewPosition[0] > 0) {
				viewPosition[0] -= posStep;
			}
			else {
				viewPosition[0] += posStep;
			}
		}

		if (fabs(viewPosition[1] - newViewPosition[1]) >= posStep) {
			if (viewPosition[1] - newViewPosition[1] > 0) {
				viewPosition[1] -= posStep;
			}
			else {
				viewPosition[1] += posStep;
			}
		}

		if (fabs(viewPosition[2] - newViewPosition[2]) >= posStep) {
			if (viewPosition[2] - newViewPosition[2] > 0) {
				viewPosition[2] -= posStep;
			}
			else {
				viewPosition[2] += posStep;
			}
		}

		if (fabs(zNear - newNear) >= nearStep) {
			if (zNear - newNear > 0) {
				zNear -= nearStep;
			}
			else {
				zNear += nearStep;
			}
		}

		if (fov - newFov >= fovStep) {
			fov -= fovStep;
		}
		else if (newFov - fov >= fovStep){
			fov += fovStep;
		}

		makeLookAt(viewPosition, targetPosition, upDirection, cameraWorldTransform);
		m3dInvertMatrix44(viewTransform, cameraWorldTransform);
		viewFrustum.SetPerspective(fov, (float)width / (float)height, zNear, zFar);
		projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());

		if (fabs(viewPosition[0] - newViewPosition[0]) < posStep
			&& fabs(viewPosition[1] - newViewPosition[1]) < posStep
			&& fabs(viewPosition[2] - newViewPosition[2]) < posStep
			&& fabs(zNear - newNear) < nearStep
			&& fabs((float)newFov - (float)fov) < fovStep) 
		{
			switchingMode = false;
			resetPanoMode(newPanoMode);
		}
	}
}
