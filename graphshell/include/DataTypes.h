#ifndef __DataTypes_h__
#define __DataTypes_h__

#include "Eigen/Core"

typedef Eigen::Matrix<double, 2, 2> Matrix2x2;
typedef Eigen::Matrix<double, 3, 3> Matrix3x3;
typedef Eigen::Matrix<double, 4, 4> Matrix4x4;

typedef Eigen::Matrix<double, 2, 1> Vector2;
typedef Eigen::Matrix<double, 3, 1> Vector3;
typedef Eigen::Matrix<double, 4, 1> Vector4;

typedef std::vector<Vector3, Eigen::aligned_allocator<Vector3> > stdvectorV3;

typedef Eigen::Matrix<float, 3, 1> Vector3f;
typedef Eigen::Matrix<float, 3, 3> Matrix3x3f;

typedef std::vector<Vector3f, Eigen::aligned_allocator<Vector3f> > stdvectorV3f;

#endif
