////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2023 Laurent Gomila (laurent@sfml-dev.org)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
template <typename T>
inline Vector2<T>::Vector2() :
spritePosX(0),
spritePosY(0)
{

}


////////////////////////////////////////////////////////////
template <typename T>
inline Vector2<T>::Vector2(T X, T Y) :
spritePosX(X),
spritePosY(Y)
{

}


////////////////////////////////////////////////////////////
template <typename T>
template <typename U>
inline Vector2<T>::Vector2(const Vector2<U>& vector) :
spritePosX(static_cast<T>(vector.spritePosX)),
spritePosY(static_cast<T>(vector.spritePosY))
{
}


////////////////////////////////////////////////////////////
template <typename T>
inline Vector2<T> operator -(const Vector2<T>& right)
{
    return Vector2<T>(-right.spritePosX, -right.spritePosY);
}


////////////////////////////////////////////////////////////
template <typename T>
inline Vector2<T>& operator +=(Vector2<T>& left, const Vector2<T>& right)
{
    left.spritePosX += right.spritePosX;
    left.spritePosY += right.spritePosY;

    return left;
}


////////////////////////////////////////////////////////////
template <typename T>
inline Vector2<T>& operator -=(Vector2<T>& left, const Vector2<T>& right)
{
    left.spritePosX -= right.spritePosX;
    left.spritePosY -= right.spritePosY;

    return left;
}


////////////////////////////////////////////////////////////
template <typename T>
inline Vector2<T> operator +(const Vector2<T>& left, const Vector2<T>& right)
{
    return Vector2<T>(left.spritePosX + right.spritePosX, left.spritePosY + right.spritePosY);
}


////////////////////////////////////////////////////////////
template <typename T>
inline Vector2<T> operator -(const Vector2<T>& left, const Vector2<T>& right)
{
    return Vector2<T>(left.spritePosX - right.spritePosX, left.spritePosY - right.spritePosY);
}


////////////////////////////////////////////////////////////
template <typename T>
inline Vector2<T> operator *(const Vector2<T>& left, T right)
{
    return Vector2<T>(left.spritePosX * right, left.spritePosY * right);
}


////////////////////////////////////////////////////////////
template <typename T>
inline Vector2<T> operator *(T left, const Vector2<T>& right)
{
    return Vector2<T>(right.spritePosX * left, right.spritePosY * left);
}


////////////////////////////////////////////////////////////
template <typename T>
inline Vector2<T>& operator *=(Vector2<T>& left, T right)
{
    left.spritePosX *= right;
    left.spritePosY *= right;

    return left;
}


////////////////////////////////////////////////////////////
template <typename T>
inline Vector2<T> operator /(const Vector2<T>& left, T right)
{
    return Vector2<T>(left.spritePosX / right, left.spritePosY / right);
}


////////////////////////////////////////////////////////////
template <typename T>
inline Vector2<T>& operator /=(Vector2<T>& left, T right)
{
    left.spritePosX /= right;
    left.spritePosY /= right;

    return left;
}


////////////////////////////////////////////////////////////
template <typename T>
inline bool operator ==(const Vector2<T>& left, const Vector2<T>& right)
{
    return (left.spritePosX == right.spritePosX) && (left.spritePosY == right.spritePosY);
}


////////////////////////////////////////////////////////////
template <typename T>
inline bool operator !=(const Vector2<T>& left, const Vector2<T>& right)
{
    return (left.spritePosX != right.spritePosX) || (left.spritePosY != right.spritePosY);
}
