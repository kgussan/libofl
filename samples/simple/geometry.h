//グラフィックス
#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__
#include <stdio.h>
#include <stdlib.h>
#include "kgstdint.h"
#include <GL/glut.h>
#include "ofl.h"
#include "macro.h"
using namespace nsOfl;
class Geometry
{
public:
	enum{
		GEOM_MODEL_KUMA=0,
		GEOM_MODEL_KUMA2,
		GEOM_MODEL_BOX,
		GEOM_MODEL_MAX,
	};
	char mGeomModelName[GEOM_MODEL_MAX][1024];

	Geometry();
	virtual ~Geometry();
	void		init(void);
	void		term(void);
	oflModel_t*	GetModel(uint32_t val);
	oflModel_t*	GetModel(void);
	void		SetModel(uint32_t index,const char *filename);
	void		SetModelName(uint32_t index,const char *name);
	const char*		GetModelName(uint32_t index);
	const char*		GetModelName();
	void		SetModelIndex(uint32_t val);
	uint32_t 	GetModelIndex(void);
	void		SetMaterialMode(uint32_t val);
	uint32_t 	GetMaterialMode(void);
	void		IncreMaterialMode(void);
	void		DecreMaterialMode(void);
	void		SetScale(double val);
	double		GetScale(void);
	void		SetSmoothingAngle(double val);
	double		GetSmoothingAngle(void);
	void		SetWeldDistance(double val);
	double		GetWeldDistance(void);
private:
	float			m_scale;						// original scale factor
	float			m_smoothing_angle;			// smoothing angle
	double			m_weld_distance;			// epsilon for welding vertices
	uint32_t		m_material_mode;			/// 0=none, 1=color, 2=material
	uint32_t		m_model_index;
	char			**mModelFilename;
	//char			*cGeometryFile2;
	oflModel_t		**oflModel;					// ofl model data structure 
	//char			*g_cModelFile = NULL;		// name of the obect file 
protected:
};
#endif // HEADER
