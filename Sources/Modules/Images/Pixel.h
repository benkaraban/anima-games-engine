/*
 * Copyright (c) 2010, Anima Games, Benjamin Karaban, Laurent Schneider,
 * Jérémie Comarmond, Didier Colin.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * - Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef PIXEL_H_
#define PIXEL_H_

#include <Core/Math/MathTools.h>

namespace Images
{

//-----------------------------------------------------------------------------
/**
 * Les structure suivante font la description d'un type de pixel associe a un type d'image.
 * L'interet est de faciliter le parcourd du buffer image. voir iterator2D
 */
//-----------------------------------------------------------------------------
struct LM_API_IMG grayU8
{
    uint8 i;

    bool operator<(const grayU8& o) {return i<o.i;}
    bool operator>(const grayU8& o) {return i>o.i;}
    bool operator<=(const grayU8& o) {return i<=o.i;}
    bool operator>=(const grayU8& o) {return i>=o.i;}
    bool operator==(const grayU8& o) {return i==o.i;}
    bool operator!=(const grayU8& o) {return i!=o.i;}

    static const grayU8 MIN_VALUE;
    static const grayU8 MAX_VALUE;
};
//-----------------------------------------------------------------------------
struct LM_API_IMG rgbU8
{
    uint8  b,g,r;

    bool operator<(const rgbU8& o) {int32 val1 = r*r+b*b*r*r; int32 val2 = o.r*o.r+o.b*o.b*o.r*o.r; return  val1<val2;}
    bool operator>(const rgbU8& o) {int32 val1 = r*r+b*b*r*r; int32 val2 = o.r*o.r+o.b*o.b*o.r*o.r; return  val1>val2;}
    bool operator<=(const rgbU8& o) {int32 val1 = r*r+b*b*r*r; int32 val2 = o.r*o.r+o.b*o.b*o.r*o.r; return  val1<=val2;}
    bool operator>=(const rgbU8& o) {int32 val1 = r*r+b*b*r*r; int32 val2 = o.r*o.r+o.b*o.b*o.r*o.r; return  val1>=val2;}
    bool operator==(const rgbU8& o) {return r==o.r && g==o.g && b==o.b;}
    bool operator!=(const rgbU8& o) {return r!=o.r || g!=o.g || b!=o.b;}

    static const rgbU8 MIN_VALUE;
    static const rgbU8 MAX_VALUE;
};
//-----------------------------------------------------------------------------
struct LM_API_IMG argbU8
{
    uint8  b,g,r,a;

    bool operator<(const argbU8& o) {int32 val1 = a*a+r*r+b*b*r*r; int32 val2 = o.a*o.a+o.r*o.r+o.b*o.b*o.r*o.r; return  val1<val2;}
    bool operator>(const argbU8& o) {int32 val1 = a*a+r*r+b*b*r*r; int32 val2 = o.a*o.a+o.r*o.r+o.b*o.b*o.r*o.r; return  val1>val2;}
    bool operator<=(const argbU8& o) {int32 val1 = a*a+r*r+b*b*r*r; int32 val2 = o.a*o.a+o.r*o.r+o.b*o.b*o.r*o.r; return  val1<=val2;}
    bool operator>=(const argbU8& o) {int32 val1 = a*a+r*r+b*b*r*r; int32 val2 = o.a*o.a+o.r*o.r+o.b*o.b*o.r*o.r; return  val1>=val2;}
    bool operator==(const argbU8& o) {return a==o.a && r==o.r && g==o.g && b==o.b;}
    bool operator!=(const argbU8& o) {return a!=o.a || r!=o.r || g!=o.g || b!=o.b;}

    static const argbU8 MIN_VALUE;
    static const argbU8 MAX_VALUE;
};
//-----------------------------------------------------------------------------
struct LM_API_IMG grayU16
{
    uint16 i;

    bool operator<(const grayU16& o) {return i<o.i;}
    bool operator>(const grayU16& o) {return i>o.i;}
    bool operator<=(const grayU16& o) {return i<=o.i;}
    bool operator>=(const grayU16& o) {return i>=o.i;}
    bool operator==(const grayU16& o) {return i==o.i;}
    bool operator!=(const grayU16& o) {return i!=o.i;}

    static const grayU16 MIN_VALUE;
    static const grayU16 MAX_VALUE;
};
//-----------------------------------------------------------------------------
struct LM_API_IMG rgbU16
{
    uint16  b,g,r;

    bool operator<(const rgbU16& o) {int32 val1 = r*r+b*b*r*r; int32 val2 = o.r*o.r+o.b*o.b*o.r*o.r; return  val1<val2;}
    bool operator>(const rgbU16& o) {int32 val1 = r*r+b*b*r*r; int32 val2 = o.r*o.r+o.b*o.b*o.r*o.r; return  val1>val2;}
    bool operator<=(const rgbU16& o) {int32 val1 = r*r+b*b*r*r; int32 val2 = o.r*o.r+o.b*o.b*o.r*o.r; return  val1<=val2;}
    bool operator>=(const rgbU16& o) {int32 val1 = r*r+b*b*r*r; int32 val2 = o.r*o.r+o.b*o.b*o.r*o.r; return  val1>=val2;}
    bool operator==(const rgbU16& o) {return r==o.r && g==o.g && b==o.b;}
    bool operator!=(const rgbU16& o) {return r!=o.r || g!=o.g || b!=o.b;}

    static const rgbU16 MIN_VALUE;
    static const rgbU16 MAX_VALUE;
};
//-----------------------------------------------------------------------------
struct LM_API_IMG argbU16
{
    uint16  b,g,r,a;

    bool operator<(const argbU16& o) {int32 val1 = a*a+r*r+b*b*r*r; int32 val2 = o.a*o.a+o.r*o.r+o.b*o.b*o.r*o.r; return  val1<val2;}
    bool operator>(const argbU16& o) {int32 val1 = a*a+r*r+b*b*r*r; int32 val2 = o.a*o.a+o.r*o.r+o.b*o.b*o.r*o.r; return  val1>val2;}
    bool operator<=(const argbU16& o) {int32 val1 = a*a+r*r+b*b*r*r; int32 val2 = o.a*o.a+o.r*o.r+o.b*o.b*o.r*o.r; return  val1<=val2;}
    bool operator>=(const argbU16& o) {int32 val1 = a*a+r*r+b*b*r*r; int32 val2 = o.a*o.a+o.r*o.r+o.b*o.b*o.r*o.r; return  val1>=val2;}
    bool operator==(const argbU16& o) {return a==o.a && r==o.r && g==o.g && b==o.b;}
    bool operator!=(const argbU16& o) {return a!=o.a || r!=o.r || g!=o.g || b!=o.b;}

    static const argbU16 MIN_VALUE;
    static const argbU16 MAX_VALUE;
};
//-----------------------------------------------------------------------------
struct LM_API_IMG grayS16
{
    int16 i;

    bool operator<(const grayS16& o) {return i<o.i;}
    bool operator>(const grayS16& o) {return i>o.i;}
    bool operator<=(const grayS16& o) {return i<=o.i;}
    bool operator>=(const grayS16& o) {return i>=o.i;}
    bool operator==(const grayS16& o) {return i==o.i;}
    bool operator!=(const grayS16& o) {return i!=o.i;}

    static const grayS16 MIN_VALUE;
    static const grayS16 MAX_VALUE;
};
//-----------------------------------------------------------------------------
struct LM_API_IMG rgbS16
{
    int16  b,g,r;

    bool operator<(const rgbS16& o) {int32 val1 = r*r+b*b*r*r; int32 val2 = o.r*o.r+o.b*o.b*o.r*o.r; return  val1<val2;}
    bool operator>(const rgbS16& o) {int32 val1 = r*r+b*b*r*r; int32 val2 = o.r*o.r+o.b*o.b*o.r*o.r; return  val1>val2;}
    bool operator<=(const rgbS16& o) {int32 val1 = r*r+b*b*r*r; int32 val2 = o.r*o.r+o.b*o.b*o.r*o.r; return  val1<=val2;}
    bool operator>=(const rgbS16& o) {int32 val1 = r*r+b*b*r*r; int32 val2 = o.r*o.r+o.b*o.b*o.r*o.r; return  val1>=val2;}
    bool operator==(const rgbS16& o) {return r==o.r && g==o.g && b==o.b;}
    bool operator!=(const rgbS16& o) {return r!=o.r || g!=o.g || b!=o.b;}

    static const rgbS16 MIN_VALUE;
    static const rgbS16 MAX_VALUE;
};
//-----------------------------------------------------------------------------
struct LM_API_IMG argbS16
{
    int16  b,g,r,a;

    bool operator<(const argbS16& o) {int32 val1 = a*a+r*r+b*b*r*r; int32 val2 = o.a*o.a+o.r*o.r+o.b*o.b*o.r*o.r; return  val1<val2;}
    bool operator>(const argbS16& o) {int32 val1 = a*a+r*r+b*b*r*r; int32 val2 = o.a*o.a+o.r*o.r+o.b*o.b*o.r*o.r; return  val1>val2;}
    bool operator<=(const argbS16& o) {int32 val1 = a*a+r*r+b*b*r*r; int32 val2 = o.a*o.a+o.r*o.r+o.b*o.b*o.r*o.r; return  val1<=val2;}
    bool operator>=(const argbS16& o) {int32 val1 = a*a+r*r+b*b*r*r; int32 val2 = o.a*o.a+o.r*o.r+o.b*o.b*o.r*o.r; return  val1>=val2;}
    bool operator==(const argbS16& o) {return a==o.a && r==o.r && g==o.g && b==o.b;}
    bool operator!=(const argbS16& o) {return a!=o.a || r!=o.r || g!=o.g || b!=o.b;}

    static const argbS16 MIN_VALUE;
    static const argbS16 MAX_VALUE;
};
//-----------------------------------------------------------------------------
struct LM_API_IMG grayS32
{
    int32 i;

    bool operator<(const grayS32& o) {return i<o.i;}
    bool operator>(const grayS32& o) {return i>o.i;}
    bool operator<=(const grayS32& o) {return i<=o.i;}
    bool operator>=(const grayS32& o) {return i>=o.i;}
    bool operator==(const grayS32& o) {return i==o.i;}
    bool operator!=(const grayS32& o) {return i!=o.i;}

    static const grayS32 MIN_VALUE;
    static const grayS32 MAX_VALUE;
};
//-----------------------------------------------------------------------------
struct LM_API_IMG rgbS32
{
    int32  b,g,r;

    bool operator<(const rgbS32& o) {int32 val1 = r*r+b*b*r*r; int32 val2 = o.r*o.r+o.b*o.b*o.r*o.r; return  val1<val2;}
    bool operator>(const rgbS32& o) {int32 val1 = r*r+b*b*r*r; int32 val2 = o.r*o.r+o.b*o.b*o.r*o.r; return  val1>val2;}
    bool operator<=(const rgbS32& o) {int32 val1 = r*r+b*b*r*r; int32 val2 = o.r*o.r+o.b*o.b*o.r*o.r; return  val1<=val2;}
    bool operator>=(const rgbS32& o) {int32 val1 = r*r+b*b*r*r; int32 val2 = o.r*o.r+o.b*o.b*o.r*o.r; return  val1>=val2;}
    bool operator==(const rgbS32& o) {return r==o.r && g==o.g && b==o.b;}
    bool operator!=(const rgbS32& o) {return r!=o.r || g!=o.g || b!=o.b;}

    static const rgbS32 MIN_VALUE;
    static const rgbS32 MAX_VALUE;
};
//-----------------------------------------------------------------------------
struct LM_API_IMG argbS32
{
    int32  b,g,r,a;

    bool operator<(const argbS32& o) {int32 val1 = a*a+r*r+b*b*r*r; int32 val2 = o.a*o.a+o.r*o.r+o.b*o.b*o.r*o.r; return  val1<val2;}
    bool operator>(const argbS32& o) {int32 val1 = a*a+r*r+b*b*r*r; int32 val2 = o.a*o.a+o.r*o.r+o.b*o.b*o.r*o.r; return  val1>val2;}
    bool operator<=(const argbS32& o) {int32 val1 = a*a+r*r+b*b*r*r; int32 val2 = o.a*o.a+o.r*o.r+o.b*o.b*o.r*o.r; return  val1<=val2;}
    bool operator>=(const argbS32& o) {int32 val1 = a*a+r*r+b*b*r*r; int32 val2 = o.a*o.a+o.r*o.r+o.b*o.b*o.r*o.r; return  val1>=val2;}
    bool operator==(const argbS32& o) {return a==o.a && r==o.r && g==o.g && b==o.b;}
    bool operator!=(const argbS32& o) {return a!=o.a || r!=o.r || g!=o.g || b!=o.b;}

    static const argbS32 MIN_VALUE;
    static const argbS32 MAX_VALUE;
};
//-----------------------------------------------------------------------------
struct LM_API_IMG grayF32
{
    float i;

    bool operator<(const grayF32& o) {return i<o.i;}
    bool operator>(const grayF32& o) {return i>o.i;}
    bool operator<=(const grayF32& o) {return i<=o.i;}
    bool operator>=(const grayF32& o) {return i>=o.i;}
    bool operator==(const grayF32& o) {return i==o.i;}
    bool operator!=(const grayF32& o) {return i!=o.i;}

    static const grayF32 MIN_VALUE;
    static const grayF32 MAX_VALUE;
};
//-----------------------------------------------------------------------------
struct LM_API_IMG rgbF32
{
    float  b,g,r;

    bool operator<(const rgbF32& o) {float val1 = r*r+b*b*r*r; float val2 = o.r*o.r+o.b*o.b*o.r*o.r; return  val1<val2;}
    bool operator>(const rgbF32& o) {float val1 = r*r+b*b*r*r; float val2 = o.r*o.r+o.b*o.b*o.r*o.r; return  val1>val2;}
    bool operator<=(const rgbF32& o) {float val1 = r*r+b*b*r*r; float val2 = o.r*o.r+o.b*o.b*o.r*o.r; return  val1<=val2;}
    bool operator>=(const rgbF32& o) {float val1 = r*r+b*b*r*r; float val2 = o.r*o.r+o.b*o.b*o.r*o.r; return  val1>=val2;}
    bool operator==(const rgbF32& o) {return r==o.r && g==o.g && b==o.b;}
    bool operator!=(const rgbF32& o) {return r!=o.r || g!=o.g || b!=o.b;}

    static const rgbF32 MIN_VALUE;
    static const rgbF32 MAX_VALUE;
};
//-----------------------------------------------------------------------------
struct LM_API_IMG argbF32
{
    float  b,g,r,a;

    bool operator<(const argbF32& o) {float val1 = a*a+r*r+b*b*r*r; float val2 = o.a*o.a+o.r*o.r+o.b*o.b*o.r*o.r; return  val1<val2;}
    bool operator>(const argbF32& o) {float val1 = a*a+r*r+b*b*r*r; float val2 = o.a*o.a+o.r*o.r+o.b*o.b*o.r*o.r; return  val1>val2;}
    bool operator<=(const argbF32& o) {float val1 = a*a+r*r+b*b*r*r; float val2 = o.a*o.a+o.r*o.r+o.b*o.b*o.r*o.r; return  val1<=val2;}
    bool operator>=(const argbF32& o) {float val1 = a*a+r*r+b*b*r*r; float val2 = o.a*o.a+o.r*o.r+o.b*o.b*o.r*o.r; return  val1>=val2;}
    bool operator==(const argbF32& o) {return a==o.a && r==o.r && g==o.g && b==o.b;}
    bool operator!=(const argbF32& o) {return a!=o.a || r!=o.r || g!=o.g || b!=o.b;}

    static const argbF32 MIN_VALUE;
    static const argbF32 MAX_VALUE;
};
//-----------------------------------------------------------------------------
inline grayF32 operator*(const grayF32& p1, const grayF32& p2)
{
    grayF32 ret;
    ret.i = p1.i * p2.i;
    return ret;
}

inline grayF32 operator+(const grayF32& p1, const grayF32& p2)
{
    grayF32 ret;
    ret.i = p1.i + p2.i;
    return ret;
}

inline grayF32 operator*(const grayF32& p, const float& f)
{
    grayF32 ret;
    ret.i = p.i * f;
    return ret;
}
//-----------------------------------------------------------------------------
inline rgbF32 operator*(const rgbF32& p1, const rgbF32& p2)
{
    rgbF32 ret;
    ret.r = p1.r * p2.r;
    ret.g = p1.g * p2.g;
    ret.b = p1.b * p2.b;
    return ret;
}

inline rgbF32 operator+(const rgbF32& p1, const rgbF32& p2)
{
    rgbF32 ret;
    ret.r = p1.r + p2.r;
    ret.g = p1.g + p2.g;
    ret.b = p1.b + p2.b;
    return ret;
}

inline rgbF32 operator*(const rgbF32& p, const float& f)
{
    rgbF32 ret;
    ret.r = p.r * f;
    ret.g = p.g * f;
    ret.b = p.b * f;
    return ret;
}

inline rgbF32 operator*(const float& f, const rgbF32& p)
{
    rgbF32 ret;
    ret.r = p.r * f;
    ret.g = p.g * f;
    ret.b = p.b * f;
    return ret;
}
//-----------------------------------------------------------------------------

inline rgbU8 operator+(const rgbU8& p1, const rgbU8& p2)
{
    rgbU8 ret;
    ret.r = (uint8) Core::clampUInt8((int32)p1.r + (int32)p2.r);
    ret.g = (uint8) Core::clampUInt8((int32)p1.g + (int32)p2.g);
    ret.b = (uint8) Core::clampUInt8((int32)p1.b + (int32)p2.b);
    return ret;
}

inline rgbU8 operator*(const rgbU8& p, const float& f)
{
    rgbU8 ret;
    ret.r = (uint8) Core::clampUInt8(p.r * f);
    ret.g = (uint8) Core::clampUInt8(p.g * f);
    ret.b = (uint8) Core::clampUInt8(p.b * f);
    return ret;
}

inline rgbU8 operator*(const float& f, const rgbU8& p)
{
    rgbU8 ret;
    ret.r = (uint8) Core::clampUInt8(p.r * f);
    ret.g = (uint8) Core::clampUInt8(p.g * f);
    ret.b = (uint8) Core::clampUInt8(p.b * f);
    return ret;
}

//-----------------------------------------------------------------------------

inline argbU8 operator+(const argbU8& p1, const argbU8& p2)
{
    argbU8 ret;
    ret.a = (uint8) Core::clampUInt8((int32)p1.a + (int32)p2.a);
    ret.r = (uint8) Core::clampUInt8((int32)p1.r + (int32)p2.r);
    ret.g = (uint8) Core::clampUInt8((int32)p1.g + (int32)p2.g);
    ret.b = (uint8) Core::clampUInt8((int32)p1.b + (int32)p2.b);
    return ret;
}

inline argbU8 operator*(const argbU8& p, const float& f)
{
    argbU8 ret;
    ret.a = (uint8) Core::clampUInt8(p.a * f);
    ret.r = (uint8) Core::clampUInt8(p.r * f);
    ret.g = (uint8) Core::clampUInt8(p.g * f);
    ret.b = (uint8) Core::clampUInt8(p.b * f);
    return ret;
}

inline argbU8 operator*(const float& f, const argbU8& p)
{
    argbU8 ret;
    ret.a = (uint8) Core::clampUInt8(p.a * f);
    ret.r = (uint8) Core::clampUInt8(p.r * f);
    ret.g = (uint8) Core::clampUInt8(p.g * f);
    ret.b = (uint8) Core::clampUInt8(p.b * f);
    return ret;
}
 /**
  * L'union Pixel permet de stocker la valeur de tous type de pixel
  */
union LM_API_IMG Pixel
{
    grayU8 as_grayU8;
    rgbU8 as_rgbU8;
    argbU8 as_argbU8;

    grayU16 as_grayU16;
    rgbU16 as_rgbU16;
    argbU16 as_argbU16;

    grayS16 as_grayS16;
    rgbS16 as_rgbS16;
    argbS16 as_argbS16;

    grayS32 as_grayS32;
    rgbS32 as_rgbS32;
    argbS32 as_argbS32;

    grayF32 as_grayF32;
    rgbF32 as_rgbF32;
    argbF32 as_argbF32;
};
//-----------------------------------------------------------------------------

} //namespace Images

#endif /* PIXEL_H_ */
