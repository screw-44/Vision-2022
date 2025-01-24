/**
 * Armor definition header.
 * \author trantuan-20048607
 * \date 2022.1.28
 * \attention It's recommended to include battlefield.h for complete function.
 */

#ifndef ARMOR_H_
#define ARMOR_H_

#include <opencv2/calib3d.hpp>
#include "data-structure/bbox_t.h"
#include "math-tools/coordinate.h"
#include "math-tools/algorithms.h"
#include "../component.h"

class Armor : public Component {
public:
    ATTR_READER_REF(corners_, Corners)

    ATTR_READER_REF(center_, Center)

    ATTR_READER_REF(rotation_vector_cam_, RotationVectorCam)

    ATTR_READER_REF(rotation_vector_world_, RotationVectorWorld)

    ATTR_READER_REF(translation_vector_cam_, TranslationVectorCam)

    ATTR_READER_REF(translation_vector_world_, TranslationVectorWorld)

    ATTR_READER(id_, ID)

    ATTR_READER(distance_, Distance)

    ATTR_READER(confidence_, Confidence)

    Armor(const bbox_t &box,
          const cv::Mat &intrinsic_mat,
          const cv::Mat &distortion_mat,
          const Eigen::Quaternionf &quaternion) :
            Component(Colors(box.color), kArmor),
            id_(box.id),
            confidence_(box.confidence) {
        const static std::vector<cv::Point3d> small_armor_pc = {
                {-0.066, 0.027,  0.},
                {-0.066, -0.027, 0.},
                {0.066,  -0.027, 0.},
                {0.066,  0.027,  0.}
        }, big_armor_pc = {
                {-0.115, 0.029,  0.},
                {-0.115, -0.029, 0.},
                {0.115,  -0.029, 0.},
                {0.115,  0.029,  0.}
        };

        for (auto i = 0; i < 4; ++i)
            corners_[i] = box.points[i];

        cv::Mat rv_cam, tv_cam;
        std::vector<cv::Point2f> image_points(corners_, corners_ + 4);

        switch (id_) {
            case 0:  // Sentry.
            case 1:  // Hero.
            case 6:  // Base.
                cv::solvePnP(big_armor_pc,
                             image_points,
                             intrinsic_mat,
                             distortion_mat,
                             rv_cam,
                             tv_cam);
                break;
            case 2:  // Engineer.
                cv::solvePnP(small_armor_pc,
                             image_points,
                             intrinsic_mat,
                             distortion_mat,
                             rv_cam,
                             tv_cam);
                break;
            case 3:
            case 4:
            case 5:
                double armor_height_pixel = std::max(
                        corners_[0].y - corners_[3].y,
                        corners_[1].y - corners_[2].y
                ), armor_width_pixel = std::max(
                        corners_[0].x - corners_[1].x,
                        corners_[1].x - corners_[2].x
                );
                // TODO Value armor_width_pixel / armor_height_pixel depends on camera and lens' chose.
                //   Further testing is required.
                if (armor_width_pixel / armor_height_pixel > 1.3) {
                    cv::solvePnP(big_armor_pc,
                                 image_points,
                                 intrinsic_mat,
                                 distortion_mat,
                                 rv_cam,
                                 tv_cam);
                } else {
                    cv::solvePnP(small_armor_pc,
                                 image_points,
                                 intrinsic_mat,
                                 distortion_mat,
                                 rv_cam,
                                 tv_cam);
                }
                break;
        }

        cv::cv2eigen(rv_cam, rotation_vector_cam_);
        cv::cv2eigen(tv_cam, translation_vector_cam_);

        center_ = (corners_[0] + corners_[1] + corners_[2] + corners_[3]) / 4;
        distance_ = (float) translation_vector_cam_.norm();

        // IMU and Camera joint calibration.
        const static double rm_cam_to_imu_data[] = {1, 0, 0, 0, 1, 0, 0, 0, 1};
        const static coordinate::RotationMatrix camera_to_imu_rotation_matrix(rm_cam_to_imu_data);

        const static double tm_cam_to_imu_data[] = {0, -0.026, 0.075};
        const static coordinate::TranslationMatrix camera_to_imu_translation_matrix(tm_cam_to_imu_data);

        translation_vector_world_ = coordinate::transform::CameraToWorld(
                translation_vector_cam_,
                coordinate::transform::QuaternionToRotationMatrix(quaternion),
                camera_to_imu_translation_matrix,
                camera_to_imu_rotation_matrix
        );
    }

    bool operator==(const Armor &armor) const {
        return id_ == armor.id_
               && corners_[0] == armor.corners_[0]
               && corners_[1] == armor.corners_[1]
               && corners_[2] == armor.corners_[2]
               && corners_[3] == armor.corners_[3]
               && rotation_vector_cam_ == armor.rotation_vector_cam_
               && translation_vector_cam_ == armor.translation_vector_cam_
               && rotation_vector_world_ == armor.rotation_vector_world_
               && translation_vector_world_ == armor.translation_vector_world_
               && distance_ == armor.distance_
               && confidence_ == armor.confidence_;
    };

    bool operator!=(const Armor &armor) const {
        return !(SELF == armor);
    };

    bool operator==(const Armor &armor) {
        return id_ == armor.id_
               && corners_[0] == armor.corners_[0]
               && corners_[1] == armor.corners_[1]
               && corners_[2] == armor.corners_[2]
               && corners_[3] == armor.corners_[3]
               && rotation_vector_cam_ == armor.rotation_vector_cam_
               && translation_vector_cam_ == armor.translation_vector_cam_
               && rotation_vector_world_ == armor.rotation_vector_world_
               && translation_vector_world_ == armor.translation_vector_world_
               && distance_ == armor.distance_
               && confidence_ == armor.confidence_;
    };

    bool operator!=(const Armor &armor) {
        return !(*this == armor);
    };

    /**
     * \brief Calculate area of quadrangle.
     * \return Area of the armor.
     */
    [[nodiscard]] inline auto Area() const {
        return algorithm::PolygonArea<4>(corners_);
    }

private:
    unsigned int id_;

    cv::Point2f corners_[4];
    cv::Point2f center_;

    coordinate::RotationVector rotation_vector_cam_;
    coordinate::TranslationVector translation_vector_cam_;

    coordinate::RotationVector rotation_vector_world_;
    coordinate::TranslationVector translation_vector_world_;

    float distance_;
    float confidence_;
};

#endif  // ARMOR_H_
