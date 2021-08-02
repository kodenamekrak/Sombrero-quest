#pragma once

#include "MiscUtils.hpp"

#include "beatsaber-hook/shared/utils/typedefs.h"
#ifdef HAS_CODEGEN
#include "UnityEngine/Vector3.hpp"
#endif

#define CONSTEXPR_GETTER(name, ...) \
constexpr static inline FastVector3 name() {\
    return __VA_ARGS__;\
}

namespace Sombrero {

    struct FastVector3;

    template<typename T>
    concept Vector3Derive = requires() {
#ifdef HAS_CODEGEN
        std::is_base_of<UnityEngine::Vector3, T>::value;
#else
        std::is_base_of<FastVector3, T>::value;
#endif
        !std::is_pointer<T>::value;
    };

#define operatorOverload(name, operator) \
    template<Vector3Derive T> \
    static T vector3##name (T const& a, T const& b) { \
        return T(a.x operator b.x, a.y operator b.y, a.z operator b.z); \
    } \
    template<Vector3Derive T> \
    static T vector3##name (T const& a, float const& b) { \
        return T(a.x operator b, a.y operator b, a.z operator b); \
    } \



    template<Vector3Derive T>
    inline std::string vector3Str(T const& vector3) {
        return std::to_string(vector3.x) + ", " + std::to_string(vector3.y) + ", " + std::to_string(vector3.z);
    }

    operatorOverload(add, +)
    operatorOverload(subtract, -)
    operatorOverload(multiply, *)
    operatorOverload(scale, *)
    operatorOverload(divide, /)

#undef operatorOverload

#ifdef HAS_CODEGEN
    struct FastVector3 : public UnityEngine::Vector3 {
#else
    struct FastVector3 {
            float x,y,z;
#endif
    public:

#ifdef HAS_CODEGEN
        // Implicit convert of vector
        constexpr FastVector3(const Vector3& vector) : Vector3(vector.x, vector.y, vector.z) {} // x(vector.x), y(vector.y), z(vector.z) {}

        constexpr FastVector3(float x = 0.0f, float y = 0.0f, float z = 0.0f) : Vector3(x, y, z) {}
#else
        constexpr FastVector3(float x = 0.0f, float y = 0.0f, float z = 0.0f) : x(x), y(y), z(z) {}
#endif

        CONSTEXPR_GETTER(one, {1.0f, 1.0f, 1.0f})
        CONSTEXPR_GETTER(zero, {0.0f, 0.0f, 0.0f})
        
        CONSTEXPR_GETTER(up, {0.0f, 1.0f, 0.0f})
        CONSTEXPR_GETTER(down, {0.0f, -1.0f, 0.0f})
        CONSTEXPR_GETTER(left, {-1.0f, 0.0f, 0.0f})
        CONSTEXPR_GETTER(right, {1.0f, 0.0f, 0.0f})
        CONSTEXPR_GETTER(forward, {0.0f, 0.0f, 1.0f})
        CONSTEXPR_GETTER(back, {0.0f, 0.0f, -1.0f})

        inline std::string toString() {
            return vector3Str(*this);
        }

        inline static FastVector3 Lerp(FastVector3 const& a, FastVector3 const& b, float const& t)
        {
            return LerpUnclamped(a, b, Clamp01(t));
        }

        static FastVector3 LerpUnclamped(FastVector3 const& a, FastVector3 const& b, float const& t)
        {
            return FastVector3(a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t, a.z + (b.z - a.z) * t);
        }

        float sqrMagnitude() const {
            return ((x * x) + (y * y) + (z * z));
        }

        float Magnitude() const {
            return std::sqrt((x * x) + (y * y) + (z * z));
        }

        float Distance(const FastVector3& b) {
            float dx = x - b.x;
            float dy = y - b.y;
            float dz = z - b.z;
            return sqrt(dx * dx + dy * dy + dz * dz);
        }

        static inline FastVector3 Normalize(const FastVector3& vec) {
            float magnitude = vec.Magnitude();
            if (magnitude == 0.0f) return zero();
            return vec / magnitude;
        }

        inline FastVector3 get_normalized()
        {
            return FastVector3::Normalize(*this);
        }

        inline void Normalize() {
            NormalizeFast();
        }

        // In case codegen method takes over
        inline void NormalizeFast() {
            float magnitude = Magnitude();
            if (magnitude == 0.0f) {
                x = 0.0f;
                y = 0.0f;
                z = 0.0f;
            }
            *this /= magnitude;
        }

#define operatorOverload(name, operatore) \
        FastVector3 operator operatore(const FastVector3& b) const { \
            return vector3##name(*this, b); \
        }                                \
        FastVector3 operator operatore(float const& b) const { \
            return vector3##name(*this, b); \
        }                                 \
        FastVector3& operator operatore##=(float const& bb) {  \
            x operatore##= bb;                       \
            y operatore##= bb;                        \
            z operatore##= bb;                        \
            return *this; \
        }                                 \
        FastVector3& operator operatore##=(const FastVector3& bb) {  \
            x operatore##= bb.x;                       \
            y operatore##= bb.y;                        \
            z operatore##= bb.z;                        \
            return *this; \
        }

        operatorOverload(add, +)
        operatorOverload(subtract, -)
        operatorOverload(multiply, *)
        operatorOverload(divide, /)

#undef operatorOverload

        FastVector3 operator-() const {
            return FastVector3(-x, -y, -z);
        }


        template<Vector3Derive T>
        bool operator ==(const T& lhs) {
            return lhs.x == x && lhs.y == y && lhs.z == z;
        }

        template<Vector3Derive T>
        inline bool operator !=(const T& lhs) {
            return !(this == lhs);
        }

        float& operator[](int i) {
            return (&x)[i];
        }
    };

#ifdef HAS_CODEGEN
    static_assert(sizeof(UnityEngine::Vector3) == sizeof(FastVector3));
#endif
}
DEFINE_IL2CPP_ARG_TYPE(Sombrero::FastVector3, "UnityEngine", "Vector3");
#undef CONSTEXPR_GETTER