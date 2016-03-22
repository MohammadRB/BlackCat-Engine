//--------------------------------------------------------------------------------
// This file is a portion of the Hieroglyph 3 Rendering Engine.  It is distributed
// under the MIT License, available in the root of this distribution and 
// at the following URL:
//
// http://www.opensource.org/licenses/mit-license.php
//
// Copyright (c) Jason Zink 
//--------------------------------------------------------------------------------

#include "Graphic/GraphicPCH.h"
#include "Graphic/Math/bcVector3f.h"
#include "Graphic/Math/bcMatrix3f.h"

namespace black_cat
{
	namespace graphic
	{
		bc_matrix3f::bc_matrix3f()
		{
		}
		
		bc_matrix3f::bc_matrix3f(bool p_zero)
		{
			if (p_zero)
				std::memset(m_entry, 0, 3 * 3 * sizeof(bcFLOAT32));
		}
		
		bc_matrix3f::bc_matrix3f(const bc_matrix3f& p_other)
		{
			std::memcpy(m_entry, static_cast<void*>(const_cast<bc_matrix3f*>(&p_other)), 9 * sizeof(bcFLOAT32));
		}
		
		bc_matrix3f::bc_matrix3f(bcFLOAT32 p_m11, bcFLOAT32 p_m12, bcFLOAT32 p_m13,
			bcFLOAT32 p_m21, bcFLOAT32 p_m22, bcFLOAT32 p_m23,
			bcFLOAT32 p_m31, bcFLOAT32 p_m32, bcFLOAT32 p_m33)
		{
			m_entry[0] = p_m11;
			m_entry[1] = p_m12;
			m_entry[2] = p_m13;

			m_entry[3] = p_m21;
			m_entry[4] = p_m22;
			m_entry[5] = p_m23;

			m_entry[6] = p_m31;
			m_entry[7] = p_m32;
			m_entry[8] = p_m33;
		}
		
		bc_matrix3f& bc_matrix3f::operator= (const bc_matrix3f& p_other)
		{
			std::memcpy(m_entry, p_other.m_entry, 9 * sizeof(bcFLOAT32));
			return *this;
		}
		
		void bc_matrix3f::rotation_x(bcFLOAT32 p_radians)
		{
			bcFLOAT32 l_sin = sinf(p_radians);
			bcFLOAT32 l_cos = cosf(p_radians);

			m_entry[0] = 1.0f;
			m_entry[1] = 0.0f;
			m_entry[2] = 0.0f;

			m_entry[3] = 0.0f;
			m_entry[4] = l_cos;
			m_entry[5] = l_sin;

			m_entry[6] = 0.0f;
			m_entry[7] = -l_sin;
			m_entry[8] = l_cos;
		}
		
		void bc_matrix3f::rotation_y(bcFLOAT32 p_radians)
		{
			bcFLOAT32 l_sin = sinf(p_radians);
			bcFLOAT32 l_cos = cosf(p_radians);

			m_entry[0] = l_cos;
			m_entry[1] = 0.0f;
			m_entry[2] = -l_sin;

			m_entry[3] = 0.0f;
			m_entry[4] = 1.0f;
			m_entry[5] = 0.0f;

			m_entry[6] = l_sin;
			m_entry[7] = 0.0f;
			m_entry[8] = l_cos;
		}
		
		void bc_matrix3f::rotation_z(bcFLOAT32 p_radians)
		{
			bcFLOAT32 l_sin = sinf(p_radians);
			bcFLOAT32 l_cos = cosf(p_radians);

			m_entry[0] = l_cos;
			m_entry[1] = l_sin;
			m_entry[2] = 0.0f;

			m_entry[3] = -l_sin;
			m_entry[4] = l_cos;
			m_entry[5] = 0.0f;

			m_entry[6] = 0.0f;
			m_entry[7] = 0.0f;
			m_entry[8] = 1.0f;
		}
		
		void bc_matrix3f::rotation(bc_vector3f& p_rot)
		{
			bc_matrix3f l_rot1;
			bc_matrix3f l_rot2;

			l_rot1.rotation_z(p_rot.z);
			l_rot2.rotation_x(p_rot.x);
			l_rot1 *= l_rot2;
			l_rot2.rotation_y(p_rot.y);
			l_rot1 *= l_rot2;
			*this = l_rot1;
		}
		
		void bc_matrix3f::rotation_zyx(bc_vector3f& p_rot)
		{
			bc_matrix3f l_rot1;
			bc_matrix3f l_rot2;

			l_rot1.rotation_z(p_rot.z);
			l_rot2.rotation_y(p_rot.y);
			l_rot1 *= l_rot2;
			l_rot2.rotation_x(p_rot.x);
			l_rot1 *= l_rot2;
			*this = l_rot1;
		}
		
		void bc_matrix3f::rotation_euler(bc_vector3f& p_axis, bcFLOAT32 p_angle)
		{
			bcFLOAT32 l_s = sinf(p_angle);
			bcFLOAT32 l_c = cosf(p_angle);
			bcFLOAT32 l_t = 1 - l_c;

			m_entry[0] = l_t*p_axis.x*p_axis.x + l_c;
			m_entry[1] = l_t*p_axis.x*p_axis.y + l_s*p_axis.z;
			m_entry[2] = l_t*p_axis.x*p_axis.z - l_s*p_axis.y;

			m_entry[3] = l_t*p_axis.x*p_axis.y - l_s*p_axis.z;
			m_entry[4] = l_t*p_axis.y*p_axis.y + l_c;
			m_entry[5] = l_t*p_axis.y*p_axis.z + l_s*p_axis.x;

			m_entry[6] = l_t*p_axis.x*p_axis.y + l_s*p_axis.y;
			m_entry[7] = l_t*p_axis.y*p_axis.z - l_s*p_axis.x;
			m_entry[8] = l_t*p_axis.z*p_axis.z + l_c;
		}
		
		void bc_matrix3f::orthonormalize()
		{
			// This method is taken from the Wild Magic library v3.11, available at
			// http://www.geometrictools.com.

			// Algorithm uses Gram-Schmidt orthogonalization.  If 'this' matrix is
			// M = [m0|m1|m2], then orthonormal output matrix is Q = [q0|q1|q2],
			//
			//   q0 = m0/|m0|
			//   q1 = (m1-(q0*m1)q0)/|m1-(q0*m1)q0|
			//   q2 = (m2-(q0*m2)q0-(q1*m2)q1)/|m2-(q0*m2)q0-(q1*m2)q1|
			//
			// where |V| indicates length of vector V and A*B indicates dot
			// product of vectors A and B.

			//(1.0/sqrt((double)fValue))

			// compute q0
			bcFLOAT32 l_inv_length = static_cast<bcFLOAT32>(1.0 / sqrt(static_cast<double>(m_entry[0] * m_entry[0] +
				m_entry[3] * m_entry[3] + m_entry[6] * m_entry[6])));

			m_entry[0] *= l_inv_length;
			m_entry[3] *= l_inv_length;
			m_entry[6] *= l_inv_length;

			// compute q1
			bcFLOAT32 l_dot0 = m_entry[0] * m_entry[1] + m_entry[3] * m_entry[4] +
				m_entry[6] * m_entry[7];

			m_entry[1] -= l_dot0*m_entry[0];
			m_entry[4] -= l_dot0*m_entry[3];
			m_entry[7] -= l_dot0*m_entry[6];

			l_inv_length = static_cast<bcFLOAT32>(1.0 / sqrt(static_cast<double>(m_entry[1] * m_entry[1] +
				m_entry[4] * m_entry[4] + m_entry[7] * m_entry[7])));

			m_entry[1] *= l_inv_length;
			m_entry[4] *= l_inv_length;
			m_entry[7] *= l_inv_length;

			// compute q2
			bcFLOAT32 fDot1 = m_entry[1] * m_entry[2] + m_entry[4] * m_entry[5] +
				m_entry[7] * m_entry[8];

			l_dot0 = m_entry[0] * m_entry[2] + m_entry[3] * m_entry[5] +
				m_entry[6] * m_entry[8];

			m_entry[2] -= l_dot0*m_entry[0] + fDot1*m_entry[1];
			m_entry[5] -= l_dot0*m_entry[3] + fDot1*m_entry[4];
			m_entry[8] -= l_dot0*m_entry[6] + fDot1*m_entry[7];

			l_inv_length = static_cast<bcFLOAT32>(1.0 / sqrt(static_cast<double>(m_entry[2] * m_entry[2] +
				m_entry[5] * m_entry[5] + m_entry[8] * m_entry[8])));

			m_entry[2] *= l_inv_length;
			m_entry[5] *= l_inv_length;
			m_entry[8] *= l_inv_length;
		}
		//----------------------------------------------------------------------------
		bcFLOAT32 bc_matrix3f::operator[] (bcINT p_pos) const
		{
			return(m_entry[p_pos]);
		}
		//----------------------------------------------------------------------------
		bcFLOAT32& bc_matrix3f::operator[] (bcINT p_pos)
		{
			return(m_entry[p_pos]);
		}
		//----------------------------------------------------------------------------
		bcFLOAT32 bc_matrix3f::operator() (bcINT p_row, bcINT p_col) const
		{
			return(m_entry[I(p_row, p_col)]);
		}
		//----------------------------------------------------------------------------
		bcFLOAT32& bc_matrix3f::operator() (bcINT p_row, bcINT p_col)
		{
			return(m_entry[I(p_row, p_col)]);
		}
		//----------------------------------------------------------------------------
		bool bc_matrix3f::operator== (const bc_matrix3f& p_other) const
		{
			return(std::memcmp(m_entry, p_other.m_entry, 3 * 3 * sizeof(bcFLOAT32)) == 0);
		}
		
		bool bc_matrix3f::operator!= (const bc_matrix3f& p_other) const
		{
			return(std::memcmp(m_entry, p_other.m_entry, 3 * 3 * sizeof(bcFLOAT32)) != 0);
		}
		
		bc_matrix3f bc_matrix3f::operator* (const bc_matrix3f& p_other) const
		{
			bc_matrix3f l_prod;

			for (bcINT l_row = 0; l_row < 3; l_row++)
			{
				for (bcINT l_col = 0; l_col < 3; l_col++)
				{
					bcINT l_i = I(l_row, l_col);
					l_prod.m_entry[l_i] = 0.0f;
					for (bcINT l_mid = 0; l_mid < 3; l_mid++)
					{
						l_prod.m_entry[l_i] +=
							m_entry[I(l_row, l_mid)] * p_other.m_entry[I(l_mid, l_col)];
					}
				}
			}
			return(l_prod);
		}
		
		bc_matrix3f bc_matrix3f::operator+ (const bc_matrix3f& p_other) const
		{
			bc_matrix3f l_sum;

			for (bcINT i = 0; i < 3 * 3; i++)
				l_sum.m_entry[i] = m_entry[i] + p_other.m_entry[i];

			return(l_sum);
		}
		
		bc_matrix3f bc_matrix3f::operator- (const bc_matrix3f& p_other) const
		{
			bc_matrix3f l_diff;

			for (bcINT i = 0; i < 3 * 3; i++)
				l_diff.m_entry[i] = m_entry[i] - p_other.m_entry[i];

			return(l_diff);
		}
		
		bc_matrix3f bc_matrix3f::operator* (bcFLOAT32 p_scalar) const
		{
			bc_matrix3f l_prod;

			for (bcINT i = 0; i < 3 * 3; i++)
				l_prod.m_entry[i] = m_entry[i] * p_scalar;

			return(l_prod);
		}
		
		bc_matrix3f bc_matrix3f::operator/ (bcFLOAT32 p_scalar) const
		{
			bc_matrix3f l_quot;
			if (p_scalar != 0.0f)
			{
				bcFLOAT32 l_inv_scalar = 1.0f / p_scalar;
				for (bcINT i = 0; i < 3 * 3; i++)
					l_quot.m_entry[i] = m_entry[i] * l_inv_scalar;
			}
			else
			{
				for (bcINT i = 0; i < 3 * 3; i++)
					l_quot.m_entry[i] = 0;
			}

			return(l_quot);
		}
		
		bc_matrix3f bc_matrix3f::operator- () const
		{
			bc_matrix3f l_neg;

			for (bcINT i = 0; i < 3 * 3; i++)
				l_neg.m_entry[i] = -m_entry[i];

			return(l_neg);
		}
		
		bc_matrix3f& bc_matrix3f::operator+= (const bc_matrix3f& p_other)
		{
			for (bcINT i = 0; i < 3 * 3; i++)
				m_entry[i] += p_other.m_entry[i];

			return(*this);
		}
		
		bc_matrix3f& bc_matrix3f::operator-= (const bc_matrix3f& p_other)
		{
			for (bcINT i = 0; i < 3 * 3; i++)
				m_entry[i] -= p_other.m_entry[i];

			return(*this);
		}
		
		bc_matrix3f& bc_matrix3f::operator*= (bcFLOAT32 p_scalar)
		{
			for (bcINT i = 0; i < 3 * 3; i++)
				m_entry[i] *= p_scalar;

			return(*this);
		}
		
		bc_matrix3f& bc_matrix3f::operator*= (const bc_matrix3f& p_matrix)
		{
			bc_matrix3f l_prod = *this;

			for (bcINT l_row = 0; l_row < 3; l_row++)
			{
				for (bcINT l_col = 0; l_col < 3; l_col++)
				{
					bcINT i = I(l_row, l_col);
					m_entry[i] = 0.0f;
					for (bcINT l_mid = 0; l_mid < 3; l_mid++)
					{
						m_entry[i] +=
							l_prod.m_entry[I(l_row, l_mid)] * p_matrix.m_entry[I(l_mid, l_col)];
					}
				}
			}
			return(*this);
		}
		
		bc_matrix3f& bc_matrix3f::operator/= (bcFLOAT32 p_scalar)
		{
			if (p_scalar != 0.0f)
			{
				bcFLOAT32 l_inv_scalar = 1.0f / p_scalar;
				for (bcINT i = 0; i < 3 * 3; i++)
					m_entry[i] *= l_inv_scalar;
			}
			else
			{
				for (bcINT i = 0; i < 3 * 3; i++)
					m_entry[i] = 0;
			}

			return(*this);
		}
		
		void bc_matrix3f::make_zero()
		{
			std::memset(m_entry, 0, 3 * 3 * sizeof(bcFLOAT32));
		}
		
		void bc_matrix3f::make_identity()
		{
			for (bcINT l_row = 0; l_row < 3; l_row++)
			{
				for (bcINT l_col = 0; l_col < 3; l_col++)
				{
					if (l_row == l_col)
						m_entry[I(l_row, l_col)] = 1.0f;
					else
						m_entry[I(l_row, l_col)] = 0.0f;
				}
			}
		}
		
		void bc_matrix3f::make_transpose()
		{
			bc_matrix3f l_transpose;

			for (bcINT l_row = 0; l_row < 3; l_row++)
			{
				for (bcINT l_col = 0; l_col < 3; l_col++)
					l_transpose.m_entry[I(l_row, l_col)] = m_entry[I(l_col, l_row)];
			}

			std::memcpy(m_entry, l_transpose.m_entry, 3 * 3 * sizeof(bcFLOAT32));
		}
		
		bc_matrix3f bc_matrix3f::zero()
		{
			bc_matrix3f l_return = bc_matrix3f(true);

			return(l_return);
		}
		
		bc_matrix3f bc_matrix3f::identity()
		{
			bc_matrix3f l_ident;

			for (bcINT l_row = 0; l_row < 3; l_row++)
			{
				for (bcINT l_col = 0; l_col < 3; l_col++)
				{
					if (l_row == l_col)
						l_ident.m_entry[I(l_row, l_col)] = 1.0f;
					else
						l_ident.m_entry[I(l_row, l_col)] = 0.0f;
				}
			}
			return(l_ident);
		}
		
		bc_matrix3f bc_matrix3f::transpose()
		{
			bc_matrix3f l_transpose;

			for (bcINT l_row = 0; l_row < 3; l_row++)
			{
				for (bcINT l_col = 0; l_col < 3; l_col++)
					l_transpose.m_entry[I(l_row, l_col)] = m_entry[I(l_col, l_row)];
			}
			return(l_transpose);
		}
		
		bcINT bc_matrix3f::I(bcINT p_row, bcINT p_col)
		{
			return(3 * p_row + p_col);
		}
		
		bc_vector3f bc_matrix3f::operator* (const bc_vector3f& p_vector) const
		{
			bc_vector3f l_prod;
			for (bcINT l_col = 0; l_col < 3; l_col++)
			{
				l_prod[l_col] = 0.0f;
				for (bcINT iRow = 0; iRow < 3; iRow++)
					l_prod[l_col] += m_entry[I(iRow, l_col)] * p_vector[iRow];
			}
			return(l_prod);
		}
		
		void bc_matrix3f::set_row(bcINT p_row, const bc_vector3f& p_vector)
		{
			for (bcINT l_col = 0; l_col < 3; l_col++)
				m_entry[I(p_row, l_col)] = p_vector[l_col];
		}
		
		bc_vector3f bc_matrix3f::get_row(bcINT p_row) const
		{
			bc_vector3f l_row;
			for (bcINT l_col = 0; l_col < 3; l_col++)
				l_row[l_col] = m_entry[I(p_row, l_col)];

			return(l_row);
		}
		
		void bc_matrix3f::set_column(bcINT p_col, const bc_vector3f& p_vector)
		{
			for (bcINT l_row = 0; l_row < 3; l_row++)
				m_entry[I(l_row, p_col)] = p_vector[l_row];
		}
		
		bc_vector3f bc_matrix3f::get_column(bcINT p_col) const
		{
			bc_vector3f l_col;
			for (bcINT l_row = 0; l_row < 3; l_row++)
				l_col[l_row] = m_entry[I(l_row, p_col)];

			return(l_col);
		}
	}
}