#include <string>
#include <unistd.h>
#include "raytracer.h"
#include "scene.h"
#include "common.h"

namespace Raytracer {
    
    Ray::Ray( const vector3& a_Origin, const vector3& a_Dir ) :
	m_Origin( a_Origin ),
	m_Direction( a_Dir )
    {
    }
    
    Engine::Engine()
    {
        m_Scene = new Scene();
    }
    
    Engine::~Engine()
    {
        delete m_Scene;
    }
    
    
    void Engine::SetTarget( Pixel* a_Dest, int a_Width, int a_Height )
    {
        // 设置像素缓存的地址和大小
        m_Dest = a_Dest;
        m_Width = a_Width;
        m_Height = a_Height;
    }
    
    
    Primitive* Engine::Raytrace( Ray& a_Ray, Color& a_Acc, int a_Depth, float a_RIndex, float& a_Dist )
    {
        // 测试深度值，防止无限循环
        if (a_Depth > TRACEDEPTH) return 0;
        
        // 初始距离设置为一个极大值
        a_Dist = 1000000.0f;
        vector3 pi;
        Primitive* prim = 0;
        int result;
        // 找到最近的相交点
        for ( int s = 0; s < m_Scene->GetNrPrimitives(); s++ )
        {
            Primitive* pr = m_Scene->GetPrimitive( s );
            int res = pr->Intersect( a_Ray, a_Dist );
            if ( res )
            {
                prim = pr;
                result = res;
            }
        }
        // 没有交点，该射线结束投射
        if (!prim) return 0;
        // 有交点
        if (prim->IsLight())
        {
            // 若射到的是光源，结束投射，得到的光颜色为白色
            a_Acc = Color( 1, 1, 1 );
        }
        else // 计算该点最终颜色
        {
            pi = a_Ray.GetOrigin() + a_Ray.GetDirection() * a_Dist; // P = E + t*D，表征交点
            // 跟踪光线
            for ( int l = 0; l < m_Scene->GetNrPrimitives(); l++ )
            {
                Primitive* p = m_Scene->GetPrimitive( l );
                if (p->IsLight()) // 若是光源，计算该光源对本交点的影响
                {
                    Primitive* light = p;
                    float shade = 1.0f;
                    // 计算阴影
                    if (light->GetType() == Primitive::SPHERE)
                    {
                        /*构造阴影射线*/
                        vector3 L = ((Sphere*)light)->GetCentre() - pi;
                        float tdist = LENGTH( L );
                        L *= (1.0f / tdist);
                        Ray r = Ray( pi + L * EPSILON, L );
                        
                        for ( int s = 0; s < m_Scene->GetNrPrimitives(); s++ )
                        {
                            Primitive* pr = m_Scene->GetPrimitive( s );
                            if ((pr != light) && (pr->Intersect( r, tdist )))
                            {
                                shade = 0;
                                break;
                            }
                        }
                    }
                    // 计算漫反射
                    vector3 L = ((Sphere*)light)->GetCentre() - pi;
                    NORMALIZE( L );
                    vector3 N = prim->GetNormal( pi );
                    if (prim->GetMaterial()->GetDiffuse() > 0)
                    {
                        float dot = DOT( L, N );
                        if (dot > 0)
                        {
                            float diff = dot * prim->GetMaterial()->GetDiffuse() * shade;
                            // 叠加漫反射效果
                            Color c1 = light->GetMaterial()->GetColor();
                            Color c2 = prim->GetMaterial()->GetColor();
                            a_Acc = a_Acc + diff * c1 * c2;
                        }
                    }
                    // 计算镜面反射
                    if (prim->GetMaterial()->GetSpecular() > 0)
                    {
                        vector3 V = a_Ray.GetDirection();
                        vector3 R = L - 2.0f * DOT( L, N ) * N;
                        float dot = DOT( V, R );
                        if (dot > 0)
                        {
                            float spec = powf( dot, 20 ) * prim->GetMaterial()->GetSpecular() * shade;
                            // 叠加镜面反射效果
                            a_Acc = a_Acc + spec * light->GetMaterial()->GetColor();
                        }
                    }
                }
            } //  结束循环
            // 计算反射光
            float refl = prim->GetMaterial()->GetReflection();
            if (refl > 0.0f)
            {
                vector3 N = prim->GetNormal( pi );
                vector3 R = a_Ray.GetDirection() - 2.0f * DOT( a_Ray.GetDirection(), N ) * N;
                if (a_Depth < TRACEDEPTH) // 若深度未达到阈值，继续计算
                {
                    Color rcol( 0, 0, 0 );
                    float dist;
                    Ray r = Ray( pi + R * EPSILON, R );
                    Raytrace( r, rcol, a_Depth + 1, a_RIndex, dist );
                    Color c = prim->GetMaterial()->GetColor();
                    a_Acc = a_Acc + refl * rcol * c;
                }
            }
        }
        
        return prim;
    }
    

    // 初始化渲染环境
    void Engine::InitRender()
    {
        // 初始渲染位置
        m_CurrLine = 0;
        m_PPos = 0;

        // 设置屏幕坐标
        m_WX1 = -4, m_WX2 = 4, m_WY1 = m_SY = -3, m_WY2 = 3;
        // 计算增量
        m_DX = (m_WX2 - m_WX1) / m_Width;
        m_DY = (m_WY2 - m_WY1) / m_Height;
        m_SY += 20 * m_DY;
        
        // 分配内存
        m_LastRow = new Primitive*[m_Width];
        memset( m_LastRow, 0, m_Width * 4 );
    }
    
    // 渲染
    bool Engine::Render()
    {
        // 视点位置
        vector3 o( 0, 0, -2.5 );
        
        // 逐像素渲染
        for ( int y = m_CurrLine; y < (m_Height - 20); y++ )
        {
            m_SX = m_WX1;
            // 渲染当前行所有像素
            for ( int x = 0; x < m_Width; x++ )
            {
                // 初始颜色为黑色
                Color acc( 0, 0, 0 );
                // 计算射线方向
                vector3 dir = vector3( m_SX, m_SY, 0 ) - o;
                NORMALIZE( dir );
                Ray r( o, dir );
                float dist;
                // 射线跟踪
                Raytrace( r, acc, 1, 1.0f, dist );
                int red = (int)(acc.r * 256);
                int green = (int)(acc.g * 256);
                int blue = (int)(acc.b * 256);
                if (red > 255) red = 255;
                if (green > 255) green = 255;
                if (blue > 255) blue = 255;
                m_Dest[m_PPos++] = (red << 16) + (green << 8) + blue;
                m_SX += m_DX;
            }
            m_SY += m_DY;
            m_CurrLine = y + 1;
            
        }
        
        return true;
    }
    
    
}; 

