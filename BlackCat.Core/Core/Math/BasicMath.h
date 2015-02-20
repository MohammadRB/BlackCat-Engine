#pragma once

#ifndef BasicMath_H
#define BasicMath_H

#include "Core/CorePCH.h"
#include "xnamath.h"

namespace black_cat
{
	namespace core
	{
		typedef XMVECTOR bcVector128;
		class bcVector2;
		class bcVector3;
		class bcVector4;
		class bcMatrix4x4;
		class bcQuaternion;

		class BC_COREDLL_EXP bcVector2 : public XMFLOAT2
		{
		public:

			bcVector2( );
			bcVector2( float x, float y );
			bcVector2( const bcVector2& Vector );

			// vector operations
			void Clamp( );
			void MakeZero( );
			float Lenght( );
			float LenghtSq( );
			void Normalize( );
			bcVector2 Perpendicular( );

			// static vector ops

			static void ToVector(const bcVector2& V2, bcVector128* out);
			static void FromVector(const bcVector128& V, bcVector2* out);

#pragma region Commented
			//static bcVector2 Add( const bcVector2& A, const bcVector2& B );
			//static bcVector2 Divide( const bcVector2& A, const bcVector2& B );
			//static bcVector2 Divide( const bcVector2& A, float fScaler );
			//static bcVector2 Multipy( const bcVector2& A, const bcVector2& B );
			//static bcVector2 Multipy( const bcVector2& A, float fScaler );
			//static bcVector2 Negate( const bcVector2& A );
			//static bcVector2 Subtract ( const bcVector2& A, const bcVector2& B );
			//static bcVector2 Clamp( const bcVector2& A );
			//static bcVector2 Cross( const bcVector2& A, const bcVector2& B );
			//static float Dot( const bcVector2& A, const bcVector2& B );
			//static bcVector2 Normalize( const bcVector2& A );
			//static bcVector2 Perpendicular( const bcVector2& A );
			//static bcVector2 Random( );
			//static bcVector2 Lerp( const bcVector2& A, const bcVector2& B, float s );
			//static bcVector2 Barycentric( const bcVector2& value1, const bcVector2& value2, const bcVector2& value3, FLOAT amount1, FLOAT amount2);
			//static bcVector2 CatmullRom( const bcVector2& value1, const bcVector2& value2, const bcVector2& value3, const bcVector2& value4, float amount);
			//static FLOAT Distance( const bcVector2& A, const bcVector2& B );
			//static FLOAT DistanceSq( const bcVector2& A, const bcVector2& B );
			//static bcVector2 Max( const bcVector2& A, const bcVector2& B );
			//static bcVector2 Min( const bcVector2& A, const bcVector2& B );
			//static bcVector2 Reflect( const bcVector2& A, const bcVector2& normal );
			//static bcVector2 Refract( const bcVector2& A, const bcVector2& normal, FLOAT refractionIndex );
			///*static bcVector2 Transform( const bcVector2& A, const bcQuaternion& Q );
			//static void Transform( const bcVector2* inArray, const bcQuaternion& Q , bcVector2* outArray);*/
			//static bcVector2 Transform( const bcVector2& A, const bcMatrix4x4& M );
			////static void Transform( const bcVector2* inArray, const bcMatrix4x4& M , bcVector2* outArray);
			//static bcVector2 TransformNormal( const bcVector2& A, const bcMatrix4x4& M );
#pragma endregion

			static void Add( const bcVector2& A, const bcVector2& B, bcVector2* out );
			static void Divide( const bcVector2& A, const bcVector2& B, bcVector2* out );
			static void Divide( const bcVector2& A, float fScaler, bcVector2* out );
			static void Multipy( const bcVector2& A, const bcVector2& B, bcVector2* out );
			static void Multipy( const bcVector2& A, float fScaler, bcVector2* out );
			static void Negate( const bcVector2& A, bcVector2* out );
			static void Subtract ( const bcVector2& A, const bcVector2& B, bcVector2* out );
			static void Clamp( const bcVector2& A, bcVector2* out );
			static void Cross( const bcVector2& A, const bcVector2& B, bcVector2* out );
			static void Dot( const bcVector2& A, const bcVector2& B, float* out );
			static void Normalize( const bcVector2& A, bcVector2* out );
			static void Perpendicular( const bcVector2& A, bcVector2* out );
			static void Random( bcVector2* out );
			static void Lerp( const bcVector2& A, const bcVector2& B, float s, bcVector2* out );
			static void Barycentric( const bcVector2& value1, const bcVector2& value2, const bcVector2& value3, FLOAT amount1, FLOAT amount2, bcVector2* out);
			static void CatmullRom( const bcVector2& value1, const bcVector2& value2, const bcVector2& value3, const bcVector2& value4, float amount, bcVector2* out);
			static void Distance( const bcVector2& A, const bcVector2& B, float* out );
			static void DistanceSq( const bcVector2& A, const bcVector2& B, float* out );
			static void Max( const bcVector2& A, const bcVector2& B, bcVector2* out );
			static void Min( const bcVector2& A, const bcVector2& B, bcVector2* out );
			static void Reflect( const bcVector2& A, const bcVector2& normal, bcVector2* out );
			static void Refract( const bcVector2& A, const bcVector2& normal, FLOAT refractionIndex, bcVector2* out );
			/*static void Transform( const bcVector2& A, const bcQuaternion& Q, bcVector2* out );
			static void Transform( const bcVector2* inArray, const bcQuaternion& Q , bcVector2* outArray);*/
			static void Transform( const bcVector2& A, const bcMatrix4x4& M, bcVector2* out );
			//static void Transform( const bcVector2* inArray, const bcMatrix4x4& M , bcVector2* outArray);
			static void TransformNormal( const bcVector2& A, const bcMatrix4x4& M, bcVector2* out );

			static void Add( const bcVector128& A, const bcVector128& B, bcVector128* out );
			static void Divide( const bcVector128& A, const bcVector128& B, bcVector128* out );
			static void Divide( const bcVector128& A, float fScaler, bcVector128* out );
			static void Multipy( const bcVector128& A, const bcVector128& B, bcVector128* out );
			static void Multipy( const bcVector128& A, float fScaler, bcVector128* out );
			static void Negate( const bcVector128& A, bcVector128* out );
			static void Subtract ( const bcVector128& A, const bcVector128& B, bcVector128* out );
			static void Clamp( const bcVector128& A, bcVector128* out );
			static void Cross( const bcVector128& A, const bcVector128& B, bcVector128* out );
			static void Dot( const bcVector128& A, const bcVector128& B, float* out );
			static void Normalize( const bcVector128& A, bcVector128* out );
			static void Lerp( const bcVector128& A, const bcVector128& B, float s, bcVector128* out );
			static void Length( const bcVector128& A, float* out );
			static void LenghtSq( const bcVector128& A, float* out );
			static void Barycentric( const bcVector128& value1, const bcVector128& value2, const bcVector128& value3, FLOAT amount1, FLOAT amount2, bcVector128* out);
			static void CatmullRom( const bcVector128& value1, const bcVector128& value2, const bcVector128& value3, const bcVector128& value4, float amount, bcVector128* out);
			static void Distance( const bcVector128& A, const bcVector128& B, float* out );
			static void DistanceSq( const bcVector128& A, const bcVector128& B, float* out );
			static void Max( const bcVector128& A, const bcVector128& B, bcVector128* out );
			static void Min( const bcVector128& A, const bcVector128& B, bcVector128* out );
			static void Reflect( const bcVector128& A, const bcVector128& normal, bcVector128* out );
			static void Refract( const bcVector128& A, const bcVector128& normal, FLOAT refractionIndex, bcVector128* out );
			/*static void Transform( const bcVector128& A, const bcQuaternion& Q, bcVector128* out );
			static void Transform( const bcVector128* inArray, const bcQuaternion& Q , bcVector128* outArray);*/
			static void Transform( const bcVector128& A, const bcMatrix4x4& M, bcVector128* out );
			//static void Transform( const bcVector128* inArray, const bcMatrix4x4& M , bcVector128* outArray);
			static void TransformNormal( const bcVector128& A, const bcMatrix4x4& M, bcVector128* out );
		
			// assignment
			bcVector2& operator= (const bcVector2& Vector);

			// member access
			float operator[] (int iPos) const;
			float& operator[] (int iPos);

			// comparison
			bool operator== ( const bcVector2& Vector ) const;
			bool operator!= ( const bcVector2& Vector ) const;

			// arithmetic operations
			bcVector2 operator+ ( const bcVector2& Vector ) const;
			bcVector2 operator- ( const bcVector2& Vector ) const;
			bcVector2 operator* ( const bcVector2& Vector ) const;
			bcVector2 operator* ( float fScalar ) const;
			bcVector2 operator/ ( const bcVector2& Vector ) const;
			bcVector2 operator/ ( float fScalar ) const;
			bcVector2 operator- ( ) const;

			// arithmetic updates
			bcVector2& operator+= ( const bcVector2& Vector );
			bcVector2& operator-= ( const bcVector2& Vector );
			bcVector2& operator*= ( const bcVector2& Vector );
			bcVector2& operator*= ( float fScalar );
			bcVector2& operator/= ( const bcVector2& Vector );
			bcVector2& operator/= ( float fScalar );
		};

		class BC_COREDLL_EXP bcVector3 : public XMFLOAT3
		{
		public:

			bcVector3( );
			bcVector3( float x, float y, float z );
			bcVector3( const bcVector3& Vector );

			// vector operations
			void Clamp( );
			void MakeZero( );
			float Lenght( );
			float LenghtSq( );
			void Normalize( );
			bcVector3 Perpendicular( );

			// static vector ops

			static void ToVector(const bcVector3& V3, bcVector128* out);
			static void FromVector(const bcVector128& V, bcVector3* out);

#pragma region Commented
			//static bcVector3 Add( const bcVector3& A, const bcVector3& B );
			//static bcVector3 Divide( const bcVector3& A, const bcVector3& B );
			//static bcVector3 Divide( const bcVector3& A, float fScaler );
			//static bcVector3 Multipy( const bcVector3& A, const bcVector3& B );
			//static bcVector3 Multipy( const bcVector3& A, float fScaler );
			//static bcVector3 Negate( const bcVector3& A );
			//static bcVector3 Subtract ( const bcVector3& A, const bcVector3& B );
			//static bcVector3 Clamp( const bcVector3& A );
			//static bcVector3 Cross( const bcVector3& A, const bcVector3& B );
			//static float Dot( const bcVector3& A, const bcVector3& B );
			//static bcVector3 Normalize( const bcVector3& A );
			//static bcVector3 Perpendicular( const bcVector3& A );
			//static bcVector3 Random( );
			//static bcVector3 Lerp( const bcVector3& A, const bcVector3& B, float s );
			//static bcVector3 Barycentric( const bcVector3& value1, const bcVector3& value2, const bcVector3& value3, FLOAT amount1, FLOAT amount2);
			//static bcVector3 CatmullRom( const bcVector3& value1, const bcVector3& value2, const bcVector3& value3, const bcVector3& value4, float amount);
			//static FLOAT Distance( const bcVector3& A, const bcVector3& B );
			//static FLOAT DistanceSq( const bcVector3& A, const bcVector3& B );
			//static bcVector3 Max( const bcVector3& A, const bcVector3& B );
			//static bcVector3 Min( const bcVector3& A, const bcVector3& B );
			//static bcVector3 Reflect( const bcVector3& A, const bcVector3& normal );
			//static bcVector3 Refract( const bcVector3& A, const bcVector3& normal, FLOAT refractionIndex );
			//static bcVector3 Transform( const bcVector3& A, const bcQuaternion& Q );
			////static void Transform( const bcVector3* inArray, const bcQuaternion& Q , bcVector3* outArray);
			//static bcVector3 Transform( const bcVector3& A, const bcMatrix4x4& Q );
			////static void Transform( const bcVector3* inArray, const bcMatrix4x4& Q , bcVector3* outArray);
			//static bcVector3 TransformNormal( const bcVector3& A, const bcMatrix4x4& M );
#pragma endregion

			static void Add( const bcVector3& A, const bcVector3& B, bcVector3* out );
			static void Divide( const bcVector3& A, const bcVector3& B, bcVector3* out );
			static void Divide( const bcVector3& A, float fScaler, bcVector3* out );
			static void Multipy( const bcVector3& A, const bcVector3& B, bcVector3* out );
			static void Multipy( const bcVector3& A, float fScaler, bcVector3* out );
			static void Negate( const bcVector3& A, bcVector3* out );
			static void Subtract ( const bcVector3& A, const bcVector3& B, bcVector3* out );
			static void Clamp( const bcVector3& A, bcVector3* out );
			static void Cross( const bcVector3& A, const bcVector3& B, bcVector3* out );
			static void Dot( const bcVector3& A, const bcVector3& B, FLOAT* out );
			static void Normalize( const bcVector3& A, bcVector3* out );
			static void Perpendicular( const bcVector3& A, bcVector3* out );
			static void Random( bcVector3* out );
			static void Lerp( const bcVector3& A, const bcVector3& B, float s, bcVector3* out );
			static void Barycentric( const bcVector3& value1, const bcVector3& value2, const bcVector3& value3, FLOAT amount1, FLOAT amount2, bcVector3* out);
			static void CatmullRom( const bcVector3& value1, const bcVector3& value2, const bcVector3& value3, const bcVector3& value4, float amount, bcVector3* out);
			static void Distance( const bcVector3& A, const bcVector3& B, FLOAT* out );
			static void DistanceSq( const bcVector3& A, const bcVector3& B, FLOAT* out );
			static void Max( const bcVector3& A, const bcVector3& B, bcVector3* out );
			static void Min( const bcVector3& A, const bcVector3& B, bcVector3* out );
			static void Reflect( const bcVector3& A, const bcVector3& normal, bcVector3* out );
			static void Refract( const bcVector3& A, const bcVector3& normal, FLOAT refractionIndex, bcVector3* out );
			static void Transform( const bcVector3& A, const bcQuaternion& Q, bcVector3* out );
			//static void Transform( const bcVector3* inArray, const bcQuaternion& Q , bcVector3* outArray);
			static void Transform( const bcVector3& A, const bcMatrix4x4& Q, bcVector3* out );
			//static void Transform( const bcVector3* inArray, const bcMatrix4x4& Q , bcVector3* outArray);
			static void TransformNormal( const bcVector3& A, const bcMatrix4x4& M, bcVector3* out );

			static void Add( const bcVector128& A, const bcVector128& B, bcVector128* out );
			static void Divide( const bcVector128& A, const bcVector128& B, bcVector128* out );
			static void Divide( const bcVector128& A, float fScaler, bcVector128* out );
			static void Multipy( const bcVector128& A, const bcVector128& B, bcVector128* out );
			static void Multipy( const bcVector128& A, float fScaler, bcVector128* out );
			static void Negate( const bcVector128& A, bcVector128* out );
			static void Subtract ( const bcVector128& A, const bcVector128& B, bcVector128* out );
			static void Clamp( const bcVector128& A, bcVector128* out );
			static void Cross( const bcVector128& A, const bcVector128& B, bcVector128* out );
			static void Dot( const bcVector128& A, const bcVector128& B, float* out );
			static void Normalize( const bcVector128& A, bcVector128* out );
			static void Lerp( const bcVector128& A, const bcVector128& B, float s, bcVector128* out );
			static void Length( const bcVector128& A, float* out );
			static void LenghtSq( const bcVector128& A, float* out );
			static void Barycentric( const bcVector128& value1, const bcVector128& value2, const bcVector128& value3, FLOAT amount1, FLOAT amount2, bcVector128* out);
			static void CatmullRom( const bcVector128& value1, const bcVector128& value2, const bcVector128& value3, const bcVector128& value4, float amount, bcVector128* out);
			static void Distance( const bcVector128& A, const bcVector128& B, float* out );
			static void DistanceSq( const bcVector128& A, const bcVector128& B, float* out );
			static void Max( const bcVector128& A, const bcVector128& B, bcVector128* out );
			static void Min( const bcVector128& A, const bcVector128& B, bcVector128* out );
			static void Reflect( const bcVector128& A, const bcVector128& normal, bcVector128* out );
			static void Refract( const bcVector128& A, const bcVector128& normal, FLOAT refractionIndex, bcVector128* out );
			static void Transform( const bcVector128& A, const bcVector128& Q, bcVector128* out );
			//static void Transform( const bcVector128* inArray, const bcVector128& Q , bcVector128* outArray);
			static void Transform( const bcVector128& A, const bcMatrix4x4& M, bcVector128* out );
			//static void Transform( const bcVector128* inArray, const bcMatrix4x4& M , bcVector128* outArray);
			static void TransformNormal( const bcVector128& A, const bcMatrix4x4& M, bcVector128* out );
		
			// assignment
			bcVector3& operator= (const bcVector3& Vector);

			// member access
			float operator[] (int iPos) const;
			float& operator[] (int iPos);

			// comparison
			bool operator== ( const bcVector3& Vector ) const;
			bool operator!= ( const bcVector3& Vector ) const;

			// arithmetic operations
			bcVector3 operator+ ( const bcVector3& Vector ) const;
			bcVector3 operator- ( const bcVector3& Vector ) const;
			bcVector3 operator* ( const bcVector3& Vector ) const;
			bcVector3 operator* ( float fScalar ) const;
			bcVector3 operator/ ( const bcVector3& Vector ) const;
			bcVector3 operator/ ( float fScalar ) const;
			bcVector3 operator- ( ) const;

			// arithmetic updates
			bcVector3& operator+= ( const bcVector3& Vector );
			bcVector3& operator-= ( const bcVector3& Vector );
			bcVector3& operator*= ( const bcVector3& Vector );
			bcVector3& operator*= ( float fScalar );
			bcVector3& operator/= ( const bcVector3& Vector );
			bcVector3& operator/= ( float fScalar );
		};

		class BC_COREDLL_EXP bcVector4 : public XMFLOAT4
		{
		public:

			bcVector4( );
			bcVector4( float x, float y, float z, float w );
			bcVector4( const bcVector4& Vector );

			// vector operations
			void Clamp( );
			void MakeZero( );
			float Lenght( );
			float LenghtSq( );
			void Normalize( );
			//bcVector4 Perpendicular( );

			// static vector ops

			static void ToVector(const bcVector4& V4, bcVector128* out);
			static void FromVector(const bcVector128& V, bcVector4* out);

#pragma region Commented
			//static bcVector4 Add( const bcVector4& A, const bcVector4& B );
			//static bcVector4 Divide( const bcVector4& A, const bcVector4& B );
			//static bcVector4 Divide( const bcVector4& A, float fScaler );
			//static bcVector4 Multipy( const bcVector4& A, const bcVector4& B );
			//static bcVector4 Multipy( const bcVector4& A, float fScaler );
			//static bcVector4 Negate( const bcVector4& A );
			//static bcVector4 Subtract ( const bcVector4& A, const bcVector4& B );
			//static bcVector4 Clamp( const bcVector4& A );
			////static bcVector4 Cross( const bcVector4& A, const bcVector4& B );
			//static float Dot( const bcVector4& A, const bcVector4& B );
			//static bcVector4 Normalize( const bcVector4& A );
			////static bcVector4 Perpendicular( const bcVector4& A );
			//static bcVector4 Random( );
			//static bcVector4 Lerp( const bcVector4& A, const bcVector4& B, float s );
			//static bcVector4 Barycentric( const bcVector4& value1, const bcVector4& value2, const bcVector4& value3, FLOAT amount1, FLOAT amount2);
			//static bcVector4 CatmullRom( const bcVector4& value1, const bcVector4& value2, const bcVector4& value3, const bcVector4& value4, float amount);
			//static FLOAT Distance( const bcVector4& A, const bcVector4& B );
			//static FLOAT DistanceSq( const bcVector4& A, const bcVector4& B );
			//static bcVector4 Max( const bcVector4& A, const bcVector4& B );
			//static bcVector4 Min( const bcVector4& A, const bcVector4& B );
			//static bcVector4 Reflect( const bcVector4& A, const bcVector4& normal );
			//static bcVector4 Refract( const bcVector4& A, const bcVector4& normal, FLOAT refractionIndex );
			////static bcVector4 Transform( const bcVector4& A, const bcQuaternion& Q );
			////static void Transform( const bcVector4* inArray, const bcQuaternion& Q , bcVector4* outArray);
			//static bcVector4 Transform( const bcVector4& A, const bcMatrix4x4& M );
			////static void Transform( const bcVector4* inArray, const bcMatrix4x4& m , bcVector4* outArray);
			////static bcVector4 TransformNormal( const bcVector4& A, const bcMatrix4x4& M );
#pragma endregion

			static void Add( const bcVector4& A, const bcVector4& B, bcVector4* out );
			static void Divide( const bcVector4& A, const bcVector4& B, bcVector4* out );
			static void Divide( const bcVector4& A, float fScaler, bcVector4* out );
			static void Multipy( const bcVector4& A, const bcVector4& B, bcVector4* out );
			static void Multipy( const bcVector4& A, float fScaler, bcVector4* out );
			static void Negate( const bcVector4& A, bcVector4* out );
			static void Subtract ( const bcVector4& A, const bcVector4& B, bcVector4* out );
			static void Clamp( const bcVector4& A, bcVector4* out );
			//static void Cross( const bcVector4& A, const bcVector4& B, bcVector4* out );
			static void Dot( const bcVector4& A, const bcVector4& B, FLOAT* out );
			static void Normalize( const bcVector4& A, bcVector4* out );
			//static void Perpendicular( const bcVector4& A, bcVector4* out );
			static void Random( bcVector4* out );
			static void Lerp( const bcVector4& A, const bcVector4& B, float s, bcVector4* out );
			static void Barycentric( const bcVector4& value1, const bcVector4& value2, const bcVector4& value3, FLOAT amount1, FLOAT amount2, bcVector4* out);
			static void CatmullRom( const bcVector4& value1, const bcVector4& value2, const bcVector4& value3, const bcVector4& value4, float amount, bcVector4* out);
			static void Distance( const bcVector4& A, const bcVector4& B, FLOAT* out );
			static void DistanceSq( const bcVector4& A, const bcVector4& B, FLOAT* out );
			static void Max( const bcVector4& A, const bcVector4& B, bcVector4* out );
			static void Min( const bcVector4& A, const bcVector4& B, bcVector4* out );
			static void Reflect( const bcVector4& A, const bcVector4& normal, bcVector4* out );
			static void Refract( const bcVector4& A, const bcVector4& normal, FLOAT refractionIndex, bcVector4* out );
			//static void Transform( const bcVector4& A, const bcQuaternion& Q , bcVector4* out);
			//static void Transform( const bcVector4* inArray, const bcQuaternion& Q , bcVector4* outArray);
			static void Transform( const bcVector4& A, const bcMatrix4x4& M, bcVector4* out );
			//static void Transform( const bcVector4* inArray, const bcMatrix4x4& m , bcVector4* outArray);
			//static void TransformNormal( const bcVector4& A, const bcMatrix4x4& M, bcVector4* out );

			static void Add( const bcVector128& A, const bcVector128& B, bcVector128* out );
			static void Divide( const bcVector128& A, const bcVector128& B, bcVector128* out );
			static void Divide( const bcVector128& A, float fScaler, bcVector128* out );
			static void Multipy( const bcVector128& A, const bcVector128& B, bcVector128* out );
			static void Multipy( const bcVector128& A, float fScaler, bcVector128* out );
			static void Negate( const bcVector128& A, bcVector128* out );
			static void Subtract ( const bcVector128& A, const bcVector128& B, bcVector128* out );
			static void Clamp( const bcVector128& A, bcVector128* out );
			//static void Cross( const bcVector128& A, const bcVector128& B, bcVector128* out );
			static void Dot( const bcVector128& A, const bcVector128& B, float* out );
			static void Normalize( const bcVector128& A, bcVector128* out );
			static void Lerp( const bcVector128& A, const bcVector128& B, float s, bcVector128* out );
			static void Length( const bcVector128& A, float* out );
			static void LenghtSq( const bcVector128& A, float* out );
			static void Barycentric( const bcVector128& value1, const bcVector128& value2, const bcVector128& value3, FLOAT amount1, FLOAT amount2, bcVector128* out);
			static void CatmullRom( const bcVector128& value1, const bcVector128& value2, const bcVector128& value3, const bcVector128& value4, float amount, bcVector128* out);
			static void Distance( const bcVector128& A, const bcVector128& B, float* out );
			static void DistanceSq( const bcVector128& A, const bcVector128& B, float* out );
			static void Max( const bcVector128& A, const bcVector128& B, bcVector128* out );
			static void Min( const bcVector128& A, const bcVector128& B, bcVector128* out );
			static void Reflect( const bcVector128& A, const bcVector128& normal, bcVector128* out );
			static void Refract( const bcVector128& A, const bcVector128& normal, FLOAT refractionIndex, bcVector128* out );
			/*static void Transform( const bcVector128& A, const bcQuaternion& Q, bcVector128* out );
			static void Transform( const bcVector128* inArray, const bcQuaternion& Q , bcVector128* outArray);*/
			static void Transform( const bcVector128& A, const bcMatrix4x4& M, bcVector128* out );
			//static void Transform( const bcVector128* inArray, const bcMatrix4x4& M , bcVector128* outArray);
			//static void TransformNormal( const bcVector128& A, const bcMatrix4x4& M, bcVector128* out );
		
			// assignment
			bcVector4& operator= (const bcVector4& Vector);

			// member access
			float operator[] (int iPos) const;
			float& operator[] (int iPos);

			// comparison
			bool operator== ( const bcVector4& Vector ) const;
			bool operator!= ( const bcVector4& Vector ) const;

			// arithmetic operations
			bcVector4 operator+ ( const bcVector4& Vector ) const;
			bcVector4 operator- ( const bcVector4& Vector ) const;
			bcVector4 operator* ( const bcVector4& Vector ) const;
			bcVector4 operator* ( float fScalar ) const;
			bcVector4 operator/ ( const bcVector4& Vector ) const;
			bcVector4 operator/ ( float fScalar ) const;
			bcVector4 operator- ( ) const;

			// arithmetic updates
			bcVector4& operator+= ( const bcVector4& Vector );
			bcVector4& operator-= ( const bcVector4& Vector );
			bcVector4& operator*= ( const bcVector4& Vector );
			bcVector4& operator*= ( float fScalar );
			bcVector4& operator/= ( const bcVector4& Vector );
			bcVector4& operator/= ( float fScalar );
		};

		__declspec(align(16)) class BC_COREDLL_EXP bcMatrix4x4 : public XMMATRIX
		{
		public:
		
			bcMatrix4x4( );
			bcMatrix4x4( float fM11, float fM12, float fM13, float fM14,
					   float fM21, float fM22, float fM23, float fM24,
					   float fM31, float fM32, float fM33, float fM34,
					   float fM41, float fM42, float fM43, float fM44 );
			bcMatrix4x4( const bcMatrix4x4& matrix);
			bcMatrix4x4( const XMMATRIX& matrix);

			void MakeIdentity( );
			void MakeTranspose( );
		
#pragma region Commented
			/*static bcMatrix4x4 Add(const bcMatrix4x4& matrix1, const bcMatrix4x4& matrix2);
			static bcMatrix4x4 Divide(const bcMatrix4x4& matrix1, float divider);
			static bcMatrix4x4 Divide(const bcMatrix4x4& matrix1, const bcMatrix4x4& matrix2);
			static bcMatrix4x4 Multiply(const bcMatrix4x4& matrix1, float scaleFactor);
			static bcMatrix4x4 Multiply(const bcMatrix4x4& matrix1, const bcMatrix4x4& matrix2);
			static bcMatrix4x4 MultiplyTraspose(const bcMatrix4x4& matrix1, const bcMatrix4x4& matrix2);
			static bcMatrix4x4 Negate(const bcMatrix4x4& matrix);
			static bcMatrix4x4 Subtract(const bcMatrix4x4& matrix1, const bcMatrix4x4& matrix2);
			static bcMatrix4x4 CreateFromAxisAngle(const bcVector3& axis, float angle);
			static bcMatrix4x4 CreateFrombcQuaternion(const bcQuaternion& quaternion);
			static bcMatrix4x4 CreateFromNormalAngle(const bcVector3& normal, float angle);
			static bcMatrix4x4 CreateFromYawPitchRoll(float yaw, float pitch, float roll);
			static bcMatrix4x4 CreateLookAtLH(const bcVector3& cameraPosition, const bcVector3& cameraTarget, const bcVector3& cameraUp);
			static bcMatrix4x4 CreateOrthographicLH(float width, float height, float zNear, float zFar);
			static bcMatrix4x4 CreateOrthographicOffCenterLH(float left, float right, float bottom, float top, float zNear, float zFar);
			static bcMatrix4x4 CreatePerspective(float Width, float height, float zNear, float zFar);
			static bcMatrix4x4 CreatePerspectiveFieldOfView(float fieldOfView, float aspectRatio, float nearPlaneDistance, float farPlaneDistance);
			static bcMatrix4x4 CreatePerspectiveOffCenter(float left, float right, float bottom, float top, float nearPlaneDistance, float farPlaneDistance);
			static bcMatrix4x4 CreateRotationX(float radians);
			static bcMatrix4x4 CreateRotationY(float radians);
			static bcMatrix4x4 CreateRotationZ(float radians);
			static bcMatrix4x4 CreateScale(float scale);
			static bcMatrix4x4 CreateScale(float xScale, float yScale, float zScale);
			static bcMatrix4x4 CreateTranslation(const bcVector3& position);
			static BOOL Decompose(bcVector3* scale, bcQuaternion* rotation, bcVector3* translation, const bcMatrix4x4& matrix);
			static FLOAT Determinant(const bcMatrix4x4& matrix);
			static bcMatrix4x4 Invert(const bcMatrix4x4& matrix);
			static bcMatrix4x4 Lerp(const bcMatrix4x4& matrix1, const bcMatrix4x4& matrix2, float amount);
			static bcMatrix4x4 Transpose(const bcMatrix4x4& matrix);*/
#pragma endregion

			static void Add(const bcMatrix4x4& matrix1, const bcMatrix4x4& matrix2, bcMatrix4x4* out);
			static void Divide(const bcMatrix4x4& matrix1, float divider, bcMatrix4x4* out);
			static void Divide(const bcMatrix4x4& matrix1, const bcMatrix4x4& matrix2, bcMatrix4x4* out);
			static void Multiply(const bcMatrix4x4& matrix1, float scaleFactor, bcMatrix4x4* out);
			static void Multiply(const bcMatrix4x4& matrix1, const bcMatrix4x4& matrix2, bcMatrix4x4* out);
			static void MultiplyTraspose(const bcMatrix4x4& matrix1, const bcMatrix4x4& matrix2, bcMatrix4x4* out);
			static void Negate(const bcMatrix4x4& matrix, bcMatrix4x4* out);
			static void Subtract(const bcMatrix4x4& matrix1, const bcMatrix4x4& matrix2, bcMatrix4x4* out);
			static void CreateFromAxisAngle(const bcVector3& axis, float angle, bcMatrix4x4* out);
			static void CreateFrombcQuaternion(const bcQuaternion& quaternion, bcMatrix4x4* out);
			static void CreateFromNormalAngle(const bcVector3& normal, float angle, bcMatrix4x4* out);
			static void CreateFromYawPitchRoll(float yaw, float pitch, float roll, bcMatrix4x4* out);
			static void CreateLookAtLH(const bcVector3& cameraPosition, const bcVector3& cameraTarget, const bcVector3& cameraUp, bcMatrix4x4* out);
			static void CreateOrthographicLH(float width, float height, float zNear, float zFar, bcMatrix4x4* out);
			static void CreateOrthographicOffCenterLH(float left, float right, float bottom, float top, float zNear, float zFar, bcMatrix4x4* out);
			static void CreatePerspective(float Width, float height, float zNear, float zFar, bcMatrix4x4* out);
			static void CreatePerspectiveFieldOfView(float fieldOfView, float aspectRatio, float nearPlaneDistance, float farPlaneDistance, bcMatrix4x4* out);
			static void CreatePerspectiveOffCenter(float left, float right, float bottom, float top, float nearPlaneDistance, float farPlaneDistance, bcMatrix4x4* out);
			static void CreateRotationX(float radians, bcMatrix4x4* out);
			static void CreateRotationY(float radians, bcMatrix4x4* out);
			static void CreateRotationZ(float radians, bcMatrix4x4* out);
			static void CreateScale(float scale, bcMatrix4x4* out);
			static void CreateScale(float xScale, float yScale, float zScale, bcMatrix4x4* out);
			static void CreateTranslation(const bcVector3& position, bcMatrix4x4* out);
			static void Decompose(bcVector3* scale, bcQuaternion* rotation, bcVector3* translation, const bcMatrix4x4& matrix, BOOL* out);
			static void Determinant(const bcMatrix4x4& matrix, FLOAT* out);
			static void Invert(const bcMatrix4x4& matrix, bcMatrix4x4* out);
			static void Lerp(const bcMatrix4x4& matrix1, const bcMatrix4x4& matrix2, float amount, bcMatrix4x4* out);
			static void Transpose(const bcMatrix4x4& matrix, bcMatrix4x4* out);

			// assignment
			bcMatrix4x4& operator= (const bcMatrix4x4& matrix);

			FLOAT       operator() (UINT Row, UINT Column) CONST { return m[Row][Column]; }
			FLOAT&      operator() (UINT Row, UINT Column) { return m[Row][Column]; }

			// comparison
			bool operator== ( const bcMatrix4x4& matrix ) const;
			bool operator!= ( const bcMatrix4x4& matrix ) const;

			// arithmetic operations
			bcMatrix4x4 operator+ ( const bcMatrix4x4& matrix ) const;
			bcMatrix4x4 operator- ( const bcMatrix4x4& matrix ) const;
			bcMatrix4x4 operator* ( const bcMatrix4x4& matrix ) const;
			bcMatrix4x4 operator* ( float fScalar ) const;
			bcMatrix4x4 operator/ ( float fScalar ) const;
			bcMatrix4x4 operator- () const;

			// arithmetic updates
			bcMatrix4x4& operator+= ( const bcMatrix4x4& matrix );
			bcMatrix4x4& operator-= ( const bcMatrix4x4& matrix );
			bcMatrix4x4& operator*= ( const bcMatrix4x4& matrix );
			bcMatrix4x4& operator*= ( float fScalar );
			bcMatrix4x4& operator/= ( float fScalar );

			static void ForwardVec(const bcMatrix4x4& matrix, bcVector3* out);
			static void BackVec(const bcMatrix4x4& matrix, bcVector3* out);
			static void RightVec(const bcMatrix4x4& matrix, bcVector3* out);
			static void LeftVec(const bcMatrix4x4& matrix, bcVector3* out);
			static void UpVec(const bcMatrix4x4& matrix, bcVector3* out);
			static void DownVec(const bcMatrix4x4& matrix, bcVector3* out);
			static void TranslationVec(const bcMatrix4x4& matrix, bcVector3* out);
			static void SetTranslationVec(bcMatrix4x4& matrix, const bcVector3& translation);
		};
	
		class BC_COREDLL_EXP bcQuaternion : public XMFLOAT4
		{
		public:

			bcQuaternion( );
			bcQuaternion( const bcQuaternion& quaternion);

			float Lenght( );
			float LenghtSq( );
			void Normalize();
		
#pragma region Commented
			//static bcQuaternion Add(const bcQuaternion& quaternion1, const bcQuaternion& quaternion2);
			//static bcQuaternion Divide(const bcQuaternion& quaternion1, const bcQuaternion& quaternion2);
			//static bcQuaternion Multiply(const bcQuaternion& quaternion1, float scaleFactor);
			//static bcQuaternion Multiply(const bcQuaternion& quaternion1, const bcQuaternion& quaternion2);
			//static bcQuaternion Negate(const bcQuaternion& quaternion);
			//static bcQuaternion Subtract(const bcQuaternion& quaternion1, const bcQuaternion& quaternion2);
			//static bcQuaternion Conjugate(const bcQuaternion& value);
			//static bcQuaternion CreateFromAxisAngle(const bcVector3& axis, float angle);
			//static bcQuaternion CreateFromRotationMatrix(const bcMatrix4x4& matrix);
			//static bcQuaternion CreateFromNormalAngle(const bcVector3& normal, float angle);
			//static bcQuaternion CreateFromYawPitchRoll(float yaw, float pitch, float roll) ;
			//static float Dot(const bcQuaternion& quaternion1, const bcQuaternion& quaternion2);
			//static bcQuaternion Inverse(const bcQuaternion& quaternion);
			////static bcQuaternion Lerp(const bcQuaternion& quaternion1, const bcQuaternion& quaternion2, float amount);
			//static bcQuaternion Normalize(const bcQuaternion& quaternion);
			//static bcQuaternion Slerp(const bcQuaternion& quaternion1, const bcQuaternion& quaternion2, float amount);
			//static void ToAxisAngle(bcVector3* axis, float* angle, const bcQuaternion& quaternion);
#pragma endregion

			/*static void Add(const bcQuaternion& quaternion1, const bcQuaternion& quaternion2, bcQuaternion* out);
			static void Divide(const bcQuaternion& quaternion1, const bcQuaternion& quaternion2, bcQuaternion* out);
			static void Multiply(const bcQuaternion& quaternion1, float scaleFactor, bcQuaternion* out);*/
			static void Multiply(const bcQuaternion& quaternion1, const bcQuaternion& quaternion2, bcQuaternion* out);
			/*static void Negate(const bcQuaternion& quaternion, bcQuaternion* out);
			static void Subtract(const bcQuaternion& quaternion1, const bcQuaternion& quaternion2, bcQuaternion* out);*/
			static void Conjugate(const bcQuaternion& value, bcQuaternion* out);
			static void CreateFromAxisAngle(const bcVector3& axis, float angle, bcQuaternion* out);
			static void CreateFromRotationMatrix(const bcMatrix4x4& matrix, bcQuaternion* out);
			static void CreateFromNormalAngle(const bcVector3& normal, float angle, bcQuaternion* out);
			static void CreateFromYawPitchRoll(float yaw, float pitch, float roll, bcQuaternion* out) ;
			static void Dot(const bcQuaternion& quaternion1, const bcQuaternion& quaternion2, FLOAT* out);
			static void Inverse(const bcQuaternion& quaternion, bcQuaternion* out);
			//static void Lerp(const bcQuaternion& quaternion1, const bcQuaternion& quaternion2, float amount, bcQuaternion* out);
			static void Normalize(const bcQuaternion& quaternion, bcQuaternion* out);
			static void Slerp(const bcQuaternion& quaternion1, const bcQuaternion& quaternion2, float amount, bcQuaternion* out);
			static void ToAxisAngle(bcVector3* axis, float* angle, const bcQuaternion& quaternion);

			/*static void Add(const bcVector128& quaternion1, const bcVector128& quaternion2, bcVector128* out);
			static void Divide(const bcVector128& quaternion1, const bcVector128& quaternion2, bcVector128* out);
			static void Multiply(const bcVector128& quaternion1, float scaleFactor, bcVector128* out);*/
			static void Multiply(const bcVector128& quaternion1, const bcVector128& quaternion2, bcVector128* out);
			/*static void Negate(const bcVector128& quaternion, bcVector128* out);
			static void Subtract(const bcVector128& quaternion1, const bcVector128& quaternion2, bcVector128* out);*/
			static void Conjugate(const bcVector128& value, bcVector128* out);
			static void CreateFromAxisAngle(const bcVector128& axis, float angle, bcVector128* out);
			static void CreateFromRotationMatrix(const bcMatrix4x4& matrix, bcVector128* out);
			static void CreateFromNormalAngle(const bcVector128& normal, float angle, bcVector128* out);
			static void CreateFromYawPitchRoll(float yaw, float pitch, float roll, bcVector128* out) ;
			static void Dot(const bcVector128& quaternion1, const bcVector128& quaternion2, FLOAT* out);
			static void Inverse(const bcVector128& quaternion, bcVector128* out);
			//static void Lerp(const bcQuaternion& quaternion1, const bcQuaternion& quaternion2, float amount, bcQuaternion* out);
			static void Normalize(const bcVector128& quaternion, bcVector128* out);
			static void Slerp(const bcVector128& quaternion1, const bcVector128& quaternion2, float amount, bcVector128* out);
			static void ToAxisAngle(bcVector128* axis, float* angle, const bcVector128& quaternion);
	    
			// assignment
			bcQuaternion& operator= (const bcQuaternion& quaternion);

			// comparison
			bool operator== ( const bcQuaternion& quaternion ) const;
			bool operator!= ( const bcQuaternion& quaternion ) const;

			// arithmetic operations
			//bcQuaternion operator+ ( const bcQuaternion& quaternion ) const;
			//bcQuaternion operator- ( const bcQuaternion& quaternion ) const;
			bcQuaternion operator* ( const bcQuaternion& quaternion ) const;
			//bcQuaternion operator* ( float fScalar ) const;
			//bcQuaternion operator/ ( const bcQuaternion& quaternion ) const;
			//bcQuaternion operator- ( ) const;

			// arithmetic updates
			//bcQuaternion& operator+= ( const bcQuaternion& quaternion );
			//bcQuaternion& operator-= ( const bcQuaternion& quaternion );
			bcQuaternion& operator*= ( const bcQuaternion& Vector );
			//bcQuaternion& operator*= ( float fScalar );
			//bcQuaternion& operator/= ( const bcQuaternion& quaternion );
		};
	}
}

#endif