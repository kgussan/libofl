#include <stdio.h>
#include <stdlib.h>

#ifdef _MSC_VER
#	include "kgstdint.h"
#else
#	include <stdint.h> 
#endif

#include "window.h"
//window class
///コンストラクタ
Window::Window(){
}
///デストラクタ
Window::~Window()
{
}
///初期化
void Window::init(void)
{
	glutInitWindowSize(512,512);
	//glutInitWindowPosition(128,128);
}
///後処理
void Window::term(void)
{
}
void Window::SetWindowWidth(uint32_t val){
	m_window_width=val;
}
void Window::SetWindowHeight(uint32_t val){
	m_window_height=val;
}

