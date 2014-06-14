#include <stdio.h>
#include <stdlib.h>

#ifdef _MSC_VER
#	include "kgstdint.h"
#else
#	include <stdint.h> 
#endif

#include <assert.h>
#include <stdarg.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include <GL/glut.h>

#include "macro.h"
#include "ofl.h"
#include "graphics.h"

Graphics gGraphics;

/// GL コールバック関数
void Reshape(int width,int height)
{
	LogTrace("begin\n");
	glViewport(0,0,width,height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//gluPerspective(60.0,(float)height/(float)width,1.0,128.0);
	gluPerspective(60.0,(float)height/(float)width,0.1,128.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0,0.0,-3.0);
	LogTrace("end\n");
}

/// GL コールバック関数
void Display(void)
{
	static float gltb_transform[4][4];
	glClearColor(1.0,1.0,1.0,1.0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glPushMatrix();
	//gGraphics.GenerateLists(gGraphics.mGeom.GetModelIndex());///<これは非効率かも。デバッグ
	//glCallList(gGraphics.GetModelList());//描画
	if(1){
		gGraphics.Draw(gGraphics.mGeom.GetModel(gGraphics.mGeom.GetModelIndex()));
	}else{
		for(unsigned int i=0;i<Geometry::GEOM_MODEL_MAX;i++){
			//glMatrixMode(GL_MODELVIEW);
			gGraphics.Draw(gGraphics.mGeom.GetModel(i));
			glTranslatef(0.1f,0.0f,0.0f);
		}
	}
	glDisable(GL_LIGHTING);
	if(gGraphics.GetDispBoundingBoxEnable()){
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
		glEnable(GL_CULL_FACE);
		glColor4f(1.0,0.0,0.0,0.25);
		glutSolidCube(2.0);
		glDisable(GL_BLEND);
	}
	glPopMatrix();
	glutSwapBuffers();
	glEnable(GL_LIGHTING);
}

/// GL コールバック関数
void Keyboard(unsigned char key,int x,int y)
{
	(void)x;(void)y;
	switch(key){
		case 'q':
		case 27:
			gGraphics.term();
			exit(EXIT_SUCCESS);
			break;
	}
	glutPostRedisplay();
}

/// GL コールバック関数
void Mouse(int button,int state,int x,int y)
{

}

/// GL コールバック関数
void Motion( int x, int y )
{

}

/// OpenGL メニューコールバック関数
void Menu(int item)
{
	int i=0;
	(void)i;
	LogDebug("Menu\n");
	if(item>0){
		Keyboard((unsigned char)item,0,0);
	}
	return;
}

///	初期化
void Init(int argc,char** argv)
{
	LogTrace("start\n");
	int iBufferingMethod=GLUT_DOUBLE;
	gGraphics.init();
	glutInit(&argc,argv);
	// モデル名称設定
	while(--argc){
		if(strcmp(argv[argc],"-sb")==0){
			///opengl single buffer mode
			iBufferingMethod=GLUT_SINGLE;
		}else{
			gGraphics.mGeom.SetModelName(0,argv[1]);
			//gGraphics.mGeom.SetModelName(argc,argv[argc]);
		}
	}
	if(!gGraphics.mGeom.GetModelName()){
		LogInfo("init geometry:%s\n",gGraphics.mGeom.GetModelName());
	}
	for(size_t i=0;i<Geometry::GEOM_MODEL_MAX;i++){
		gGraphics.mGeom.SetModel(i,gGraphics.mGeom.GetModelName(i));
	}
	glutInitDisplayMode(GLUT_RGB|GLUT_DEPTH|iBufferingMethod);
	char windowName[256];
	sprintf(windowName,"obj file loader %s %s",__DATE__,__TIME__);
	glutCreateWindow(windowName);
	LogTrace("end\n");
	return;
}

//メニュー初期化
void InitMenu(int models)
{
	LogTrace("init menu start \n");
	gGraphics.mGeom.SetScale(oflUnitize(gGraphics.mGeom.GetModel()));
	oflFacetNormals(gGraphics.mGeom.GetModel());
	oflVertexNormals(gGraphics.mGeom.GetModel(),gGraphics.mGeom.GetSmoothingAngle());
	if(gGraphics.mGeom.GetModel()->nummaterials>0){
		gGraphics.mGeom.SetMaterialMode(2);
	}
	// create new display GenerateLists
	for(size_t i=0;i<Geometry::GEOM_MODEL_MAX;i++){
		gGraphics.mGeom.SetModelIndex(i);
		//gGraphics.GenerateLists(i);
	}
	gGraphics.mGeom.SetModelIndex(Geometry::GEOM_MODEL_KUMA);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	LogTrace("init menu end \n");
	return;
}

void Idle(void)
{
	static float rotate=0.0f;
	rotate=0.2f;
	glRotatef(rotate,0.0f,1.0f,0.0f);
	//printf("rotate%03.2fdegree\n",fmod(rotate,3.141f)/(3.141f)*360.0f);
	glutPostRedisplay();
}

/// エントリポイント
int main(int argc,char** argv)
{
	LogTrace("begin\n");
	Init(argc,argv);
	glutReshapeFunc(Reshape);
	glutDisplayFunc(Display);
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glutIdleFunc(Idle);
	int models;
	models=glutCreateMenu(Menu);
	InitMenu(models);
	glutMainLoop();
	LogTrace("end\n");
	return EXIT_SUCCESS;
}

