#ifndef ALGORITHMS_H_
#define ALGORITHMS_H_

#include <cmath>
#include <opencv2/core/types.hpp>
#include "hardware_acceleration.h"

namespace algorithm {

    /**
     * \brief Calculate both sine and cosine for 4 floats at the same time.
     * \param [in] x Input 4 floats.
     * \param [out] s Output sine value.
     * \param [out] c Output cosine value.
     */
    inline void SinCosFloatX4(const float x[4], float s[4], float c[4]) {
#if defined(__x86_64__) | defined(__aarch64__)
        v4sf x_v4sf = {x[0], x[1], x[2], x[3]}, s_v4sf, c_v4sf;
        sincos_ps(x_v4sf, &s_v4sf, &c_v4sf);
        auto s_ptr = (float *) &s_v4sf, c_ptr = (float *) &c_v4sf;
        s[0] = *s_ptr++;
        c[0] = *c_ptr++;
        s[1] = *s_ptr++;
        c[1] = *c_ptr++;
        s[2] = *s_ptr++;
        c[2] = *c_ptr++;
        s[3] = *s_ptr;
        c[3] = *c_ptr;
#else
        for (auto i = 0; i < 4; i++) {
            s[i] = std::sin(x[i]);
            c[i] = std::cos(x[i]);
        }
#endif
    }

    /**
     * \brief Calculate sine for 4 floats at the same time.
     * \param [in,out] x Input 4 floats and will be output.
     */
    [[maybe_unused]] inline void SinFloatX4(float x[4]) {
#if defined(__x86_64__) | defined(__aarch64__)
        v4sf x_v4sf = {x[0], x[1], x[2], x[3]}, s_v4sf = sin_ps(x_v4sf);
        auto s_ptr = (float *) &s_v4sf;
        x[0] = *s_ptr++;
        x[1] = *s_ptr++;
        x[2] = *s_ptr++;
        x[3] = *s_ptr;
#else
        for (auto i = 0; i < 4; i++)
            x[i] = std::sin(x[i]);
#endif
    }

    /**
     * \brief Calculate cosine for 4 floats at the same time.
     * \param [in,out] x Input 4 floats and will be output.
     */
    [[maybe_unused]] inline void CosFloatX4(float x[4]) {
#if defined(__x86_64__) | defined(__aarch64__)
        v4sf x_v4sf = {x[0], x[1], x[2], x[3]}, c_v4sf = cos_ps(x_v4sf);
        auto c_ptr = (float *) &c_v4sf;
        x[0] = *c_ptr++;
        x[1] = *c_ptr++;
        x[2] = *c_ptr++;
        x[3] = *c_ptr;
#else
        for (auto i = 0; i < 4; i++)
            x[i] = std::cos(x[i]);
#endif
    }

    /**
     * \brief Calculate tangent for 4 floats at the same time.
     * \param [in,out] x Input 4 floats and will be output.
     */
    [[maybe_unused]] inline void TanFloatX4(float x[4]) {
#if defined(__x86_64__) | defined(__aarch64__)
        v4sf x_v4sf = {x[0], x[1], x[2], x[3]}, t_v4sf = tan_ps(x_v4sf);
        auto t_ptr = (float *) &t_v4sf;
        x[0] = *t_ptr++;
        x[1] = *t_ptr++;
        x[2] = *t_ptr++;
        x[3] = *t_ptr;
#else
        for (auto i = 0; i < 4; i++)
            x[i] = std::tan(x[i]);
#endif
    }

    /**
     * \brief Calculate cotangent for 4 floats at the same time.
     * \param [in,out] x Input 4 floats and will be output.
     */
    [[maybe_unused]] inline void CotFloatX4(float x[4]) {
#if defined(__x86_64__) | defined(__aarch64__)
        v4sf x_v4sf = {x[0], x[1], x[2], x[3]}, t_v4sf = cot_ps(x_v4sf);
        auto t_ptr = (float *) &t_v4sf;
        x[0] = *t_ptr++;
        x[1] = *t_ptr++;
        x[2] = *t_ptr++;
        x[3] = *t_ptr;
#else
        for (auto i = 0; i < 4; i++)
            x[i] = 1.f / std::tan(x[i]);
#endif
    }

    /**
     * \brief Calculate arc tangent for 4 floats at the same time.
     * \param [in,out] x Input 4 floats and will be output.
     */
    [[maybe_unused]] inline void AtanFloatX4(float x[4]) {
#if defined(__x86_64__) | defined(__aarch64__)
        v4sf x_v4sf = {x[0], x[1], x[2], x[3]}, t_v4sf = atan_ps(x_v4sf);
        auto t_ptr = (float *) &t_v4sf;
        x[0] = *t_ptr++;
        x[1] = *t_ptr++;
        x[2] = *t_ptr++;
        x[3] = *t_ptr;
#else
        for (auto i = 0; i < 4; i++)
            x[i] = std::atan(x[i]);
#endif
    }

    // ------------------------------------------------

    /**
     * \brief Calculate arc tangent II for 4 floats at the same time.
     * \param [in] y Input 4 ys.
     * \param [in] x Input 4 xs.
     * \param [out] res Output 4 results.
     */
    [[maybe_unused]] inline void Atan2FloatX4(const float y[4], const float x[4], float res[4]) {
#if defined(__x86_64__) | defined(__aarch64__)
        v4sf x_v4sf = {x[0], x[1], x[2], x[3]}, y_v4sf = {y[0], y[1], y[2], y[3]};
        v4sf res_v4sf = atan2_ps(y_v4sf, x_v4sf);
        auto res_ptr = (float *) &res_v4sf;
        res[0] = *res_ptr++;
        res[1] = *res_ptr++;
        res[2] = *res_ptr++;
        res[3] = *res_ptr;
#else
        for (auto i = 0; i < 4; i++)
            res[i] = std::atan2(y[i], x[i]);
#endif
    }

    /**
     * \brief Calculate arc tangent II for single float at the same time.
     * \param [in] y Input y.
     * \param [in] x Input x.
     * \return atan2(y, x).
     */
    [[maybe_unused]] inline float Atan2Float(float y, float x) {
#if defined(__x86_64__) | defined(__aarch64__)
        return atan2_ref(y, x);
#else
        return std::atan2(y, x);
#endif
    }

    // ------------------------------------------------

    /**
     * \brief Calculate area of N-side polygon.
     * \param [in] points Input vector, should in clockwise order.
     * \return Area value.
     */
    [[maybe_unused]] inline double PolygonArea(const std::vector<cv::Point2f> &points) {
        auto n = points.size();
        double area = 0;
        for (auto i = 0; i < n; ++i) {
            auto j = (i + 1) % n;
            area += points[i].x * points[j].y;
            area -= points[i].y * points[j].x;
        }
        area *= 0.5;
        return (area < 0 ? -area : area);
    }

    /**
     * \brief Calculate area of N-side polygon.
     * \tparam N Size of input array.
     * \param [in] points Input array, should in clockwise order.
     * \return Area value.
     */
    template<unsigned int N>
    double PolygonArea(const cv::Point2f points[N]) {
        double area = 0;
        for (auto i = 0; i < N; ++i) {
            auto j = (i + 1) % N;
            area += points[i].x * points[j].y;
            area -= points[i].y * points[j].x;
        }
        area *= 0.5;
        return (area < 0 ? -area : area);
    }

    // ------------------------------------------------

    [[maybe_unused]] inline float SqrtFloat(float x) {
#if defined(__x86_64__) | defined(__aarch64__)
        return sqrt_ps(x);
#else
        return std::sqrt(x);
#endif
    }

    inline float RsqrtFloat(float x) {
#if defined(__x86_64__) | defined(__aarch64__)
        return rsqrt_ps(x);
#else
        return 1 / std::sqrt(x);
#endif
    }

    // ------------------------------------------------

    /**
     * \brief Calculate angle between two 2D vectors.
     * \param [in] vector_a Vector A.
     * \param [in] vector_b Vector B.
     * \return Angle of two vectors, in DEGREE.
     */
    inline float VectorAngle(const cv::Point2f &vector_a, const cv::Point2f &vector_b) {
        static const float rad_to_deg = 180.0 / CV_PI;
        return std::acos(float(vector_a.dot(vector_b))
                         * RsqrtFloat(vector_a.x * vector_a.x + vector_a.y * vector_a.y)
                         * RsqrtFloat(vector_b.x * vector_b.x + vector_b.y * vector_b.y))
               * rad_to_deg;
    }
}

#endif  // ALGORITHMS_H_
