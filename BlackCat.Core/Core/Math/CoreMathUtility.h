
#pragma once

#include "Core/CorePCH.h"
//#include "Input.h"
//#include "Utility.h"
//#include "bcShapes.h"
//#include "Core/Math/BasicMath.h"

namespace black_cat
{
	namespace core
	{
		const float Pi = 3.14159f;
		const float PiOver2 = Pi / 2.0f;
		const float PiOver4 = Pi / 4.0f;
		const float TwoPi = 6.28319f;

		#define DegreeToRadian(d)  d * (Pi / 180)

		inline float Log2(float n)
		{
			// log(n)/log(2) is log2.  
			return log(n) / log(2.0f);
		}

		inline float RoundDown(float pValue)
		{
			return static_cast<int>(pValue);
		}

		inline float RoundUp(float pValue)
		{
			if((pValue - static_cast<int>(pValue)) > 0)
				return static_cast<int>(pValue) + 1;
			return pValue;
		}

		//// Returns the forward vector from a transform matrix
		//inline bcVector ForwardVec(const bcMatrix4x4& matrix)
		//{
		//	return XMLoadFloat3(&XMFLOAT3(matrix._31, matrix._32, matrix._33));
		//}

		//// Returns the forward vector from a transform matrix
		//inline bcVector BackVec(const bcMatrix4x4& matrix)
		//{
		//	return XMLoadFloat3(&XMFLOAT3(-matrix._31, -matrix._32, -matrix._33));
		//}

		//// Returns the forward vector from a transform matrix
		//inline bcVector RightVec(const bcMatrix4x4& matrix)
		//{
		//	return XMLoadFloat3(&XMFLOAT3(matrix._11, matrix._12, matrix._13));
		//}

		//// Returns the forward vector from a transform matrix
		//inline bcVector LeftVec(const bcMatrix4x4& matrix)
		//{
		//	return XMLoadFloat3(&XMFLOAT3(-matrix._11, -matrix._12, -matrix._13));
		//}

		//// Returns the forward vector from a transform matrix
		//inline bcVector UpVec(const bcMatrix4x4& matrix)
		//{
		//	return XMLoadFloat3(&XMFLOAT3(matrix._21, matrix._22, matrix._23));
		//}

		//// Returns the forward vector from a transform matrix
		//inline bcVector DownVec(const bcMatrix4x4& matrix)
		//{
		//	return XMLoadFloat3(&XMFLOAT3(-matrix._21, -matrix._22, -matrix._23));
		//}

		//// Returns the translation vector from a transform matrix
		//inline bcVector TranslationVec(const bcMatrix4x4& matrix)
		//{
		//	return XMLoadFloat3(&XMFLOAT3(matrix._41, matrix._42, matrix._43));
		//}

		//// Sets the translation vector in a transform matrix
		//inline void SetTranslationVec(bcMatrix4x4& matrix, const bcVector& translation)
		//{
		//	matrix._41 = XMVectorGetX(translation);
		//	matrix._42 = XMVectorGetY(translation);
		//	matrix._43 = XMVectorGetZ(translation);
		//}

		//// Clamps a value to the specified range
		//inline float Clamp(float val, float min, float max)
		//{
		//	bcAssert(max >= min);

		//	if (val < min)
		//		val = min;
		//	else if (val > max)
		//		val = max;
		//	return val;
		//}

		//inline bcVector2 Clamp(const bcVector2& val, const bcVector2 min, const bcVector2 max)
		//{
		//	XMFLOAT2 retVal;
		//	retVal.x = Clamp(val.x, min.x, max.x);
		//	retVal.y = Clamp(val.y, min.y, max.y);
		//}

		//inline bcVector3 Clamp(const bcVector3& val, const bcVector3 min, const bcVector3 max)
		//{
		//	XMFLOAT3 retVal;
		//	retVal.x = Clamp(val.x, min.x, max.x);
		//	retVal.y = Clamp(val.y, min.y, max.y);
		//	retVal.z = Clamp(val.z, min.z, max.z);
		//}

		//inline bcVector4 Clamp(const bcVector4& val, const bcVector4 min, const bcVector4 max)
		//{
		//	XMFLOAT4 retVal;
		//	retVal.x = Clamp(val.x, min.x, max.x);
		//	retVal.y = Clamp(val.y, min.y, max.y);
		//	retVal.z = Clamp(val.z, min.z, max.z);
		//	retVal.w = Clamp(val.w, min.w, max.w);
		//}

		//inline bcVector3 Normalize(const bcVector3& val)
		//{
		//	float len = std::sqrtf(val.x * val.x + val.y * val.y + val.z * val.z);
		//	bcVector3 ret(val.x / len, val.y / len, val.z / len);
		//	return ret;
		//}

		//inline float Distance(const bcVector3& a, const bcVector3& b)
		//{
		//	XMVECTOR x = XMLoadFloat3(&a);
		//	XMVECTOR y = XMLoadFloat3(&b);
		//	XMVECTOR length = XMVector3Length(XMVectorSubtract(x, y));
		//	return XMVectorGetX(length);
		//}

		//inline float Distance(const bcVector2& a, const bcVector2& b)
		//{
		//	XMVECTOR x = XMLoadFloat2(&a);
		//	XMVECTOR y = XMLoadFloat2(&b);
		//	XMVECTOR length = XMVector2Length(XMVectorSubtract(x, y));
		//	return XMVectorGetX(length);
		//}

		//inline bcVector3 Cross(const bcVector3& a, const bcVector3& b)
		//{
		//	XMVECTOR x = XMLoadFloat3(&a);
		//	XMVECTOR y = XMLoadFloat3(&b);
		//	XMVECTOR cross = XMVector3Cross(x, y);
		//	bcVector3 result;
		//	XMStoreFloat3(&result, cross);
		//	return result;
		//}

		//inline float Dot(const bcVector3& a, const bcVector3& b)
		//{
		//	XMVECTOR x = XMLoadFloat3(&a);
		//	XMVECTOR y = XMLoadFloat3(&b);
		//	XMVECTOR dot = XMVector3Dot(x, y);
		//	float result;
		//	XMStoreFloat(&result, dot);
		//	return result;
		//}

		//inline bcVector3 TransformXMFloat3(const bcVector3& v, const bcMatrix4x4& m)
		//{
		//	XMVECTOR v1 = XMLoadFloat3(&v);
		//	v1 = XMVector3Transform(v1, m);
		//	bcVector3 result;
		//	XMStoreFloat3(&result, v1);
		//	return result;
		//}

		//inline bcVector3 TransformNormalXMFloat3(const bcVector3& v, const bcMatrix4x4& m)
		//{
		//	XMVECTOR v1 = XMLoadFloat3(&v);
		//	v1 = XMVector3TransformNormal(v1, m);
		//	bcVector3 result;
		//	XMStoreFloat3(&result, v1);
		//	return result;
		//}

		//inline bcVector3 RotateXMFloat3(const bcVector3& v, const bcVector& q)
		//{
		//	XMVECTOR v1 = XMLoadFloat3(&v);
		//	v1 = XMVector3Rotate(v1, q);
		//	bcVector3 result;
		//	XMStoreFloat3(&result, v1);
		//	return result;
		//}

		//inline bcVector3 RotateXMFloat3(const bcVector3& v, const bcMatrix4x4& m)
		//{
		//	XMVECTOR v1 = XMLoadFloat3(&v);
		//	XMVECTOR s;
		//	XMVECTOR q;
		//	XMVECTOR t;
		//	XMMatrixDecompose(&s, &q, &t, m);
		//	v1 = XMVector3Rotate(v1, q);
		//	bcVector3 result;
		//	XMStoreFloat3(&result, v1);
		//	return result;
		//}

		//// == Convert from Assimp to DirectX types ======================================

		//inline void aiVector3DToXMFLOAT3(aiVector3D& aiV, XMFLOAT3& XMV)
		//{
		//	XMV.x = aiV.x;
		//	XMV.y = aiV.y;
		//	XMV.z = aiV.z;
		//}

		//inline void aiMatrix4x4ToXMMATRIX(aiMatrix4x4& aiM, XMMATRIX& XMM)
		//{
		//	/*aiVector3D pos;
		//	aiQuaternion rot;
		//	aiVector3D scale;
		//	aiM.Decompose(scale, rot, pos);

		//	XMFLOAT3 f3;
		//	XMVECTOR pos1;
		//	XMVECTOR rot1;
		//	XMVECTOR scale1;

		//	aiVector3DToXMFLOAT3(pos, f3);
		//	pos1 = XMLoadFloat3(&f3);
		//	aiVector3DToXMFLOAT3(scale, f3);
		//	scale1 = XMLoadFloat3(&f3);
		//	rot1 = XMQuaternionIdentity();
		//	rot1 = XMLoadFloat4(&XMFLOAT4(rot.x, rot.y, rot.z, rot.w));

		//	XMM = XMMatrixAffineTransformation(scale1, XMLoadFloat3(&XMFLOAT3(0,0,0)), rot1, pos1);*/

		//	aiMatrix4x4 tempM = aiM.Transpose();
		//	XMM._11 = tempM.a1;
		//	XMM._12 = tempM.a2;
		//	XMM._13 = tempM.a3;
		//	XMM._14 = tempM.a4;

		//	XMM._21 = tempM.b1;
		//	XMM._22 = tempM.b2;
		//	XMM._23 = tempM.b3;
		//	XMM._24 = tempM.b4;

		//	XMM._31 = tempM.c1;
		//	XMM._32 = tempM.c2;
		//	XMM._33 = tempM.c3;
		//	XMM._34 = tempM.c4;

		//	XMM._41 = tempM.d1;
		//	XMM._42 = tempM.d2;
		//	XMM._43 = tempM.d3;
		//	XMM._44 = tempM.d4;
		//}

		//inline bcRay GetRayFromScreenPointer(INT mouseDX, INT mouseDY, UINT width, UINT height, const XMMATRIX& view, const XMMATRIX& proj)
		//{
		//	// Compute picking ray in view space.
		//	float vx = (+2.0f*mouseDX/width  - 1.0f)/proj(0,0);
		//	float vy = (-2.0f*mouseDY/height + 1.0f)/proj(1,1);

		//	// Ray definition in view space.
		//	XMVECTOR rayOrigin = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		//	XMVECTOR rayDir    = XMVectorSet(vx, vy, 1.0f, 0.0f);

		//	// Transform ray to world space
		//	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(view), view);

		//	rayOrigin = XMVector3TransformCoord(rayOrigin, invView);
		//	rayDir = XMVector3TransformNormal(rayDir, invView);

		//	// Make the ray direction unit length for the intersection tests.
		//	rayDir = XMVector3Normalize(rayDir);

		//	XMFLOAT3 rayPosition;
		//	XMFLOAT3 rayDirection;
		//	XMStoreFloat3(&rayPosition, rayOrigin);
		//	XMStoreFloat3(&rayDirection, rayDir);
		//	bcRay ray(rayPosition, rayDirection);
		//	return ray;
		//}

		//inline FLOAT GetExactHeightFromHeightMap(FLOAT x, FLOAT z, const FLOAT* heightMap, const XMFLOAT3& heightMapCenter, UINT heightMapWidth, UINT heightMapHeight, float heightMapMultiplier)
		//{
		//	FLOAT xCoord = x;
		//	FLOAT zCoord = z;

		//	xCoord -= heightMapCenter.x;
		//	zCoord -= heightMapCenter.z;

		//	xCoord /= heightMapMultiplier;
		//	zCoord /= heightMapMultiplier;

		//	xCoord = xCoord + (heightMapWidth / 2);
		//	zCoord = zCoord + (heightMapHeight / 2);

		//	bool invalid = xCoord < 0;
	 //           invalid |= zCoord < 0;
	 //           invalid |= xCoord > heightMapWidth;
	 //           invalid |= zCoord > heightMapHeight;
	 //           if (invalid)
	 //               return 0;
	 //                       
		//	int xLower = (int)xCoord;
		//	int xHigher = xLower + 1;
		//	float xRelative = (xCoord - xLower) / ((float)xHigher - (float)xLower);

		//	int zLower = (int)zCoord;
		//	int zHigher = zLower + 1;
		//	float zRelative = (zCoord - zLower) / ((float)zHigher - (float)zLower);
		//	
		//	float heightLxLz = heightMap[Convert2DTo1DIndex(xLower, zLower, heightMapWidth + 1, heightMapHeight + 1)];
		//	float heightLxHz = heightMap[Convert2DTo1DIndex(xLower, zHigher, heightMapWidth + 1, heightMapHeight + 1)];
		//	float heightHxLz = heightMap[Convert2DTo1DIndex(xHigher, zLower, heightMapWidth + 1, heightMapHeight + 1)];
		//	float heightHxHz = heightMap[Convert2DTo1DIndex(xHigher, zHigher, heightMapWidth + 1, heightMapHeight + 1)];

		//	bool cameraAboveLowerTriangle = (xRelative + zRelative < 1);

		//	float finalHeight;
		//	if (cameraAboveLowerTriangle)
		//	{
		//		finalHeight = heightLxLz;
		//		finalHeight += zRelative * (heightLxHz - heightLxLz);
		//		finalHeight += xRelative * (heightHxLz - heightLxLz);
		//	}
		//	else
		//	{
		//		finalHeight = heightHxHz;
		//		finalHeight += (1.0f - zRelative) * (heightHxLz - heightHxHz);
		//		finalHeight += (1.0f - xRelative) * (heightLxHz - heightHxHz);
		//	}

		//	return finalHeight + heightMapCenter.y;
		//}

		////template<typename T>
		//class SmoothStep
		//{
		//public:

		//	enum SmoothStepAction
		//	{
		//		SmoothStepAction_Release = 0,
		//		SmoothStepAction_Up = 1,
		//		SmoothStepAction_Down = 2
		//	};

		//	SmoothStep();
		//	SmoothStep(float stepValue, float maxValue, float minValue, float middleValue, Keys upKey, Keys downKey, bool downValueOnRelease, float onReleaseStepValue);
		//	~SmoothStep();

		//	void Update(KeyboardState* keyState);
		//	void Update(SmoothStepAction action);

		//	float Value() { return value; };
		//	float StepValue() { return stepValue; };
		//	void StepValue(float newValue) { stepValue = newValue; };
		//	float MaxValue() { return maxValue; };
		//	void MaxValue(float newValue) { maxValue = newValue; };
		//	float MinValue() { return minValue; };
		//	void MinValue(float newValue) { minValue = newValue; };
		//	float MiddleValue() { return middleValue; };
		//	void MiddleValue(float newValue) { middleValue = newValue; };
		//	float OnReleaseStepValue() { return onReleaseStepValue; };
		//	void OnReleaseStepValue(float newValue) { onReleaseStepValue = newValue; };

		//protected:
		//	float value;
		//	float stepValue;
		//	float maxValue;
		//	float minValue;
		//	float middleValue;

		//	bool downValueOnRelease;
		//	float onReleaseStepValue;

		//	Keys upKey;
		//	Keys downKey;

		//private:

		//};
	}
}