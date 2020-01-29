//--------------------------------------------------------------------------------
// This file is a portion of the Hieroglyph 3 Rendering Engine.  It is distributed
// under the MIT License, available in the root of this distribution and 
// at the following URL:
//
// http://www.opensource.org/licenses/mit-license.php
//
// Copyright (c) Jason Zink 
//--------------------------------------------------------------------------------

#include "Core/CorePCH.h"
#include "Core/Math/bcMatrix4f.h"

namespace black_cat
{
	namespace core
	{
		bc_matrix4f::bc_matrix4f() noexcept
			: m_entry{}
		{
		}

		bc_matrix4f::bc_matrix4f(bool p_zero) noexcept
			: m_entry{}
		{
			if (p_zero)
			{
				std::memset(m_entry, 0, 4 * 4 * sizeof(bcFLOAT));
			}
		}
		
		bc_matrix4f::bc_matrix4f(const bc_matrix4f& p_matrix) noexcept
		{
			std::memcpy(m_entry, reinterpret_cast<const void*>(&p_matrix), 16 * sizeof(bcFLOAT));
		}
		
		bc_matrix4f::bc_matrix4f(bcFLOAT p_m11, bcFLOAT p_m12, bcFLOAT p_m13, bcFLOAT p_m14,
			bcFLOAT p_m21, bcFLOAT p_m22, bcFLOAT p_m23, bcFLOAT p_m24,
			bcFLOAT p_m31, bcFLOAT p_m32, bcFLOAT p_m33, bcFLOAT p_m34,
			bcFLOAT p_m41, bcFLOAT p_m42, bcFLOAT p_m43, bcFLOAT p_m44) noexcept
		{
			m_entry[0] = p_m11;
			m_entry[1] = p_m12;
			m_entry[2] = p_m13;
			m_entry[3] = p_m14;

			m_entry[4] = p_m21;
			m_entry[5] = p_m22;
			m_entry[6] = p_m23;
			m_entry[7] = p_m24;

			m_entry[8] = p_m31;
			m_entry[9] = p_m32;
			m_entry[10] = p_m33;
			m_entry[11] = p_m34;

			m_entry[12] = p_m41;
			m_entry[13] = p_m42;
			m_entry[14] = p_m43;
			m_entry[15] = p_m44;
		}
		
		bc_matrix4f& bc_matrix4f::operator=(const bc_matrix4f& p_other) noexcept
		{
			std::memcpy(m_entry, p_other.m_entry, 16 * sizeof(bcFLOAT));
			return(*this);
		}
		
		void bc_matrix4f::rotation_x_lh(bcFLOAT p_radians) noexcept
		{
			bcFLOAT l_sin = sinf(p_radians);
			bcFLOAT l_cos = cosf(p_radians);

			m_entry[0] = 1.0f;
			m_entry[1] = 0.0f;
			m_entry[2] = 0.0f;
			m_entry[3] = 0.0f;

			m_entry[4] = 0.0f;
			m_entry[5] = l_cos;
			m_entry[6] = l_sin;
			m_entry[7] = 0.0f;

			m_entry[8] = 0.0f;
			m_entry[9] = -l_sin;
			m_entry[10] = l_cos;
			m_entry[11] = 0.0f;

			m_entry[12] = 0.0f;
			m_entry[13] = 0.0f;
			m_entry[14] = 0.0f;
			m_entry[15] = 1.0f;
		}
		
		void bc_matrix4f::rotation_y_lh(bcFLOAT p_radians) noexcept
		{
			bcFLOAT l_sin = sinf(p_radians);
			bcFLOAT l_cos = cosf(p_radians);

			m_entry[0] = l_cos;
			m_entry[1] = 0.0f;
			m_entry[2] = -l_sin;
			m_entry[3] = 0.0f;

			m_entry[4] = 0.0f;
			m_entry[5] = 1.0f;
			m_entry[6] = 0.0f;
			m_entry[7] = 0.0f;

			m_entry[8] = l_sin;
			m_entry[9] = 0.0f;
			m_entry[10] = l_cos;
			m_entry[11] = 0.0f;

			m_entry[12] = 0.0f;
			m_entry[13] = 0.0f;
			m_entry[14] = 0.0f;
			m_entry[15] = 1.0f;
		}
		
		void bc_matrix4f::rotation_z_lh(bcFLOAT p_radians) noexcept
		{
			bcFLOAT l_sin = sinf(p_radians);
			bcFLOAT l_cos = cosf(p_radians);

			m_entry[0] = l_cos;
			m_entry[1] = l_sin;
			m_entry[2] = 0.0f;
			m_entry[3] = 0.0f;

			m_entry[4] = -l_sin;
			m_entry[5] = l_cos;
			m_entry[6] = 0.0f;
			m_entry[7] = 0.0f;

			m_entry[8] = 0.0f;
			m_entry[9] = 0.0f;
			m_entry[10] = 1.0f;
			m_entry[11] = 0.0f;

			m_entry[12] = 0.0f;
			m_entry[13] = 0.0f;
			m_entry[14] = 0.0f;
			m_entry[15] = 1.0f;
		}

		void bc_matrix4f::rotation_x_rh(bcFLOAT p_radians) noexcept
		{
			rotation_x_lh(p_radians);

			m_entry[6] = -m_entry[6];
			m_entry[9] = -m_entry[9];
		}

		void bc_matrix4f::rotation_y_rh(bcFLOAT p_radians) noexcept
		{
			rotation_y_lh(p_radians);

			m_entry[2] = -m_entry[2];
			m_entry[8] = -m_entry[8];
		}

		void bc_matrix4f::rotation_z_rh(bcFLOAT p_radians) noexcept
		{
			rotation_z_lh(p_radians);

			m_entry[1] = -m_entry[1];
			m_entry[4] = -m_entry[4];
		}

		void bc_matrix4f::scale(bcFLOAT p_scale) noexcept
		{
			m_entry[0] = p_scale;
			m_entry[1] = 0.0f;
			m_entry[2] = 0.0f;
			m_entry[3] = 0.0f;

			m_entry[4] = 0.0f;
			m_entry[5] = p_scale;
			m_entry[6] = 0.0f;
			m_entry[7] = 0.0f;

			m_entry[8] = 0.0f;
			m_entry[9] = 0.0f;
			m_entry[10] = p_scale;
			m_entry[11] = 0.0f;

			m_entry[12] = 0.0f;
			m_entry[13] = 0.0f;
			m_entry[14] = 0.0f;
			m_entry[15] = 1.0f;
		}
		
		void bc_matrix4f::translate(bcFLOAT p_x, bcFLOAT p_y, bcFLOAT p_z) noexcept
		{
			m_entry[0] = 1.0f;
			m_entry[1] = 0.0f;
			m_entry[2] = 0.0f;
			m_entry[3] = 0.0f;

			m_entry[4] = 0.0f;
			m_entry[5] = 1.0f;
			m_entry[6] = 0.0f;
			m_entry[7] = 0.0f;

			m_entry[8] = 0.0f;
			m_entry[9] = 0.0f;
			m_entry[10] = 1.0f;
			m_entry[11] = 0.0f;

			m_entry[12] = p_x;
			m_entry[13] = p_y;
			m_entry[14] = p_z;
			m_entry[15] = 1.0f;
		}

		void bc_matrix4f::translate(const bc_vector3f& p_translate) noexcept
		{
			translate(p_translate.x, p_translate.y, p_translate.z);
		}

		bc_vector3f bc_matrix4f::get_basis_x() const noexcept
		{
			bc_vector3f l_basis;

			for (bcINT i = 0; i < 3; i++)
				l_basis[i] = m_entry[I(0, i)];

			return(l_basis);
		}
		
		bc_vector3f bc_matrix4f::get_basis_y() const noexcept
		{
			bc_vector3f l_basis;

			for (bcINT i = 0; i < 3; i++)
				l_basis[i] = m_entry[I(1, i)];

			return(l_basis);
		}
		
		bc_vector3f bc_matrix4f::get_basis_z() const noexcept
		{
			bc_vector3f l_basis;

			for (bcINT i = 0; i < 3; i++)
				l_basis[i] = m_entry[I(2, i)];

			return(l_basis);
		}
		
		bc_vector3f bc_matrix4f::get_translation() const noexcept
		{
			bc_vector3f l_pos;

			for (bcINT i = 0; i < 3; i++)
				l_pos[i] = m_entry[I(3, i)];

			return(l_pos);
		}

		bc_matrix3f bc_matrix4f::get_rotation() const noexcept
		{
			bc_matrix3f mRet;

			mRet.set_row(0, get_basis_x());
			mRet.set_row(1, get_basis_y());
			mRet.set_row(2, get_basis_z());

			return(mRet);
		}
				
		void bc_matrix4f::set_rotation(const bc_matrix3f& p_rot) noexcept
		{
			for (bcINT i = 0; i < 3; i++)
				for (bcINT j = 0; j < 3; j++)
					m_entry[I(i, j)] = p_rot[(3 * i + j)];
		}

		void bc_matrix4f::set_translation(const bc_vector3f& p_trans) noexcept
		{
			for (bcINT i = 0; i < 3; i++)
				m_entry[I(3, i)] = p_trans[i];
		}

		bc_matrix4f bc_matrix4f::inverse() const noexcept
		{
			bcFLOAT l_a0 = m_entry[0] * m_entry[5] - m_entry[1] * m_entry[4];
			bcFLOAT l_a1 = m_entry[0] * m_entry[6] - m_entry[2] * m_entry[4];
			bcFLOAT l_a2 = m_entry[0] * m_entry[7] - m_entry[3] * m_entry[4];
			bcFLOAT l_a3 = m_entry[1] * m_entry[6] - m_entry[2] * m_entry[5];
			bcFLOAT l_a4 = m_entry[1] * m_entry[7] - m_entry[3] * m_entry[5];
			bcFLOAT l_a5 = m_entry[2] * m_entry[7] - m_entry[3] * m_entry[6];
			bcFLOAT l_b0 = m_entry[8] * m_entry[13] - m_entry[9] * m_entry[12];
			bcFLOAT l_b1 = m_entry[8] * m_entry[14] - m_entry[10] * m_entry[12];
			bcFLOAT l_b2 = m_entry[8] * m_entry[15] - m_entry[11] * m_entry[12];
			bcFLOAT l_b3 = m_entry[9] * m_entry[14] - m_entry[10] * m_entry[13];
			bcFLOAT l_b4 = m_entry[9] * m_entry[15] - m_entry[11] * m_entry[13];
			bcFLOAT l_b5 = m_entry[10] * m_entry[15] - m_entry[11] * m_entry[14];

			bcFLOAT l_det = l_a0*l_b5 - l_a1*l_b4 + l_a2*l_b3 + l_a3*l_b2 - l_a4*l_b1 + l_a5*l_b0;
			//if ( Math<Real>::FAbs(fDet) <= Math<Real>::EPSILON )
			//    return Matrix4::ZERO;

			bc_matrix4f l_inv;
			l_inv(0, 0) = +m_entry[5] * l_b5 - m_entry[6] * l_b4 + m_entry[7] * l_b3;
			l_inv(1, 0) = -m_entry[4] * l_b5 + m_entry[6] * l_b2 - m_entry[7] * l_b1;
			l_inv(2, 0) = +m_entry[4] * l_b4 - m_entry[5] * l_b2 + m_entry[7] * l_b0;
			l_inv(3, 0) = -m_entry[4] * l_b3 + m_entry[5] * l_b1 - m_entry[6] * l_b0;
			l_inv(0, 1) = -m_entry[1] * l_b5 + m_entry[2] * l_b4 - m_entry[3] * l_b3;
			l_inv(1, 1) = +m_entry[0] * l_b5 - m_entry[2] * l_b2 + m_entry[3] * l_b1;
			l_inv(2, 1) = -m_entry[0] * l_b4 + m_entry[1] * l_b2 - m_entry[3] * l_b0;
			l_inv(3, 1) = +m_entry[0] * l_b3 - m_entry[1] * l_b1 + m_entry[2] * l_b0;
			l_inv(0, 2) = +m_entry[13] * l_a5 - m_entry[14] * l_a4 + m_entry[15] * l_a3;
			l_inv(1, 2) = -m_entry[12] * l_a5 + m_entry[14] * l_a2 - m_entry[15] * l_a1;
			l_inv(2, 2) = +m_entry[12] * l_a4 - m_entry[13] * l_a2 + m_entry[15] * l_a0;
			l_inv(3, 2) = -m_entry[12] * l_a3 + m_entry[13] * l_a1 - m_entry[14] * l_a0;
			l_inv(0, 3) = -m_entry[9] * l_a5 + m_entry[10] * l_a4 - m_entry[11] * l_a3;
			l_inv(1, 3) = +m_entry[8] * l_a5 - m_entry[10] * l_a2 + m_entry[11] * l_a1;
			l_inv(2, 3) = -m_entry[8] * l_a4 + m_entry[9] * l_a2 - m_entry[11] * l_a0;
			l_inv(3, 3) = +m_entry[8] * l_a3 - m_entry[9] * l_a1 + m_entry[10] * l_a0;

			bcFLOAT l_inv_det = static_cast< bcFLOAT >(1.0) / l_det;
			for (bcINT l_row = 0; l_row < 4; l_row++)
			{
				for (bcINT l_col = 0; l_col < 4; l_col++)
					l_inv(l_row, l_col) *= l_inv_det;
			}

			return(l_inv);
		}
		
		void bc_matrix4f::make_zero() noexcept
		{
			std::memset(m_entry, 0, 4 * 4 * sizeof(bcFLOAT));
		}

		void bc_matrix4f::make_identity() noexcept
		{
			for (bcINT l_row = 0; l_row < 4; l_row++)
			{
				for (bcINT l_col = 0; l_col < 4; l_col++)
				{
					if (l_row == l_col)
						m_entry[I(l_row, l_col)] = 1.0f;
					else
						m_entry[I(l_row, l_col)] = 0.0f;
				}
			}
		}

		void bc_matrix4f::make_transpose() noexcept
		{
			bc_matrix4f l_transpose;

			for (bcINT l_row = 0; l_row < 4; l_row++)
			{
				for (bcINT l_col = 0; l_col < 4; l_col++)
					l_transpose.m_entry[I(l_row, l_col)] = m_entry[I(l_col, l_row)];
			}

			std::memcpy(m_entry, l_transpose.m_entry, 4 * 4 * sizeof(bcFLOAT));
		}

		bc_matrix4f bc_matrix4f::transpose() const noexcept
		{
			bc_matrix4f l_transpose;

			for (bcINT l_row = 0; l_row < 4; l_row++)
			{
				for (bcINT l_col = 0; l_col < 4; l_col++)
					l_transpose.m_entry[I(l_row, l_col)] = m_entry[I(l_col, l_row)];
			}

			return(l_transpose);

		}

		void bc_matrix4f::set_row(bcINT p_row, const bc_vector4f& p_vector) noexcept
		{
			for (bcINT l_col = 0; l_col < 4; l_col++)
				m_entry[I(p_row, l_col)] = p_vector[l_col];
		}

		void bc_matrix4f::set_row(bcINT p_row, const bc_vector3f& p_vector) noexcept
		{
			for (bcINT l_col = 0; l_col < 3; l_col++)
				m_entry[I(p_row, l_col)] = p_vector[l_col];
		}

		bc_vector4f bc_matrix4f::get_row(bcINT p_row) const noexcept
		{
			bc_vector4f l_row;
			for (bcINT iCol = 0; iCol < 4; iCol++)
				l_row[iCol] = m_entry[I(p_row, iCol)];

			return(l_row);
		}

		void bc_matrix4f::set_column(bcINT p_col, const bc_vector4f& p_vector) noexcept
		{
			for (bcINT l_row = 0; l_row < 4; l_row++)
				m_entry[I(l_row, p_col)] = p_vector[l_row];
		}

		bc_vector4f bc_matrix4f::get_column(bcINT p_col) const noexcept
		{
			bc_vector4f l_col;
			for (bcINT l_row = 0; l_row < 4; l_row++)
				l_col[l_row] = m_entry[I(l_row, p_col)];

			return(l_col);
		}

		bcFLOAT bc_matrix4f::operator() (bcINT p_row, bcINT p_col) const noexcept
		{
			return(m_entry[I(p_row, p_col)]);
		}

		bcFLOAT& bc_matrix4f::operator() (bcINT p_row, bcINT p_col) noexcept
		{
			return(m_entry[I(p_row, p_col)]);
		}

		bcFLOAT bc_matrix4f::operator[] (bcINT p_pos) const noexcept
		{
			return(m_entry[p_pos]);
		}
		
		bcFLOAT& bc_matrix4f::operator[] (bcINT p_pos) noexcept
		{
			return(m_entry[p_pos]);
		}
		
		bool bc_matrix4f::operator== (const bc_matrix4f& p_other) const noexcept
		{
			return(std::memcmp(m_entry, p_other.m_entry, 4 * 4 * sizeof(bcFLOAT)) == 0);
		}
		
		bool bc_matrix4f::operator!= (const bc_matrix4f& p_other) const noexcept
		{
			return(std::memcmp(m_entry, p_other.m_entry, 4 * 4 * sizeof(bcFLOAT)) != 0);
		}
		
		bc_matrix4f bc_matrix4f::operator* (const bc_matrix4f& p_other) const noexcept
		{
			bc_matrix4f l_prod;

			for (bcINT l_row = 0; l_row < 4; l_row++)
			{
				for (bcINT l_col = 0; l_col < 4; l_col++)
				{
					bcINT i = I(l_row, l_col);
					l_prod.m_entry[i] = 0.0f;
					for (bcINT iMid = 0; iMid < 4; iMid++)
					{
						l_prod.m_entry[i] +=
							m_entry[I(l_row, iMid)] * p_other.m_entry[I(iMid, l_col)];
					}
				}
			}
			return(l_prod);
		}
		
		bc_matrix4f bc_matrix4f::operator+ (const bc_matrix4f& p_other) const noexcept
		{
			bc_matrix4f l_sum;

			for (bcINT i = 0; i < 4 * 4; i++)
				l_sum.m_entry[i] = m_entry[i] + p_other.m_entry[i];

			return(l_sum);
		}
		
		bc_matrix4f bc_matrix4f::operator- (const bc_matrix4f& p_other) const noexcept
		{
			bc_matrix4f l_diff;

			for (bcINT i = 0; i < 4 * 4; i++)
				l_diff.m_entry[i] = m_entry[i] - p_other.m_entry[i];

			return(l_diff);
		}
		
		bc_matrix4f bc_matrix4f::operator* (bcFLOAT p_scalar) const noexcept
		{
			bc_matrix4f l_prod;

			for (bcINT i = 0; i < 4 * 4; i++)
				l_prod.m_entry[i] = m_entry[i] * p_scalar;

			return(l_prod);
		}
		
		bc_matrix4f bc_matrix4f::operator/ (bcFLOAT p_scalar) const noexcept
		{
			bc_matrix4f l_quot;
			if (p_scalar != 0.0f)
			{
				bcFLOAT l_inv_scalar = 1.0f / p_scalar;
				for (bcINT i = 0; i < 4 * 4; i++)
					l_quot.m_entry[i] = m_entry[i] * l_inv_scalar;
			}
			else
			{
				for (bcINT i = 0; i < 4 * 4; i++)
					l_quot.m_entry[i] = 0;
			}

			return(l_quot);
		}
		
		bc_matrix4f bc_matrix4f::operator- () const noexcept
		{
			bc_matrix4f l_neg;

			for (bcINT i = 0; i < 4 * 4; i++)
				l_neg.m_entry[i] = -m_entry[i];

			return(l_neg);
		}
		
		bc_matrix4f& bc_matrix4f::operator+= (const bc_matrix4f& p_other) noexcept
		{
			for (bcINT i = 0; i < 4 * 4; i++)
				m_entry[i] += p_other.m_entry[i];

			return(*this);
		}
		
		bc_matrix4f& bc_matrix4f::operator-= (const bc_matrix4f& p_other) noexcept
		{
			for (bcINT i = 0; i < 4 * 4; i++)
				m_entry[i] -= p_other.m_entry[i];

			return(*this);
		}
		
		bc_matrix4f& bc_matrix4f::operator*= (bcFLOAT p_scalar) noexcept
		{
			for (bcINT i = 0; i < 4 * 4; i++)
				m_entry[i] *= p_scalar;

			return(*this);
		}
		
		bc_matrix4f& bc_matrix4f::operator*= (const bc_matrix4f& p_other) noexcept
		{
			bc_matrix4f l_prod = *this;

			for (bcINT l_row = 0; l_row < 4; l_row++)
			{
				for (bcINT l_col = 0; l_col < 4; l_col++)
				{
					bcINT i = I(l_row, l_col);
					m_entry[i] = 0.0f;
					for (bcINT l_mid = 0; l_mid < 4; l_mid++)
					{
						m_entry[i] +=
							l_prod.m_entry[I(l_row, l_mid)] * p_other.m_entry[I(l_mid, l_col)];
					}
				}
			}
			return(*this);
		}
		
		bc_matrix4f& bc_matrix4f::operator/= (bcFLOAT p_scalar) noexcept
		{
			if (p_scalar != 0.0f)
			{
				bcFLOAT l_inv_scalar = 1.0f / p_scalar;
				for (bcINT i = 0; i < 4 * 4; i++)
					m_entry[i] *= l_inv_scalar;
			}
			else
			{
				for (bcINT i = 0; i < 4 * 4; i++)
					m_entry[i] = 0;
			}

			return(*this);
		}
		
		bc_vector4f bc_matrix4f::operator* (const bc_vector4f& p_vector) const noexcept
		{
			bc_vector4f l_prod;
			for (bcINT l_col = 0; l_col < 4; l_col++)
			{
				l_prod[l_col] = 0.0f;
				for (bcINT l_row = 0; l_row < 4; l_row++)
					l_prod[l_col] += m_entry[I(l_row, l_col)] * p_vector[l_row];
			}
			return(l_prod);
		}

		bc_matrix4f bc_matrix4f::rotation_matrix_xyz_lh(bcFLOAT p_radians_x, bcFLOAT p_radians_y, bcFLOAT p_radians_z) noexcept
		{
			return bc_matrix4f::rotation_matrix_z_lh(p_radians_z) * bc_matrix4f::rotation_matrix_x_lh(p_radians_x) * bc_matrix4f::rotation_matrix_y_lh(p_radians_y);
		}

		bc_matrix4f bc_matrix4f::rotation_matrix_x_lh(bcFLOAT p_radians) noexcept
		{
			bc_matrix4f l_ret;
			l_ret.rotation_x_lh(p_radians);

			return l_ret;
		}

		bc_matrix4f bc_matrix4f::rotation_matrix_y_lh(bcFLOAT p_radians) noexcept
		{
			bc_matrix4f l_ret;
			l_ret.rotation_y_lh(p_radians);

			return l_ret;
		}

		bc_matrix4f bc_matrix4f::rotation_matrix_z_lh(bcFLOAT p_radians) noexcept
		{
			bc_matrix4f l_ret;
			l_ret.rotation_z_lh(p_radians);

			return l_ret;
		}

		bc_matrix4f bc_matrix4f::rotation_matrix_xyz_rh(bcFLOAT p_radians_x, bcFLOAT p_radians_y, bcFLOAT p_radians_z) noexcept
		{
			return bc_matrix4f::rotation_matrix_z_rh(p_radians_z) * bc_matrix4f::rotation_matrix_x_rh(p_radians_x) * bc_matrix4f::rotation_matrix_y_rh(p_radians_y);
		}

		bc_matrix4f bc_matrix4f::rotation_matrix_x_rh(bcFLOAT p_radians) noexcept
		{
			bc_matrix4f l_ret;
			l_ret.rotation_x_rh(p_radians);

			return l_ret;
		}

		bc_matrix4f bc_matrix4f::rotation_matrix_y_rh(bcFLOAT p_radians) noexcept
		{
			bc_matrix4f l_ret;
			l_ret.rotation_y_rh(p_radians);

			return l_ret;
		}

		bc_matrix4f bc_matrix4f::rotation_matrix_z_rh(bcFLOAT p_radians) noexcept
		{
			bc_matrix4f l_ret;
			l_ret.rotation_z_rh(p_radians);

			return l_ret;
		}

		bc_matrix4f bc_matrix4f::scale_matrix(bcFLOAT p_scale) noexcept
		{
			bc_matrix4f l_ret;
			l_ret.scale(p_scale);
			return(l_ret);
		}

		bc_matrix4f bc_matrix4f::scale_matrix(const bc_vector3f& p_scale) noexcept
		{
			bc_matrix4f l_ret;

			l_ret.m_entry[0] = p_scale.x;
			l_ret.m_entry[1] = 0.0f;
			l_ret.m_entry[2] = 0.0f;
			l_ret.m_entry[3] = 0.0f;

			l_ret.m_entry[4] = 0.0f;
			l_ret.m_entry[5] = p_scale.y;
			l_ret.m_entry[6] = 0.0f;
			l_ret.m_entry[7] = 0.0f;

			l_ret.m_entry[8] = 0.0f;
			l_ret.m_entry[9] = 0.0f;
			l_ret.m_entry[10] = p_scale.z;
			l_ret.m_entry[11] = 0.0f;

			l_ret.m_entry[12] = 0.0f;
			l_ret.m_entry[13] = 0.0f;
			l_ret.m_entry[14] = 0.0f;
			l_ret.m_entry[15] = 1.0f;

			return(l_ret);
		}

		bc_matrix4f bc_matrix4f::scale_matrix_xyz(bcFLOAT p_x, bcFLOAT p_y, bcFLOAT p_z) noexcept
		{
			bc_matrix4f l_ret;

			l_ret.m_entry[0] = p_x;
			l_ret.m_entry[1] = 0.0f;
			l_ret.m_entry[2] = 0.0f;
			l_ret.m_entry[3] = 0.0f;

			l_ret.m_entry[4] = 0.0f;
			l_ret.m_entry[5] = p_y;
			l_ret.m_entry[6] = 0.0f;
			l_ret.m_entry[7] = 0.0f;

			l_ret.m_entry[8] = 0.0f;
			l_ret.m_entry[9] = 0.0f;
			l_ret.m_entry[10] = p_z;
			l_ret.m_entry[11] = 0.0f;

			l_ret.m_entry[12] = 0.0f;
			l_ret.m_entry[13] = 0.0f;
			l_ret.m_entry[14] = 0.0f;
			l_ret.m_entry[15] = 1.0f;

			return(l_ret);
		}

		bc_matrix4f bc_matrix4f::translation_matrix(bcFLOAT p_x, bcFLOAT p_y, bcFLOAT p_z) noexcept
		{
			bc_matrix4f l_ret;
			l_ret.translate(p_x, p_y, p_z);
			return(l_ret);
		}

		bc_matrix4f bc_matrix4f::translation_matrix(const bc_vector3f& p_translation) noexcept
		{
			bc_matrix4f l_ret;
			l_ret.translate(p_translation);
			return(l_ret);
		}

		bc_matrix4f bc_matrix4f::look_at_matrix_lh(const bc_vector3f& p_eye, const bc_vector3f& p_at, const bc_vector3f& p_up) noexcept
		{
			// This method is based on the method of the same name from the D3DX library.

			bc_matrix4f l_ret;

			bc_vector3f l_zaxis = p_at - p_eye;
			l_zaxis.normalize();

			bc_vector3f l_xaxis = p_up.cross(l_zaxis);
			l_xaxis.normalize();

			bc_vector3f l_yaxis = l_zaxis.cross(l_xaxis);

			l_ret.m_entry[0] = l_xaxis.x;
			l_ret.m_entry[1] = l_yaxis.x;
			l_ret.m_entry[2] = l_zaxis.x;
			l_ret.m_entry[3] = 0.0f;

			l_ret.m_entry[4] = l_xaxis.y;
			l_ret.m_entry[5] = l_yaxis.y;
			l_ret.m_entry[6] = l_zaxis.y;
			l_ret.m_entry[7] = 0.0f;

			l_ret.m_entry[8] = l_xaxis.z;
			l_ret.m_entry[9] = l_yaxis.z;
			l_ret.m_entry[10] = l_zaxis.z;
			l_ret.m_entry[11] = 0.0f;

			l_ret.m_entry[12] = -l_xaxis.dot(p_eye);
			l_ret.m_entry[13] = -l_yaxis.dot(p_eye);
			l_ret.m_entry[14] = -l_zaxis.dot(p_eye);
			l_ret.m_entry[15] = 1.0f;

			return l_ret;
		}

		bc_matrix4f bc_matrix4f::perspective_fov_matrix_lh(bcFLOAT p_fovy, bcFLOAT p_aspect, bcFLOAT p_zn, bcFLOAT p_zf) noexcept
		{
			// This method is based on the method of the same name from the D3DX library.

			bc_matrix4f l_ret;

			bcFLOAT l_tan_y = tan(p_fovy / 2.0f);
			if (0.0f == l_tan_y) l_tan_y = 0.001f;
			bcFLOAT l_y_scale = 1.0f / l_tan_y;

			if (0.0f == p_aspect) p_aspect = 0.001f;
			bcFLOAT l_x_scale = l_y_scale / p_aspect;

			l_ret.m_entry[0] = l_x_scale;
			l_ret.m_entry[1] = 0.0f;
			l_ret.m_entry[2] = 0.0f;
			l_ret.m_entry[3] = 0.0f;

			l_ret.m_entry[4] = 0.0f;
			l_ret.m_entry[5] = l_y_scale;
			l_ret.m_entry[6] = 0.0f;
			l_ret.m_entry[7] = 0.0f;

			l_ret.m_entry[8] = 0.0f;
			l_ret.m_entry[9] = 0.0f;
			l_ret.m_entry[10] = p_zf / (p_zf - p_zn);
			l_ret.m_entry[11] = 1.0f;

			l_ret.m_entry[12] = 0.0f;
			l_ret.m_entry[13] = 0.0f;
			l_ret.m_entry[14] = -p_zn*p_zf / (p_zf - p_zn);
			l_ret.m_entry[15] = 0.0f;

			return l_ret;
		}

		bc_matrix4f bc_matrix4f::orthographic_matrix_lh(bcFLOAT p_zn, bcFLOAT p_zf, bcFLOAT p_width, bcFLOAT p_height) noexcept
		{
			// This method is based on the method of the same name from the D3DX library.

			bc_matrix4f l_ret;

			if (p_zn == p_zf) p_zf = p_zn + 0.1f;
			if (p_width <= 0.0f) p_width = 1.0f;
			if (p_height <= 0.0f) p_height = 1.0f;

			l_ret.m_entry[0] = 2.0f / p_width;
			l_ret.m_entry[1] = 0.0f;
			l_ret.m_entry[2] = 0.0f;
			l_ret.m_entry[3] = 0.0f;

			l_ret.m_entry[4] = 0.0f;
			l_ret.m_entry[5] = 2.0f / p_height;
			l_ret.m_entry[6] = 0.0f;
			l_ret.m_entry[7] = 0.0f;

			l_ret.m_entry[8] = 0.0f;
			l_ret.m_entry[9] = 0.0f;
			l_ret.m_entry[10] = 1.0f / (p_zf - p_zn);
			l_ret.m_entry[11] = 0.0f;

			l_ret.m_entry[12] = 0.0f;
			l_ret.m_entry[13] = 0.0f;
			l_ret.m_entry[14] = -p_zn / (p_zf - p_zn);
			l_ret.m_entry[15] = 1.0f;

			return l_ret;
		}

		bc_matrix4f bc_matrix4f::look_at_matrix_rh(const bc_vector3f& p_eye, const bc_vector3f& p_at, const bc_vector3f& p_up) noexcept
		{
			bc_matrix4f l_ret;

			bc_vector3f l_zaxis = p_eye - p_at;
			l_zaxis.normalize();

			bc_vector3f l_xaxis = p_up.cross(l_zaxis);
			l_xaxis.normalize();

			bc_vector3f l_yaxis = l_zaxis.cross(l_xaxis);

			l_ret.m_entry[0] = l_xaxis.x;
			l_ret.m_entry[1] = l_yaxis.x;
			l_ret.m_entry[2] = l_zaxis.x;
			l_ret.m_entry[3] = 0.0f;

			l_ret.m_entry[4] = l_xaxis.y;
			l_ret.m_entry[5] = l_yaxis.y;
			l_ret.m_entry[6] = l_zaxis.y;
			l_ret.m_entry[7] = 0.0f;

			l_ret.m_entry[8] = l_xaxis.z;
			l_ret.m_entry[9] = l_yaxis.z;
			l_ret.m_entry[10] = l_zaxis.z;
			l_ret.m_entry[11] = 0.0f;

			l_ret.m_entry[12] = l_xaxis.dot(p_eye);
			l_ret.m_entry[13] = l_yaxis.dot(p_eye);
			l_ret.m_entry[14] = l_zaxis.dot(p_eye);
			l_ret.m_entry[15] = 1.0f;

			return l_ret;
		}

		bc_matrix4f bc_matrix4f::perspective_fov_matrix_rh(bcFLOAT p_fovy, bcFLOAT p_aspect, bcFLOAT p_zn, bcFLOAT p_zf) noexcept
		{
			bc_matrix4f l_ret;

			bcFLOAT l_tan_y = tan(p_fovy / 2.0f);
			if (0.0f == l_tan_y) l_tan_y = 0.001f;
			bcFLOAT l_y_scale = 1.0f / l_tan_y;

			if (0.0f == p_aspect) p_aspect = 0.001f;
			bcFLOAT l_x_scale = l_y_scale / p_aspect;

			l_ret.m_entry[0] = l_x_scale;
			l_ret.m_entry[1] = 0.0f;
			l_ret.m_entry[2] = 0.0f;
			l_ret.m_entry[3] = 0.0f;

			l_ret.m_entry[4] = 0.0f;
			l_ret.m_entry[5] = l_y_scale;
			l_ret.m_entry[6] = 0.0f;
			l_ret.m_entry[7] = 0.0f;

			l_ret.m_entry[8] = 0.0f;
			l_ret.m_entry[9] = 0.0f;
			l_ret.m_entry[10] = p_zf / (p_zf - p_zn);
			l_ret.m_entry[11] = -1.0f;

			l_ret.m_entry[12] = 0.0f;
			l_ret.m_entry[13] = 0.0f;
			l_ret.m_entry[14] = p_zn*p_zf / (p_zf - p_zn);
			l_ret.m_entry[15] = 0.0f;

			return l_ret;
		}

		bc_matrix4f bc_matrix4f::orthographic_matrix_rh(bcFLOAT p_zn, bcFLOAT p_zf, bcFLOAT p_width, bcFLOAT p_height) noexcept
		{
			bc_matrix4f l_ret;

			if (p_zn == p_zf) p_zf = p_zn + 0.1f;
			if (p_width <= 0.0f) p_width = 1.0f;
			if (p_height <= 0.0f) p_height = 1.0f;

			l_ret.m_entry[0] = 2.0f / p_width;
			l_ret.m_entry[1] = 0.0f;
			l_ret.m_entry[2] = 0.0f;
			l_ret.m_entry[3] = 0.0f;

			l_ret.m_entry[4] = 0.0f;
			l_ret.m_entry[5] = 2.0f / p_height;
			l_ret.m_entry[6] = 0.0f;
			l_ret.m_entry[7] = 0.0f;

			l_ret.m_entry[8] = 0.0f;
			l_ret.m_entry[9] = 0.0f;
			l_ret.m_entry[10] = 1.0f / (p_zn - p_zf);
			l_ret.m_entry[11] = 0.0f;

			l_ret.m_entry[12] = 0.0f;
			l_ret.m_entry[13] = 0.0f;
			l_ret.m_entry[14] = p_zn / (p_zn - p_zf);
			l_ret.m_entry[15] = 1.0f;

			return l_ret;
		}

		bc_matrix4f bc_matrix4f::zero() noexcept
		{
			bc_matrix4f l_return = bc_matrix4f(true);

			return(l_return);
		}

		bc_matrix4f bc_matrix4f::identity() noexcept
		{
			bc_matrix4f l_ident;

			for (bcINT l_row = 0; l_row < 4; l_row++)
			{
				for (bcINT l_col = 0; l_col < 4; l_col++)
				{
					if (l_row == l_col)
						l_ident.m_entry[I(l_row, l_col)] = 1.0f;
					else
						l_ident.m_entry[I(l_row, l_col)] = 0.0f;
				}
			}

			return(l_ident);
		}

		bcINT bc_matrix4f::I(bcINT iRow, bcINT iCol)
		{
			return(4 * iRow + iCol);
		}
	}
}