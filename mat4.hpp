#ifndef __MAT4_H_INCLUDED__
#define __MAT4_H_INCLUDED__

#include <immintrin.h>
#include <smmintrin.h>
#include <iostream>
#include <cmath>
#include "vec.hpp"

// typedef const mat4f cmat4f;

typedef const struct mat4f mat4fc;
typedef __m128 row4f;
typedef const __m128 row4fc;
typedef const float cfloat;

struct mat4f {
public:
    row4f row1;
    row4f row2;
    row4f row3;
    row4f row4;

    mat4f(row4fc r1, row4fc r2, row4fc r3, row4fc r4) {
        row1 = r1;
        row2 = r2;
        row3 = r3;
        row4 = r4;
    }

    static inline mat4f identity() {
        return mat4f(row4fc{1.0f,0.0f,0.0f,0.0f}, row4fc{0.0f,1.0f,0.0f,0.0f}, row4fc{0.0f,0.0f,1.0f,0.0f}, row4fc{0.0f,0.0f,0.0f,1.0f});
    }

    static inline mat4f transition(cfloat x, cfloat y, cfloat z) {
        return mat4f(
            row4f{1.0f, 0.0f, 0.0f, x},
            row4f{0.0f, 1.0f, 0.0f, y},
            row4f{0.0f, 0.0f, 1.0f, z},
            row4f{0.0f, 0.0f, 0.0f, 1.0f}
        );
    }

    static inline mat4f rotation_x(cfloat angle) {
        cfloat s = std::sin(angle);
        cfloat c = std::cos(angle);
        return mat4f(
            row4f{1.0f, 0.0f, 0.0f, 0.0f},
            row4f{0.0f, c, -s, 0.0f},
            row4f{0.0f, s, c, 0.0f},
            row4f{0.0f, 0.0f, 0.0f, 1.0f}
        );
    }
    
    static inline mat4f rotation_y(cfloat angle) {
        cfloat s = std::sin(angle);
        cfloat c = std::cos(angle);
        return mat4f(
            row4f{c, 0.0f, s, 0.0f},
            row4f{0.0f, 1.0f, 0.0f, 0.0f},
            row4f{-s, 0.0f, c, 0.0f},
            row4f{0.0f, 0.0f, 0.0f, 1.0f}
        );
    }

    static inline mat4f rotation_z(cfloat angle) {
        cfloat s = std::sin(angle);
        cfloat c = std::cos(angle);
        return mat4f(
            row4f{c, -s, 0.0f, 0.0f},
            row4f{s, c, 0.0f, 0.0f},
            row4f{0.0f, 0.0f, 1.0f, 0.0f},
            row4f{0.0f, 0.0f, 0.0f, 1.0f}
        );
    }

    static inline mat4f scaling(cfloat x, cfloat y, cfloat z) {
        return mat4f(
            row4f{x, 0.0f, 0.0f, 0.0f},
            row4f{0.0f, y, 0.0f, 0.0f},
            row4f{0.0f, 0.0f, z, 0.0f},
            row4f{0.0f, 0.0f, 0.0f, 1.0f}
        );
    }

    mat4f operator+(mat4fc rhs) const;
    mat4f operator-(mat4fc rhs) const;
    mat4f operator*(mat4fc rhs) const;
    mat4f operator/(mat4fc rhs) const;
    vec4f operator*(vec4fc rhs) const;
    mat4f mul(mat4fc rhs) const;
    mat4f operator-() const;
    mat4f T() const;
    float det() const;
    mat4f inv() const;

    friend std::ostream& operator<<(std::ostream& os, mat4fc& mat) {
        os << "mat4[\n";
        os << "\t[" << mat.row1[0] << ",\t" << mat.row1[1] << ",\t" << mat.row1[2] << ",\t" << mat.row1[3] << "\t],\n";
        os << "\t[" << mat.row2[0] << ",\t" << mat.row2[1] << ",\t" << mat.row2[2] << ",\t" << mat.row2[3] << "\t],\n";
        os << "\t[" << mat.row3[0] << ",\t" << mat.row3[1] << ",\t" << mat.row3[2] << ",\t" << mat.row3[3] << "\t],\n";
        os << "\t[" << mat.row4[0] << ",\t" << mat.row4[1] << ",\t" << mat.row4[2] << ",\t" << mat.row4[3] << "\t]\n";
        return os << "]";
    }
};

inline mat4f mat4f::operator+(mat4fc rhs) const {
    return mat4f(row1 + rhs.row1, row2 + rhs.row2, row3 + rhs.row3, row4 + rhs.row4);
}

inline mat4f mat4f::operator-(mat4fc rhs) const {
    return mat4f(row1 - rhs.row1, row2 - rhs.row2, row3 - rhs.row3, row4 - rhs.row4);
}

inline mat4f mat4f::mul(mat4fc rhs) const {
    return mat4f(row1 * rhs.row1, row2 * rhs.row2, row3 * rhs.row3, row4 * rhs.row4);
}

inline mat4f mat4f::operator/(mat4fc rhs) const {
    return mat4f(row1 / rhs.row1, row2 / rhs.row2, row3 / rhs.row3, row4 / rhs.row4);
}

inline vec4f mat4f::operator*(vec4fc rhs) const {
    return vec4f{
        dot4f(row1, rhs),
        dot4f(row2, rhs),
        dot4f(row3, rhs),
        dot4f(row4, rhs)
    };
}

static inline __m128 linear_combination(row4fc a, mat4fc b) {
    row4f result;
    result = _mm_shuffle_ps(a, a, 0) * b.row1;
    result += _mm_shuffle_ps(a, a, 0x55) * b.row2;
    result += _mm_shuffle_ps(a, a, 0xAA) * b.row3;
    result += _mm_shuffle_ps(a, a, 0xFF) * b.row4;
    return result;
}

inline mat4f mat4f::operator*(mat4fc rhs) const {
    row4fc out1 = linear_combination(row1, rhs);
    row4fc out2 = linear_combination(row2, rhs);
    row4fc out3 = linear_combination(row3, rhs);
    row4fc out4 = linear_combination(row4, rhs);

    return mat4f(out1, out2, out3, out4);
}

inline mat4f mat4f::operator-() const {
    return mat4f(-row1, -row2, -row3, -row4);
}

inline mat4f mat4f::T() const {
    return mat4f(
        row4f{row1[0], row2[0], row3[0], row4[0]},
        row4f{row1[1], row2[1], row3[1], row4[1]},
        row4f{row1[2], row2[2], row3[2], row4[2]},
        row4f{row1[3], row2[3], row3[3], row4[3]}
    );
}

/// @brief The `mat` is interpreted as a 2x2 matrix 
/// @param mat
static inline row4f inv2f(row4fc mat) {
    return row4fc{mat[3], -mat[1], -mat[2], mat[0]}/(mat[0]*mat[3] - mat[1]*mat[2]);
}

static inline float det2f(row4fc mat) {
    return 1.0f/(mat[0]*mat[3] - mat[1]*mat[2]);
}

static inline row4f mul2f(row4fc lhs, row4fc rhs) {
    return row4f{
        lhs[0]*rhs[0] + lhs[1]*rhs[2],
        lhs[0]*rhs[1] + lhs[1]*rhs[3],
        lhs[2]*rhs[0] + lhs[3]*rhs[2],
        lhs[2]*rhs[1] + lhs[3]*rhs[3]
    };
}

inline float mat4f::det() const {
    row4fc a0 = {row1[0], row1[1], row2[0], row2[1]};
    row4fc a1 = {row1[2], row1[3], row2[2], row2[3]};
    row4fc a2 = {row3[0], row3[1], row4[0], row4[1]};
    row4fc a3 = {row3[2], row3[3], row4[2], row4[3]};

    return det2f(a0 - mul2f(mul2f(a1,inv2f(a3)),a2)) * det2f(a3);
}

//TODO: Improve the performance
inline mat4f mat4f::inv() const {
    row4fc A = {row1[0], row1[1], row2[0], row2[1]};
    row4fc B = {row1[2], row1[3], row2[2], row2[3]};
    row4fc C = {row3[0], row3[1], row4[0], row4[1]};
    row4fc D = {row3[2], row3[3], row4[2], row4[3]};

    row4fc invA = inv2f(A);
    row4fc invDCAB = inv2f(D - mul2f(C, mul2f(invA, B)));

    row4fc b0 = invA + mul2f((invA, B), mul2f(invDCAB, mul2f(C, invA)));
    row4fc b1 = -mul2f(mul2f(invA, B), invDCAB);
    row4fc b2 = -mul2f(invDCAB, mul2f(C, invA));
    row4fc b3 = invDCAB;
    return mat4f(
        row4f{b0[0], b0[1], b1[0], b1[1]},
        row4f{b0[2], b0[3], b1[2], b1[3]},
        row4f{b2[0], b2[1], b3[0], b3[1]},
        row4f{b2[2], b2[3], b3[2], b3[3]}
    );
}

#endif