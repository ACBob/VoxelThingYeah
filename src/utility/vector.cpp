#include "vector.hpp"

template <typename T> Vector3<T>::Vector3() : x(0), y(0), z(0) {}
template <typename T> Vector3<T>::Vector3(T x ) : x(x), y(x), z(x) {}
template <typename T> Vector3<T>::Vector3(T x, T y) : x(x), y(y), z(0) {}
template <typename T> Vector3<T>::Vector3(T x, T y, T z) : x(x), y(y), z(z) {}
template <typename T> Vector3<T>::Vector3(const Vector3<T> &v) : x(v.x), y(v.y), z(v.z) {}

// Operator Overloads
#define IMPL_VEC3_INPLACE_OPERATOR(op) \
    template <typename T> const Vector3<T> &Vector3<T>::operator op(const Vector3<T> &v) { \
        x op v.x; \
        y op v.y; \
        z op v.z; \
        return *this; \
    } \
    template <typename T> const Vector3<T> &Vector3<T>::operator op(T v) { \
        x op v; \
        y op v; \
        z op v; \
        return *this; \
    }

#define IMPL_VEC3_OPERATOR(op) \
    template <typename T> const Vector3<T> Vector3<T>::operator op(const Vector3<T> &v) const { \
        return Vector3<T>(x op v.x, y op v.y, z op v.z); \
    } \
    template <typename T> const Vector3<T> Vector3<T>::operator op(T v) const { \
        return Vector3<T>(x op v, y op v, z op v); \
    }

#define IMPL_VEC3_COMPARE_OPERATOR(op) \
    template <typename T> bool Vector3<T>::operator op(const Vector3<T> &v) const { \
        return x op v.x && y op v.y && z op v.z; \
    } \
    template <typename T> bool Vector3<T>::operator op(T v) const { \
        return x op v && y op v && z op v; \
    }

// = cannot be implemented with above macros
template <typename T> const Vector3<T> &Vector3<T>::operator=(const Vector3<T> &v)
{
    x = v.x;
    y = v.y;
    z = v.z;
    return *this;
}
template <typename T> const Vector3<T> &Vector3<T>::operator=(T v)
{
    x = v;
    y = v;
    z = v;
    return *this;
}

IMPL_VEC3_INPLACE_OPERATOR(+=)
IMPL_VEC3_INPLACE_OPERATOR(-=)
IMPL_VEC3_INPLACE_OPERATOR(*=)
IMPL_VEC3_INPLACE_OPERATOR(/=)

IMPL_VEC3_OPERATOR(+)
IMPL_VEC3_OPERATOR(-)
IMPL_VEC3_OPERATOR(*)
IMPL_VEC3_OPERATOR(/)

template <typename T> T Vector3<T>::Magnitude() const
{
    return sqrt(x * x + y * y + z * z);
}

template <typename T> T Vector3<T>::MagnitudeSq() const
{
    return x * x + y * y + z * z;
}

template <typename T> T Vector3<T>::Dot(const Vector3<T> &v) const
{
    return x * v.x + y * v.y + z * v.z;
}

template <typename T> Vector3<T> Vector3<T>::Cross( const Vector3<T> &v ) const
{
    return Vector3<T>( y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x );
}

template <typename T> Vector3<T> Vector3<T>::Normal() const
{
    T len = Magnitude();
    if (len == 0)
        return Vector3<T>(0, 0, 0);
    return Vector3<T>(x / len, y / len, z / len);
}

// Normalize performs this operation in-place
template <typename T> const Vector3<T> &Vector3<T>::Normalize()
{
    T len = Magnitude();
    if (len == 0)
        return *this;
    x /= len;
    y /= len;
    z /= len;
    return *this;
}

template <typename T> Vector3<T> Vector3<T>::Floor() const
{
    return Vector3<T>(std::floor(x), std::floor(y), std::floor(z));
}
template <typename T> Vector3<T> Vector3<T>::Ceil() const
{
    return Vector3<T>(std::ceil(x), std::ceil(y), std::ceil(z));
}

template <typename T> Vector3<T> Vector3<T>::Lerp(const Vector3<T> &v, T t) const
{
    return Vector3<T>(x + (v.x - x) * t, y + (v.y - y) * t, z + (v.z - z) * t);
}
template <typename T> Vector3<T> Vector3<T>::Slerp(const Vector3<T> &v, T t) const
{
    // Stole this from somewhere, need to learn how it works
    T dot = Dot(v);
    if (dot < 0)
    {
        dot = -dot;
        return Vector3<T>(x + (v.x - x) * t, y + (v.y - y) * t, z + (v.z - z) * t).Normalize();
    }
    return Vector3<T>(x + (v.x - x) * t, y + (v.y - y) * t, z + (v.z - z) * t).Normalize();
}

template <typename T> const Vector3<T> &Vector3<T>::LerpInPlace(const Vector3<T> &v, T t)
{
    x += (v.x - x) * t;
    y += (v.y - y) * t;
    z += (v.z - z) * t;
    return *this;
}

template <typename T> const Vector3<T> &Vector3<T>::SlerpInPlace(const Vector3<T> &v, T t)
{
    T dot = Dot(v);
    if (dot < 0)
    {
        dot = -dot;
        x += (v.x - x) * t;
        y += (v.y - y) * t;
        z += (v.z - z) * t;
        return Normalize();
    }
    x += (v.x - x) * t;
    y += (v.y - y) * t;
    z += (v.z - z) * t;
    return Normalize();
}

// TODO: the following rotation functions are subject to gimbal lock
//       in future, I want to implement rotors (because quaternions are for NERDS)
//       but for now, I'll just use Euler angles

template<typename T> Vector3<T> Vector3<T>::Rotate(const Vector3<T> &axis, T angle) const
{
    T ca = cos( angle );
    T sa = sin( angle );

    T x = x * ca + y * sa * axis.z - z * sa * axis.y;
    T y = y * ca + z * sa * axis.x - x * sa * axis.z;
    T z = z * ca + x * sa * axis.y - y * sa * axis.x;

    return Vector3<T>(x, y, z);
}

template<typename T> const Vector3<T> &Vector3<T>::RotateInPlace(const Vector3<T> &axis, T angle)
{
    T ca = cos( angle );
    T sa = sin( angle );

    x = x * ca + y * sa * axis.z - z * sa * axis.y;
    y = y * ca + z * sa * axis.x - x * sa * axis.z;
    z = z * ca + x * sa * axis.y - y * sa * axis.x;

    return *this;
}

template<typename T> Vector3<T> Vector3<T>::LookingAt(const Vector3<T> &target, const Vector3<T> &up) const
{
    Vector3<T> forward = target - *this;
    Vector3<T> right = forward.Cross(up);
    Vector3<T> up2 = right.Cross(forward);

    return Vector3<T>(right.x, right.y, right.z);
}

template<typename T> const Vector3<T> &Vector3<T>::LookAt(const Vector3<T> &target, const Vector3<T> &up)
{
    Vector3<T> forward = target - *this;
    Vector3<T> right = forward.Cross(up);
    Vector3<T> up2 = right.Cross(forward);

    x = right.x;
    y = right.y;
    z = right.z;

    return *this;
}

template<typename T> T Vector3<T>::Distance( const Vector3<T> &v ) const
{
    return sqrt( (x - v.x) * (x - v.x) + (y - v.y) * (y - v.y) + (z - v.z) * (z - v.z) );
}

IMPL_VEC3_COMPARE_OPERATOR(==);
IMPL_VEC3_COMPARE_OPERATOR(!=);
IMPL_VEC3_COMPARE_OPERATOR(<);
IMPL_VEC3_COMPARE_OPERATOR(>);
IMPL_VEC3_COMPARE_OPERATOR(<=);
IMPL_VEC3_COMPARE_OPERATOR(>=);

template <typename T> template <typename U> Vector3<T>::operator Vector3<U> () const
{
    return Vector3<U>( (U)x, (U)y, (U)z );
}

template <typename T> Vector3<T>::operator Vector4<T>() const
{
    return Vector4<T>(x, y, z, 1);
}


// Vector4s

template <typename T> Vector4<T>::Vector4() : x(0), y(0), z(0), w(0) {}
template <typename T> Vector4<T>::Vector4(T x ) : x(x), y(x), z(x), w(x) {}
template <typename T> Vector4<T>::Vector4(T x, T y) : x(x), y(y), z(0), w(0) {}
template <typename T> Vector4<T>::Vector4(T x, T y, T z) : x(x), y(y), z(z), w(0) {}
template <typename T> Vector4<T>::Vector4(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}
template <typename T> Vector4<T>::Vector4(const Vector4<T> &v) : x(v.x), y(v.y), z(v.z) {}

#define IMPL_VEC4_INPLACE_OPERATOR(op) \
    template <typename T> const Vector4<T> &Vector4<T>::operator op (const Vector4<T> &v) { \
        x op v.x; \
        y op v.y; \
        z op v.z; \
        w op v.w; \
        return *this; \
    } \
    template <typename T> const Vector4<T> &Vector4<T>::operator op (T s) { \
        x op s; \
        y op s; \
        z op s; \
        w op s; \
        return *this; \
    }

#define IMPL_VEC4_OPERATOR(op)  \
    template <typename T> const Vector4<T> Vector4<T>::operator op(const Vector4<T> &v) const { \
        return Vector4<T>(x op v.x, y op v.y, z op v.z, w op v.w); \
    } \
    template <typename T> const Vector4<T> Vector4<T>::operator op(T v) const { \
        return Vector4<T>(x op v, y op v, z op v, w op v); \
    }

#define IMPL_VEC4_COMPARE_OPERATOR(op) \
    template <typename T> bool Vector4<T>::operator op (const Vector4<T> &v) const { \
        return x op v.x && y op v.y && z op v.z && w op v.w; \
    } \
    template <typename T> bool Vector4<T>::operator op (T s) const { \
        return x op s && y op s && z op s && w op s; \
    }

template <typename T> const Vector4<T> &Vector4<T>::operator=(const Vector4<T> &v)
{
    x = v.x;
    y = v.y;
    z = v.z;
    w = v.w;
    return *this;
}
template <typename T> const Vector4<T> &Vector4<T>::operator=(T s)
{
    x = s;
    y = s;
    z = s;
    w = s;
    return *this;
}

IMPL_VEC4_INPLACE_OPERATOR(+=);
IMPL_VEC4_INPLACE_OPERATOR(-=);
IMPL_VEC4_INPLACE_OPERATOR(*=);
IMPL_VEC4_INPLACE_OPERATOR(/=);

IMPL_VEC4_OPERATOR(+);
IMPL_VEC4_OPERATOR(-);
IMPL_VEC4_OPERATOR(*);
IMPL_VEC4_OPERATOR(/);

template <typename T> T Vector4<T>::Magnitude() const
{
    return sqrt(x * x + y * y + z * z + w * w);
}

template <typename T> T Vector4<T>::MagnitudeSq() const
{
    return x * x + y * y + z * z + w * w;
}

template <typename T> T Vector4<T>::Dot(const Vector4<T> &v) const
{
    return x * v.x + y * v.y + z * v.z + w * v.w;
}

template <typename T> Vector4<T> Vector4<T>::Cross(const Vector4<T> &v) const
{
    return Vector4<T>(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x, 0);
}

template <typename T> Vector4<T> Vector4<T>::Normal() const
{
    T m = Magnitude();
    return Vector4<T>(x / m, y / m, z / m, w / m);
}
template <typename T> const Vector4<T> &Vector4<T>::Normalize()
{
    T m = Magnitude();
    x /= m;
    y /= m;
    z /= m;
    w /= m;
    return *this;
}

template <typename T> Vector4<T> Vector4<T>::Lerp(const Vector4<T> &v, T t) const
{
    return Vector4<T>(x + (v.x - x) * t, y + (v.y - y) * t, z + (v.z - z) * t, w + (v.w - w) * t);
}

// TODO: does not slerp the W component
template <typename T> Vector4<T> Vector4<T>::Slerp(const Vector4<T> &v, T t) const
{
    // Stole this from somewhere, need to learn how it works
    T dot = Dot(v);
    if (dot < 0)
    {
        dot = -dot;
        return Vector4<T>(x + (v.x - x) * t, y + (v.y - y) * t, z + (v.z - z) * t, w).Normalize();
    }
    return Vector4<T>(x + (v.x - x) * t, y + (v.y - y) * t, z + (v.z - z) * t, w).Normalize();
}

template <typename T> const Vector4<T> &Vector4<T>::LerpInPlace(const Vector4<T> &v, T t)
{
    x += (v.x - x) * t;
    y += (v.y - y) * t;
    z += (v.z - z) * t;
    w += (v.w - w) * t;
    return *this;
}

// TODO: does not slerp the W component
template <typename T> const Vector4<T> &Vector4<T>::SlerpInPlace(const Vector4<T> &v, T t)
{
    T dot = Dot(v);
    if (dot < 0)
    {
        dot = -dot;
        x += (v.x - x) * t;
        y += (v.y - y) * t;
        z += (v.z - z) * t;
        w = 1;
        return Normalize();
    }
    x += (v.x - x) * t;
    y += (v.y - y) * t;
    z += (v.z - z) * t;
    w = 1;
    return Normalize();
}

// TODO: doesn't rotate the W component (does it have to?)
template <typename T> Vector4<T> Vector4<T>::Rotate(const Vector4<T> &axis, T angle) const
{
    T ca = cos( angle );
    T sa = sin( angle );

    T x = x * ca + y * sa * axis.z - z * sa * axis.y;
    T y = y * ca + z * sa * axis.x - x * sa * axis.z;
    T z = z * ca + x * sa * axis.y - y * sa * axis.x;

    return Vector4<T>(x, y, z, w);
}

template<typename T> const Vector4<T> &Vector4<T>::RotateInPlace(const Vector4<T> &axis, T angle)
{
    T ca = cos( angle );
    T sa = sin( angle );

    x = x * ca + y * sa * axis.z - z * sa * axis.y;
    y = y * ca + z * sa * axis.x - x * sa * axis.z;
    z = z * ca + x * sa * axis.y - y * sa * axis.x;

    return *this;
}

template<typename T> T Vector4<T>::Distance(const Vector4<T> &v) const
{
    return sqrt( (x - v.x) * (x - v.x) + (y - v.y) * (y - v.y) + (z - v.z) * (z - v.z) + (w - v.w) * (w - v.w) );
}

IMPL_VEC4_COMPARE_OPERATOR(==);
IMPL_VEC4_COMPARE_OPERATOR(!=);
IMPL_VEC4_COMPARE_OPERATOR(<);
IMPL_VEC4_COMPARE_OPERATOR(>);
IMPL_VEC4_COMPARE_OPERATOR(<=);
IMPL_VEC4_COMPARE_OPERATOR(>=);

template <typename T> template <typename U> Vector4<T>::operator Vector4<U>() const
{
    return Vector4<U>((U)x, (U)y, (U)z, (U)w);
}

template <typename T> Vector4<T>::operator Vector3<T>() const
{
    return Vector3<T>(x, y, z);
}