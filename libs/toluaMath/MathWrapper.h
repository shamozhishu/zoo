#ifndef __TOLUA_MATHWRAPPER_H__
#define __TOLUA_MATHWRAPPER_H__

#include <osg/Math>
#include <osg/Vec2f>
#include <osg/Vec2d>
#include <osg/Vec3f>
#include <osg/Vec3d>
#include <osg/Vec4f>
#include <osg/Vec4d>
#include <osg/Matrixf>
#include <osg/Matrixd>
#include <osg/Quat>

inline osg::Vec3f Cross(const osg::Vec3f& lhs, const osg::Vec3f& rhs)
{
	return lhs ^ rhs;
}

inline osg::Vec3d Cross(const osg::Vec3d& lhs, const osg::Vec3d& rhs)
{
	return lhs ^ rhs;
}

inline float Dot(const osg::Vec3f& lhs, const osg::Vec4f& rhs)
{
	return lhs * rhs;
}

inline float Dot(const osg::Vec4f& lhs, const osg::Vec3f& rhs)
{
	return lhs * rhs;
}

inline double Dot(const osg::Vec3d& lhs, const osg::Vec4d& rhs)
{
	return lhs * rhs;
}

inline double Dot(const osg::Vec3f& lhs, const osg::Vec4d& rhs)
{
	return lhs * rhs;
}

inline double Dot(const osg::Vec3d& lhs, const osg::Vec4f& rhs)
{
	return lhs * rhs;
}

inline double Dot(const osg::Vec4d& lhs, const osg::Vec3d& rhs)
{
	return lhs * rhs;
}

inline double Dot(const osg::Vec4d& lhs, const osg::Vec3f& rhs)
{
	return lhs * rhs;
}

inline double Dot(const osg::Vec4f& lhs, const osg::Vec3d& rhs)
{
	return lhs * rhs;
}

inline float GetBy(const osg::Matrixf& mat, int row, int col)
{
	return mat(row, col);
}

inline double GetBy(const osg::Matrixd& mat, int row, int col)
{
	return mat(row, col);
}

inline osg::Vec3f PreMult(const osg::Vec3f& v, const osg::Matrixf& m)
{
	return v * m;
}

inline osg::Vec3d PreMult(const osg::Vec3d& v, const osg::Matrixf& m)
{
	return v * m;
}

inline osg::Vec4f PreMult(const osg::Vec4f& v, const osg::Matrixf& m)
{
	return v * m;
}

inline osg::Vec4d PreMult(const osg::Vec4d& v, const osg::Matrixf& m)
{
	return v * m;
}

inline osg::Vec3f PreMult(const osg::Vec3f& v, const osg::Matrixd& m)
{
	return v * m;
}

inline osg::Vec3d PreMult(const osg::Vec3d& v, const osg::Matrixd& m)
{
	return v * m;
}

inline osg::Vec4f PreMult(const osg::Vec4f& v, const osg::Matrixd& m)
{
	return v * m;
}

inline osg::Vec4d PreMult(const osg::Vec4d& v, const osg::Matrixd& m)
{
	return v * m;
}

#endif // __TOLUA_MATHWRAPPER_H__
