#include "colour.hpp"
#include "vector.hpp"

CColour::CColour()
{
    r = g = b = 0;
    a = 255;
}

CColour::CColour( uchar_t r, uchar_t g, uchar_t b, uchar_t a )
{
    this->r = r;
    this->g = g;
    this->b = b;
    this->a = a;
}

CColour::CColour( uint16_t colour )
{
    this->r = (( colour >> 12 ) & 0xF) * 17;
    this->g = (( colour >> 8 ) & 0xF) * 17;
    this->b = (( colour >> 4 ) & 0xF) * 17;
    this->a = (( colour >> 0 ) & 0xF) * 17;
}

CColour::CColour( uint32_t colour )
{
    this->r = ( colour >> 24 ) & 0xFF;
    this->g = ( colour >> 16 ) & 0xFF;
    this->b = ( colour >> 8 ) & 0xFF;
    this->a = colour & 0xFF;
}

CColour::CColour( const CColour& colour )
{
    this->r = colour.r;
    this->g = colour.g;
    this->b = colour.b;
    this->a = colour.a;
}

CColour::CColour( const Vector3c& colour )
{
    this->r = ( uchar_t )( colour.x );
    this->g = ( uchar_t )( colour.y );
    this->b = ( uchar_t )( colour.z );
    this->a = 255;
}

CColour::CColour( const Vector4c& colour )
{
    this->r = colour.x;
    this->g = colour.y;
    this->b = colour.z;
    this->a = colour.w;
}

CColour::CColour( const Vector3f& colour )
{
    this->r = ( uchar_t )( colour.x * 255 );
    this->g = ( uchar_t )( colour.y * 255 );
    this->b = ( uchar_t )( colour.z * 255 );
    this->a = 255;
}

CColour::CColour( const Vector4f& colour )
{
    this->r = ( uchar_t )( colour.x * 255 );
    this->g = ( uchar_t )( colour.y * 255 );
    this->b = ( uchar_t )( colour.z * 255 );
    this->a = ( uchar_t )( colour.w * 255 );
}

// Returns the colour in 0xRRGGBBAA format
uint32_t CColour::GetColour32b() const
{
    return ( ( this->r << 24 ) | ( this->g << 16 ) | ( this->b << 8 ) | this->a );
}
// Returns the colour in 0xRGBA format
uint16_t CColour::GetColour16b() const 
{
    return ( ( this->r << 12 ) | ( this->g << 8 ) | ( this->b << 4 ) | this->a );
}

CColour CColour::operator/(CColour colour) const
{
    return CColour( this->r / colour.r, this->g / colour.g, this->b / colour.b, this->a / colour.a );
}
CColour CColour::operator*(CColour colour) const
{
    return CColour( this->r * colour.r, this->g * colour.g, this->b * colour.b, this->a * colour.a );
}
CColour CColour::operator+(CColour colour) const
{
    return CColour( this->r + colour.r, this->g + colour.g, this->b + colour.b, this->a + colour.a );
}
CColour CColour::operator-(CColour colour) const
{
    return CColour( this->r - colour.r, this->g - colour.g, this->b - colour.b, this->a - colour.a );
}

CColour CColour::operator/(int i) const
{
    return CColour( this->r / i, this->g / i, this->b / i, this->a / i );
}
CColour CColour::operator*(int i) const
{
    return CColour( this->r * i, this->g * i, this->b * i, this->a * i );
}
CColour CColour::operator+(int i) const
{
    return CColour( this->r + i, this->g + i, this->b + i, this->a + i );
}
CColour CColour::operator-(int i) const
{
    return CColour( this->r - i, this->g - i, this->b - i, this->a - i );
}

CColour& CColour::operator/=(CColour colour)
{
    this->r /= colour.r;
    this->g /= colour.g;
    this->b /= colour.b;
    this->a /= colour.a;
    return *this;
}
CColour& CColour::operator*=(CColour colour)
{
    this->r *= colour.r;
    this->g *= colour.g;
    this->b *= colour.b;
    this->a *= colour.a;
    return *this;
}
CColour& CColour::operator+=(CColour colour)
{
    this->r += colour.r;
    this->g += colour.g;
    this->b += colour.b;
    this->a += colour.a;
    return *this;
}
CColour& CColour::operator-=(CColour colour)
{
    this->r -= colour.r;
    this->g -= colour.g;
    this->b -= colour.b;
    this->a -= colour.a;
    return *this;
}

CColour& CColour::operator/=(int i)
{
    this->r /= i;
    this->g /= i;
    this->b /= i;
    this->a /= i;
    return *this;
}
CColour& CColour::operator*=(int i)
{
    this->r *= i;
    this->g *= i;
    this->b *= i;
    this->a *= i;
    return *this;
}
CColour& CColour::operator+=(int i)
{
    this->r += i;
    this->g += i;
    this->b += i;
    this->a += i;
    return *this;
}
CColour& CColour::operator-=(int i)
{
    this->r -= i;
    this->g -= i;
    this->b -= i;
    this->a -= i;
    return *this;
}

bool CColour::operator==(CColour colour) const
{
    return ( this->r == colour.r && this->g == colour.g && this->b == colour.b && this->a == colour.a );
}
bool CColour::operator!=(CColour colour) const
{
    return ( this->r != colour.r || this->g != colour.g || this->b != colour.b || this->a != colour.a );
}

CColour::operator uint16_t() const
{
    return this->GetColour16b();
}
CColour::operator uint32_t() const
{
    return this->GetColour32b();
}
CColour::operator Vector3f() const
{
    return Vector3f( this->r / 255.0f, this->g / 255.0f, this->b / 255.0f );
}
CColour::operator Vector4f() const
{
    return Vector4f( this->r / 255.0f, this->g / 255.0f, this->b / 255.0f, this->a / 255.0f );
}