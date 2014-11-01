#ifndef I_SURFACE_H
#define I_SURFACE_H

#include <string.h>
#include "common.h"

namespace Raytracer {
    
    class Surface
    {
        enum
        {
            OWNER = 1
        };
        
    public:
        Surface( int a_Width, int a_Height );
        Surface( char* a_File );
        ~Surface();
        
        Pixel* GetBuffer() { return m_Buffer; }
        int GetWidth() { return m_Width; }
        int GetHeight() { return m_Height; }
        void Clear( Pixel a_Color );
        
    private:
        Pixel* m_Buffer;
        int m_Width, m_Height;
	
    };
    
}; // namespace Raytracer

#endif