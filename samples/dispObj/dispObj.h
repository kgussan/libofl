//@file
//@brief
//wavefront obj file loader
//@par 概要
//-オブジェクトファイルローダ
//-obj ファイルモデルデータの読み込み
#ifndef __OBJLOADER_H__
#define __OBJLOADER_H__

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <stdint.h>
#include "util/kgstdint.h"
#include <assert.h>
#include <stdarg.h>
#include <GL/glut.h>
#include "gltb.h"
#include "ofl.h"
#include "util/ofl_ppm.h"
#include "util/ofl_opengl.h"
#define READ_MODELS_FROM_MENU 1
//#define _WIN32
#define OBJLOADER_DEBUG 0
#define OBJLOADER_TRACE 0
#if OBJLOADER_DEBUG
#	define dprintf(x) printf("d %s %s %d:",__FILE__,__FUNCTION__,__LINE__);printf x
#else
#	define dprintf(x)
#endif
#if OBJLOADER_TRACE
#	define tprintf(x) printf("t %s %s %d:",__FILE__,__FUNCTION__,__LINE__);printf x
#else
#	define tprintf(x)
#endif
#if defined(_WIN32)
#	include <sys/timeb.h>
#else
#	include <limits.h>
#	include <unistd.h>
#	include <sys/types.h>
#	include <sys/times.h>
#endif

#define PROGRAM_NAME "objloader"
using namespace nsOfl;
#define  CLK_TCK 1000
//static const uint32_t CLK_TCK=1000;
static inline float Elapsed(void)
{
	static long begin=0;
	static long finish,difference;
#	if defined(_WIN32)
	static struct timeb tb;
	ftime(&tb);
	finish=(long)tb.time*1000+(long)tb.millitm;
#	else
	static struct tms tb;
	finish=times(&tb);
#	endif
	difference=finish-begin;
	begin=finish;
	return (float)difference/(float)CLK_TCK;
}
static const char gDataDirName[1024]="../../data/";
static const uint32_t OFL_MODEL_MAX=16;
enum{
	MODEL1=0,
	MODEL2,
	MODELBG,
};
struct GraphicsState_t{
	int iWindowWidth;
	int iWindowHeight;
	float asp;
	//char **u8ModelFilename;
	char **u8ModelFilename;
	//char *cGeometryFile2;
	bool bDispStatus;
	//char *g_cModelFile=NULL;// name of the obect file 
	int iModelList;// display list for object
	uint32_t u32ModelTarget;// ofl model 
	oflModel_t **model;// ofl model data structure 
	float fScale;// original scale factor
	float fSmoothingAngle;// smoothing angle
	float fWeldDistance;// epsilon for welding vertices
	bool bDrawFacetNormal;// draw with facet normal?
	bool bDispBoundingBox;// bounding box on?
	bool bDispPerformance;// performance counter on?
	//bool g_DispStatus=false;// statistics on?
	unsigned int uiMaterialMode;// 0=none,1=color,2=material
	int iEntries;// g_gs.iEntries in model menu
	double fTranX;
	double fTranY;
	double fTranZ;
};
int objloaderMain(int argc,char **argv);
void Init(void);
//graphics
void InitGraphicsState(GraphicsState_t *gs);
void CleanAll(GraphicsState_t *gs);
void CleanGraphicsState(GraphicsState_t *gs);
void CleanExit(GraphicsState_t *gs);
void DrawShadowText(int x,int y,char *s);

#endif //__OBJLOADER_H__
