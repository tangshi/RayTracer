#if defined(__APPLE__) || defined(MACOSX)
#include <OpenCL/OpenCL.h>
#include <GLUT/GLUT.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>

#pragma comment (lib, "opengl32.lib")
#pragma comment (lib, "glu32.lib")
#pragma comment (lib, "glut32.lib")

#endif

#include <cstdlib>
#include <unistd.h>
#include <iostream>
using namespace std;

#include "common.h"
#include "surface.h"
#include "raytracer.h"
#include "scene.h"



void Initialize();	// 初始化应用程序
void DrawFunc();	// glut描绘回调函数
void ReshapeFunc(int, int);	// glut窗口重置回调函数
void KeyboardFunc(unsigned char, int, int);	// glut键盘回调函数


// 窗口尺寸
int g_WindowWidth = 800;
int g_WindowHeight = 600;

Raytracer::Surface* surface = 0;
Pixel* buffer = 0;
Raytracer::Engine* tracer = 0;


int main(int argc, char * argv[])
{
    
    // 初始化
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    
	// 创建window
	glutInitWindowPosition(240, 200);
	glutInitWindowSize(g_WindowWidth, g_WindowHeight);
	glutCreateWindow("Ray Tracing");
	
	// 初始化应用程序
	Initialize() ;
	
	// 设定glut回调函数
	glutDisplayFunc(DrawFunc);
	glutReshapeFunc(ReshapeFunc);
    glutKeyboardFunc(KeyboardFunc);
    
	// 进入glut事件处理循环
	glutMainLoop();
    
    return 0;
}

// 初始化应用程序
void Initialize()
{
	// 设置清屏颜色
	glClearColor (0.0, 0.0, 0.0, 0.0);
    
	// 设置视口，投影信息
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
    
    surface = new Raytracer::Surface( g_WindowWidth, g_WindowHeight );
    buffer = surface->GetBuffer();
    surface->Clear(0);  //整个位图设为黑色
    
    tracer = new Raytracer::Engine();
    tracer->GetScene()->InitScene();
    tracer->SetTarget( surface->GetBuffer(), g_WindowWidth, g_WindowHeight );
    
}


// 描绘函数
void DrawFunc()
{
    clock_t time = clock();
    
	// 清屏
	glClear (GL_COLOR_BUFFER_BIT);
    
    // 左下角开始坐标
    glWindowPos2i(0, 0);
    
    tracer->InitRender();
    
    tracer->Render();
    
    glDrawPixels(g_WindowWidth, g_WindowHeight, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, buffer);
    
    time = clock() - time;
    cout << "Time used: " << (double)time/CLOCKS_PER_SEC << endl;
    
    glFlush();

}

// 响应窗口重置事件
void ReshapeFunc(int width, int height)
{
	g_WindowWidth = width;
	g_WindowHeight = height;
	glViewport(0, 0, g_WindowWidth, g_WindowHeight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}


void KeyboardFunc(unsigned char key, int x, int y)
{
    switch (key) {
            // Q 键退出程序
        case 'Q':
        case 'q':
            exit(0);
            break;
            
        default:
            break;
    }
}



