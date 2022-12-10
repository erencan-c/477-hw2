#ifndef __VEC_H__
#define __VEC_H__

#include <stdio.h>
#include <array>
#include <math.h>
#include <iostream>
#include <immintrin.h>

#define EPISLON 1e-9
#include <array>

typedef __m256d vec4;
typedef const vec4 vec4c;

typedef const float cfloat;

typedef std::array<double,4> arr4;
typedef const arr4 arr4c;

static inline double dot4(vec4c lhs, vec4c rhs) {
   auto multiplied = lhs * rhs;
   auto sum = _mm256_hadd_pd(multiplied, multiplied);
   auto sum_high = _mm256_extractf128_pd(sum, 1);
   return (sum_high + _mm256_castpd256_pd128(sum))[0];
   // auto lo_dual = multiplied;
   // auto hi_dual = _mm256_movehl_ps(multiplied, multiplied);
   // auto sum_dual = lo_dual + hi_dual;
   // auto lo = sum_dual;
   // auto hi = _mm_shuffle_ps(sum_dual, sum_dual, 0x1);
   // auto sum = lo + hi;
   // return _mm_cvtss_f32(sum);
}

static inline vec4 cross4(vec4c lhs, vec4c rhs) {
   auto tmp0 = _mm256_shuffle_pd(lhs, lhs, _MM_SHUFFLE(3,0,2,1));
   auto tmp2 = _mm256_shuffle_pd(lhs, lhs, _MM_SHUFFLE(3,1,0,2));
   auto tmp1 = _mm256_shuffle_pd(rhs, rhs, _MM_SHUFFLE(3,1,0,2));
   auto tmp3 = _mm256_shuffle_pd(rhs, rhs, _MM_SHUFFLE(3,0,2,1));
   return tmp0*tmp1 - tmp2*tmp3;
}

static inline float len4f(vec4c self) {
   return sqrtf(dot4(self,self));
}

static inline vec4 normalize4(vec4c self) {
   return self / len4f(self);
}

static inline vec4 vmake(arr4c& arr) {
   return _mm256_load_pd(arr.data());
}

static inline arr4 amake(vec4c v) {
   arr4 a;
   _mm256_store_pd(a.data(), v);
   return a;
}

static inline std::ostream& operator<<(std::ostream& os, vec4c v) {
   arr4c arr = amake(v);
   std::printf("vec4f{%g, %g, %g, %g}", arr[0], arr[1], arr[2], arr[3]);
   return os;
}

static inline vec4 pow4(vec4c lhs, vec4c rhs) {
   arr4c lhsa = amake(lhs);
   arr4c rhsa = amake(rhs);
   return vec4{powf(lhsa[0], rhsa[0]), powf(lhsa[1], rhsa[1]), powf(lhsa[2], rhsa[2]), powf(lhsa[3], rhsa[3])};
}

static inline vec4 min4(vec4c lhs, vec4c rhs) {
   arr4c lhsa = amake(lhs);
   arr4c rhsa = amake(rhs);
   return vec4{fmin(lhsa[0], rhsa[0]), fmin(lhsa[1], rhsa[1]), fmin(lhsa[2], rhsa[2]), fmin(lhsa[3], rhsa[3])};
}

static inline vec4 max4(vec4c lhs, vec4c rhs) {
   arr4c lhsa = amake(lhs);
   arr4c rhsa = amake(rhs);
   return vec4{fmax(lhsa[0], rhsa[0]), fmax(lhsa[1], rhsa[1]), fmax(lhsa[2], rhsa[2]), fmax(lhsa[3], rhsa[3])};
}

#endif
