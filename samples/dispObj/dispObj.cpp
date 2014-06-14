#include "dispObj.h"
static GraphicsState_t g_gs;
static uint32_t mouse_state;
static uint32_t mouse_button;
using namespace nsOfl;

//描画：OpenGL のディスプレイリストの作成
void GenerateLists(uint32_t target)
{
	dprintf(("target=%d\n",target));
	uint32_t listnum=1;
	float ambient[]={0.2f,0.2f,0.2f,1.0f};
	float diffuse[]={0.8f,0.8f,0.8f,1.0f};
	float specular[]={0.1f,0.1f,0.1f,1.0f};
	float shininess=65.0f;
	glMaterialfv(GL_FRONT,GL_AMBIENT,ambient);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,diffuse);
	glMaterialfv(GL_FRONT,GL_SPECULAR,specular);
	glMaterialf( GL_FRONT,GL_SHININESS,shininess);
	if(g_gs.iModelList){
		glDeleteLists(g_gs.iModelList,listnum);
	}
	dprintf(("g_gs.uiMaterialMode=%d\n",g_gs.uiMaterialMode));
	switch(g_gs.uiMaterialMode){
		case 0:
			if(g_gs.bDrawFacetNormal){
				g_gs.iModelList=oflList(g_gs.model[target],listnum,OFL_FLAT);
			}else{
				g_gs.iModelList=oflList(g_gs.model[target],listnum,OFL_SMOOTH);
			}
			break;
		case 1:
			if(g_gs.bDrawFacetNormal){
				g_gs.iModelList=oflList(g_gs.model[target],listnum,OFL_FLAT|OFL_COLOR);
			}else{
				g_gs.iModelList=oflList(g_gs.model[target],listnum,OFL_SMOOTH|OFL_COLOR);
			}
			break;
		case 2:
			if(g_gs.bDrawFacetNormal){
				g_gs.iModelList=oflList(g_gs.model[target],listnum,OFL_FLAT|OFL_MATERIAL);
			}else{
				g_gs.iModelList=oflList(g_gs.model[target],listnum,OFL_SMOOTH|OFL_MATERIAL);
			}
			break;
		default:
			perror("no such material mode.\n");
			assert(false);
	}
	return;
}

///OpenGL callback
///ウィンドウサイズ設定
void Reshape(int width,int height)
{
	tprintf(("begin\n"));
	dprintf(("size %dx%d \n",width,height));
	//float asp;
	float eye[3];
	eye[0]=0.0f-0.5f*(width-(float)g_gs.iWindowWidth);
	eye[1]=0.0f-0.5f*(height-(float)g_gs.iWindowHeight);
	eye[2]=-3.0f;
	gltbReshape(width,height);
	glViewport(0,0,width,height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f,(float)width/(float)height,0.1f,1024.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(g_gs.fTranX,g_gs.fTranY,g_gs.fTranZ);
	tprintf(("end\n"));
}

/// OpenGL callback
void Display(void)
{
	tprintf(("begin\n"));
	static const uint32_t NUM_FRAMES=5;
	static char s[256],t[32];
	static uint32_t frames=0;
	glClearColor(1.0f,1.0f,1.0f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glPushMatrix();
	glTranslatef(g_gs.fTranX,g_gs.fTranY,g_gs.fTranZ);
	gltbMatrix();
	float fSpace=0.9f;
	int iNums=1;
	g_gs.u32ModelTarget=MODEL1;
	GenerateLists(g_gs.u32ModelTarget);
	glCallList(g_gs.iModelList);
	glDisable(GL_LIGHTING);
	if(g_gs.bDispBoundingBox){
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
		glEnable(GL_CULL_FACE);
		glColor4f(1.0f,0.0f,0.0f,0.25f);
		glutSolidCube(2.0f);
		glDisable(GL_BLEND);
	}
	glPopMatrix();
	if(g_gs.bDispStatus){
		int height=glutGet(GLUT_WINDOW_HEIGHT);
		glColor3ub(0,0,0);
		sprintf(s,"%s\n%u vertices\n%u triangles\n%u normals\n%u texcoords\n%u groups\n%u materials",
				g_gs.model[g_gs.u32ModelTarget]->pathname,
				(unsigned int)g_gs.model[g_gs.u32ModelTarget]->numvertices,
				(unsigned int)g_gs.model[g_gs.u32ModelTarget]->numtriangles, 
				(unsigned int)g_gs.model[g_gs.u32ModelTarget]->numnormals,
				(unsigned int)g_gs.model[g_gs.u32ModelTarget]->numtexcoords,
				(unsigned int)g_gs.model[g_gs.u32ModelTarget]->numgroups,
				(unsigned int)g_gs.model[g_gs.u32ModelTarget]->nummaterials);
		DrawShadowText(5,height-(5+18*1),s);
	}
	//フレームレート表示
	frames++;
	if(frames>NUM_FRAMES){
		sprintf(t,"%g fps",frames/Elapsed());
		frames=0;
	}
	if(g_gs.bDispPerformance){
		DrawShadowText(5,5,t);
	}
	glutSwapBuffers();
	glEnable(GL_LIGHTING);
	tprintf(("end\n"));
	return;
}

/// OpenGL callback
void Keyboard(unsigned char key,int x,int y)
{
	(void)x;(void)y;
	GLint params[2];
	dprintf(("size %d \n",sizeof(GLint)));
	switch(key){
		case 'h':
			printf("help\n\n" );
			printf("w		 - Toggle wireframe/filled\n" );
			printf("c		 - Toggle culling\n" );
			printf("n		 - Toggle facet/smooth normal\n" );
			printf("b		 - Toggle bounding box\n" );
			printf("r		 - Reverse polygon winding\n" );
			printf("m		 - Toggle color/material/none mode\n" );
			printf("p		 - Toggle performance indicator\n" );
			printf("s/S		 - Scale model smaller/larger\n" );
			printf("t		 - Show model stats\n" );
			printf("o		 - Weld vertices in model\n" );
			printf("+/-		 - Increase/decrease smoothing angle\n" );
			printf("W		 - Write model to file( out.obj )\n" );
			printf("q/escape - Quit\n\n" );
			break;
		case 't':
			g_gs.bDispStatus=!g_gs.bDispStatus;
			break;
		case 'p':
			g_gs.bDispPerformance=!g_gs.bDispPerformance;
			break;
		case 'm':
			g_gs.uiMaterialMode++;
			if(g_gs.uiMaterialMode>2){
				g_gs.uiMaterialMode=0;
			}
			printf("g_gs.uiMaterialMode=%d\n",g_gs.uiMaterialMode);
			GenerateLists(g_gs.u32ModelTarget);
			break;
		case 'd':
			oflDelete(g_gs.model[g_gs.u32ModelTarget]);
			Init();
			GenerateLists(g_gs.u32ModelTarget);
			break;
		case 'w':
			glGetIntegerv(GL_POLYGON_MODE,params);
			if(params[0]==GL_FILL){
				glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
			}else{
				glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
			}
			break;
		case 'c':
			if(glIsEnabled(GL_CULL_FACE))
				glDisable(GL_CULL_FACE);
			else
				glEnable(GL_CULL_FACE);
			break;
		case 'b':
			g_gs.bDispBoundingBox=!g_gs.bDispBoundingBox;
			break;
		case 'n':
			g_gs.bDrawFacetNormal=!g_gs.bDrawFacetNormal;
			GenerateLists(g_gs.u32ModelTarget);
			break;
		case 'r':
			oflReverseWinding(g_gs.model[g_gs.u32ModelTarget]);
			GenerateLists(g_gs.u32ModelTarget);
			break;
		case 's':
			oflScale(g_gs.model[g_gs.u32ModelTarget],0.8f);
			GenerateLists(g_gs.u32ModelTarget);
			break;
		case 'S':
			oflScale(g_gs.model[g_gs.u32ModelTarget],1.25f);
			GenerateLists(g_gs.u32ModelTarget);
			break;
		case 'z':
			g_gs.fTranZ+=1.1f;
			GenerateLists(g_gs.u32ModelTarget);
			break;
		case 'Z':
			g_gs.fTranZ-=1.1f;
			GenerateLists(g_gs.u32ModelTarget);
			break;
		case 'o':
			oflVertexNormals(g_gs.model[g_gs.u32ModelTarget],g_gs.fSmoothingAngle);
			GenerateLists(g_gs.u32ModelTarget);
			break;
		case 'O':
			g_gs.fWeldDistance+=0.01f;
			printf("Welddistance:%.2f\n",g_gs.fWeldDistance);
			oflWeld(g_gs.model[g_gs.u32ModelTarget],g_gs.fWeldDistance);
			oflFacetNormals(g_gs.model[g_gs.u32ModelTarget]);
			oflVertexNormals(g_gs.model[g_gs.u32ModelTarget],g_gs.fSmoothingAngle);
			GenerateLists(g_gs.u32ModelTarget);
			break;
		case '-':
			g_gs.fSmoothingAngle-=1.0f;
			printf("Smoothingangle:%.1f\n",g_gs.fSmoothingAngle);
			oflVertexNormals(g_gs.model[g_gs.u32ModelTarget],g_gs.fSmoothingAngle);
			GenerateLists(g_gs.u32ModelTarget);
			break;
		case '+':
			g_gs.fSmoothingAngle+=1.0f;
			printf("Smoothingangle:%.1f\n",g_gs.fSmoothingAngle);
			oflVertexNormals(g_gs.model[g_gs.u32ModelTarget],g_gs.fSmoothingAngle);
			GenerateLists(g_gs.u32ModelTarget);
			break;
		case 'W':
			oflScale(g_gs.model[g_gs.u32ModelTarget],1.0f/g_gs.fScale);
			oflWriteObj(g_gs.model[g_gs.u32ModelTarget],"out.obj",OFL_SMOOTH|OFL_MATERIAL);
			break;
		case 'R':
			{
				uint32_t i;
				float swap;
				for(i=1;i<=g_gs.model[g_gs.u32ModelTarget]->numvertices;i++){
					swap=g_gs.model[g_gs.u32ModelTarget]->vertices[3*i+1];
					g_gs.model[g_gs.u32ModelTarget]->vertices[3*i+1]
						=g_gs.model[g_gs.u32ModelTarget]->vertices[3*i+2];
					g_gs.model[g_gs.u32ModelTarget]->vertices[3*i+2]
						=-swap;
				}
				oflFacetNormals(g_gs.model[g_gs.u32ModelTarget]);
				GenerateLists(g_gs.u32ModelTarget);
				break;
			}
		case 'q':
		case 27:
			CleanExit(&g_gs);
			exit(EXIT_SUCCESS);
			break;
	}
	glutPostRedisplay();
	return;
}

//メニュー
void Menu(int item)
{
	dprintf(("\n"));
	int i=0;
	(void)i;
	dprintf(("Menu\n"));
	if(item>0){
		Keyboard((unsigned char)item,0,0);
	}
	dprintf(("\n"));
	return;
}

//マウスコールバック
void Mouse(int button,int state,int x,int y)
{
	GLdouble model[4*4];
	GLdouble proj[4*4];
	GLint view[4];
	// fix for two-button mice -- left mouse + shift = middle mouse
	if(button==GLUT_LEFT_BUTTON&&(glutGetModifiers()&GLUT_ACTIVE_SHIFT)){
		button=GLUT_MIDDLE_BUTTON;
	}
	gltbMouse(button,state,x,y);
	mouse_state=state;
	mouse_button=button;
	if(state==GLUT_DOWN&&button==GLUT_MIDDLE_BUTTON){
		glGetDoublev(GL_MODELVIEW_MATRIX,model);
		glGetDoublev(GL_PROJECTION_MATRIX,proj);
		glGetIntegerv(GL_VIEWPORT,view);
		gluProject((GLdouble)x,(GLdouble)y,0.0,model,proj,view,&g_gs.fTranX,&g_gs.fTranY,&g_gs.fTranZ);
		gluUnProject((GLdouble)x,(GLdouble)y,g_gs.fTranZ,model,proj,view,&g_gs.fTranX,&g_gs.fTranY,&g_gs.fTranZ);
		g_gs.fTranY=-g_gs.fTranY;	///<Yをマウス入力から反転。GLは右手。
	}
	glutPostRedisplay();
	return;
}

//モーションコールバック
void Motion(int x,int y)
{
	tprintf(("begin\n"));
	double	model[4*4];
	double	proj[4*4];
	GLint	view[4];
	gltbMotion(x,y);
	if(mouse_state==GLUT_DOWN&&mouse_button==GLUT_MIDDLE_BUTTON){
		glGetDoublev(GL_MODELVIEW_MATRIX,model);
		glGetDoublev(GL_PROJECTION_MATRIX,proj);
		glGetIntegerv(GL_VIEWPORT,view);
		gluProject((GLdouble)x,(GLdouble)y,0.0,
					model,proj,view,
					&g_gs.fTranX,&g_gs.fTranY,&g_gs.fTranZ);
		gluUnProject((GLdouble)x,(GLdouble)y,g_gs.fTranZ,
					model,proj,view,
					&g_gs.fTranX,&g_gs.fTranY,&g_gs.fTranZ);
		g_gs.fTranY=-g_gs.fTranY;
	}
	glutPostRedisplay();
	tprintf(("end\n"));
	return;
}

//初期化
void Init(void)
{
	tprintf(("begin\n"));
	gltbInit(GLUT_LEFT_BUTTON);
	//モデル読み込み
	g_gs.model[MODEL1]=oflReadObj(g_gs.u8ModelFilename[MODEL1]);
	//debug
	/*
	for(int i=0;i<g_gs.model[MODEL1]->groups->numtriangles;i++){
		oflTriangle_t *triangle;
		triangle=&g_gs.model[MODEL1]->triangles[(g_gs.model[MODEL1]->groups->triangles[i])];
		//debug 
		printf("pos0 = %f %f %f \n",g_gs.model[MODEL1]->vertices[3*triangle->vindices[0]+0],
									g_gs.model[MODEL1]->vertices[3*triangle->vindices[0]+1],
									g_gs.model[MODEL1]->vertices[3*triangle->vindices[0]+2]);
		printf("pos1 = %f %f %f \n",g_gs.model[MODEL1]->vertices[3*triangle->vindices[1]+0],
									g_gs.model[MODEL1]->vertices[3*triangle->vindices[1]+1],
									g_gs.model[MODEL1]->vertices[3*triangle->vindices[1]+2]);
		printf("pos2 = %f %f %f \n",g_gs.model[MODEL1]->vertices[3*triangle->vindices[2]+0],
									g_gs.model[MODEL1]->vertices[3*triangle->vindices[2]+1],
									g_gs.model[MODEL1]->vertices[3*triangle->vindices[2]+2]);
	}
	*/
	//モデル整形
	g_gs.fScale=oflUnitize(g_gs.model[g_gs.u32ModelTarget]);
	oflFacetNormals(g_gs.model[g_gs.u32ModelTarget]);
	oflVertexNormals(g_gs.model[g_gs.u32ModelTarget],g_gs.fSmoothingAngle);
	if(g_gs.model[g_gs.u32ModelTarget]->nummaterials>0){
		g_gs.uiMaterialMode=2;
	}
	// create new display GenerateLists
	g_gs.u32ModelTarget=MODEL1;
	GenerateLists(g_gs.u32ModelTarget);
	g_gs.u32ModelTarget=MODEL1;
	g_gs.fTranX= 0.0f;
	g_gs.fTranY= 0.0f;
	g_gs.fTranZ=-2.0f;
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	tprintf(("end\n"));
	return;
}

///	グラフィックス処理の初期化
void InitGraphicsState(GraphicsState_t *gs)
{
	gs->asp=1.0f;
	gs->iWindowWidth=512;
	gs->iWindowHeight=512;
	gs->u32ModelTarget=MODEL1;
	gs->model=(oflModel_t **)malloc(OFL_MODEL_MAX*sizeof(oflModel_t));
	gs->u8ModelFilename=(char **)malloc(OFL_MODEL_MAX*(sizeof(char*)));
	for(size_t i=0;i<OFL_MODEL_MAX;i++){
		gs->u8ModelFilename[i]=(char*)malloc(sizeof(char)*1024);
	}
	//gs->cGeometryFile2=(char *)malloc(sizeof(char)*1024);
	sprintf(gs->u8ModelFilename[MODEL1],"%sbox.obj",gDataDirName);
	sprintf(gs->u8ModelFilename[MODEL2],"%skuma_tex.obj",gDataDirName);
	gs->bDispStatus=true;
	gs->iModelList=0;
	gs->fSmoothingAngle=90.0f;
	gs->fWeldDistance=0.00001f;
	gs->bDrawFacetNormal=false;
	gs->bDispBoundingBox=false;
	gs->bDispPerformance=true;
	gs->uiMaterialMode=0;
	gs->iEntries=0;
	gs->fTranX=0.0f;
	gs->fTranY=0.0f;
	gs->fTranZ=0.0f;
	return;
}

///	終了処理
void CleanExit(GraphicsState_t *gs)
{
	dprintf(("CleanExit\n"));
	CleanAll(gs);
	exit(EXIT_SUCCESS);
}

/// 全てのメモリ開放
void CleanAll(GraphicsState_t *gs)
{
	CleanGraphicsState(gs);
}

/// グラフィックス関係変数のメモリ開放
void CleanGraphicsState(GraphicsState_t *gs)
{
	tprintf(("start\n"));
	for(size_t i=0;i<OFL_MODEL_MAX;i++){
		free(gs->u8ModelFilename[i]);
	}
	free(gs->u8ModelFilename);
	free(gs->model);
	tprintf(("end\n"));
}

void DrawShadowText(int x,int y,char *s)
{
	tprintf(("begin\n"));
	int lines;
	char *p;
	glDisable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0,glutGet(GLUT_WINDOW_WIDTH),0,glutGet(GLUT_WINDOW_HEIGHT),-1,1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glColor3ub(0,0,0);
	glRasterPos2i(x+1,y-1);
	for(p=s,lines=0;*p;p++){
		if(*p=='\n'){
			lines++;
			///この場所をウィンドウサイズに依存するようにしたい。
			glRasterPos2i(x+1,y-1-(lines*18));
		}
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,*p);
	}
	glColor3ub(0,0x80,0xff);
	glRasterPos2i(x,y);
	for(p=s,lines=0;*p;p++){
		if(*p=='\n'){
			lines++;
			glRasterPos2i(x,y-(lines*18));
		}
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,*p);
	}
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glEnable(GL_DEPTH_TEST);
	tprintf(("end\n"));
}


//エントリポイント
int objloaderMain(int argc,char **argv)
{
	tprintf(("begin\n"));
	int iBufferingMethod=GLUT_DOUBLE;
	int models;
	InitGraphicsState(&g_gs);
	glutInitWindowSize(g_gs.iWindowWidth,g_gs.iWindowHeight);
	//glutInitWindowPosition(g_gs.iWindowX,g_gs.iWindowY);
	glutInit(&argc,argv);
	int modelIndex=0;
	while(--argc){
		if(strcmp(argv[argc],"-sb")==0){
			iBufferingMethod=GLUT_SINGLE;
		}else{
			sprintf(g_gs.u8ModelFilename[modelIndex],"%s",argv[argc]);
			//g_gs.u8ModelFilename[modelIndex]=argv[argc];
			modelIndex++;
			//g_gs.u8ModelFilename[g_gs.u32ModelTarget]=argv[argc];
			printf("Load:\"%s\"\n",argv[argc]);
		}
	}
	tprintf(("\n"));
	if(!g_gs.u8ModelFilename[g_gs.u32ModelTarget]){
		dprintf(("initialgeometry:%s\n",g_gs.u8ModelFilename[g_gs.u32ModelTarget]));
	}
	tprintf(("\n"));
	glutInitDisplayMode(GLUT_RGB|GLUT_DEPTH|iBufferingMethod);
	char title[256];
	sprintf(title, "%s %s %s",PROGRAM_NAME,__DATE__,__TIME__);
	glutCreateWindow(title);
	glutReshapeFunc(Reshape);
	glutDisplayFunc(Display);
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	models=glutCreateMenu(Menu);
	glutCreateMenu(Menu);
	glutAddSubMenu("Load models",models);
	glutAddMenuEntry("------",0);
	glutAddMenuEntry("[w] Toggle wireframe/filled",'w');
	glutAddMenuEntry("[c] Toggle culling on/off",'c');
	glutAddMenuEntry("[n] Toggle face/smooth normals",'n');
	glutAddMenuEntry("[b] Toggle bounding box on/off",'b');
	glutAddMenuEntry("[p] Toggle frame rate(performance)on/off",'p');
	glutAddMenuEntry("[t] Toggle model statistics",'t');
	glutAddMenuEntry("[m] Toggle color/material/none mode",'m');
	glutAddMenuEntry("[r] Reverse polygon winding",'r');
	glutAddMenuEntry("[s] Scale model smaller",'s');
	glutAddMenuEntry("[S] Scale model larger",'S');
	glutAddMenuEntry("[z] Move view z pos +",'s');
	glutAddMenuEntry("[Z] Move view z pos -",'S');
	glutAddMenuEntry("[o] Weld redundant vertices",'o');
	glutAddMenuEntry("[+] Increase smoothing angle",'+');
	glutAddMenuEntry("[-] Decrease smoothing angle",'-');
	glutAddMenuEntry("[W] Write model to file(out.obj)",'W');
	glutAddMenuEntry("-----",0);
	glutAddMenuEntry("[Esc/q] Quit",27);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	tprintf(("\n"));
	Init();
	tprintf(("\n"));
	glutMainLoop();
	tprintf(("exit\n"));
	return 0;
}
