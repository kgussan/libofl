//グラフィックス
#ifndef __WINDOW_H__
#define __WINDOW_H__
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

class Window
{
	public:
		Window();
		virtual ~Window();

		void		init(void);
		void		term(void);

		uint32_t	GetWindowWidth(void);
		void		SetWindowWidth(uint32_t val);
		uint32_t	GetWindowHeight(void);
		void		SetWindowHeight(uint32_t val);

		uint32_t	GetWindowX(void );
		void		SetWindowX(uint32_t val);
		uint32_t	GetWindowY(void);
		void		SetWindowY(uint32_t val);

	private:
		uint32_t	m_window_width;
		uint32_t	m_window_height;
		uint32_t	m_window_x;
		uint32_t	m_window_y;

	protected:

};
#endif // HEADER

