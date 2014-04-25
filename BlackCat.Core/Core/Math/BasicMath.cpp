#include "CorePCH.h"
#include "BasicMath.h"

namespace BlackCat
{
	namespace Core
	{
	// == bcVector2 =======================================================
#pragma region vector2
		bcVector2::bcVector2( )
		{
		}
		bcVector2::bcVector2( float X, float Y )
		{
			x = X;
			y = Y;
		}
		bcVector2::bcVector2( const bcVector2& Vector )
		{
			x = Vector.x;
			y = Vector.y;
		}
		void bcVector2::Clamp()
		{
			if ( x > 1.0f ) x = 1.0f;
			if ( x < 0.0f ) x = 0.0f;

			if ( y > 1.0f ) y = 1.0f;
			if ( y < 0.0f ) y = 0.0f;
		}
		void bcVector2::MakeZero( )
		{
			x = 0.0f;
			y = 0.0f;
		}
		float bcVector2::Lenght( )
		{
			XMVECTOR v = XMLoadFloat2(this);
			v = XMVector2Length(v);
			return XMVectorGetX(v);
		}
		float bcVector2::LenghtSq( )
		{
			XMVECTOR v = XMLoadFloat2(this);
			v = XMVector2LengthSq(v);
			return XMVectorGetX(v);
		}
		void bcVector2::Normalize( )
		{
			XMVECTOR v = XMLoadFloat2(this);
			v = XMVector2Normalize(v);
			XMStoreFloat2(this, v);
		}
		bcVector2 bcVector2::Perpendicular( )
		{
			bcVector2 lReturn;

			float xAbs = fabs( x );
			float yAbs = fabs( y );
			float minVal = min( xAbs, yAbs );

			if ( xAbs == minVal )
				bcVector2::Cross( *this, bcVector2( 1.0f, 0.0f ), &lReturn );
			else if ( yAbs == minVal )
				bcVector2::Cross( *this, bcVector2( 0.0f, 1.0f ), &lReturn );

			return lReturn;
		}
		bcVector2& bcVector2::operator= ( const bcVector2& Vector )
		{
			x = Vector.x;
			y = Vector.y;

			return( *this );
		}
		float bcVector2::operator[] ( int iPos ) const
		{
			if ( iPos == 0 ) return( x );
			return( y );
		}
		float& bcVector2::operator[] ( int iPos )
		{
			if ( iPos == 0 ) return( x );
			return( y );
		}
		bool bcVector2::operator== ( const bcVector2& Vector ) const
		{
			XMVECTOR v = XMLoadFloat2(this);
			XMVECTOR v1 = XMLoadFloat2(&Vector);
			return XMVector2Equal(v, v1);
		}
		bool bcVector2::operator!= ( const bcVector2& Vector ) const
		{
			return( !( *this == Vector ) );
		}
		bcVector2 bcVector2::operator+ ( const bcVector2& Vector ) const
		{
			bcVector2 sum;

			sum.x = x + Vector.x;
			sum.y = y + Vector.y;

			return( sum );
		}
		bcVector2 bcVector2::operator- ( const bcVector2& Vector ) const
		{
			bcVector2 diff;

			diff.x = x - Vector.x;
			diff.y = y - Vector.y;

			return( diff );
		}
		bcVector2 bcVector2::operator* ( float fScalar ) const
		{
			bcVector2 prod;

			prod.x = x * fScalar;
			prod.y = y * fScalar;

			return( prod );
		}
		bcVector2 bcVector2::operator* ( const bcVector2& Vector ) const
		{
			bcVector2 prod;

			prod.x = x * Vector.x;
			prod.y = y * Vector.y;

			return( prod );
		}
		bcVector2 bcVector2::operator/ ( float fScalar ) const
		{
			bcVector2 quot;
			if ( fScalar != 0.0f )
			{
				float fInvScalar = 1.0f / fScalar;
				quot.x = x * fInvScalar;
				quot.y = y * fInvScalar;
			}
			else
			{
				quot.MakeZero();
			}

			return( quot );
		}
		bcVector2 bcVector2::operator/ ( const bcVector2& Vector ) const
		{
			bcVector2 quot;
			quot.x = Vector.x != 0.0f ? x / Vector.x : 0.0f;
			quot.y = Vector.y != 0.0f ? y / Vector.y : 0.0f;

			return( quot );
		}
		bcVector2 bcVector2::operator- ( ) const
		{
			bcVector2 neg;

			neg.x = -x;
			neg.y = -y;

			return( neg );
		}
		bcVector2& bcVector2::operator+= ( const bcVector2& Vector )
		{
			x += Vector.x;
			y += Vector.y;

			return( *this );
		}
		bcVector2& bcVector2::operator-= ( const bcVector2& Vector )
		{
			x -= Vector.x;
			y -= Vector.y;

			return( *this );
		}
		bcVector2& bcVector2::operator*= ( float fScalar )
		{
			x *= fScalar;
			y *= fScalar;

			return( *this );
		}
		bcVector2& bcVector2::operator*= ( const bcVector2& Vector )
		{
			x *= Vector.x;
			y *= Vector.y;

			return( *this );
		}
		bcVector2& bcVector2::operator/= ( float fScalar )
		{
			if ( fScalar != 0.0f )
			{
				float fInvScalar = 1.0f / fScalar;	
				x *= fInvScalar;
				y *= fInvScalar;
			}
			else
			{
				MakeZero();
			}

			return( *this );
		}
		bcVector2& bcVector2::operator/= ( const bcVector2& Vector )
		{
			x = Vector.x != 0.0f ? x / Vector.x : 0.0f;
			y = Vector.y != 0.0f ? y / Vector.y : 0.0f;

			return( *this );
		}

		void bcVector2::ToVector(const bcVector2& V2, bcVector128* out)
		{
			*out = XMLoadFloat2(&V2);
		}
		void bcVector2::FromVector(const bcVector128& V, bcVector2* out)
		{
			XMStoreFloat2(out, V);
		}

#pragma region Commented
		//bcVector2 bcVector2::Add( const bcVector2& A, const bcVector2& B )
		//{
		//	bcVector2 sum;

		//	sum.x = A.x + B.x;
		//	sum.y = A.y + B.y;

		//	return( sum );
		//}
		//bcVector2 bcVector2::Divide( const bcVector2& A, const bcVector2& B )
		//{
		//	bcVector2 quot;
		//	quot.x = B.x != 0.0f ? A.x / B.x : 0.0f;
		//	quot.y = B.y != 0.0f ? A.y / B.y : 0.0f;

		//	return( quot );
		//}
		//bcVector2 bcVector2::Divide( const bcVector2& A, float fScaler )
		//{
		//	bcVector2 quot;
		//	if ( fScaler != 0.0f )
		//	{
		//		float fInvScalar = 1.0f / fScaler;
		//		quot.x = A.x * fInvScalar;
		//		quot.y = A.y * fInvScalar;
		//	}
		//	else
		//	{
		//		quot.MakeZero();
		//	}

		//	return( quot );
		//}
		//bcVector2 bcVector2::Multipy( const bcVector2& A, const bcVector2& B )
		//{
		//	bcVector2 prod;

		//	prod.x = A.x * B.x;
		//	prod.y = A.y * B.y;

		//	return( prod );
		//}
		//bcVector2 bcVector2::Multipy( const bcVector2& A, float fScaler )
		//{
		//	bcVector2 prod;

		//	prod.x = A.x * fScaler;
		//	prod.y = A.y * fScaler;

		//	return( prod );
		//}
		//bcVector2 bcVector2::Negate( const bcVector2& A )
		//{
		//	bcVector2 neg;

		//	neg.x = -A.x;
		//	neg.y = -A.y;

		//	return( neg );
		//}
		//bcVector2 bcVector2::Subtract ( const bcVector2& A, const bcVector2& B )
		//{
		//	bcVector2 diff;

		//	diff.x = A.x - B.x;
		//	diff.y = A.y - B.y;

		//	return( diff );
		//}
		//bcVector2 bcVector2::Clamp( const bcVector2& A )
		//{
		//	bcVector2 vec = A;
		//	vec.Clamp();
		//	return vec;
		//}
		//bcVector2 bcVector2::Cross( const bcVector2& A, const bcVector2& B )
		//{    
		//	XMVECTOR v = XMLoadFloat2(&A);
		//	XMVECTOR v1 = XMLoadFloat2(&B);
		//	v = XMVector2Cross(v, v1);
		//	
		//	bcVector2 output;
		//	XMStoreFloat2(&output, v);
		//	return output;
		//}
		//float bcVector2::Dot( const bcVector2& A, const bcVector2& B )
		//{    
		//	XMVECTOR v = XMLoadFloat2(&A);
		//	XMVECTOR v1 = XMLoadFloat2(&B);
		//	v = XMVector2Dot(v, v1);
		//	
		//	return XMVectorGetX(v);
		//}
		//bcVector2 bcVector2::Normalize( const bcVector2& A )
		//{
		//	bcVector2 vec = A;
		//	vec.Normalize();
		//	return vec;
		//}
		//bcVector2 bcVector2::Perpendicular( const bcVector2& A )
		//{
		//	bcVector2 vec = A;
		//	return vec.Perpendicular();    
		//}
		//bcVector2 bcVector2::Random( )
		//{
		//	float x = static_cast<float>( (double)rand() / RAND_MAX ) * 2.0f - 1.0f;
		//	float y = static_cast<float>( (double)rand() / RAND_MAX ) * 2.0f - 1.0f;
		//
		//	bcVector2 random = bcVector2( x, y );
		//	random.Normalize();

		//	return( random );
		//}
		//bcVector2 bcVector2::Lerp( const bcVector2& A, const bcVector2& B, FLOAT s)
		//{
		//	XMVECTOR v = XMLoadFloat2(&A);
		//	XMVECTOR v1 = XMLoadFloat2(&B);
		//	v = XMVectorLerp(v, v1, s);

		//	bcVector2 output;
		//	XMStoreFloat2(&output, v);
		//	return output;
		//}
		//bcVector2 bcVector2::Barycentric( const bcVector2& value1, const bcVector2& value2, const bcVector2& value3, FLOAT amount1, FLOAT amount2)
		//{
		//	XMVECTOR v0 = XMLoadFloat2(&value1);
		//	XMVECTOR v1 = XMLoadFloat2(&value2);
		//	XMVECTOR v2 = XMLoadFloat2(&value3);

		//	v0 = XMVectorBaryCentric(v0, v1, v2, amount1, amount2);

		//	bcVector2 output;
		//	XMStoreFloat2(&output, v0);
		//	return output;
		//}
		//bcVector2 bcVector2::CatmullRom( const bcVector2& value1, const bcVector2& value2, const bcVector2& value3, const bcVector2& value4, float amount)
		//{
		//	XMVECTOR v0 = XMLoadFloat2(&value1);
		//	XMVECTOR v1 = XMLoadFloat2(&value1);
		//	XMVECTOR v2 = XMLoadFloat2(&value2);
		//	XMVECTOR v3 = XMLoadFloat2(&value3);

		//	v0 = XMVectorCatmullRom(v0, v1, v2, v3, amount);

		//	bcVector2 output;
		//	XMStoreFloat2(&output, v0);
		//	return output;
		//}
		//FLOAT bcVector2::Distance( const bcVector2& A, const bcVector2& B )
		//{
		//	XMVECTOR x = XMLoadFloat2(&A);
		//	XMVECTOR y = XMLoadFloat2(&B);
		//	XMVECTOR length = XMVector2Length(XMVectorSubtract(x, y));
		//	return XMVectorGetX(length);
		//}
		//FLOAT bcVector2::DistanceSq( const bcVector2& A, const bcVector2& B )
		//{
		//	XMVECTOR x = XMLoadFloat2(&A);
		//	XMVECTOR y = XMLoadFloat2(&B);
		//	XMVECTOR length = XMVector2LengthSq(XMVectorSubtract(x, y));
		//	return XMVectorGetX(length);
		//}
		//bcVector2 bcVector2::Max( const bcVector2& A, const bcVector2& B )
		//{
		//	XMVECTOR v = XMLoadFloat2(&A);
		//	XMVECTOR v1 = XMLoadFloat2(&B);
		//	v = XMVectorMax(v, v1);

		//	bcVector2 output;
		//	XMStoreFloat2(&output, v);
		//	return output;
		//}
		//bcVector2 bcVector2::Min( const bcVector2& A, const bcVector2& B )
		//{
		//	XMVECTOR v = XMLoadFloat2(&A);
		//	XMVECTOR v1 = XMLoadFloat2(&B);
		//	v = XMVectorMin(v, v1);

		//	bcVector2 output;
		//	XMStoreFloat2(&output, v);
		//	return output;
		//}
		//bcVector2 bcVector2::Reflect( const bcVector2& A, const bcVector2& Normal )
		//{
		//	XMVECTOR v = XMLoadFloat2(&A);
		//	XMVECTOR v1 = XMLoadFloat2(&Normal);
		//	v = XMVector2Reflect(v, v1);

		//	bcVector2 output;
		//	XMStoreFloat2(&output, v);
		//	return output;
		//}
		//bcVector2 bcVector2::Refract( const bcVector2& A, const bcVector2& Normal, FLOAT RefractionIndex )
		//{
		//	XMVECTOR v = XMLoadFloat2(&A);
		//	XMVECTOR v1 = XMLoadFloat2(&Normal);
		//	v = XMVector2Refract(v, v1, RefractionIndex);

		//	bcVector2 output;
		//	XMStoreFloat2(&output, v);
		//	return output;
		//}
		////bcVector2 bcVector2::Transform( const bcVector2& A, const bcQuaternion& Q );
		////void bcVector2::Transform( const bcVector2* inArray, const bcQuaternion& Q , bcVector2* outArray);
		//bcVector2 bcVector2::Transform( const bcVector2& A, const bcMatrix4x4& M )
		//{
		//	XMVECTOR aV = XMLoadFloat2(&A);
		//	aV = XMVector2Transform(aV, M);

		//	bcVector2 output;
		//	XMStoreFloat2(&output, aV);
		//	return output;
		//}
		////void bcVector2::Transform( const bcVector2* inArray, const bcMatrix4x4& M , bcVector2* outArray);
		//bcVector2 bcVector2::TransformNormal( const bcVector2& A, const bcMatrix4x4& M )
		//{
		//	XMVECTOR aV = XMLoadFloat2(&A);
		//	aV = XMVector2TransformNormal(aV, M);
		//	
		//	bcVector2 output;
		//	XMStoreFloat2(&output, aV);
		//	return output;
		//}
#pragma endregion

		void bcVector2::Add( const bcVector2& A, const bcVector2& B, bcVector2* out )
		{
			out->x = A.x + B.x;
			out->y = A.y + B.y;
		}
		void bcVector2::Divide( const bcVector2& A, const bcVector2& B, bcVector2* out )
		{
			out->x = B.x != 0.0f ? A.x / B.x : 0.0f;
			out->y = B.y != 0.0f ? A.y / B.y : 0.0f;
		}
		void bcVector2::Divide( const bcVector2& A, float fScaler, bcVector2* out )
		{
			if ( fScaler != 0.0f )
			{
				float fInvScalar = 1.0f / fScaler;
				out->x = A.x * fInvScalar;
				out->y = A.y * fInvScalar;
			}
			else
			{
				out->MakeZero();
			}
		}
		void bcVector2::Multipy( const bcVector2& A, const bcVector2& B, bcVector2* out )
		{
			out->x = A.x * B.x;
			out->y = A.y * B.y;
		}
		void bcVector2::Multipy( const bcVector2& A, float fScaler, bcVector2* out )
		{
			out->x = A.x * fScaler;
			out->y = A.y * fScaler;
		}
		void bcVector2::Negate( const bcVector2& A, bcVector2* out )
		{
			out->x = -A.x;
			out->y = -A.y;
		}
		void bcVector2::Subtract ( const bcVector2& A, const bcVector2& B, bcVector2* out )
		{
			out->x = A.x - B.x;
			out->y = A.y - B.y;
		}
		void bcVector2::Clamp( const bcVector2& A, bcVector2* out )
		{
			*out = A;
			out->Clamp();
		}
		void bcVector2::Cross( const bcVector2& A, const bcVector2& B, bcVector2* out )
		{    
			XMVECTOR v = XMLoadFloat2(&A);
			XMVECTOR v1 = XMLoadFloat2(&B);
			v = XMVector2Cross(v, v1);
		
			XMStoreFloat2(out, v);
		}
		void bcVector2::Dot( const bcVector2& A, const bcVector2& B, FLOAT* out )
		{    
			XMVECTOR v = XMLoadFloat2(&A);
			XMVECTOR v1 = XMLoadFloat2(&B);
			v = XMVector2Dot(v, v1);
		
			*out = XMVectorGetX(v);
		}
		void bcVector2::Normalize( const bcVector2& A, bcVector2* out )
		{
			*out = A;
			out->Normalize();
		}
		void bcVector2::Perpendicular( const bcVector2& A, bcVector2* out )
		{
			*out = A;
			out->Perpendicular();    
		}
		void bcVector2::Random( bcVector2* out )
		{
			float x = static_cast<float>( (double)rand() / RAND_MAX ) * 2.0f - 1.0f;
			float y = static_cast<float>( (double)rand() / RAND_MAX ) * 2.0f - 1.0f;
	
			*out = bcVector2( x, y );
			out->Normalize();
		}
		void bcVector2::Lerp( const bcVector2& A, const bcVector2& B, FLOAT s, bcVector2* out )
		{
			XMVECTOR v = XMLoadFloat2(&A);
			XMVECTOR v1 = XMLoadFloat2(&B);
			v = XMVectorLerp(v, v1, s);

			XMStoreFloat2(out, v);
		}
		void bcVector2::Barycentric( const bcVector2& value1, const bcVector2& value2, const bcVector2& value3, FLOAT amount1, FLOAT amount2, bcVector2* out )
		{
			XMVECTOR v0 = XMLoadFloat2(&value1);
			XMVECTOR v1 = XMLoadFloat2(&value2);
			XMVECTOR v2 = XMLoadFloat2(&value3);

			v0 = XMVectorBaryCentric(v0, v1, v2, amount1, amount2);

			XMStoreFloat2(out, v0);
		}
		void bcVector2::CatmullRom( const bcVector2& value1, const bcVector2& value2, const bcVector2& value3, const bcVector2& value4, float amount, bcVector2* out )
		{
			XMVECTOR v0 = XMLoadFloat2(&value1);
			XMVECTOR v1 = XMLoadFloat2(&value1);
			XMVECTOR v2 = XMLoadFloat2(&value2);
			XMVECTOR v3 = XMLoadFloat2(&value3);

			v0 = XMVectorCatmullRom(v0, v1, v2, v3, amount);

			XMStoreFloat2(out, v0);
		}
		void bcVector2::Distance( const bcVector2& A, const bcVector2& B, FLOAT* out )
		{
			XMVECTOR x = XMLoadFloat2(&A);
			XMVECTOR y = XMLoadFloat2(&B);
			XMVECTOR length = XMVector2Length(XMVectorSubtract(x, y));
			*out = XMVectorGetX(length);
		}
		void bcVector2::DistanceSq( const bcVector2& A, const bcVector2& B, FLOAT* out )
		{
			XMVECTOR x = XMLoadFloat2(&A);
			XMVECTOR y = XMLoadFloat2(&B);
			XMVECTOR length = XMVector2LengthSq(XMVectorSubtract(x, y));
			*out = XMVectorGetX(length);
		}
		void bcVector2::Max( const bcVector2& A, const bcVector2& B, bcVector2* out )
		{
			XMVECTOR v = XMLoadFloat2(&A);
			XMVECTOR v1 = XMLoadFloat2(&B);
			v = XMVectorMax(v, v1);

			XMStoreFloat2(out, v);
		}
		void bcVector2::Min( const bcVector2& A, const bcVector2& B, bcVector2* out )
		{
			XMVECTOR v = XMLoadFloat2(&A);
			XMVECTOR v1 = XMLoadFloat2(&B);
			v = XMVectorMin(v, v1);

			XMStoreFloat2(out, v);
		}
		void bcVector2::Reflect( const bcVector2& A, const bcVector2& Normal, bcVector2* out )
		{
			XMVECTOR v = XMLoadFloat2(&A);
			XMVECTOR v1 = XMLoadFloat2(&Normal);
			v = XMVector2Reflect(v, v1);

			XMStoreFloat2(out, v);
		}
		void bcVector2::Refract( const bcVector2& A, const bcVector2& Normal, FLOAT RefractionIndex, bcVector2* out )
		{
			XMVECTOR v = XMLoadFloat2(&A);
			XMVECTOR v1 = XMLoadFloat2(&Normal);
			v = XMVector2Refract(v, v1, RefractionIndex);

			XMStoreFloat2(out, v);
		}
		//void bcVector2::Transform( const bcVector2& A, const bcQuaternion& Q, bcVector2* out );
		//void bcVector2::Transform( const bcVector2* inArray, const bcQuaternion& Q , bcVector2* outArray);
		void bcVector2::Transform( const bcVector2& A, const bcMatrix4x4& M, bcVector2* out )
		{
			XMVECTOR aV = XMLoadFloat2(&A);
			aV = XMVector2Transform(aV, M);

			XMStoreFloat2(out, aV);
		}
		//void bcVector2::Transform( const bcVector2* inArray, const bcMatrix4x4& M , bcVector2* outArray);
		void bcVector2::TransformNormal( const bcVector2& A, const bcMatrix4x4& M, bcVector2* out )
		{
			XMVECTOR aV = XMLoadFloat2(&A);
			aV = XMVector2TransformNormal(aV, M);
		
			XMStoreFloat2(out, aV);
		}

		void bcVector2::Add( const bcVector128& A, const bcVector128& B, bcVector128* out )
		{
			*out = XMVectorAdd(A, B);
		}
		void bcVector2::Divide( const bcVector128& A, const bcVector128& B, bcVector128* out )
		{
			*out = XMVectorDivide(A, B);
		}
		void bcVector2::Divide( const bcVector128& A, float fScaler, bcVector128* out )
		{
			XMVECTOR lScaler = XMLoadFloat2(&XMFLOAT2(fScaler, fScaler));
			*out = XMVectorDivide(A, lScaler);
		}
		void bcVector2::Multipy( const bcVector128& A, const bcVector128& B, bcVector128* out )
		{
			*out = XMVectorMultiply(A, B);
		}
		void bcVector2::Multipy( const bcVector128& A, float fScaler, bcVector128* out )
		{
			XMVECTOR lScaler = XMLoadFloat2(&XMFLOAT2(fScaler, fScaler));
			*out = XMVectorMultiply(A, lScaler);
		}
		void bcVector2::Negate( const bcVector128& A, bcVector128* out )
		{
			*out = XMVectorNegate(A);
		}
		void bcVector2::Subtract ( const bcVector128& A, const bcVector128& B, bcVector128* out )
		{
			*out = XMVectorSubtract(A, B);
		}
		void bcVector2::Clamp( const bcVector128& A, bcVector128* out )
		{
			*out = XMVectorClamp(A, XMLoadFloat2(&XMFLOAT2(0, 0)), XMLoadFloat2(&XMFLOAT2(1, 1)));
		}
		void bcVector2::Cross( const bcVector128& A, const bcVector128& B, bcVector128* out )
		{    
			*out = XMVector2Cross(A, B);
		}
		void bcVector2::Dot( const bcVector128& A, const bcVector128& B, FLOAT* out )
		{    
			XMVECTOR dot = XMVector2Dot(A, B);
			*out = XMVectorGetX(dot);
		}
		void bcVector2::Normalize( const bcVector128& A, bcVector128* out )
		{
			*out = XMVector2Normalize(A);
		}
		void bcVector2::Lerp( const bcVector128& A, const bcVector128& B, FLOAT s, bcVector128* out )
		{
			*out = XMVectorLerp(A, B, s);
		}
		void bcVector2::Length( const bcVector128& A, float* out )
		{
			XMVECTOR length = XMVector2Length(A);
			*out = XMVectorGetX(length);
		}
		void bcVector2::LenghtSq( const bcVector128& A, float* out )
		{
			XMVECTOR length = XMVector2LengthSq(A);
			*out = XMVectorGetX(length);
		}
		void bcVector2::Barycentric( const bcVector128& value1, const bcVector128& value2, const bcVector128& value3, FLOAT amount1, FLOAT amount2, bcVector128* out )
		{
			*out = XMVectorBaryCentric(value1, value2, value3, amount1, amount2);
		}
		void bcVector2::CatmullRom( const bcVector128& value1, const bcVector128& value2, const bcVector128& value3, const bcVector128& value4, float amount, bcVector128* out )
		{
			*out = XMVectorCatmullRom(value1, value2, value3, value4, amount);
		}
		void bcVector2::Distance( const bcVector128& A, const bcVector128& B, FLOAT* out )
		{
			XMVECTOR length = XMVector2Length(XMVectorSubtract(A, B));
			*out = XMVectorGetX(length);
		}
		void bcVector2::DistanceSq( const bcVector128& A, const bcVector128& B, FLOAT* out )
		{
			XMVECTOR length = XMVector2LengthSq(XMVectorSubtract(A, B));
			*out = XMVectorGetX(length);
		}
		void bcVector2::Max( const bcVector128& A, const bcVector128& B, bcVector128* out )
		{
			*out = XMVectorMax(A, B);
		}
		void bcVector2::Min( const bcVector128& A, const bcVector128& B, bcVector128* out )
		{
			*out = XMVectorMin(A, B);
		}
		void bcVector2::Reflect( const bcVector128& A, const bcVector128& Normal, bcVector128* out )
		{
			*out = XMVector2Reflect(A, Normal);
		}
		void bcVector2::Refract( const bcVector128& A, const bcVector128& Normal, FLOAT RefractionIndex, bcVector128* out )
		{
			*out = XMVector2Refract(A, Normal, RefractionIndex);
		}
		//void bcVector2::Transform( const bcVector128& A, const bcQuaternion& Q, bcVector128* out );
		//void bcVector2::Transform( const bcVector128* inArray, const bcQuaternion& Q , bcVector128* outArray);
		void bcVector2::Transform( const bcVector128& A, const bcMatrix4x4& M, bcVector128* out )
		{
			*out = XMVector2Transform(A, M);
		}
		//void bcVector2::Transform( const bcVector128* inArray, const bcMatrix4x4& M , bcVector128* outArray);
		void bcVector2::TransformNormal( const bcVector128& A, const bcMatrix4x4& M, bcVector128* out )
		{
			*out = XMVector2TransformNormal(A, M);
		}
#pragma endregion

	// == bcVector3 =======================================================
#pragma region vector3
		bcVector3::bcVector3( )
		{
		}
		bcVector3::bcVector3( float X, float Y, float Z )
		{
			x = X;
			y = Y;
			z = Z;
		}
		bcVector3::bcVector3( const bcVector3& Vector )
		{
			x = Vector.x;
			y = Vector.y;
			z = Vector.z;
		}
		void bcVector3::Clamp()
		{
			if ( x > 1.0f ) x = 1.0f;
			if ( x < 0.0f ) x = 0.0f;

			if ( y > 1.0f ) y = 1.0f;
			if ( y < 0.0f ) y = 0.0f;

			if ( z > 1.0f ) z = 1.0f;
			if ( z < 0.0f ) z = 0.0f;
		}
		void bcVector3::MakeZero( )
		{
			x = 0.0f;
			y = 0.0f;
			z = 0.0f;
		}
		float bcVector3::Lenght( )
		{
			XMVECTOR v = XMLoadFloat3(this);
			v = XMVector3Length(v);
			return XMVectorGetX(v);
		}
		float bcVector3::LenghtSq( )
		{
			XMVECTOR v = XMLoadFloat3(this);
			v = XMVector3LengthSq(v);
			return XMVectorGetX(v);
		}
		void bcVector3::Normalize( )
		{
			XMVECTOR v = XMLoadFloat3(this);
			v = XMVector3Normalize(v);
			XMStoreFloat3(this, v);
		}
		bcVector3 bcVector3::Perpendicular( )
		{
			bcVector3 lReturn;

			float xAbs = fabs( x );
			float yAbs = fabs( y );
			float zAbs = fabs( z );
			float minVal = min( min( xAbs, yAbs ), zAbs );

			if ( xAbs == minVal )
				bcVector3::Cross( *this, bcVector3( 1.0f, 0.0f, 0.0f ), &lReturn );
			else if ( yAbs == minVal )
				bcVector3::Cross( *this, bcVector3( 0.0f, 1.0f, 0.0f ), &lReturn );
			else
				bcVector3::Cross( *this, bcVector3( 0.0f, 0.0f, 1.0f ), &lReturn );

			return lReturn;
		}
		bcVector3& bcVector3::operator= ( const bcVector3& Vector )
		{
			x = Vector.x;
			y = Vector.y;
			z = Vector.z;

			return( *this );
		}
		float bcVector3::operator[] ( int iPos ) const
		{
			if ( iPos == 0 ) return( x );
			if ( iPos == 1 ) return( y );
			return( z );
		}
		float& bcVector3::operator[] ( int iPos )
		{
			if ( iPos == 0 ) return( x );
			if ( iPos == 1 ) return( y );
			return( z );
		}
		bool bcVector3::operator== ( const bcVector3& Vector ) const
		{
			XMVECTOR v = XMLoadFloat3(this);
			XMVECTOR v1 = XMLoadFloat3(&Vector);
			return XMVector3Equal(v, v1);
		}
		bool bcVector3::operator!= ( const bcVector3& Vector ) const
		{
			return( !( *this == Vector ) );
		}
		bcVector3 bcVector3::operator+ ( const bcVector3& Vector ) const
		{
			bcVector3 sum;

			sum.x = x + Vector.x;
			sum.y = y + Vector.y;
			sum.z = z + Vector.z;

			return( sum );
		}
		bcVector3 bcVector3::operator- ( const bcVector3& Vector ) const
		{
			bcVector3 diff;

			diff.x = x - Vector.x;
			diff.y = y - Vector.y;
			diff.z = z - Vector.z;

			return( diff );
		}
		bcVector3 bcVector3::operator* ( float fScalar ) const
		{
			bcVector3 prod;

			prod.x = x * fScalar;
			prod.y = y * fScalar;
			prod.z = z * fScalar;

			return( prod );
		}
		bcVector3 bcVector3::operator* ( const bcVector3& Vector ) const
		{
			bcVector3 prod;

			prod.x = x * Vector.x;
			prod.y = y * Vector.y;
			prod.z = z * Vector.z;

			return( prod );
		}
		bcVector3 bcVector3::operator/ ( float fScalar ) const
		{
			bcVector3 quot;
			if ( fScalar != 0.0f )
			{
				float fInvScalar = 1.0f / fScalar;
				quot.x = x * fInvScalar;
				quot.y = y * fInvScalar;
				quot.z = z * fInvScalar;
			}
			else
			{
				quot.MakeZero();
			}

			return( quot );
		}
		bcVector3 bcVector3::operator/ ( const bcVector3& Vector ) const
		{
			bcVector3 quot;
			quot.x = Vector.x != 0.0f ? x / Vector.x : 0.0f;
			quot.y = Vector.y != 0.0f ? y / Vector.y : 0.0f;
			quot.z = Vector.z != 0.0f ? z / Vector.z : 0.0f;

			return( quot );
		}
		bcVector3 bcVector3::operator- ( ) const
		{
			bcVector3 neg;

			neg.x = -x;
			neg.y = -y;
			neg.z = -z;

			return( neg );
		}
		bcVector3& bcVector3::operator+= ( const bcVector3& Vector )
		{
			x += Vector.x;
			y += Vector.y;
			z += Vector.z;

			return( *this );
		}
		bcVector3& bcVector3::operator-= ( const bcVector3& Vector )
		{
			x -= Vector.x;
			y -= Vector.y;
			z -= Vector.z;

			return( *this );
		}
		bcVector3& bcVector3::operator*= ( float fScalar )
		{
			x *= fScalar;
			y *= fScalar;
			z *= fScalar;

			return( *this );
		}
		bcVector3& bcVector3::operator*= ( const bcVector3& Vector )
		{
			x *= Vector.x;
			y *= Vector.y;
			z *= Vector.z;

			return( *this );
		}
		bcVector3& bcVector3::operator/= ( float fScalar )
		{
			if ( fScalar != 0.0f )
			{
				float fInvScalar = 1.0f / fScalar;	
				x *= fInvScalar;
				y *= fInvScalar;
				z *= fInvScalar;
			}
			else
			{
				MakeZero();
			}

			return( *this );
		}
		bcVector3& bcVector3::operator/= ( const bcVector3& Vector )
		{
			x = Vector.x != 0.0f ? x / Vector.x : 0.0f;
			y = Vector.y != 0.0f ? y / Vector.y : 0.0f;
			z = Vector.z != 0.0f ? z / Vector.z : 0.0f;

			return( *this );
		}

		void bcVector3::ToVector(const bcVector3& V3, bcVector128* out)
		{
			*out = XMLoadFloat3(&V3);
		}
		void bcVector3::FromVector(const bcVector128& V, bcVector3* out)
		{
			XMStoreFloat3(out, V);
		}

#pragma region Commented
		//bcVector3 bcVector3::Add( const bcVector3& A, const bcVector3& B )
		//{
		//	bcVector3 sum;

		//	sum.x = A.x + B.x;
		//	sum.y = A.y + B.y;
		//	sum.z = A.z + B.z;

		//	return( sum );
		//}
		//bcVector3 bcVector3::Divide( const bcVector3& A, const bcVector3& B )
		//{
		//	bcVector3 quot;
		//	quot.x = B.x != 0.0f ? A.x / B.x : 0.0f;
		//	quot.y = B.y != 0.0f ? A.y / B.y : 0.0f;
		//	quot.z = B.z != 0.0f ? A.z / B.z : 0.0f;

		//	return( quot );
		//}
		//bcVector3 bcVector3::Divide( const bcVector3& A, float fScaler )
		//{
		//	bcVector3 quot;
		//	if ( fScaler != 0.0f )
		//	{
		//		float fInvScalar = 1.0f / fScaler;
		//		quot.x = A.x * fInvScalar;
		//		quot.y = A.y * fInvScalar;
		//		quot.z = A.z * fInvScalar;
		//	}
		//	else
		//	{
		//		quot.MakeZero();
		//	}

		//	return( quot );
		//}
		//bcVector3 bcVector3::Multipy( const bcVector3& A, const bcVector3& B )
		//{
		//	bcVector3 prod;

		//	prod.x = A.x * B.x;
		//	prod.y = A.y * B.y;
		//	prod.z = A.z * B.z;

		//	return( prod );
		//}
		//bcVector3 bcVector3::Multipy( const bcVector3& A, float fScaler )
		//{
		//	bcVector3 prod;

		//	prod.x = A.x * fScaler;
		//	prod.y = A.y * fScaler;
		//	prod.z = A.z * fScaler;

		//	return( prod );
		//}
		//bcVector3 bcVector3::Negate( const bcVector3& A )
		//{
		//	bcVector3 neg;

		//	neg.x = -A.x;
		//	neg.y = -A.y;
		//	neg.z = -A.z;

		//	return( neg );
		//}
		//bcVector3 bcVector3::Subtract ( const bcVector3& A, const bcVector3& B )
		//{
		//	bcVector3 diff;

		//	diff.x = A.x - B.x;
		//	diff.y = A.y - B.y;
		//	diff.z = A.z - B.z;

		//	return( diff );
		//}
		//bcVector3 bcVector3::Clamp( const bcVector3& A )
		//{
		//	bcVector3 vec = A;
		//	vec.Clamp();
		//	return vec;
		//}
		//bcVector3 bcVector3::Cross( const bcVector3& A, const bcVector3& B )
		//{    
		//	XMVECTOR v = XMLoadFloat3(&A);
		//	XMVECTOR v1 = XMLoadFloat3(&B);
		//	v = XMVector3Cross(v, v1);
		//	
		//	bcVector3 output;
		//	XMStoreFloat3(&output, v);
		//	return output;
		//}
		//float bcVector3::Dot( const bcVector3& A, const bcVector3& B )
		//{    
		//	XMVECTOR v = XMLoadFloat3(&A);
		//	XMVECTOR v1 = XMLoadFloat3(&B);
		//	v = XMVector3Dot(v, v1);
		//	
		//	return XMVectorGetX(v);
		//}
		//bcVector3 bcVector3::Normalize( const bcVector3& A )
		//{
		//	bcVector3 vec = A;
		//	vec.Normalize();
		//	return vec;
		//}
		//bcVector3 bcVector3::Perpendicular( const bcVector3& A )
		//{
		//	bcVector3 vec = A;
		//	return vec.Perpendicular();    
		//}
		//bcVector3 bcVector3::Random( )
		//{
		//	float x = static_cast<float>( (double)rand() / RAND_MAX ) * 2.0f - 1.0f;
		//	float y = static_cast<float>( (double)rand() / RAND_MAX ) * 2.0f - 1.0f;
		//	float z = static_cast<float>( (double)rand() / RAND_MAX ) * 2.0f - 1.0f;
		//
		//	bcVector3 random = bcVector3( x, y, z );
		//	random.Normalize();

		//	return( random );
		//}
		//bcVector3 bcVector3::Lerp( const bcVector3& A, const bcVector3& B, FLOAT s)
		//{
		//	XMVECTOR v = XMLoadFloat3(&A);
		//	XMVECTOR v1 = XMLoadFloat3(&B);
		//	v = XMVectorLerp(v, v1, s);

		//	bcVector3 output;
		//	XMStoreFloat3(&output, v);
		//	return output;
		//}
		//bcVector3 bcVector3::Barycentric( const bcVector3& value1, const bcVector3& value2, const bcVector3& value3, FLOAT amount1, FLOAT amount2)
		//{
		//	XMVECTOR v0 = XMLoadFloat3(&value1);
		//	XMVECTOR v1 = XMLoadFloat3(&value2);
		//	XMVECTOR v2 = XMLoadFloat3(&value3);

		//	v0 = XMVectorBaryCentric(v0, v1, v2, amount1, amount2);

		//	bcVector3 output;
		//	XMStoreFloat3(&output, v0);
		//	return output;
		//}
		//bcVector3 bcVector3::CatmullRom( const bcVector3& value1, const bcVector3& value2, const bcVector3& value3, const bcVector3& value4, float amount)
		//{
		//	XMVECTOR v0 = XMLoadFloat3(&value1);
		//	XMVECTOR v1 = XMLoadFloat3(&value1);
		//	XMVECTOR v2 = XMLoadFloat3(&value2);
		//	XMVECTOR v3 = XMLoadFloat3(&value3);

		//	v0 = XMVectorCatmullRom(v0, v1, v2, v3, amount);

		//	bcVector3 output;
		//	XMStoreFloat3(&output, v0);
		//	return output;
		//}
		//FLOAT bcVector3::Distance( const bcVector3& A, const bcVector3& B )
		//{
		//	XMVECTOR x = XMLoadFloat3(&A);
		//	XMVECTOR y = XMLoadFloat3(&B);
		//	XMVECTOR length = XMVector3Length(XMVectorSubtract(x, y));
		//	return XMVectorGetX(length);
		//}
		//FLOAT bcVector3::DistanceSq( const bcVector3& A, const bcVector3& B )
		//{
		//	XMVECTOR x = XMLoadFloat3(&A);
		//	XMVECTOR y = XMLoadFloat3(&B);
		//	XMVECTOR length = XMVector3LengthSq(XMVectorSubtract(x, y));
		//	return XMVectorGetX(length);
		//}
		//bcVector3 bcVector3::Max( const bcVector3& A, const bcVector3& B )
		//{
		//	XMVECTOR v = XMLoadFloat3(&A);
		//	XMVECTOR v1 = XMLoadFloat3(&B);
		//	v = XMVectorMax(v, v1);

		//	bcVector3 output;
		//	XMStoreFloat3(&output, v);
		//	return output;
		//}
		//bcVector3 bcVector3::Min( const bcVector3& A, const bcVector3& B )
		//{
		//	XMVECTOR v = XMLoadFloat3(&A);
		//	XMVECTOR v1 = XMLoadFloat3(&B);
		//	v = XMVectorMin(v, v1);

		//	bcVector3 output;
		//	XMStoreFloat3(&output, v);
		//	return output;
		//}
		//bcVector3 bcVector3::Reflect( const bcVector3& A, const bcVector3& Normal )
		//{
		//	XMVECTOR v = XMLoadFloat3(&A);
		//	XMVECTOR v1 = XMLoadFloat3(&Normal);
		//	v = XMVector3Reflect(v, v1);

		//	bcVector3 output;
		//	XMStoreFloat3(&output, v);
		//	return output;
		//}
		//bcVector3 bcVector3::Refract( const bcVector3& A, const bcVector3& Normal, FLOAT RefractionIndex )
		//{
		//	XMVECTOR v = XMLoadFloat3(&A);
		//	XMVECTOR v1 = XMLoadFloat3(&Normal);
		//	v = XMVector3Refract(v, v1, RefractionIndex);

		//	bcVector3 output;
		//	XMStoreFloat3(&output, v);
		//	return output;
		//}
		//bcVector3 bcVector3::Transform( const bcVector3& A, const bcQuaternion& Q )
		//{
		//	XMVECTOR aV = XMLoadFloat3(&A);
		//	XMVECTOR qV = XMLoadFloat4(&Q);
		//	aV = XMVector3Rotate(aV, qV);

		//	bcVector3 output;
		//	XMStoreFloat3(&output, aV);
		//	return output;
		//}
		////static inline void Transform( const bcVector3* inArray, const bcQuaternion& Q , bcVector3* outArray);
		//bcVector3 bcVector3::Transform( const bcVector3& A, const bcMatrix4x4& M )
		//{
		//	XMVECTOR aV = XMLoadFloat3(&A);
		//	aV = XMVector3Transform(aV, M);

		//	bcVector3 output;
		//	XMStoreFloat3(&output, aV);
		//	return output;
		//}
		////static inline void Transform( const bcVector3* inArray, const bcMatrix4x4& M , bcVector3* outArray);
		//bcVector3 bcVector3::TransformNormal( const bcVector3& A, const bcMatrix4x4& M )
		//{
		//	XMVECTOR aV = XMLoadFloat3(&A);
		//	aV = XMVector3TransformNormal(aV, M);

		//	bcVector3 output;
		//	XMStoreFloat3(&output, aV);
		//	return output;
		//}
#pragma endregion

		void bcVector3::Add( const bcVector3& A, const bcVector3& B, bcVector3* out )
		{
			out->x = A.x + B.x;
			out->y = A.y + B.y;
			out->z = A.z + B.z;
		}
		void bcVector3::Divide( const bcVector3& A, const bcVector3& B, bcVector3* out )
		{
			out->x = B.x != 0.0f ? A.x / B.x : 0.0f;
			out->y = B.y != 0.0f ? A.y / B.y : 0.0f;
			out->z = B.z != 0.0f ? A.z / B.z : 0.0f;
		}
		void bcVector3::Divide( const bcVector3& A, float fScaler, bcVector3* out )
		{
			if ( fScaler != 0.0f )
			{
				float fInvScalar = 1.0f / fScaler;
				out->x = A.x * fInvScalar;
				out->y = A.y * fInvScalar;
				out->z = A.z * fInvScalar;
			}
			else
			{
				out->MakeZero();
			}

		}
		void bcVector3::Multipy( const bcVector3& A, const bcVector3& B, bcVector3* out )
		{
			out->x = A.x * B.x;
			out->y = A.y * B.y;
			out->z = A.z * B.z;
		}
		void bcVector3::Multipy( const bcVector3& A, float fScaler, bcVector3* out )
		{
			out->x = A.x * fScaler;
			out->y = A.y * fScaler;
			out->z = A.z * fScaler;
		}
		void bcVector3::Negate( const bcVector3& A, bcVector3* out )
		{
			out->x = -A.x;
			out->y = -A.y;
			out->z = -A.z;
		}
		void bcVector3::Subtract ( const bcVector3& A, const bcVector3& B, bcVector3* out )
		{
			out->x = A.x - B.x;
			out->y = A.y - B.y;
			out->z = A.z - B.z;
		}
		void bcVector3::Clamp( const bcVector3& A, bcVector3* out )
		{
			*out = A;
			out->Clamp();
		}
		void bcVector3::Cross( const bcVector3& A, const bcVector3& B, bcVector3* out )
		{    
			XMVECTOR v = XMLoadFloat3(&A);
			XMVECTOR v1 = XMLoadFloat3(&B);
			v = XMVector3Cross(v, v1);
		
			XMStoreFloat3(out, v);
		}
		void bcVector3::Dot( const bcVector3& A, const bcVector3& B, FLOAT* out )
		{    
			XMVECTOR v = XMLoadFloat3(&A);
			XMVECTOR v1 = XMLoadFloat3(&B);
			v = XMVector3Dot(v, v1);
		
			*out = XMVectorGetX(v);
		}
		void bcVector3::Normalize( const bcVector3& A, bcVector3* out )
		{
			*out = A;
			out->Normalize();
		}
		void bcVector3::Perpendicular( const bcVector3& A, bcVector3* out )
		{
			*out = A;
			out->Perpendicular();    
		}
		void bcVector3::Random( bcVector3* out )
		{
			float x = static_cast<float>( (double)rand() / RAND_MAX ) * 2.0f - 1.0f;
			float y = static_cast<float>( (double)rand() / RAND_MAX ) * 2.0f - 1.0f;
			float z = static_cast<float>( (double)rand() / RAND_MAX ) * 2.0f - 1.0f;
	
			*out = bcVector3( x, y, z );
			out->Normalize();
		}
		void bcVector3::Lerp( const bcVector3& A, const bcVector3& B, FLOAT s, bcVector3* out)
		{
			XMVECTOR v = XMLoadFloat3(&A);
			XMVECTOR v1 = XMLoadFloat3(&B);
			v = XMVectorLerp(v, v1, s);

			XMStoreFloat3(out, v);
		}
		void bcVector3::Barycentric( const bcVector3& value1, const bcVector3& value2, const bcVector3& value3, FLOAT amount1, FLOAT amount2, bcVector3* out)
		{
			XMVECTOR v0 = XMLoadFloat3(&value1);
			XMVECTOR v1 = XMLoadFloat3(&value2);
			XMVECTOR v2 = XMLoadFloat3(&value3);

			v0 = XMVectorBaryCentric(v0, v1, v2, amount1, amount2);

			XMStoreFloat3(out, v0);
		}
		void bcVector3::CatmullRom( const bcVector3& value1, const bcVector3& value2, const bcVector3& value3, const bcVector3& value4, float amount, bcVector3* out)
		{
			XMVECTOR v0 = XMLoadFloat3(&value1);
			XMVECTOR v1 = XMLoadFloat3(&value1);
			XMVECTOR v2 = XMLoadFloat3(&value2);
			XMVECTOR v3 = XMLoadFloat3(&value3);

			v0 = XMVectorCatmullRom(v0, v1, v2, v3, amount);

			XMStoreFloat3(out, v0);
		}
		void bcVector3::Distance( const bcVector3& A, const bcVector3& B, FLOAT* out )
		{
			XMVECTOR x = XMLoadFloat3(&A);
			XMVECTOR y = XMLoadFloat3(&B);
			XMVECTOR length = XMVector3Length(XMVectorSubtract(x, y));
			*out = XMVectorGetX(length);
		}
		void bcVector3::DistanceSq( const bcVector3& A, const bcVector3& B, FLOAT* out )
		{
			XMVECTOR x = XMLoadFloat3(&A);
			XMVECTOR y = XMLoadFloat3(&B);
			XMVECTOR length = XMVector3LengthSq(XMVectorSubtract(x, y));
			*out = XMVectorGetX(length);
		}
		void bcVector3::Max( const bcVector3& A, const bcVector3& B, bcVector3* out )
		{
			XMVECTOR v = XMLoadFloat3(&A);
			XMVECTOR v1 = XMLoadFloat3(&B);
			v = XMVectorMax(v, v1);

			XMStoreFloat3(out, v);
		}
		void bcVector3::Min( const bcVector3& A, const bcVector3& B, bcVector3* out )
		{
			XMVECTOR v = XMLoadFloat3(&A);
			XMVECTOR v1 = XMLoadFloat3(&B);
			v = XMVectorMin(v, v1);

			XMStoreFloat3(out, v);
		}
		void bcVector3::Reflect( const bcVector3& A, const bcVector3& Normal, bcVector3* out )
		{
			XMVECTOR v = XMLoadFloat3(&A);
			XMVECTOR v1 = XMLoadFloat3(&Normal);
			v = XMVector3Reflect(v, v1);

			XMStoreFloat3(out, v);
		}
		void bcVector3::Refract( const bcVector3& A, const bcVector3& Normal, FLOAT RefractionIndex, bcVector3* out )
		{
			XMVECTOR v = XMLoadFloat3(&A);
			XMVECTOR v1 = XMLoadFloat3(&Normal);
			v = XMVector3Refract(v, v1, RefractionIndex);

			XMStoreFloat3(out, v);
		}
		void bcVector3::Transform( const bcVector3& A, const bcQuaternion& Q, bcVector3* out )
		{
			XMVECTOR aV = XMLoadFloat3(&A);
			XMVECTOR qV = XMLoadFloat4(&Q);
			aV = XMVector3Rotate(aV, qV);

			XMStoreFloat3(out, aV);
		}
		//static inline void Transform( const bcVector3* inArray, const bcQuaternion& Q , bcVector3* outArray);
		void bcVector3::Transform( const bcVector3& A, const bcMatrix4x4& M, bcVector3* out )
		{
			XMVECTOR aV = XMLoadFloat3(&A);
			aV = XMVector3Transform(aV, M);

			XMStoreFloat3(out, aV);
		}
		//static inline void Transform( const bcVector3* inArray, const bcMatrix4x4& M , bcVector3* outArray);
		void bcVector3::TransformNormal( const bcVector3& A, const bcMatrix4x4& M, bcVector3* out )
		{
			XMVECTOR aV = XMLoadFloat3(&A);
			aV = XMVector3TransformNormal(aV, M);

			XMStoreFloat3(out, aV);
		}

		void bcVector3::Add( const bcVector128& A, const bcVector128& B, bcVector128* out )
		{
			*out = XMVectorAdd(A, B);
		}
		void bcVector3::Divide( const bcVector128& A, const bcVector128& B, bcVector128* out )
		{
			*out = XMVectorDivide(A, B);
		}
		void bcVector3::Divide( const bcVector128& A, float fScaler, bcVector128* out )
		{
			XMVECTOR lScaler = XMLoadFloat3(&XMFLOAT3(fScaler, fScaler, fScaler));
			*out = XMVectorDivide(A, lScaler);
		}
		void bcVector3::Multipy( const bcVector128& A, const bcVector128& B, bcVector128* out )
		{
			*out = XMVectorMultiply(A, B);
		}
		void bcVector3::Multipy( const bcVector128& A, float fScaler, bcVector128* out )
		{
			XMVECTOR lScaler = XMLoadFloat3(&XMFLOAT3(fScaler, fScaler, fScaler));
			*out = XMVectorMultiply(A, lScaler);
		}
		void bcVector3::Negate( const bcVector128& A, bcVector128* out )
		{
			*out = XMVectorNegate(A);
		}
		void bcVector3::Subtract ( const bcVector128& A, const bcVector128& B, bcVector128* out )
		{
			*out = XMVectorSubtract(A, B);
		}
		void bcVector3::Clamp( const bcVector128& A, bcVector128* out )
		{
			*out = XMVectorClamp(A, XMLoadFloat3(&XMFLOAT3(0, 0, 0)), XMLoadFloat3(&XMFLOAT3(1, 1, 1)));
		}
		void bcVector3::Cross( const bcVector128& A, const bcVector128& B, bcVector128* out )
		{    
			*out = XMVector3Cross(A, B);
		}
		void bcVector3::Dot( const bcVector128& A, const bcVector128& B, FLOAT* out )
		{    
			XMVECTOR dot = XMVector3Dot(A, B);
			*out = XMVectorGetX(dot);
		}
		void bcVector3::Normalize( const bcVector128& A, bcVector128* out )
		{
			*out = XMVector3Normalize(A);
		}
		void bcVector3::Lerp( const bcVector128& A, const bcVector128& B, FLOAT s, bcVector128* out )
		{
			*out = XMVectorLerp(A, B, s);
		}
		void bcVector3::Length( const bcVector128& A, float* out )
		{
			XMVECTOR length = XMVector3Length(A);
			*out = XMVectorGetX(length);
		}
		void bcVector3::LenghtSq( const bcVector128& A, float* out )
		{
			XMVECTOR length = XMVector3LengthSq(A);
			*out = XMVectorGetX(length);
		}
		void bcVector3::Barycentric( const bcVector128& value1, const bcVector128& value2, const bcVector128& value3, FLOAT amount1, FLOAT amount2, bcVector128* out )
		{
			*out = XMVectorBaryCentric(value1, value2, value3, amount1, amount2);
		}
		void bcVector3::CatmullRom( const bcVector128& value1, const bcVector128& value2, const bcVector128& value3, const bcVector128& value4, float amount, bcVector128* out )
		{
			*out = XMVectorCatmullRom(value1, value2, value3, value4, amount);
		}
		void bcVector3::Distance( const bcVector128& A, const bcVector128& B, FLOAT* out )
		{
			XMVECTOR length = XMVector3Length(XMVectorSubtract(A, B));
			*out = XMVectorGetX(length);
		}
		void bcVector3::DistanceSq( const bcVector128& A, const bcVector128& B, FLOAT* out )
		{
			XMVECTOR length = XMVector3LengthSq(XMVectorSubtract(A, B));
			*out = XMVectorGetX(length);
		}
		void bcVector3::Max( const bcVector128& A, const bcVector128& B, bcVector128* out )
		{
			*out = XMVectorMax(A, B);
		}
		void bcVector3::Min( const bcVector128& A, const bcVector128& B, bcVector128* out )
		{
			*out = XMVectorMin(A, B);
		}
		void bcVector3::Reflect( const bcVector128& A, const bcVector128& Normal, bcVector128* out )
		{
			*out = XMVector3Reflect(A, Normal);
		}
		void bcVector3::Refract( const bcVector128& A, const bcVector128& Normal, FLOAT RefractionIndex, bcVector128* out )
		{
			*out = XMVector3Refract(A, Normal, RefractionIndex);
		}
		void bcVector3::Transform( const bcVector128& A, const bcVector128& Q, bcVector128* out )
		{
			*out = XMVector3Rotate(A, Q);
		}
		//void bcVector3::Transform( const bcVector128* inArray, const bcQuaternion& Q , bcVector128* outArray);
		void bcVector3::Transform( const bcVector128& A, const bcMatrix4x4& M, bcVector128* out )
		{
			*out = XMVector3Transform(A, M);
		}
		//void bcVector3::Transform( const bcVector128* inArray, const bcMatrix4x4& M , bcVector128* outArray);
		void bcVector3::TransformNormal( const bcVector128& A, const bcMatrix4x4& M, bcVector128* out )
		{
			*out = XMVector3TransformNormal(A, M);
		}
#pragma endregion

	// == bcVector4 =======================================================
#pragma region vector4
		bcVector4::bcVector4( )
		{
		}
		bcVector4::bcVector4( float X, float Y, float Z, float W )
		{
			x = X;
			y = Y;
			z = Z;
			w = W;
		}
		bcVector4::bcVector4( const bcVector4& Vector )
		{
			x = Vector.x;
			y = Vector.y;
			z = Vector.z;
			w = Vector.w;
		}
		void bcVector4::Clamp()
		{
			if ( x > 1.0f ) x = 1.0f;
			if ( x < 0.0f ) x = 0.0f;

			if ( y > 1.0f ) y = 1.0f;
			if ( y < 0.0f ) y = 0.0f;

			if ( z > 1.0f ) z = 1.0f;
			if ( z < 0.0f ) z = 0.0f;

			if ( w > 1.0f ) w = 1.0f;
			if ( w < 0.0f ) w = 0.0f;
		}
		void bcVector4::MakeZero( )
		{
			x = 0.0f;
			y = 0.0f;
			z = 0.0f;
			w = 0.0f;
		}
		float bcVector4::Lenght( )
		{
			XMVECTOR v = XMLoadFloat4(this);
			v = XMVector4Length(v);
			return XMVectorGetX(v);
		}
		float bcVector4::LenghtSq( )
		{
			XMVECTOR v = XMLoadFloat4(this);
			v = XMVector4LengthSq(v);
			return XMVectorGetX(v);
		}
		void bcVector4::Normalize( )
		{
			XMVECTOR v = XMLoadFloat4(this);
			v = XMVector4Normalize(v);
			XMStoreFloat4(this, v);
		}
		/*bcVector4 bcVector4::Perpendicular( )
		{
			float xAbs = fabs( x );
			float yAbs = fabs( y );
			float zAbs = fabs( z );
			float wAbs = fabs( w );
			float minVal = min(min( min( xAbs, yAbs ), zAbs ), wAbs);

			if ( xAbs == minVal )
				return bcVector4::Cross( *this, bcVector4( 1.0f, 0.0f, 0.0f, 0.0f ) );
			else if ( yAbs == minVal )
				return bcVector4::Cross( *this, bcVector4( 0.0f, 1.0f, 0.0f, 0.0f ) );
			else if ( zAbs == minVal )
				return bcVector4::Cross( *this, bcVector4( 0.0f, 0.0f, 1.0f, 0.0f ) );
			else
				return bcVector4::Cross( *this, bcVector4( 0.0f, 0.0f, 0.0f, 1.0f ) );
		}*/
		bcVector4& bcVector4::operator= ( const bcVector4& Vector )
		{
			x = Vector.x;
			y = Vector.y;
			z = Vector.z;
			w = Vector.w;

			return( *this );
		}
		float bcVector4::operator[] ( int iPos ) const
		{
			if ( iPos == 0 ) return( x );
			if ( iPos == 1 ) return( y );
			if ( iPos == 2 ) return( z );
			return( w );
		}
		float& bcVector4::operator[] ( int iPos )
		{
			if ( iPos == 0 ) return( x );
			if ( iPos == 1 ) return( y );
			if ( iPos == 2 ) return( z );
			return( w );
		}
		bool bcVector4::operator== ( const bcVector4& Vector ) const
		{
			XMVECTOR v = XMLoadFloat4(this);
			XMVECTOR v1 = XMLoadFloat4(&Vector);
			return XMVector4Equal(v, v1);
		}
		bool bcVector4::operator!= ( const bcVector4& Vector ) const
		{
			return( !( *this == Vector ) );
		}
		bcVector4 bcVector4::operator+ ( const bcVector4& Vector ) const
		{
			bcVector4 sum;

			sum.x = x + Vector.x;
			sum.y = y + Vector.y;
			sum.z = z + Vector.z;
			sum.w = w + Vector.w;

			return( sum );
		}
		bcVector4 bcVector4::operator- ( const bcVector4& Vector ) const
		{
			bcVector4 diff;

			diff.x = x - Vector.x;
			diff.y = y - Vector.y;
			diff.z = z - Vector.z;
			diff.w = w - Vector.w;

			return( diff );
		}
		bcVector4 bcVector4::operator* ( float fScalar ) const
		{
			bcVector4 prod;

			prod.x = x * fScalar;
			prod.y = y * fScalar;
			prod.z = z * fScalar;
			prod.w = w * fScalar;

			return( prod );
		}
		bcVector4 bcVector4::operator* ( const bcVector4& Vector ) const
		{
			bcVector4 prod;

			prod.x = x * Vector.x;
			prod.y = y * Vector.y;
			prod.z = z * Vector.z;
			prod.w = w * Vector.w;

			return( prod );
		}
		bcVector4 bcVector4::operator/ ( float fScalar ) const
		{
			bcVector4 quot;
			if ( fScalar != 0.0f )
			{
				float fInvScalar = 1.0f / fScalar;
				quot.x = x * fInvScalar;
				quot.y = y * fInvScalar;
				quot.z = z * fInvScalar;
				quot.w = w * fInvScalar;
			}
			else
			{
				quot.MakeZero();
			}

			return( quot );
		}
		bcVector4 bcVector4::operator/ ( const bcVector4& Vector ) const
		{
			bcVector4 quot;
			quot.x = Vector.x != 0.0f ? x / Vector.x : 0.0f;
			quot.y = Vector.y != 0.0f ? y / Vector.y : 0.0f;
			quot.z = Vector.z != 0.0f ? z / Vector.z : 0.0f;
			quot.w = Vector.w != 0.0f ? z / Vector.w : 0.0f;

			return( quot );
		}
		bcVector4 bcVector4::operator- ( ) const
		{
			bcVector4 neg;

			neg.x = -x;
			neg.y = -y;
			neg.z = -z;
			neg.w = -w;

			return( neg );
		}
		bcVector4& bcVector4::operator+= ( const bcVector4& Vector )
		{
			x += Vector.x;
			y += Vector.y;
			z += Vector.z;
			w += Vector.w;

			return( *this );
		}
		bcVector4& bcVector4::operator-= ( const bcVector4& Vector )
		{
			x -= Vector.x;
			y -= Vector.y;
			z -= Vector.z;
			w -= Vector.w;

			return( *this );
		}
		bcVector4& bcVector4::operator*= ( float fScalar )
		{
			x *= fScalar;
			y *= fScalar;
			z *= fScalar;
			w *= fScalar;

			return( *this );
		}
		bcVector4& bcVector4::operator*= ( const bcVector4& Vector )
		{
			x *= Vector.x;
			y *= Vector.y;
			z *= Vector.z;
			w *= Vector.w;

			return( *this );
		}
		bcVector4& bcVector4::operator/= ( float fScalar )
		{
			if ( fScalar != 0.0f )
			{
				float fInvScalar = 1.0f / fScalar;	
				x *= fInvScalar;
				y *= fInvScalar;
				z *= fInvScalar;
				w *= fInvScalar;
			}
			else
			{
				MakeZero();
			}

			return( *this );
		}
		bcVector4& bcVector4::operator/= ( const bcVector4& Vector )
		{
			x = Vector.x != 0.0f ? x / Vector.x : 0.0f;
			y = Vector.y != 0.0f ? y / Vector.y : 0.0f;
			z = Vector.z != 0.0f ? z / Vector.z : 0.0f;
			w = Vector.w != 0.0f ? z / Vector.w : 0.0f;

			return( *this );
		}

		void bcVector4::ToVector(const bcVector4& V4, bcVector128* out)
		{
			*out = XMLoadFloat4(&V4);
		}
		void bcVector4::FromVector(const bcVector128& V, bcVector4* out)
		{
			XMStoreFloat4(out, V);
		}

#pragma region Commented
		//bcVector4 bcVector4::Add( const bcVector4& A, const bcVector4& B )
		//{
		//	bcVector4 sum;

		//	sum.x = A.x + B.x;
		//	sum.y = A.y + B.y;
		//	sum.z = A.z + B.z;
		//	sum.w = A.w + B.w;

		//	return( sum );
		//}
		//bcVector4 bcVector4::Divide( const bcVector4& A, const bcVector4& B )
		//{
		//	bcVector4 quot;
		//	quot.x = B.x != 0.0f ? A.x / B.x : 0.0f;
		//	quot.y = B.y != 0.0f ? A.y / B.y : 0.0f;
		//	quot.z = B.z != 0.0f ? A.z / B.z : 0.0f;
		//	quot.w = B.w != 0.0f ? A.w / B.w : 0.0f;

		//	return( quot );
		//}
		//bcVector4 bcVector4::Divide( const bcVector4& A, float fScaler )
		//{
		//	bcVector4 quot;
		//	if ( fScaler != 0.0f )
		//	{
		//		float fInvScalar = 1.0f / fScaler;
		//		quot.x = A.x * fInvScalar;
		//		quot.y = A.y * fInvScalar;
		//		quot.z = A.z * fInvScalar;
		//		quot.w = A.w * fInvScalar;
		//	}
		//	else
		//	{
		//		quot.MakeZero();
		//	}

		//	return( quot );
		//}
		//bcVector4 bcVector4::Multipy( const bcVector4& A, const bcVector4& B )
		//{
		//	bcVector4 prod;

		//	prod.x = A.x * B.x;
		//	prod.y = A.y * B.y;
		//	prod.z = A.z * B.z;
		//	prod.w = A.w * B.w;

		//	return( prod );
		//}
		//bcVector4 bcVector4::Multipy( const bcVector4& A, float fScaler )
		//{
		//	bcVector4 prod;

		//	prod.x = A.x * fScaler;
		//	prod.y = A.y * fScaler;
		//	prod.z = A.z * fScaler;
		//	prod.w = A.w * fScaler;

		//	return( prod );
		//}
		//bcVector4 bcVector4::Negate( const bcVector4& A )
		//{
		//	bcVector4 neg;

		//	neg.x = -A.x;
		//	neg.y = -A.y;
		//	neg.z = -A.z;
		//	neg.w = -A.w;

		//	return( neg );
		//}
		//bcVector4 bcVector4::Subtract ( const bcVector4& A, const bcVector4& B )
		//{
		//	bcVector4 diff;

		//	diff.x = A.x - B.x;
		//	diff.y = A.y - B.y;
		//	diff.z = A.z - B.z;
		//	diff.w = A.w - B.w;

		//	return( diff );
		//}
		//bcVector4 bcVector4::Clamp( const bcVector4& A )
		//{
		//	bcVector4 vec = A;
		//	vec.Clamp();
		//	return vec;
		//}
		///*bcVector4 bcVector4::Cross( const bcVector4& A, const bcVector4& B )
		//{    
		//	XMVECTOR v = XMLoadFloat4(&A);
		//	XMVECTOR v1 = XMLoadFloat4(&B);
		//	v = XMVector4Cross(v, v1);
		//	
		//	bcVector4 output;
		//	XMStoreFloat4(&output, v);
		//	return output;
		//}*/
		//float bcVector4::Dot( const bcVector4& A, const bcVector4& B )
		//{    
		//	XMVECTOR v = XMLoadFloat4(&A);
		//	XMVECTOR v1 = XMLoadFloat4(&B);
		//	v = XMVector4Dot(v, v1);
		//	
		//	return XMVectorGetX(v);
		//}
		//bcVector4 bcVector4::Normalize( const bcVector4& A )
		//{
		//	bcVector4 vec = A;
		//	vec.Normalize();
		//	return vec;
		//}
		///*bcVector4 bcVector4::Perpendicular( const bcVector4& A )
		//{
		//	bcVector4 vec = A;
		//	return vec.Perpendicular();    
		//}*/
		//bcVector4 bcVector4::Random( )
		//{
		//	float x = static_cast<float>( (double)rand() / RAND_MAX ) * 2.0f - 1.0f;
		//	float y = static_cast<float>( (double)rand() / RAND_MAX ) * 2.0f - 1.0f;
		//	float z = static_cast<float>( (double)rand() / RAND_MAX ) * 2.0f - 1.0f;
		//	float w = static_cast<float>( (double)rand() / RAND_MAX ) * 2.0f - 1.0f;
		//
		//	bcVector4 random = bcVector4( x, y, z, w );
		//	random.Normalize();

		//	return( random );
		//}
		//bcVector4 bcVector4::Lerp( const bcVector4& A, const bcVector4& B, FLOAT s)
		//{
		//	XMVECTOR v = XMLoadFloat4(&A);
		//	XMVECTOR v1 = XMLoadFloat4(&B);
		//	v = XMVectorLerp(v, v1, s);

		//	bcVector4 output;
		//	XMStoreFloat4(&output, v);
		//	return output;
		//}
		//bcVector4 bcVector4::Barycentric( const bcVector4& value1, const bcVector4& value2, const bcVector4& value3, FLOAT amount1, FLOAT amount2)
		//{
		//	XMVECTOR v0 = XMLoadFloat4(&value1);
		//	XMVECTOR v1 = XMLoadFloat4(&value2);
		//	XMVECTOR v2 = XMLoadFloat4(&value3);

		//	v0 = XMVectorBaryCentric(v0, v1, v2, amount1, amount2);

		//	bcVector4 output;
		//	XMStoreFloat4(&output, v0);
		//	return output;
		//}
		//bcVector4 bcVector4::CatmullRom( const bcVector4& value1, const bcVector4& value2, const bcVector4& value3, const bcVector4& value4, float amount)
		//{
		//	XMVECTOR v0 = XMLoadFloat4(&value1);
		//	XMVECTOR v1 = XMLoadFloat4(&value1);
		//	XMVECTOR v2 = XMLoadFloat4(&value2);
		//	XMVECTOR v3 = XMLoadFloat4(&value3);

		//	v0 = XMVectorCatmullRom(v0, v1, v2, v3, amount);

		//	bcVector4 output;
		//	XMStoreFloat4(&output, v0);
		//	return output;
		//}
		//FLOAT bcVector4::Distance( const bcVector4& A, const bcVector4& B )
		//{
		//	XMVECTOR x = XMLoadFloat4(&A);
		//	XMVECTOR y = XMLoadFloat4(&B);
		//	XMVECTOR length = XMVector4Length(XMVectorSubtract(x, y));
		//	return XMVectorGetX(length);
		//}
		//FLOAT bcVector4::DistanceSq( const bcVector4& A, const bcVector4& B )
		//{
		//	XMVECTOR x = XMLoadFloat4(&A);
		//	XMVECTOR y = XMLoadFloat4(&B);
		//	XMVECTOR length = XMVector4LengthSq(XMVectorSubtract(x, y));
		//	return XMVectorGetX(length);
		//}
		//bcVector4 bcVector4::Max( const bcVector4& A, const bcVector4& B )
		//{
		//	XMVECTOR v = XMLoadFloat4(&A);
		//	XMVECTOR v1 = XMLoadFloat4(&B);
		//	v = XMVectorMax(v, v1);

		//	bcVector4 output;
		//	XMStoreFloat4(&output, v);
		//	return output;
		//}
		//bcVector4 bcVector4::Min( const bcVector4& A, const bcVector4& B )
		//{
		//	XMVECTOR v = XMLoadFloat4(&A);
		//	XMVECTOR v1 = XMLoadFloat4(&B);
		//	v = XMVectorMin(v, v1);

		//	bcVector4 output;
		//	XMStoreFloat4(&output, v);
		//	return output;
		//}
		//bcVector4 bcVector4::Reflect( const bcVector4& A, const bcVector4& Normal )
		//{
		//	XMVECTOR v = XMLoadFloat4(&A);
		//	XMVECTOR v1 = XMLoadFloat4(&Normal);
		//	v = XMVector4Reflect(v, v1);

		//	bcVector4 output;
		//	XMStoreFloat4(&output, v);
		//	return output;
		//}
		//bcVector4 bcVector4::Refract( const bcVector4& A, const bcVector4& Normal, FLOAT RefractionIndex )
		//{
		//	XMVECTOR v = XMLoadFloat4(&A);
		//	XMVECTOR v1 = XMLoadFloat4(&Normal);
		//	v = XMVector4Refract(v, v1, RefractionIndex);

		//	bcVector4 output;
		//	XMStoreFloat4(&output, v);
		//	return output;
		//}
		////static inline bcVector4 Transform( const bcVector4& A, const bcQuaternion& Q );
		////static inline void Transform( const bcVector4* inArray, const bcQuaternion& Q , bcVector4* outArray);
		//bcVector4 bcVector4::Transform( const bcVector4& A, const bcMatrix4x4& M )
		//{
		//	XMVECTOR aV = XMLoadFloat4(&A);
		//	aV = XMVector4Transform(aV, M);

		//	bcVector4 output;
		//	XMStoreFloat4(&output, aV);
		//	return output;
		//}
		////static inline void Transform( const bcVector4* inArray, const bcMatrix4x4& M , bcVector4* outArray);
		////static inline bcVector4 TransformNormal( const bcVector4& A, const bcMatrix4x4& M );
#pragma endregion

		void bcVector4::Add( const bcVector4& A, const bcVector4& B, bcVector4* out )
		{
			out->x = A.x + B.x;
			out->y = A.y + B.y;
			out->z = A.z + B.z;
			out->w = A.w + B.w;
		}
		void bcVector4::Divide( const bcVector4& A, const bcVector4& B, bcVector4* out )
		{
			out->x = B.x != 0.0f ? A.x / B.x : 0.0f;
			out->y = B.y != 0.0f ? A.y / B.y : 0.0f;
			out->z = B.z != 0.0f ? A.z / B.z : 0.0f;
			out->w = B.w != 0.0f ? A.w / B.w : 0.0f;
		}
		void bcVector4::Divide( const bcVector4& A, float fScaler, bcVector4* out )
		{
			if ( fScaler != 0.0f )
			{
				float fInvScalar = 1.0f / fScaler;
				out->x = A.x * fInvScalar;
				out->y = A.y * fInvScalar;
				out->z = A.z * fInvScalar;
				out->w = A.w * fInvScalar;
			}
			else
			{
				out->MakeZero();
			}
		}
		void bcVector4::Multipy( const bcVector4& A, const bcVector4& B, bcVector4* out )
		{
			out->x = A.x * B.x;
			out->y = A.y * B.y;
			out->z = A.z * B.z;
			out->w = A.w * B.w;
		}
		void bcVector4::Multipy( const bcVector4& A, float fScaler, bcVector4* out )
		{
			out->x = A.x * fScaler;
			out->y = A.y * fScaler;
			out->z = A.z * fScaler;
			out->w = A.w * fScaler;
		}
		void bcVector4::Negate( const bcVector4& A, bcVector4* out )
		{
			out->x = -A.x;
			out->y = -A.y;
			out->z = -A.z;
			out->w = -A.w;
		}
		void bcVector4::Subtract ( const bcVector4& A, const bcVector4& B, bcVector4* out )
		{
			out->x = A.x - B.x;
			out->y = A.y - B.y;
			out->z = A.z - B.z;
			out->w = A.w - B.w;
		}
		void bcVector4::Clamp( const bcVector4& A, bcVector4* out )
		{
			*out = A;
			out->Clamp();
		}
		/*void bcVector4::Cross( const bcVector4& A, const bcVector4& B, bcVector4* out )
		{    
			XMVECTOR v = XMLoadFloat4(&A);
			XMVECTOR v1 = XMLoadFloat4(&B);
			v = XMVector4Cross(v, v1);
		
			XMStoreFloat4(out, v);
		}*/
		void bcVector4::Dot( const bcVector4& A, const bcVector4& B, FLOAT* out )
		{    
			XMVECTOR v = XMLoadFloat4(&A);
			XMVECTOR v1 = XMLoadFloat4(&B);
			v = XMVector4Dot(v, v1);
		
			*out = XMVectorGetX(v);
		}
		void bcVector4::Normalize( const bcVector4& A, bcVector4* out )
		{
			*out = A;
			out->Normalize();
		}
		/*void bcVector4::Perpendicular( const bcVector4& A, bcVector4* out )
		{
			*out = A;
			out->Perpendicular();    
		}*/
		void bcVector4::Random( bcVector4* out )
		{
			float x = static_cast<float>( (double)rand() / RAND_MAX ) * 2.0f - 1.0f;
			float y = static_cast<float>( (double)rand() / RAND_MAX ) * 2.0f - 1.0f;
			float z = static_cast<float>( (double)rand() / RAND_MAX ) * 2.0f - 1.0f;
			float w = static_cast<float>( (double)rand() / RAND_MAX ) * 2.0f - 1.0f;
	
			*out = bcVector4( x, y, z, w );
			out->Normalize();
		}
		void bcVector4::Lerp( const bcVector4& A, const bcVector4& B, FLOAT s, bcVector4* out )
		{
			XMVECTOR v = XMLoadFloat4(&A);
			XMVECTOR v1 = XMLoadFloat4(&B);
			v = XMVectorLerp(v, v1, s);

			XMStoreFloat4(out, v);
		}
		void bcVector4::Barycentric( const bcVector4& value1, const bcVector4& value2, const bcVector4& value3, FLOAT amount1, FLOAT amount2, bcVector4* out )
		{
			XMVECTOR v0 = XMLoadFloat4(&value1);
			XMVECTOR v1 = XMLoadFloat4(&value2);
			XMVECTOR v2 = XMLoadFloat4(&value3);

			v0 = XMVectorBaryCentric(v0, v1, v2, amount1, amount2);

			XMStoreFloat4(out, v0);
		}
		void bcVector4::CatmullRom( const bcVector4& value1, const bcVector4& value2, const bcVector4& value3, const bcVector4& value4, float amount, bcVector4* out )
		{
			XMVECTOR v0 = XMLoadFloat4(&value1);
			XMVECTOR v1 = XMLoadFloat4(&value1);
			XMVECTOR v2 = XMLoadFloat4(&value2);
			XMVECTOR v3 = XMLoadFloat4(&value3);

			v0 = XMVectorCatmullRom(v0, v1, v2, v3, amount);

			XMStoreFloat4(out, v0);
		}
		void bcVector4::Distance( const bcVector4& A, const bcVector4& B, FLOAT* out )
		{
			XMVECTOR x = XMLoadFloat4(&A);
			XMVECTOR y = XMLoadFloat4(&B);
			XMVECTOR length = XMVector4Length(XMVectorSubtract(x, y));
			*out = XMVectorGetX(length);
		}
		void bcVector4::DistanceSq( const bcVector4& A, const bcVector4& B, FLOAT* out )
		{
			XMVECTOR x = XMLoadFloat4(&A);
			XMVECTOR y = XMLoadFloat4(&B);
			XMVECTOR length = XMVector4LengthSq(XMVectorSubtract(x, y));
			*out = XMVectorGetX(length);
		}
		void bcVector4::Max( const bcVector4& A, const bcVector4& B, bcVector4* out )
		{
			XMVECTOR v = XMLoadFloat4(&A);
			XMVECTOR v1 = XMLoadFloat4(&B);
			v = XMVectorMax(v, v1);

			XMStoreFloat4(out, v);
		}
		void bcVector4::Min( const bcVector4& A, const bcVector4& B, bcVector4* out )
		{
			XMVECTOR v = XMLoadFloat4(&A);
			XMVECTOR v1 = XMLoadFloat4(&B);
			v = XMVectorMin(v, v1);

			XMStoreFloat4(out, v);
		}
		void bcVector4::Reflect( const bcVector4& A, const bcVector4& Normal, bcVector4* out )
		{
			XMVECTOR v = XMLoadFloat4(&A);
			XMVECTOR v1 = XMLoadFloat4(&Normal);
			v = XMVector4Reflect(v, v1);

			XMStoreFloat4(out, v);
		}
		void bcVector4::Refract( const bcVector4& A, const bcVector4& Normal, FLOAT RefractionIndex, bcVector4* out )
		{
			XMVECTOR v = XMLoadFloat4(&A);
			XMVECTOR v1 = XMLoadFloat4(&Normal);
			v = XMVector4Refract(v, v1, RefractionIndex);

			XMStoreFloat4(out, v);
		}
		//static inline void Transform( const bcVector4& A, const bcQuaternion& Q, bcVector4* out );
		//static inline void Transform( const bcVector4* inArray, const bcQuaternion& Q , bcVector4* outArray);
		void bcVector4::Transform( const bcVector4& A, const bcMatrix4x4& M, bcVector4* out )
		{
			XMVECTOR aV = XMLoadFloat4(&A);
			aV = XMVector4Transform(aV, M);

			XMStoreFloat4(out, aV);
		}
		//static inline void Transform( const bcVector4* inArray, const bcMatrix4x4& M , bcVector4* outArray);
		//static inline void TransformNormal( const bcVector4& A, const bcMatrix4x4& M, bcVector4* out );

		void bcVector4::Add( const bcVector128& A, const bcVector128& B, bcVector128* out )
		{
			*out = XMVectorAdd(A, B);
		}
		void bcVector4::Divide( const bcVector128& A, const bcVector128& B, bcVector128* out )
		{
			*out = XMVectorDivide(A, B);
		}
		void bcVector4::Divide( const bcVector128& A, float fScaler, bcVector128* out )
		{
			XMVECTOR lScaler = XMLoadFloat4(&XMFLOAT4(fScaler, fScaler, fScaler, fScaler));
			*out = XMVectorDivide(A, lScaler);
		}
		void bcVector4::Multipy( const bcVector128& A, const bcVector128& B, bcVector128* out )
		{
			*out = XMVectorMultiply(A, B);
		}
		void bcVector4::Multipy( const bcVector128& A, float fScaler, bcVector128* out )
		{
			XMVECTOR lScaler = XMLoadFloat4(&XMFLOAT4(fScaler, fScaler, fScaler, fScaler));
			*out = XMVectorMultiply(A, lScaler);
		}
		void bcVector4::Negate( const bcVector128& A, bcVector128* out )
		{
			*out = XMVectorNegate(A);
		}
		void bcVector4::Subtract ( const bcVector128& A, const bcVector128& B, bcVector128* out )
		{
			*out = XMVectorSubtract(A, B);
		}
		void bcVector4::Clamp( const bcVector128& A, bcVector128* out )
		{
			*out = XMVectorClamp(A, XMLoadFloat4(&XMFLOAT4(0, 0, 0, 0)), XMLoadFloat4(&XMFLOAT4(1, 1, 1, 1)));
		}
		/*void bcVector4::Cross( const bcVector128& A, const bcVector128& B, bcVector128* out )
		{    
			*out = XMVector4Cross(A, B);
		}*/
		void bcVector4::Dot( const bcVector128& A, const bcVector128& B, FLOAT* out )
		{    
			XMVECTOR dot = XMVector4Dot(A, B);
			*out = XMVectorGetX(dot);
		}
		void bcVector4::Normalize( const bcVector128& A, bcVector128* out )
		{
			*out = XMVector4Normalize(A);
		}
		void bcVector4::Lerp( const bcVector128& A, const bcVector128& B, FLOAT s, bcVector128* out )
		{
			*out = XMVectorLerp(A, B, s);
		}
		void bcVector4::Length( const bcVector128& A, float* out )
		{
			XMVECTOR length = XMVector4Length(A);
			*out = XMVectorGetX(length);
		}
		void bcVector4::LenghtSq( const bcVector128& A, float* out )
		{
			XMVECTOR length = XMVector4LengthSq(A);
			*out = XMVectorGetX(length);
		}
		void bcVector4::Barycentric( const bcVector128& value1, const bcVector128& value2, const bcVector128& value3, FLOAT amount1, FLOAT amount2, bcVector128* out )
		{
			*out = XMVectorBaryCentric(value1, value2, value3, amount1, amount2);
		}
		void bcVector4::CatmullRom( const bcVector128& value1, const bcVector128& value2, const bcVector128& value3, const bcVector128& value4, float amount, bcVector128* out )
		{
			*out = XMVectorCatmullRom(value1, value2, value3, value4, amount);
		}
		void bcVector4::Distance( const bcVector128& A, const bcVector128& B, FLOAT* out )
		{
			XMVECTOR length = XMVector4Length(XMVectorSubtract(A, B));
			*out = XMVectorGetX(length);
		}
		void bcVector4::DistanceSq( const bcVector128& A, const bcVector128& B, FLOAT* out )
		{
			XMVECTOR length = XMVector4LengthSq(XMVectorSubtract(A, B));
			*out = XMVectorGetX(length);
		}
		void bcVector4::Max( const bcVector128& A, const bcVector128& B, bcVector128* out )
		{
			*out = XMVectorMax(A, B);
		}
		void bcVector4::Min( const bcVector128& A, const bcVector128& B, bcVector128* out )
		{
			*out = XMVectorMin(A, B);
		}
		void bcVector4::Reflect( const bcVector128& A, const bcVector128& Normal, bcVector128* out )
		{
			*out = XMVector4Reflect(A, Normal);
		}
		void bcVector4::Refract( const bcVector128& A, const bcVector128& Normal, FLOAT RefractionIndex, bcVector128* out )
		{
			*out = XMVector4Refract(A, Normal, RefractionIndex);
		}
		//void bcVector4::Transform( const bcVector128& A, const bcQuaternion& Q, bcVector128* out );
		//void bcVector4::Transform( const bcVector128* inArray, const bcQuaternion& Q , bcVector128* outArray);
		void bcVector4::Transform( const bcVector128& A, const bcMatrix4x4& M, bcVector128* out )
		{
			*out = XMVector4Transform(A, M);
		}
		//void bcVector4::Transform( const bcVector128* inArray, const bcMatrix4x4& M , bcVector128* outArray);
		//void bcVector4::TransformNormal( const bcVector128& A, const bcMatrix4x4& M, bcVector128* out );
#pragma endregion

	// == bcMatrix4x4 ====================================================
#pragma region matrix4x4
		bcMatrix4x4::bcMatrix4x4( )
		{
		}
		bcMatrix4x4::bcMatrix4x4(	float fM11, float fM12, float fM13, float fM14,
								float fM21, float fM22, float fM23, float fM24,
								float fM31, float fM32, float fM33, float fM34,
								float fM41, float fM42, float fM43, float fM44 )
		{
			this->_11 = fM11;
			this->_12 = fM12;
			this->_13 = fM13;
			this->_14 = fM14;
			this->_21 = fM21;
			this->_22 = fM22;
			this->_23 = fM23;
			this->_24 = fM24;
			this->_31 = fM31;
			this->_32 = fM32;
			this->_33 = fM33;
			this->_34 = fM34;
			this->_41 = fM41;
			this->_42 = fM42;
			this->_43 = fM43;
			this->_44 = fM44;
		}
		bcMatrix4x4::bcMatrix4x4( const bcMatrix4x4& matrix)
		{
			this->_11 = matrix._11;
			this->_12 = matrix._12;
			this->_13 = matrix._13;
			this->_14 = matrix._14;
			this->_21 = matrix._21;
			this->_22 = matrix._22;
			this->_23 = matrix._23;
			this->_24 = matrix._24;
			this->_31 = matrix._31;
			this->_32 = matrix._32;
			this->_33 = matrix._33;
			this->_34 = matrix._34;
			this->_41 = matrix._41;
			this->_42 = matrix._42;
			this->_43 = matrix._43;
			this->_44 = matrix._44;
		}
		bcMatrix4x4::bcMatrix4x4( const XMMATRIX& matrix)
		{
			this->_11 = matrix._11;
			this->_12 = matrix._12;
			this->_13 = matrix._13;
			this->_14 = matrix._14;
			this->_21 = matrix._21;
			this->_22 = matrix._22;
			this->_23 = matrix._23;
			this->_24 = matrix._24;
			this->_31 = matrix._31;
			this->_32 = matrix._32;
			this->_33 = matrix._33;
			this->_34 = matrix._34;
			this->_41 = matrix._41;
			this->_42 = matrix._42;
			this->_43 = matrix._43;
			this->_44 = matrix._44;
		}

		void bcMatrix4x4::MakeIdentity( )
		{
			for ( int iRow = 0; iRow < 4; iRow++ ) 
			{
				for ( int iCol = 0; iCol < 4; iCol++ )
				{
					if ( iRow == iCol )
						this->m[iRow][iCol] = 1.0f;
					else
						this->m[iRow][iCol] = 0.0f;
				}
			}
		}
		void bcMatrix4x4::MakeTranspose( )
		{
			*this = XMMatrixTranspose(*this);
		}

		bcMatrix4x4& bcMatrix4x4::operator= (const bcMatrix4x4& matrix)
		{
			this->_11 = matrix._11;
			this->_12 = matrix._12;
			this->_13 = matrix._13;
			this->_14 = matrix._14;
			this->_21 = matrix._21;
			this->_22 = matrix._22;
			this->_23 = matrix._23;
			this->_24 = matrix._24;
			this->_31 = matrix._31;
			this->_32 = matrix._32;
			this->_33 = matrix._33;
			this->_34 = matrix._34;
			this->_41 = matrix._41;
			this->_42 = matrix._42;
			this->_43 = matrix._43;
			this->_44 = matrix._44;
			return *this;
		}
		bool bcMatrix4x4::operator== ( const bcMatrix4x4& matrix ) const
		{
			return( memcmp( m, matrix.m, 4*4*sizeof(float) ) == 0 );
		}
		bool bcMatrix4x4::operator!= ( const bcMatrix4x4& matrix ) const
		{
			return( memcmp( m, matrix.m, 4*4*sizeof(float) ) != 0 );
		}

		bcMatrix4x4 bcMatrix4x4::operator+ ( const bcMatrix4x4& matrix ) const
		{
			bcMatrix4x4 mOutput;

			mOutput.r[0] = XMVectorAdd(this->r[0], matrix.r[0]);
			mOutput.r[1] = XMVectorAdd(this->r[1], matrix.r[1]);
			mOutput.r[2] = XMVectorAdd(this->r[2], matrix.r[2]);
			mOutput.r[3] = XMVectorAdd(this->r[3], matrix.r[3]);

			return mOutput;
		}
		bcMatrix4x4 bcMatrix4x4::operator- ( const bcMatrix4x4& matrix ) const
		{
			bcMatrix4x4 mOutput;

			mOutput.r[0] = XMVectorSubtract(this->r[0], matrix.r[0]);
			mOutput.r[1] = XMVectorSubtract(this->r[1], matrix.r[1]);
			mOutput.r[2] = XMVectorSubtract(this->r[2], matrix.r[2]);
			mOutput.r[3] = XMVectorSubtract(this->r[3], matrix.r[3]);

			return mOutput;
		}
		bcMatrix4x4 bcMatrix4x4::operator* ( const bcMatrix4x4& matrix ) const
		{
			bcMatrix4x4 mOutput;

			mOutput = XMMatrixMultiply(*this, matrix);

			return mOutput;
		}
		bcMatrix4x4 bcMatrix4x4::operator* ( float fScalar ) const
		{
			bcMatrix4x4 mOutput;

			mOutput.r[0] = XMVectorMultiply(this->r[0], XMLoadFloat4(&XMFLOAT4(fScalar, fScalar, fScalar, fScalar)));
			mOutput.r[1] = XMVectorMultiply(this->r[1], XMLoadFloat4(&XMFLOAT4(fScalar, fScalar, fScalar, fScalar)));
			mOutput.r[2] = XMVectorMultiply(this->r[2], XMLoadFloat4(&XMFLOAT4(fScalar, fScalar, fScalar, fScalar)));
			mOutput.r[3] = XMVectorMultiply(this->r[3], XMLoadFloat4(&XMFLOAT4(fScalar, fScalar, fScalar, fScalar)));

			return mOutput;
		}
		bcMatrix4x4 bcMatrix4x4::operator/ ( float fScalar ) const
		{
			bcMatrix4x4 mOutput;

			mOutput.r[0] = XMVectorDivide(this->r[0], XMLoadFloat4(&XMFLOAT4(fScalar, fScalar, fScalar, fScalar)));
			mOutput.r[1] = XMVectorDivide(this->r[1], XMLoadFloat4(&XMFLOAT4(fScalar, fScalar, fScalar, fScalar)));
			mOutput.r[2] = XMVectorDivide(this->r[2], XMLoadFloat4(&XMFLOAT4(fScalar, fScalar, fScalar, fScalar)));
			mOutput.r[3] = XMVectorDivide(this->r[3], XMLoadFloat4(&XMFLOAT4(fScalar, fScalar, fScalar, fScalar)));

			return mOutput;
		}
		bcMatrix4x4 bcMatrix4x4::operator- () const
		{
			bcMatrix4x4 mOutput;

			mOutput.r[0] = XMVectorNegate(this->r[0]);
			mOutput.r[1] = XMVectorNegate(this->r[1]);
			mOutput.r[2] = XMVectorNegate(this->r[2]);
			mOutput.r[3] = XMVectorNegate(this->r[3]);

			return mOutput;
		}

		bcMatrix4x4& bcMatrix4x4::operator+= ( const bcMatrix4x4& matrix )
		{
			this->r[0] = XMVectorAdd(this->r[0], matrix.r[0]);
			this->r[1] = XMVectorAdd(this->r[1], matrix.r[1]);
			this->r[2] = XMVectorAdd(this->r[2], matrix.r[2]);
			this->r[3] = XMVectorAdd(this->r[3], matrix.r[3]);

			return *this;
		}
		bcMatrix4x4& bcMatrix4x4::operator-= ( const bcMatrix4x4& matrix )
		{
			this->r[0] = XMVectorSubtract(this->r[0], matrix.r[0]);
			this->r[1] = XMVectorSubtract(this->r[1], matrix.r[1]);
			this->r[2] = XMVectorSubtract(this->r[2], matrix.r[2]);
			this->r[3] = XMVectorSubtract(this->r[3], matrix.r[3]);

			return *this;
		}
		bcMatrix4x4& bcMatrix4x4::operator*= ( const bcMatrix4x4& matrix )
		{
			*this = XMMatrixMultiply(*this, matrix);

			return *this;
		}
		bcMatrix4x4& bcMatrix4x4::operator*= ( float fScalar )
		{
			this->r[0] = XMVectorMultiply(this->r[0], XMLoadFloat4(&XMFLOAT4(fScalar, fScalar, fScalar, fScalar)));
			this->r[1] = XMVectorMultiply(this->r[1], XMLoadFloat4(&XMFLOAT4(fScalar, fScalar, fScalar, fScalar)));
			this->r[2] = XMVectorMultiply(this->r[2], XMLoadFloat4(&XMFLOAT4(fScalar, fScalar, fScalar, fScalar)));
			this->r[3] = XMVectorMultiply(this->r[3], XMLoadFloat4(&XMFLOAT4(fScalar, fScalar, fScalar, fScalar)));

			return *this;
		}
		bcMatrix4x4& bcMatrix4x4::operator/= ( float fScalar )
		{
			this->r[0] = XMVectorDivide(this->r[0], XMLoadFloat4(&XMFLOAT4(fScalar, fScalar, fScalar, fScalar)));
			this->r[1] = XMVectorDivide(this->r[1], XMLoadFloat4(&XMFLOAT4(fScalar, fScalar, fScalar, fScalar)));
			this->r[2] = XMVectorDivide(this->r[2], XMLoadFloat4(&XMFLOAT4(fScalar, fScalar, fScalar, fScalar)));
			this->r[3] = XMVectorDivide(this->r[3], XMLoadFloat4(&XMFLOAT4(fScalar, fScalar, fScalar, fScalar)));

			return *this;
		}

#pragma region Commented
		/*bcMatrix4x4 bcMatrix4x4::Add(const bcMatrix4x4& matrix1, const bcMatrix4x4& matrix2)
		{
			bcMatrix4x4 mOutput;

			mOutput.r[0] = XMVectorAdd(matrix1.r[0], matrix2.r[0]);
			mOutput.r[1] = XMVectorAdd(matrix1.r[1], matrix2.r[1]);
			mOutput.r[2] = XMVectorAdd(matrix1.r[2], matrix2.r[2]);
			mOutput.r[3] = XMVectorAdd(matrix1.r[3], matrix2.r[3]);

			return mOutput;
		}
		bcMatrix4x4 bcMatrix4x4::Divide(const bcMatrix4x4& matrix1, float divider)
		{
			bcMatrix4x4 mOutput;

			mOutput.r[0] = XMVectorAdd(matrix1.r[0], XMLoadFloat4(&XMFLOAT4(divider, divider, divider, divider)));
			mOutput.r[1] = XMVectorAdd(matrix1.r[1], XMLoadFloat4(&XMFLOAT4(divider, divider, divider, divider)));
			mOutput.r[2] = XMVectorAdd(matrix1.r[2], XMLoadFloat4(&XMFLOAT4(divider, divider, divider, divider)));
			mOutput.r[3] = XMVectorAdd(matrix1.r[3], XMLoadFloat4(&XMFLOAT4(divider, divider, divider, divider)));

			return mOutput;
		}
		bcMatrix4x4 bcMatrix4x4::Divide(const bcMatrix4x4& matrix1, const bcMatrix4x4& matrix2)
		{
			bcMatrix4x4 mOutput;

			mOutput.r[0] = XMVectorDivide(matrix1.r[0], matrix2.r[0]);
			mOutput.r[1] = XMVectorDivide(matrix1.r[1], matrix2.r[1]);
			mOutput.r[2] = XMVectorDivide(matrix1.r[2], matrix2.r[2]);
			mOutput.r[3] = XMVectorDivide(matrix1.r[3], matrix2.r[3]);

			return mOutput;
		}
		bcMatrix4x4 bcMatrix4x4::Multiply(const bcMatrix4x4& matrix1, float scaleFactor)
		{
			bcMatrix4x4 mOutput;

			mOutput.r[0] = XMVectorMultiply(matrix1.r[0], XMLoadFloat4(&XMFLOAT4(scaleFactor, scaleFactor, scaleFactor, scaleFactor)));
			mOutput.r[1] = XMVectorMultiply(matrix1.r[1], XMLoadFloat4(&XMFLOAT4(scaleFactor, scaleFactor, scaleFactor, scaleFactor)));
			mOutput.r[2] = XMVectorMultiply(matrix1.r[2], XMLoadFloat4(&XMFLOAT4(scaleFactor, scaleFactor, scaleFactor, scaleFactor)));
			mOutput.r[3] = XMVectorMultiply(matrix1.r[3], XMLoadFloat4(&XMFLOAT4(scaleFactor, scaleFactor, scaleFactor, scaleFactor)));

			return mOutput;
		}
		bcMatrix4x4 bcMatrix4x4::Multiply(const bcMatrix4x4& matrix1, const bcMatrix4x4& matrix2)
		{
			bcMatrix4x4 mOutput;

			mOutput = XMMatrixMultiply(matrix1, matrix2);

			return mOutput;
		}
		bcMatrix4x4 bcMatrix4x4::MultiplyTraspose(const bcMatrix4x4& matrix1, const bcMatrix4x4& matrix2)
		{
			bcMatrix4x4 mOutput;

			mOutput = XMMatrixMultiplyTranspose(matrix1, matrix2);

			return mOutput;
		}
		bcMatrix4x4 bcMatrix4x4::Negate(const bcMatrix4x4& matrix)
		{
			bcMatrix4x4 mOutput;

			mOutput.r[0] = XMVectorNegate(matrix.r[0]);
			mOutput.r[1] = XMVectorNegate(matrix.r[1]);
			mOutput.r[2] = XMVectorNegate(matrix.r[2]);
			mOutput.r[3] = XMVectorNegate(matrix.r[3]);

			return mOutput;
		}
		bcMatrix4x4 bcMatrix4x4::Subtract(const bcMatrix4x4& matrix1, const bcMatrix4x4& matrix2)
		{
			bcMatrix4x4 mOutput;

			mOutput.r[0] = XMVectorSubtract(matrix1.r[0], matrix2.r[0]);
			mOutput.r[1] = XMVectorSubtract(matrix1.r[1], matrix2.r[1]);
			mOutput.r[2] = XMVectorSubtract(matrix1.r[2], matrix2.r[2]);
			mOutput.r[3] = XMVectorSubtract(matrix1.r[3], matrix2.r[3]);

			return mOutput;
		}
		bcMatrix4x4 bcMatrix4x4::CreateFromAxisAngle(const bcVector3& axis, float angle)
		{
			bcMatrix4x4 mOutput;
		
			mOutput = XMMatrixRotationAxis(XMLoadFloat3(&axis), angle);

			return mOutput;
		}
		bcMatrix4x4 bcMatrix4x4::CreateFrombcQuaternion(const bcQuaternion& quaternion)
		{
			bcMatrix4x4 mOutput;
		
			mOutput = XMMatrixRotationbcQuaternion(XMLoadFloat4(&quaternion));

			return mOutput;
		}
		bcMatrix4x4 bcMatrix4x4::CreateFromNormalAngle(const bcVector3& normal, float angle)
		{
			bcMatrix4x4 mOutput;
		
			mOutput = XMMatrixRotationNormal(XMLoadFloat3(&normal), angle);

			return mOutput;
		}
		bcMatrix4x4 bcMatrix4x4::CreateFromYawPitchRoll(float yaw, float pitch, float roll)
		{
			bcMatrix4x4 mOutput;
		
			mOutput = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

			return mOutput;
		}
		bcMatrix4x4 bcMatrix4x4::CreateLookAtLH(const bcVector3& cameraPosition, const bcVector3& cameraTarget, const bcVector3& cameraUp)
		{
			bcMatrix4x4 mOutput;
		
			mOutput = XMMatrixLookAtLH(XMLoadFloat3(&cameraPosition), XMLoadFloat3(&cameraTarget), XMLoadFloat3(&cameraUp));

			return mOutput;
		}
		bcMatrix4x4 bcMatrix4x4::CreateOrthographicLH(float width, float height, float zNear, float zFar)
		{
			bcMatrix4x4 mOutput;
		
			mOutput = XMMatrixOrthographicLH(width, height, zNear, zFar);

			return mOutput;
		}
		bcMatrix4x4 bcMatrix4x4::CreateOrthographicOffCenterLH(float left, float right, float bottom, float top, float zNear, float zFar)
		{
			bcMatrix4x4 mOutput;
		
			mOutput = XMMatrixOrthographicOffCenterLH(left, right, bottom, top, zNear, zFar);

			return mOutput;
		}
		bcMatrix4x4 bcMatrix4x4::CreatePerspective(float Width, float height, float zNear, float zFar)
		{
			bcMatrix4x4 mOutput;
		
			mOutput = XMMatrixPerspectiveLH(Width, height, zNear, zFar);

			return mOutput;
		}
		bcMatrix4x4 bcMatrix4x4::CreatePerspectiveFieldOfView(float fieldOfView, float aspectRatio, float nearPlaneDistance, float farPlaneDistance)
		{
			bcMatrix4x4 mOutput;
		
			mOutput = XMMatrixPerspectiveFovLH(fieldOfView, aspectRatio, nearPlaneDistance, farPlaneDistance);

			return mOutput;
		}
		bcMatrix4x4 bcMatrix4x4::CreatePerspectiveOffCenter(float left, float right, float bottom, float top, float nearPlaneDistance, float farPlaneDistance)
		{
			bcMatrix4x4 mOutput;
		
			mOutput = XMMatrixOrthographicOffCenterLH(left, right, bottom, top, nearPlaneDistance, farPlaneDistance);

			return mOutput;
		}
		bcMatrix4x4 bcMatrix4x4::CreateRotationX(float radians)
		{
			bcMatrix4x4 mOutput;
		
			mOutput = XMMatrixRotationX(radians);

			return mOutput;
		}
		bcMatrix4x4 bcMatrix4x4::CreateRotationY(float radians)
		{
			bcMatrix4x4 mOutput;
		
			mOutput = XMMatrixRotationY(radians);

			return mOutput;
		}
		bcMatrix4x4 bcMatrix4x4::CreateRotationZ(float radians)
		{
			bcMatrix4x4 mOutput;
		
			mOutput = XMMatrixRotationZ(radians);

			return mOutput;
		}
		bcMatrix4x4 bcMatrix4x4::CreateScale(float scale)
		{
			bcMatrix4x4 mOutput;
		
			mOutput = XMMatrixScaling(scale, scale, scale);

			return mOutput;
		}
		bcMatrix4x4 bcMatrix4x4::CreateScale(float xScale, float yScale, float zScale)
		{
			bcMatrix4x4 mOutput;
		
			mOutput = XMMatrixScaling(xScale, yScale, zScale);

			return mOutput;
		}
		bcMatrix4x4 bcMatrix4x4::CreateTranslation(const bcVector3& position)
		{
			bcMatrix4x4 mOutput;
		
			mOutput = XMMatrixTranslation(position.x, position.y, position.z);

			return mOutput;
		}
		BOOL bcMatrix4x4::Decompose(bcVector3* scale, bcQuaternion* rotation, bcVector3* translation, const bcMatrix4x4& matrix)
		{
			XMVECTOR scaleV;
			XMVECTOR rotationV;
			XMVECTOR translationV;

			BOOL ret = XMMatrixDecompose(&scaleV, &rotationV, &translationV, matrix);

			XMStoreFloat3(scale, scaleV);
			XMStoreFloat4(rotation, rotationV);
			XMStoreFloat3(translation, translationV);

			return ret;
		}
		FLOAT bcMatrix4x4::Determinant(const bcMatrix4x4& matrix)
		{
			return XMVectorGetX(XMMatrixDeterminant(matrix));
		}
		bcMatrix4x4 bcMatrix4x4::Invert(const bcMatrix4x4& matrix)
		{
			bcMatrix4x4 mOutput;
			XMVECTOR det;
		
			mOutput = XMMatrixInverse(&det, matrix);

			return mOutput;
		}
		bcMatrix4x4 bcMatrix4x4::Lerp(const bcMatrix4x4& matrix1, const bcMatrix4x4& matrix2, float amount)
		{
			bcMatrix4x4 mOutput;
		
			mOutput.r[0] = XMVectorLerp(matrix1.r[0], matrix2.r[0], amount);
			mOutput.r[1] = XMVectorLerp(matrix1.r[1], matrix2.r[1], amount);
			mOutput.r[2] = XMVectorLerp(matrix1.r[2], matrix2.r[2], amount);
			mOutput.r[3] = XMVectorLerp(matrix1.r[3], matrix2.r[3], amount);

			return mOutput;
		}
		bcMatrix4x4 bcMatrix4x4::Transpose(const bcMatrix4x4& matrix)
		{
			bcMatrix4x4 mOutput;
		
			mOutput = XMMatrixTranspose(matrix);
		
			return mOutput;
		}*/
#pragma endregion

		void bcMatrix4x4::Add(const bcMatrix4x4& matrix1, const bcMatrix4x4& matrix2, bcMatrix4x4* out)
		{
			out->r[0] = XMVectorAdd(matrix1.r[0], matrix2.r[0]);
			out->r[1] = XMVectorAdd(matrix1.r[1], matrix2.r[1]);
			out->r[2] = XMVectorAdd(matrix1.r[2], matrix2.r[2]);
			out->r[3] = XMVectorAdd(matrix1.r[3], matrix2.r[3]);
		}
		void bcMatrix4x4::Divide(const bcMatrix4x4& matrix1, float divider, bcMatrix4x4* out)
		{
			out->r[0] = XMVectorAdd(matrix1.r[0], XMLoadFloat4(&XMFLOAT4(divider, divider, divider, divider)));
			out->r[1] = XMVectorAdd(matrix1.r[1], XMLoadFloat4(&XMFLOAT4(divider, divider, divider, divider)));
			out->r[2] = XMVectorAdd(matrix1.r[2], XMLoadFloat4(&XMFLOAT4(divider, divider, divider, divider)));
			out->r[3] = XMVectorAdd(matrix1.r[3], XMLoadFloat4(&XMFLOAT4(divider, divider, divider, divider)));
		}
		void bcMatrix4x4::Divide(const bcMatrix4x4& matrix1, const bcMatrix4x4& matrix2, bcMatrix4x4* out)
		{
			out->r[0] = XMVectorDivide(matrix1.r[0], matrix2.r[0]);
			out->r[1] = XMVectorDivide(matrix1.r[1], matrix2.r[1]);
			out->r[2] = XMVectorDivide(matrix1.r[2], matrix2.r[2]);
			out->r[3] = XMVectorDivide(matrix1.r[3], matrix2.r[3]);
		}
		void bcMatrix4x4::Multiply(const bcMatrix4x4& matrix1, float scaleFactor, bcMatrix4x4* out)
		{
			out->r[0] = XMVectorMultiply(matrix1.r[0], XMLoadFloat4(&XMFLOAT4(scaleFactor, scaleFactor, scaleFactor, scaleFactor)));
			out->r[1] = XMVectorMultiply(matrix1.r[1], XMLoadFloat4(&XMFLOAT4(scaleFactor, scaleFactor, scaleFactor, scaleFactor)));
			out->r[2] = XMVectorMultiply(matrix1.r[2], XMLoadFloat4(&XMFLOAT4(scaleFactor, scaleFactor, scaleFactor, scaleFactor)));
			out->r[3] = XMVectorMultiply(matrix1.r[3], XMLoadFloat4(&XMFLOAT4(scaleFactor, scaleFactor, scaleFactor, scaleFactor)));
		}
		void bcMatrix4x4::Multiply(const bcMatrix4x4& matrix1, const bcMatrix4x4& matrix2, bcMatrix4x4* out)
		{
			*out = XMMatrixMultiply(matrix1, matrix2);
		}
		void bcMatrix4x4::MultiplyTraspose(const bcMatrix4x4& matrix1, const bcMatrix4x4& matrix2, bcMatrix4x4* out)
		{
			*out = XMMatrixMultiplyTranspose(matrix1, matrix2);
		}
		void bcMatrix4x4::Negate(const bcMatrix4x4& matrix, bcMatrix4x4* out)
		{
			out->r[0] = XMVectorNegate(matrix.r[0]);
			out->r[1] = XMVectorNegate(matrix.r[1]);
			out->r[2] = XMVectorNegate(matrix.r[2]);
			out->r[3] = XMVectorNegate(matrix.r[3]);
		}
		void bcMatrix4x4::Subtract(const bcMatrix4x4& matrix1, const bcMatrix4x4& matrix2, bcMatrix4x4* out)
		{
			out->r[0] = XMVectorSubtract(matrix1.r[0], matrix2.r[0]);
			out->r[1] = XMVectorSubtract(matrix1.r[1], matrix2.r[1]);
			out->r[2] = XMVectorSubtract(matrix1.r[2], matrix2.r[2]);
			out->r[3] = XMVectorSubtract(matrix1.r[3], matrix2.r[3]);
		}
		void bcMatrix4x4::CreateFromAxisAngle(const bcVector3& axis, float angle, bcMatrix4x4* out)
		{
			*out = XMMatrixRotationAxis(XMLoadFloat3(&axis), angle);
		}
		void bcMatrix4x4::CreateFrombcQuaternion(const bcQuaternion& quaternion, bcMatrix4x4* out)
		{
			*out = XMMatrixRotationQuaternion(XMLoadFloat4(&quaternion));
		}
		void bcMatrix4x4::CreateFromNormalAngle(const bcVector3& normal, float angle, bcMatrix4x4* out)
		{
			*out = XMMatrixRotationNormal(XMLoadFloat3(&normal), angle);
		}
		void bcMatrix4x4::CreateFromYawPitchRoll(float yaw, float pitch, float roll, bcMatrix4x4* out)
		{
			*out = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);
		}
		void bcMatrix4x4::CreateLookAtLH(const bcVector3& cameraPosition, const bcVector3& cameraTarget, const bcVector3& cameraUp, bcMatrix4x4* out)
		{
			*out = XMMatrixLookAtLH(XMLoadFloat3(&cameraPosition), XMLoadFloat3(&cameraTarget), XMLoadFloat3(&cameraUp));
		}
		void bcMatrix4x4::CreateOrthographicLH(float width, float height, float zNear, float zFar, bcMatrix4x4* out)
		{
			*out = XMMatrixOrthographicLH(width, height, zNear, zFar);
		}
		void bcMatrix4x4::CreateOrthographicOffCenterLH(float left, float right, float bottom, float top, float zNear, float zFar, bcMatrix4x4* out)
		{
			*out = XMMatrixOrthographicOffCenterLH(left, right, bottom, top, zNear, zFar);
		}
		void bcMatrix4x4::CreatePerspective(float Width, float height, float zNear, float zFar, bcMatrix4x4* out)
		{
			*out = XMMatrixPerspectiveLH(Width, height, zNear, zFar);
		}
		void bcMatrix4x4::CreatePerspectiveFieldOfView(float fieldOfView, float aspectRatio, float nearPlaneDistance, float farPlaneDistance, bcMatrix4x4* out)
		{
			*out = XMMatrixPerspectiveFovLH(fieldOfView, aspectRatio, nearPlaneDistance, farPlaneDistance);
		}
		void bcMatrix4x4::CreatePerspectiveOffCenter(float left, float right, float bottom, float top, float nearPlaneDistance, float farPlaneDistance, bcMatrix4x4* out)
		{
			*out = XMMatrixOrthographicOffCenterLH(left, right, bottom, top, nearPlaneDistance, farPlaneDistance);
		}
		void bcMatrix4x4::CreateRotationX(float radians, bcMatrix4x4* out)
		{
			*out = XMMatrixRotationX(radians);
		}
		void bcMatrix4x4::CreateRotationY(float radians, bcMatrix4x4* out)
		{
			*out = XMMatrixRotationY(radians);
		}
		void bcMatrix4x4::CreateRotationZ(float radians, bcMatrix4x4* out)
		{
			*out = XMMatrixRotationZ(radians);
		}
		void bcMatrix4x4::CreateScale(float scale, bcMatrix4x4* out)
		{
			*out = XMMatrixScaling(scale, scale, scale);
		}
		void bcMatrix4x4::CreateScale(float xScale, float yScale, float zScale, bcMatrix4x4* out)
		{
			*out = XMMatrixScaling(xScale, yScale, zScale);
		}
		void bcMatrix4x4::CreateTranslation(const bcVector3& position, bcMatrix4x4* out)
		{
			*out = XMMatrixTranslation(position.x, position.y, position.z);
		}
		void bcMatrix4x4::Decompose(bcVector3* scale, bcQuaternion* rotation, bcVector3* translation, const bcMatrix4x4& matrix, BOOL* out)
		{
			XMVECTOR scaleV;
			XMVECTOR rotationV;
			XMVECTOR translationV;

			*out = XMMatrixDecompose(&scaleV, &rotationV, &translationV, matrix);

			XMStoreFloat3(scale, scaleV);
			XMStoreFloat4(rotation, rotationV);
			XMStoreFloat3(translation, translationV);
		}
		void bcMatrix4x4::Determinant(const bcMatrix4x4& matrix, FLOAT* out)
		{
			*out = XMVectorGetX(XMMatrixDeterminant(matrix));
		}
		void bcMatrix4x4::Invert(const bcMatrix4x4& matrix, bcMatrix4x4* out)
		{
			XMVECTOR det;
		
			*out = XMMatrixInverse(&det, matrix);
		}
		void bcMatrix4x4::Lerp(const bcMatrix4x4& matrix1, const bcMatrix4x4& matrix2, float amount, bcMatrix4x4* out)
		{
			out->r[0] = XMVectorLerp(matrix1.r[0], matrix2.r[0], amount);
			out->r[1] = XMVectorLerp(matrix1.r[1], matrix2.r[1], amount);
			out->r[2] = XMVectorLerp(matrix1.r[2], matrix2.r[2], amount);
			out->r[3] = XMVectorLerp(matrix1.r[3], matrix2.r[3], amount);
		}
		void bcMatrix4x4::Transpose(const bcMatrix4x4& matrix, bcMatrix4x4* out)
		{
			*out = XMMatrixTranspose(matrix);
		}

		void bcMatrix4x4::ForwardVec(const bcMatrix4x4& matrix, bcVector3* out)
		{
			 *out = bcVector3(matrix._31, matrix._32, matrix._33);
		}
		void bcMatrix4x4::BackVec(const bcMatrix4x4& matrix, bcVector3* out)
		{
			*out = bcVector3(-matrix._31, -matrix._32, -matrix._33);
		}
		void bcMatrix4x4::RightVec(const bcMatrix4x4& matrix, bcVector3* out)
		{
			*out = bcVector3(matrix._11, matrix._12, matrix._13);
		}
		void bcMatrix4x4::LeftVec(const bcMatrix4x4& matrix, bcVector3* out)
		{
			*out = bcVector3(-matrix._11, -matrix._12, -matrix._13);
		}
		void bcMatrix4x4::UpVec(const bcMatrix4x4& matrix, bcVector3* out)
		{
			*out = bcVector3(matrix._21, matrix._22, matrix._23);
		}
		void bcMatrix4x4::DownVec(const bcMatrix4x4& matrix, bcVector3* out)
		{
			*out = bcVector3(-matrix._21, -matrix._22, -matrix._23);
		}
		void bcMatrix4x4::TranslationVec(const bcMatrix4x4& matrix, bcVector3* out)
		{
			*out = bcVector3(matrix._41, matrix._42, matrix._43);
		}
		void bcMatrix4x4::SetTranslationVec(bcMatrix4x4& matrix, const bcVector3& translation)
		{
			matrix._41 = translation.x;
			matrix._42 = translation.y;
			matrix._43 = translation.z;
		}
#pragma endregion

	// == bcQuaternion =======================================================
#pragma region quaternion
		bcQuaternion::bcQuaternion( )
		{
		}
		bcQuaternion::bcQuaternion( const bcQuaternion& quaternion)
		{
			this->x = quaternion.x;
			this->y = quaternion.y;
			this->z = quaternion.z;
			this->w = quaternion.w;
		}

		float bcQuaternion::Lenght( )
		{
			return XMVectorGetX(XMQuaternionLength(XMLoadFloat4(this)));
		}
		float bcQuaternion::LenghtSq( )
		{
			return XMVectorGetX(XMQuaternionLengthSq(XMLoadFloat4(&*this)));
		}
		void bcQuaternion::Normalize()
		{
			XMVECTOR qV = XMLoadFloat4(this);
			qV = XMQuaternionNormalize(qV);

			XMStoreFloat4(this, qV);
		}
	
		bcQuaternion& bcQuaternion::operator= (const bcQuaternion& quaternion)
		{
			this->x = quaternion.x;
			this->y = quaternion.y;
			this->z = quaternion.z;
			this->w = quaternion.w;
			return *this;
		}

		// comparison
		bool bcQuaternion::operator== ( const bcQuaternion& quaternion ) const
		{
			return XMQuaternionEqual(XMLoadFloat4(this), XMLoadFloat4(&quaternion));
		}
		bool bcQuaternion::operator!= ( const bcQuaternion& quaternion ) const
		{
			return !XMQuaternionEqual(XMLoadFloat4(this), XMLoadFloat4(&quaternion));
		}

		// arithmetic operations
		//bcQuaternion bcQuaternion::operator+ ( const bcQuaternion& quaternion ) const;
		//bcQuaternion bcQuaternion::operator- ( const bcQuaternion& quaternion ) const;
		bcQuaternion bcQuaternion::operator* ( const bcQuaternion& quaternion ) const
		{
			XMVECTOR v1 = XMLoadFloat4(this);
			XMVECTOR v2 = XMLoadFloat4(&quaternion);
			v1 = XMQuaternionMultiply(v1, v2);

			bcQuaternion output;
			XMStoreFloat4(&output, v1);
			return output;
		}
		//bcQuaternion bcQuaternion::operator* ( float fScalar ) const;
		//bcQuaternion bcQuaternion::operator/ ( const bcQuaternion& quaternion ) const;
		//bcQuaternion bcQuaternion::operator- ( ) const;

		// arithmetic updates
		//bcQuaternion& bcQuaternion::operator+= ( const bcQuaternion& quaternion );
		//bcQuaternion& bcQuaternion::operator-= ( const bcQuaternion& quaternion );
		bcQuaternion& bcQuaternion::operator*= ( const bcQuaternion& quaternion )
		{
			XMVECTOR v1 = XMLoadFloat4(this);
			XMVECTOR v2 = XMLoadFloat4(&quaternion);
			v1 = XMQuaternionMultiply(v1, v2);

			XMStoreFloat4(this, v1);
			return *this;
		}
		//bcQuaternion& bcQuaternion::operator*= ( float fScalar );
		//bcQuaternion& bcQuaternion::operator/= ( const bcQuaternion& quaternion );

#pragma region Commented
		////bcQuaternion bcQuaternion::Add(const bcQuaternion& quaternion1, const bcQuaternion& quaternion2);
		////bcQuaternion bcQuaternion::Divide(const bcQuaternion& quaternion1, const bcQuaternion& quaternion2);
		////bcQuaternion bcQuaternion::Multiply(const bcQuaternion& quaternion1, float scaleFactor);
		//bcQuaternion bcQuaternion::Multiply(const bcQuaternion& quaternion1, const bcQuaternion& quaternion2)
		//{
		//	XMVECTOR v1 = XMLoadFloat4(&quaternion1);
		//	XMVECTOR v2 = XMLoadFloat4(&quaternion2);
		//	v1 = XMQuaternionMultiply(v1, v2);

		//	bcQuaternion output;
		//	XMStoreFloat4(&output, v1);
		//	return output;
		//}
		////bcQuaternion bcQuaternion::Negate(const bcQuaternion& quaternion);
		////bcQuaternion bcQuaternion::Subtract(const bcQuaternion& quaternion1, const bcQuaternion& quaternion2);
		//bcQuaternion bcQuaternion::Conjugate(const bcQuaternion& value)
		//{
		//	XMVECTOR v1 = XMLoadFloat4(&value);
		//	v1 = XMQuaternionConjugate(v1);

		//	bcQuaternion output;
		//	XMStoreFloat4(&output, v1);
		//	return output;
		//}
		//bcQuaternion bcQuaternion::CreateFromAxisAngle(const bcVector3& axis, float angle)
		//{
		//	XMVECTOR v1 = XMLoadFloat3(&axis);
		//	v1 = XMQuaternionRotationAxis(v1, angle);

		//	bcQuaternion output;
		//	XMStoreFloat4(&output, v1);
		//	return output;
		//}
		//bcQuaternion bcQuaternion::CreateFromRotationMatrix(const bcMatrix4x4& matrix)
		//{
		//	XMVECTOR v1;
		//	v1 = XMQuaternionRotationMatrix(matrix);

		//	bcQuaternion output;
		//	XMStoreFloat4(&output, v1);
		//	return output;
		//}
		//bcQuaternion bcQuaternion::CreateFromNormalAngle(const bcVector3& normal, float angle)
		//{
		//	XMVECTOR v1 = XMLoadFloat3(&normal);
		//	v1 = XMQuaternionRotationNormal(v1, angle);

		//	bcQuaternion output;
		//	XMStoreFloat4(&output, v1);
		//	return output;
		//}
		//bcQuaternion bcQuaternion::CreateFromYawPitchRoll(float yaw, float pitch, float roll)
		//{
		//	XMVECTOR v1;
		//	v1 = XMQuaternionRotationRollPitchYaw(pitch, yaw, roll);

		//	bcQuaternion output;
		//	XMStoreFloat4(&output, v1);
		//	return output;
		//}
		//float bcQuaternion::Dot(const bcQuaternion& quaternion1, const bcQuaternion& quaternion2)
		//{
		//	XMVECTOR v1 = XMLoadFloat4(&quaternion1);
		//	XMVECTOR v2 = XMLoadFloat4(&quaternion2);
		//	v1 = XMQuaternionDot(v1, v2);

		//	return XMVectorGetX(v1);
		//}
		//bcQuaternion bcQuaternion::Inverse(const bcQuaternion& quaternion)
		//{
		//	XMVECTOR v1 = XMLoadFloat4(&quaternion);
		//	v1 = XMQuaternionInverse(v1);

		//	bcQuaternion output;
		//	XMStoreFloat4(&output, v1);
		//	return output;
		//}
		////bcQuaternion bcQuaternion::Lerp(const bcQuaternion& quaternion1, const bcQuaternion& quaternion2, float amount);
		//bcQuaternion bcQuaternion::Normalize(const bcQuaternion& quaternion)
		//{
		//	XMVECTOR v1 = XMLoadFloat4(&quaternion);
		//	v1 = XMQuaternionNormalize(v1);

		//	bcQuaternion output;
		//	XMStoreFloat4(&output, v1);
		//	return output;
		//}
		//bcQuaternion bcQuaternion::Slerp(const bcQuaternion& quaternion1, const bcQuaternion& quaternion2, float amount)
		//{
		//	XMVECTOR v1 = XMLoadFloat4(&quaternion1);
		//	XMVECTOR v2 = XMLoadFloat4(&quaternion2);
		//	v1 = XMQuaternionSlerp(v1, v2, amount);

		//	bcQuaternion output;
		//	XMStoreFloat4(&output, v1);
		//	return output;
		//}
#pragma endregion

		//void bcQuaternion::Add(const bcQuaternion& quaternion1, const bcQuaternion& quaternion2, bcQuaternion* out);
		//void bcQuaternion::Divide(const bcQuaternion& quaternion1, const bcQuaternion& quaternion2, bcQuaternion* out);
		//void bcQuaternion::Multiply(const bcQuaternion& quaternion1, float scaleFactor, bcQuaternion* out);
		void bcQuaternion::Multiply(const bcQuaternion& quaternion1, const bcQuaternion& quaternion2, bcQuaternion* out)
		{
			XMVECTOR v1 = XMLoadFloat4(&quaternion1);
			XMVECTOR v2 = XMLoadFloat4(&quaternion2);
			v1 = XMQuaternionMultiply(v1, v2);

			XMStoreFloat4(out, v1);
		}
		//void bcQuaternion::Negate(const bcQuaternion& quaternion, bcQuaternion* out);
		//void bcQuaternion::Subtract(const bcQuaternion& quaternion1, const bcQuaternion& quaternion2, bcQuaternion* out);
		void bcQuaternion::Conjugate(const bcQuaternion& value, bcQuaternion* out)
		{
			XMVECTOR v1 = XMLoadFloat4(&value);
			v1 = XMQuaternionConjugate(v1);

			XMStoreFloat4(out, v1);
		}
		void bcQuaternion::CreateFromAxisAngle(const bcVector3& axis, float angle, bcQuaternion* out)
		{
			XMVECTOR v1 = XMLoadFloat3(&axis);
			v1 = XMQuaternionRotationAxis(v1, angle);

			XMStoreFloat4(out, v1);
		}
		void bcQuaternion::CreateFromRotationMatrix(const bcMatrix4x4& matrix, bcQuaternion* out)
		{
			XMVECTOR v1;
			v1 = XMQuaternionRotationMatrix(matrix);

			XMStoreFloat4(out, v1);
		}
		void bcQuaternion::CreateFromNormalAngle(const bcVector3& normal, float angle, bcQuaternion* out)
		{
			XMVECTOR v1 = XMLoadFloat3(&normal);
			v1 = XMQuaternionRotationNormal(v1, angle);

			XMStoreFloat4(out, v1);
		}
		void bcQuaternion::CreateFromYawPitchRoll(float yaw, float pitch, float roll, bcQuaternion* out)
		{
			XMVECTOR v1;
			v1 = XMQuaternionRotationRollPitchYaw(pitch, yaw, roll);

			XMStoreFloat4(out, v1);
		}
		void bcQuaternion::Dot(const bcQuaternion& quaternion1, const bcQuaternion& quaternion2, FLOAT* out)
		{
			XMVECTOR v1 = XMLoadFloat4(&quaternion1);
			XMVECTOR v2 = XMLoadFloat4(&quaternion2);
			v1 = XMQuaternionDot(v1, v2);

			*out = XMVectorGetX(v1);
		}
		void bcQuaternion::Inverse(const bcQuaternion& quaternion, bcQuaternion* out)
		{
			XMVECTOR v1 = XMLoadFloat4(&quaternion);
			v1 = XMQuaternionInverse(v1);

			XMStoreFloat4(out, v1);
		}
		//void bcQuaternion::Lerp(const bcQuaternion& quaternion1, const bcQuaternion& quaternion2, float amount);
		void bcQuaternion::Normalize(const bcQuaternion& quaternion, bcQuaternion* out)
		{
			XMVECTOR v1 = XMLoadFloat4(&quaternion);
			v1 = XMQuaternionNormalize(v1);

			XMStoreFloat4(out, v1);
		}
		void bcQuaternion::Slerp(const bcQuaternion& quaternion1, const bcQuaternion& quaternion2, float amount, bcQuaternion* out)
		{
			XMVECTOR v1 = XMLoadFloat4(&quaternion1);
			XMVECTOR v2 = XMLoadFloat4(&quaternion2);
			v1 = XMQuaternionSlerp(v1, v2, amount);

			XMStoreFloat4(out, v1);
		}
		void bcQuaternion::ToAxisAngle(bcVector3* axis, float* angle, const bcQuaternion& quaternion)
		{
			XMVECTOR v1 = XMLoadFloat4(&quaternion);
			XMVECTOR v2 = XMLoadFloat3(axis);
			XMQuaternionToAxisAngle(&v2, angle, v1);

			XMStoreFloat3(axis, v2);
		}

		//void bcQuaternion::Add(const bcVector128& quaternion1, const bcVector128& quaternion2, bcVector128* out);
		//void bcQuaternion::Divide(const bcVector128& quaternion1, const bcVector128& quaternion2, bcVector128* out);
		//void bcQuaternion::Multiply(const bcVector128& quaternion1, float scaleFactor, bcVector128* out);
		void bcQuaternion::Multiply(const bcVector128& quaternion1, const bcVector128& quaternion2, bcVector128* out)
		{
			*out = XMQuaternionMultiply(quaternion1, quaternion2);
		}
		//void bcQuaternion::Negate(const bcVector128& quaternion, bcVector128* out);
		//void bcQuaternion::Subtract(const bcVector128& quaternion1, const bcVector128& quaternion2, bcVector128* out);
		void bcQuaternion::Conjugate(const bcVector128& value, bcVector128* out)
		{
			*out = XMQuaternionConjugate(value);
		}
		void bcQuaternion::CreateFromAxisAngle(const bcVector128& axis, float angle, bcVector128* out)
		{
			*out = XMQuaternionRotationAxis(axis, angle);
		}
		void bcQuaternion::CreateFromRotationMatrix(const bcMatrix4x4& matrix, bcVector128* out)
		{
			*out = XMQuaternionRotationMatrix(matrix);
		}
		void bcQuaternion::CreateFromNormalAngle(const bcVector128& normal, float angle, bcVector128* out)
		{
			*out = XMQuaternionRotationNormal(normal, angle);
		}
		void bcQuaternion::CreateFromYawPitchRoll(float yaw, float pitch, float roll, bcVector128* out)
		{
			*out = XMQuaternionRotationRollPitchYaw(pitch, yaw, roll);
		}
		void bcQuaternion::Dot(const bcVector128& quaternion1, const bcVector128& quaternion2, FLOAT* out)
		{
			XMVECTOR lR = XMQuaternionDot(quaternion1, quaternion2);
			*out = XMVectorGetX(lR);
		}
		void bcQuaternion::Inverse(const bcVector128& quaternion, bcVector128* out)
		{
			*out = XMQuaternionInverse(quaternion);
		}
		//void bcQuaternion::Lerp(const bcQuaternion& quaternion1, const bcQuaternion& quaternion2, float amount);
		void bcQuaternion::Normalize(const bcVector128& quaternion, bcVector128* out)
		{
			*out = XMQuaternionNormalize(quaternion);
		}
		void bcQuaternion::Slerp(const bcVector128& quaternion1, const bcVector128& quaternion2, float amount, bcVector128* out)
		{
			*out = XMQuaternionSlerp(quaternion1, quaternion2, amount);
		}
		void bcQuaternion::ToAxisAngle(bcVector128* axis, float* angle, const bcVector128& quaternion)
		{
			XMQuaternionToAxisAngle(axis, angle, quaternion);
		}
#pragma endregion
	}
}
