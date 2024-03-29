#include <string.h>
#include "common.h"
#include "scene.h"
#include "raytracer.h"

namespace Raytracer {
    
    
    void Primitive::SetName( const char* a_Name )
    {
        delete m_Name;
        m_Name = new char[strlen( a_Name ) + 1];
        strcpy( m_Name, a_Name );
    }
    
    
    
    Material::Material() :
	m_Color( Color( 0.2f, 0.2f, 0.2f ) ),
	m_Refl( 0 ), m_Diff( 0.2f )
    {
    }
    
    
    int Sphere::Intersect( Ray& a_Ray, float& a_Dist )
    {
        vector3 v = a_Ray.GetOrigin() - m_Centre;
        float b = -DOT( v, a_Ray.GetDirection() );
        float det = (b * b) - DOT( v, v ) + m_SqRadius;
        int retval = MISS;
        if (det > 0)
        {
            det = sqrtf( det );
            float i1 = b - det;
            float i2 = b + det;
            if (i2 > 0)
            {
                if (i1 < 0)
                {
                    if (i2 < a_Dist)
                    {
                        a_Dist = i2;
                        retval = INPRIM;
                    }
                }
                else
                {
                    if (i1 < a_Dist)
                    {
                        a_Dist = i1;
                        retval = HIT;
                    }
                }
            }
        }
        return retval;
    }
    
    
    
    int PlanePrim::Intersect( Ray& a_Ray, float& a_Dist )
    {
        float d = DOT( m_Plane.N, a_Ray.GetDirection() );
        if (d != 0)
        {
            float dist = -(DOT( m_Plane.N, a_Ray.GetOrigin() ) + m_Plane.D) / d;
            if (dist > 0)
            {
                if (dist < a_Dist)
                {
                    a_Dist = dist;
                    return HIT;
                }
            }
        }
        return MISS;
    }
    
    vector3 PlanePrim::GetNormal( vector3& a_Pos )
    {
        return m_Plane.N;
    }
    
    
    
    Scene::~Scene()
    {
        delete m_Primitive;
    }
    
    void Scene::InitScene()
    {
        m_Primitive = new Primitive*[5];
        
        // 底面
        m_Primitive[0] = new PlanePrim( vector3( 0, 1, 0 ), 4.4f );
        m_Primitive[0]->SetName( "plane" );
        m_Primitive[0]->GetMaterial()->SetReflection( 0.5 );
        m_Primitive[0]->GetMaterial()->SetDiffuse( 1.0f );
        m_Primitive[0]->GetMaterial()->SetColor( Color( 0.8f, 0.8f, 0.0f ) );
        
        // 大球
        m_Primitive[1] = new Sphere(vector3( 1, -0.8f, 3 ), 2.5f );
        m_Primitive[1]->SetName( "big sphere" );
        m_Primitive[1]->GetMaterial()->SetReflection( 0.6f );
        m_Primitive[1]->GetMaterial()->SetColor( Color( 0.7f, 0.2f, 0.1f ) );
        
        // 小球
        m_Primitive[2] = new Sphere( vector3( -5.5f, -0.5, 7 ), 2 );
        m_Primitive[2]->SetName( "small sphere" );
        m_Primitive[2]->GetMaterial()->SetReflection( 1.0f );
        m_Primitive[2]->GetMaterial()->SetDiffuse( 0.1f );
        m_Primitive[2]->GetMaterial()->SetColor( Color( 0.2f, 0.7f, 0.8f ) );
        
        // 光源1
        m_Primitive[3] = new Sphere( vector3( 0, 5, 5 ), 0.1f );
        m_Primitive[3]->Light( true );
        m_Primitive[3]->GetMaterial()->SetColor( Color( 0.8f, 0.8f, 0.0f ) );
        
        // 光源2
        m_Primitive[4] = new Sphere( vector3( 2, 5, 1 ), 0.1f );
        m_Primitive[4]->Light( true );
        m_Primitive[4]->GetMaterial()->SetColor( Color( 0.6f, 0.6f, 0.6f ) );


        // 设置物体数量
        m_Primitives = 5;
    }
    
};
