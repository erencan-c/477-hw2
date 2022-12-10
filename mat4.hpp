#ifndef __MAT4_H_INCLUDED__
#define __MAT4_H_INCLUDED__

#include <immintrin.h>
#include <smmintrin.h>
#include <iostream>
#include <cmath>
#include "vec.hpp"

// typedef const mat4f cmat4f;

typedef const struct mat4 mat4c;
typedef __m256d row4;
typedef const row4 row4c;
typedef const double cdouble;

struct mat4 {
public:
    row4 data[4];

    mat4() {
        
    }

    mat4(row4c r1, row4c r2, row4c r3, row4c r4) {
        data[0] = r1;
        data[1] = r2;
        data[2] = r3;
        data[3] = r4;
    }

    vec4& operator[](int idx) {
        return data[idx];
    }

    static inline mat4 identity() {
        return mat4(row4c{1.0f,0.0f,0.0f,0.0f}, row4c{0.0f,1.0f,0.0f,0.0f}, row4c{0.0f,0.0f,1.0f,0.0f}, row4c{0.0f,0.0f,0.0f,1.0f});
    }

    static inline mat4 transition(cdouble x, cdouble y, cdouble z) {
        return mat4(
            row4{1.0f, 0.0f, 0.0f, x},
            row4{0.0f, 1.0f, 0.0f, y},
            row4{0.0f, 0.0f, 1.0f, z},
            row4{0.0f, 0.0f, 0.0f, 1.0f}
        );
    }
    static inline mat4 transition(arr4c arr) {
        return mat4::transition(arr[0], arr[1], arr[2]);
    }

    static inline mat4 rotation_x(cdouble angle) {
        cdouble s = std::sin(angle);
        cdouble c = std::cos(angle);
        return mat4(
            row4{1.0f, 0.0f, 0.0f, 0.0f},
            row4{0.0f, c, -s, 0.0f},
            row4{0.0f, s, c, 0.0f},
            row4{0.0f, 0.0f, 0.0f, 1.0f}
        );
    }
    
    static inline mat4 rotation_y(cdouble angle) {
        cdouble s = std::sin(angle);
        cdouble c = std::cos(angle);
        return mat4(
            row4{c, 0.0f, s, 0.0f},
            row4{0.0f, 1.0f, 0.0f, 0.0f},
            row4{-s, 0.0f, c, 0.0f},
            row4{0.0f, 0.0f, 0.0f, 1.0f}
        );
    }

    static inline mat4 rotation_z(cdouble angle) {
        cdouble s = std::sin(angle);
        cdouble c = std::cos(angle);
        return mat4(
            row4{c, -s, 0.0f, 0.0f},
            row4{s, c, 0.0f, 0.0f},
            row4{0.0f, 0.0f, 1.0f, 0.0f},
            row4{0.0f, 0.0f, 0.0f, 1.0f}
        );
    }

    static inline mat4 rotation(vec4c p2, cdouble angle) {
        p2[3] = 0;
        vec4c ux{p2[0], 0, 0, 0};
        vec4c uy{0, p2[1], 0, 0};
        vec4c uz{0, 0, p2[2], 0};
        vec4c uprime = uy + uz;
        cdouble len_uprime = len4f(uprime);
        cdouble cosa = p2[3]/len_uprime;
        cdouble sina = p2[2]/len_uprime;
        mat4c rxa(
            row4{1, 0, 0, 0},
            row4{0, cosa, -sina, 0},
            row4{0, sina, cosa, 0},
            row4{0, 0, 0, 1}
        );

        cdouble lenu = len4f(p2);
        cdouble cosb = (sqrt(p2[1]*p2[1] + p2[2]*p2[2]))/lenu;
        cdouble sinb = p2[0]/lenu;

        mat4c ryb(
            row4{cosb, 0, -sinb, 0},
            row4{0,1,0,0},
            row4{sinb, 0, cosb, 0},
            row4{0,0,0,1}
        );
        return rxa.inv() * ryb * mat4::rotation_z(angle) * ryb.inv() * rxa;
    }

    static inline mat4 scaling(cdouble x, cdouble y, cdouble z) {
        return mat4(
            row4{x, 0.0f, 0.0f, 0.0f},
            row4{0.0f, y, 0.0f, 0.0f},
            row4{0.0f, 0.0f, z, 0.0f},
            row4{0.0f, 0.0f, 0.0f, 1.0f}
        );
    }
    static inline mat4 scaling(arr4c arr) {
        return mat4::scaling(arr[0], arr[1], arr[2]);
    }

    mat4 operator+(mat4c rhs) const;
    mat4 operator-(mat4c rhs) const;
    mat4 operator*(mat4c rhs) const;
    mat4 operator/(mat4c rhs) const;
    vec4 operator*(vec4c rhs) const;
    mat4 mul(mat4c rhs) const;
    mat4 operator-() const;
    mat4 T() const;
    float det() const;
    mat4 inv() const;

    friend std::ostream& operator<<(std::ostream& os, mat4c& mat) {
        os << "mat4[\n";
        os << "\t[" << mat.data[0][0] << ",\t" << mat.data[0][1] << ",\t" << mat.data[0][2] << ",\t" << mat.data[0][3] << "\t],\n";
        os << "\t[" << mat.data[1][0] << ",\t" << mat.data[1][1] << ",\t" << mat.data[1][2] << ",\t" << mat.data[1][3] << "\t],\n";
        os << "\t[" << mat.data[2][0] << ",\t" << mat.data[2][1] << ",\t" << mat.data[2][2] << ",\t" << mat.data[2][3] << "\t],\n";
        os << "\t[" << mat.data[3][0] << ",\t" << mat.data[3][1] << ",\t" << mat.data[3][2] << ",\t" << mat.data[3][3] << "\t]\n";
        return os << "]";
    }
};

inline mat4 mat4::operator+(mat4c rhs) const {
    return mat4(data[0] + rhs.data[0], data[1] + rhs.data[1], data[2] + rhs.data[2], data[3] + rhs.data[3]);
}

inline mat4 mat4::operator-(mat4c rhs) const {
    return mat4(data[0] - rhs.data[0], data[1] - rhs.data[1], data[2] - rhs.data[2], data[3] - rhs.data[3]);
}

inline mat4 mat4::mul(mat4c rhs) const {
    return mat4(data[0] * rhs.data[0], data[1] * rhs.data[1], data[2] * rhs.data[2], data[3] * rhs.data[3]);
}

inline mat4 mat4::operator/(mat4c rhs) const {
    return mat4(data[0] / rhs.data[0], data[1] / rhs.data[1], data[2] / rhs.data[2], data[3] / rhs.data[3]);
}

inline vec4 mat4::operator*(vec4c rhs) const {
    return vec4{
        dot4(data[0], rhs),
        dot4(data[1], rhs),
        dot4(data[2], rhs),
        dot4(data[3], rhs)
    };
}

static inline row4 linear_combination(row4c a, mat4c b) {
    row4 result;
    result = _mm256_shuffle_pd(a, a, 0) * b.data[0];
    result += _mm256_shuffle_pd(a, a, 0x55) * b.data[1];
    result += _mm256_shuffle_pd(a, a, 0xAA) * b.data[2];
    result += _mm256_shuffle_pd(a, a, 0xFF) * b.data[3];
    return result;
}

inline mat4 mat4::operator*(mat4c rhs) const {
    row4c out1 = linear_combination(data[0], rhs);
    row4c out2 = linear_combination(data[1], rhs);
    row4c out3 = linear_combination(data[2], rhs);
    row4c out4 = linear_combination(data[3], rhs);

    return mat4(out1, out2, out3, out4);
}

inline mat4 mat4::operator-() const {
    return mat4(-data[0], -data[1], -data[2], -data[3]);
}

inline mat4 mat4::T() const {
    return mat4(
        row4{data[0][0], data[1][0], data[2][0], data[3][0]},
        row4{data[0][1], data[1][1], data[2][1], data[3][1]},
        row4{data[0][2], data[1][2], data[2][2], data[3][2]},
        row4{data[0][3], data[1][3], data[2][3], data[3][3]}
    );
}

/// @brief The `mat` is interpreted as a 2x2 matrix 
/// @param mat
static inline row4 inv2f(row4c mat) {
    return row4c{mat[3], -mat[1], -mat[2], mat[0]}/(mat[0]*mat[3] - mat[1]*mat[2]);
}

static inline float det2f(row4c mat) {
    return 1.0f/(mat[0]*mat[3] - mat[1]*mat[2]);
}

static inline row4 mul2f(row4c lhs, row4c rhs) {
    return row4{
        lhs[0]*rhs[0] + lhs[1]*rhs[2],
        lhs[0]*rhs[1] + lhs[1]*rhs[3],
        lhs[2]*rhs[0] + lhs[3]*rhs[2],
        lhs[2]*rhs[1] + lhs[3]*rhs[3]
    };
}

inline float mat4::det() const {
    row4c a0 = {data[0][0], data[0][1], data[1][0], data[1][1]};
    row4c a1 = {data[0][2], data[0][3], data[1][2], data[1][3]};
    row4c a2 = {data[2][0], data[2][1], data[3][0], data[3][1]};
    row4c a3 = {data[2][2], data[2][3], data[3][2], data[3][3]};

    return det2f(a0 - mul2f(mul2f(a1,inv2f(a3)),a2)) * det2f(a3);
}

//TODO: Improve the performance
inline mat4 mat4::inv() const {
    row4c A = {data[0][0], data[0][1], data[1][0], data[1][1]};
    row4c B = {data[0][2], data[0][3], data[1][2], data[1][3]};
    row4c C = {data[2][0], data[2][1], data[3][0], data[3][1]};
    row4c D = {data[2][2], data[2][3], data[3][2], data[3][3]};

    row4c invA = inv2f(A);
    row4c invDCAB = inv2f(D - mul2f(C, mul2f(invA, B)));

    row4c b0 = invA + mul2f((invA, B), mul2f(invDCAB, mul2f(C, invA)));
    row4c b1 = -mul2f(mul2f(invA, B), invDCAB);
    row4c b2 = -mul2f(invDCAB, mul2f(C, invA));
    row4c b3 = invDCAB;
    return mat4(
        row4{b0[0], b0[1], b1[0], b1[1]},
        row4{b0[2], b0[3], b1[2], b1[3]},
        row4{b2[0], b2[1], b3[0], b3[1]},
        row4{b2[2], b2[3], b3[2], b3[3]}
    );
}

#endif