//グラフィックス
#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__
#include <stdio.h>
#include <stdlib.h>

#ifdef _MSC_VER
#	include "kgstdint.h"
#else
#	include <stdint.h> 
#endif

#include <GL/glut.h>
#include "ofl.h"
#include "macro.h"
#include "geometry.h"
#include "window.h"
class Graphics
{
public:
	Graphics();
	virtual ~Graphics();
	Geometry	mGeom;
	Window		mWnd;
	void		init(void);
	void		term(void);
	void		CleanGraphicsState( void );
	void		Draw(oflModel_t* model);
	void		oflDraw(oflModel_t *model,uint32_t mode);
	uint32_t	GetMenuEntries( void );
	void		SetMenuEntries( uint32_t entries );
	bool		GetDrawFacetNormalEnable( void );
	bool		GetDispBoundingBoxEnable( void );
	bool		GetDispStatusEnable( void );
	bool		GetDispPerformanceEnable( void );
private:
	bool		m_draw_facet_normal_enable;	// draw with facet normal?
	bool		m_disp_status_enable;		/// 状態表示
	bool		m_disp_bounding_box_enable;	/// bounding box on?
	bool		m_disp_performance_enable;	/// performance counter on?
	int			m_menu_entries;					// g_gs.iEntries in model menu
protected:
};
#endif // HEADER

