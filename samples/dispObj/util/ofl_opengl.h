#ifndef __OFL_OPENGL_H__
#define __OFL_OPENGL_H__
//#include <stdint.h>
#include "kgstdint.h"

#include <stdlib.h>
#include <GL/glut.h>
#include <assert.h>
#include <stdio.h>
#include "ofl.h"
//#include "ofl_type.h"
//#include "ofl_macro.h"
#define OFL_OPENGL_DEBUG 0
#define OFL_OPENGL_TRACE 0
#if (OFL_OPENGL_DEBUG==1)
#	define dprintf(x) printf("d %s %s %d:",__FILE__,__FUNCTION__,__LINE__);printf x
#else
#	define dprintf(x)
#endif
#if (OFL_OPENGL_TRACE==1)
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

namespace nsOfl {
///	Renders the model to the current OpenGL context using the mode specified.
///	OpenGL oriented function
///	-	model - initialized OFLmodel structure
///	-	mode  - a bitwise OR of values describing what is to be rendered.
///	-	OFL_NONE    - render with only vertices
///	-	OFL_FLAT    - render with facet normals
///	-	OFL_SMOOTH  - render with vertex normals
///	-	OFL_TEXTURE - render with texture coords
///	-	OFL_COLOR   - render with colors (color material)
///	-	OFL_MATERIAL- render with materials
///	-	OFL_COLOR and OFL_MATERIAL should not both be specified.  
///	-	OFL_FLAT and OFL_SMOOTH should not both be specified.  
void oflDraw(oflModel_t *model,uint32_t mode);
///	Generates and returns a display list for the model using the mode specified.
///	-	model - initialized OFLmodel structure
///	-	mode  - a bitwise OR of values describing what is to be rendered.
///	-	OFL_NONE    - render with only vertices
///	-	OFL_FLAT    - render with facet normals
///	-	OFL_SMOOTH  - render with vertex normals
///	-	OFL_TEXTURE - render with texture coords
///	-	OFL_COLOR   - render with colors (color material)
///	-	OFL_MATERIAL- render with materials
///	-	OFL_COLOR and OFL_MATERIAL should not both be specified.  
///	-	OFL_FLAT and OFL_SMOOTH should not both be specified.  
//uint32_t oflList(OFLmodel *model, unsigned long mode);
uint32_t oflList(oflModel_t *model,uint32_t listnum,uint32_t mode);

} //nsOfl

#endif
