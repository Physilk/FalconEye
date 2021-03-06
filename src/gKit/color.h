
#ifndef _COLOR_H
#define _COLOR_H

#include "LuaInterface/luaObjectInterface.h"
//! \addtogroup image
///@{

//! \file
//! manipulation de couleurs

//! representation d'une couleur (rgba) transparente ou opaque.
struct Color
{
    //! constructeur par defaut.
    Color( const float _r= 0.f, const float _g= 0.f, const float _b= 0.f, const float _a= 1.f ) : r(_r), g(_g), b(_b), a(_a) {}
    //! cree une couleur avec les memes composantes que color, mais remplace sa composante alpha (color.r, color.g, color.b, alpha).
    Color( const Color& color, const float alpha ) : r(color.r), g(color.g), b(color.b), a(alpha) {}  // remplace alpha.
    
    float r, g, b, a;
    
    float getR() const { return r; }
    float getG() const { return g; }
    float getB() const { return b; }
    float getA() const { return a; }

    void setR(float parR) { r = parR; }
    void setG(float parG) { g = parG; }
    void setB(float parB) { b = parB; }
    void setA(float parA) { a = parA; }

	//for LUA
	bool equals(const Color& other) const;
	
    LUA_BEGIN_BIND_METHODS(Color)
        LUA_BIND_CONSTRUCTOR(float, float, float, float)
        LUA_BIND_PROPERTY(Color, r, getR, setR)
        LUA_BIND_PROPERTY(Color, g, getG, setG)
        LUA_BIND_PROPERTY(Color, b, getB, setB)
        LUA_BIND_PROPERTY(Color, a, getA, setA)
        LUA_BIND_METHOD_ARGS(Color, equals, Color)
        LUA_END_BIND_METHODS
};

//! utilitaire. renvoie une couleur noire.
Color Black( );
//! utilitaire. renvoie une couleur blanche.
Color White( );
//! utilitaire. renvoie une couleur rouge.
Color Red( );
//! utilitaire. renvoie une couleur verte.
Color Green( );
//! utilitaire. renvoie une couleur bleue.
Color Blue( );

Color operator+ ( const Color& a, const Color& b );
Color operator- ( const Color& a, const Color& b );
Color operator- ( const Color& c );
Color operator* ( const Color& a, const Color& b );
Color operator* ( const Color& c, const float k );
Color operator* ( const float k, const Color& c );
Color operator/ ( const Color& a, const Color& b );
Color operator/ ( const float k, const Color& c );
Color operator/ ( const Color& c, const float k );

///@}
#endif
