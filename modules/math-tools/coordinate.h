/**
 * Coordinate transformer header.
 * \author trantuan-20048607
 * \date 2022.1.30
 * \attention It's recommended to include battlefield.h for complete function.
 */

#ifndef COORDINATE_H_
#define COORDINATE_H_

#include <Eigen/Core>
#include <Eigen/Dense>
#include <opencv2/core/types.hpp>
#include <opencv2/core/eigen.hpp>
#include <ceres/jet.h>
#include "algorithms.h"

namespace coordinate {
    typedef Eigen::Vector3d TranslationVector;
    typedef Eigen::Vector3d RotationVector;

    typedef Eigen::Matrix<double, 3, 1> TranslationMatrix;
    typedef Eigen::Matrix3d RotationMatrix;

    typedef Eigen::Quaternionf Quaternion;
}

namespace coordinate::transform {
    [[maybe_unused]] inline RotationMatrix
    QuaternionToRotationMatrix(const Quaternion &quaternion) {
        // Prefix "e_" here means "Euler angle".
        double
                e_yaw = atan2(2 * (quaternion.w() * quaternion.z() + quaternion.x() * quaternion.y()),
                              2 * (quaternion.w() * quaternion.w() + quaternion.x() * quaternion.x()) - 1),
                e_roll = asin(-2 * (quaternion.x() * quaternion.z() - quaternion.w() * quaternion.y())),
                e_pitch = atan2(2 * (quaternion.w() * quaternion.x() + quaternion.y() * quaternion.z()),
                                2 * (quaternion.w() * quaternion.w() + quaternion.z() * quaternion.z()) - 1);

        // Prefix "r_" here means "rotation".
        RotationMatrix r_yaw, r_roll, r_pitch;

        // [Experimental] Use SSE2 or NEON to accelerate sine and cosine.
#if defined(__x86_64__) | defined(__aarch64__)
        float r[4] = {float(e_yaw), float(e_roll), float(e_pitch), float(0)}, sin_r[4], cos_r[4];
        algorithm::SinCosFloatX4(r, sin_r, cos_r);
        r_yaw << cos_r[0], 0, sin_r[0],
                0, 1, 0,
                -sin_r[0], 0, cos_r[0];
        r_roll << cos_r[1], -sin_r[1], 0,
                sin_r[1], cos_r[1], 0,
                0, 0, 1;
        r_pitch << 1, 0, 0,
                0, cos_r[2], -sin_r[2],
                0, sin_r[2], cos_r[2];
#else
        r_yaw << cos(e_yaw), 0, sin(e_yaw),
                0, 1, 0,
                -sin(e_yaw), 0, cos(e_yaw);
        r_roll << cos(e_roll), -sin(e_roll), 0,
                sin(e_roll), cos(e_roll), 0,
                0, 0, 1;
        r_pitch << 1, 0, 0,
                0, cos(e_pitch), -sin(e_pitch),
                0, sin(e_pitch), cos(e_pitch);
#endif

        return r_yaw * r_pitch * r_roll;
    }

    [[maybe_unused]] inline TranslationVector
    CameraToWorld(const TranslationVector &tv_cam,
                  const RotationMatrix &rm_imu,
                  const TranslationMatrix &tm_cam_to_imu,
                  const RotationMatrix &rm_cam_to_imu) {
        return (rm_cam_to_imu * rm_imu).transpose() * (tv_cam + tm_cam_to_imu);
    }

    [[maybe_unused]] inline TranslationVector
    WorldToCamera(const TranslationVector &tv_world,
                  const RotationMatrix &rm_imu_to_world,
                  const TranslationMatrix &tm_cam_to_imu,
                  const RotationMatrix &rm_cam_to_imu) {
        return (rm_cam_to_imu * rm_imu_to_world) * tv_world - tm_cam_to_imu;
    }
}

namespace coordinate::convert {
    /**
     * \brief Transform rectangular coordinate to spherical coordinate.
     * \tparam T Template type.
     * \param [in] rectangular Point (x, y, z) in rectangular coordinate system.
     * \param [out] spherical Point (yaw, pitch, distance) in spherical coordinate system.
     */
    template<typename T>
    inline void Rectangular2Spherical(const T rectangular[3], T spherical[3]) {
        spherical[0] = ceres::atan2(rectangular[0], rectangular[2]);
        spherical[1] = ceres::atan2(rectangular[1],
                                    ceres::sqrt(rectangular[0] * rectangular[0] + rectangular[2] * rectangular[2]));
        spherical[2] = ceres::sqrt(rectangular[0] * rectangular[0] +
                                   rectangular[1] * rectangular[1] +
                                   rectangular[2] * rectangular[2]);
    }

    /**
     * \brief Transform rectangular coordinate to spherical coordinate.
     * \param rectangular Point (x, y, z) in rectangular coordinate system.
     * \return Point (yaw, pitch, distance) in spherical coordinate system.
     */
    inline Eigen::Vector3d Rectangular2Spherical(const Eigen::Vector3d &rectangular) {
        Eigen::Vector3d spherical;
        spherical(0, 0) = atan2(rectangular(0, 0), rectangular(2, 0));
        spherical(1, 0) = atan2(rectangular(1, 0),
                                sqrt(rectangular(0, 0) * rectangular(0, 0) +
                                     rectangular(2, 0) * rectangular(2, 0)));
        spherical(2, 0) = sqrt(rectangular(0, 0) * rectangular(0, 0) +
                               rectangular(1, 0) * rectangular(1, 0) +
                               rectangular(2, 0) * rectangular(2, 0));
        return spherical;
    }
}

#endif  // COORDINATE_H_
