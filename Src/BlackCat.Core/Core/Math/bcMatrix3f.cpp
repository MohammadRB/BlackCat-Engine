#include "Core/CorePCH.h"
#include "Core/Math/bcMatrix3f.h"

namespace black_cat
{
	namespace core
	{		
		bc_matrix3f::bc_matrix3f(bool p_zero) noexcept
			: m_entry{}
		{
			if (p_zero)
			{
				std::memset(m_entry, 0, 3 * 3 * sizeof(bcFLOAT));
			}
		}
		
		bc_matrix3f::bc_matrix3f(bcFLOAT p_m11, bcFLOAT p_m12, bcFLOAT p_m13,
			bcFLOAT p_m21, bcFLOAT p_m22, bcFLOAT p_m23,
			bcFLOAT p_m31, bcFLOAT p_m32, bcFLOAT p_m33) noexcept
			: m_entry{}
		{
			m_entry[i(0, 0)] = p_m11;
			m_entry[i(0, 1)] = p_m12;
			m_entry[i(0, 2)] = p_m13;

			m_entry[i(1, 0)] = p_m21;
			m_entry[i(1, 1)] = p_m22;
			m_entry[i(1, 2)] = p_m23;

			m_entry[i(2, 0)] = p_m31;
			m_entry[i(2, 1)] = p_m32;
			m_entry[i(2, 2)] = p_m33;
		}
		
		void bc_matrix3f::rotation_x_lh(bcFLOAT p_radians) noexcept
		{
			const bcFLOAT l_sin = sinf(p_radians);
			const bcFLOAT l_cos = cosf(p_radians);

			m_entry[i(0, 0)] = 1.0f;
			m_entry[i(0, 1)] = 0.0f;
			m_entry[i(0, 2)] = 0.0f;

			m_entry[i(1, 0)] = 0.0f;
			m_entry[i(1, 1)] = l_cos;
			m_entry[i(1, 2)] = l_sin;

			m_entry[i(2, 0)] = 0.0f;
			m_entry[i(2, 1)] = -l_sin;
			m_entry[i(2, 2)] = l_cos;
		}
		
		void bc_matrix3f::rotation_y_lh(bcFLOAT p_radians) noexcept
		{
			const bcFLOAT l_sin = sinf(p_radians);
			const bcFLOAT l_cos = cosf(p_radians);

			m_entry[i(0, 0)] = l_cos;
			m_entry[i(0, 1)] = 0.0f;
			m_entry[i(0, 2)] = -l_sin;

			m_entry[i(1, 0)] = 0.0f;
			m_entry[i(1, 1)] = 1.0f;
			m_entry[i(1, 2)] = 0.0f;

			m_entry[i(2, 0)] = l_sin;
			m_entry[i(2, 1)] = 0.0f;
			m_entry[i(2, 2)] = l_cos;
		}
		
		void bc_matrix3f::rotation_z_lh(bcFLOAT p_radians) noexcept
		{
			const bcFLOAT l_sin = sinf(p_radians);
			const bcFLOAT l_cos = cosf(p_radians);

			m_entry[i(0, 0)] = l_cos;
			m_entry[i(0, 1)] = l_sin;
			m_entry[i(0, 2)] = 0.0f;

			m_entry[i(1, 0)] = -l_sin;
			m_entry[i(1, 1)] = l_cos;
			m_entry[i(1, 2)] = 0.0f;

			m_entry[i(2, 0)] = 0.0f;
			m_entry[i(2, 1)] = 0.0f;
			m_entry[i(2, 2)] = 1.0f;
		}
		
		void bc_matrix3f::rotation_xyz_lh(const bc_vector3f& p_rot) noexcept
		{
			bc_matrix3f l_rot1;
			bc_matrix3f l_rot2;

			l_rot1.rotation_z_lh(p_rot.z);
			l_rot2.rotation_x_lh(p_rot.x);
			l_rot1 *= l_rot2;
			l_rot2.rotation_y_lh(p_rot.y);
			l_rot1 *= l_rot2;
			*this = l_rot1;
		}
		
		void bc_matrix3f::rotation_zyx_lh(const bc_vector3f& p_rot) noexcept
		{
			bc_matrix3f l_rot1;
			bc_matrix3f l_rot2;

			l_rot1.rotation_z_lh(p_rot.z);
			l_rot2.rotation_y_lh(p_rot.y);
			l_rot1 *= l_rot2;
			l_rot2.rotation_x_lh(p_rot.x);
			l_rot1 *= l_rot2;
			*this = l_rot1;
		}
		
		void bc_matrix3f::rotation_euler_lh(const bc_vector3f& p_axis, bcFLOAT p_radians) noexcept
		{
			const bcFLOAT l_s = sinf(p_radians);
			const bcFLOAT l_c = cosf(p_radians);
			const bcFLOAT l_t = 1 - l_c;

			m_entry[i(0, 0)] = l_t * p_axis.x * p_axis.x + l_c;
			m_entry[i(0, 1)] = l_t * p_axis.x * p_axis.y + l_s * p_axis.z;
			m_entry[i(0, 2)] = l_t * p_axis.x * p_axis.z - l_s * p_axis.y;

			m_entry[i(1, 0)] = l_t * p_axis.x * p_axis.y - l_s * p_axis.z;
			m_entry[i(1, 1)] = l_t * p_axis.y * p_axis.y + l_c;
			m_entry[i(1, 2)] = l_t * p_axis.y * p_axis.z + l_s * p_axis.x;

			m_entry[i(2, 0)] = l_t * p_axis.x * p_axis.y + l_s * p_axis.y;
			m_entry[i(2, 1)] = l_t * p_axis.y * p_axis.z - l_s * p_axis.x;
			m_entry[i(2, 2)] = l_t * p_axis.z * p_axis.z + l_c;
		}

		void bc_matrix3f::rotation_between_two_vector_lh(const bc_vector3f& p_v1, const bc_vector3f& p_v2) noexcept
		{
			const auto l_dot = bc_vector3f::dot(p_v1, p_v2);
			const bcFLOAT l_angle = std::acos(l_dot);
			const bc_vector3f l_cross = bc_vector3f::normalize(bc_vector3f::cross(p_v1, p_v2));

			rotation_euler_lh(l_cross, l_angle);
		}

		void bc_matrix3f::rotation_between_two_vector_checked_lh(const bc_vector3f& p_v1, const bc_vector3f& p_v2) noexcept
		{
			const auto l_dot = bc_vector3f::dot(p_v1, p_v2);
			const bcFLOAT l_angle = std::acos(std::min(std::max(l_dot, -1.f), 1.f));
			bc_vector3f l_cross = bc_vector3f::normalize(bc_vector3f::cross(p_v1, p_v2));
			
			if(bc_vector3f::length_sq(l_cross) == 0)
			{
				l_cross = bc_vector3f::normalize(bc_vector3f::cross(p_v1, p_v2 + bc_vector3f(0, 0.0001f, 0)));
			}

			rotation_euler_lh(l_cross, l_angle);
		}

		void bc_matrix3f::rotation_x_rh(bcFLOAT p_radians) noexcept
		{
			rotation_x_lh(p_radians);

			m_entry[i(1, 2)] = -m_entry[i(1, 2)];
			m_entry[i(2, 1)] = -m_entry[i(2, 1)];
		}

		void bc_matrix3f::rotation_y_rh(bcFLOAT p_radians) noexcept
		{
			rotation_y_lh(p_radians);

			m_entry[i(0, 2)] = -m_entry[i(0, 2)];
			m_entry[i(2, 0)] = -m_entry[i(2, 0)];
		}

		void bc_matrix3f::rotation_z_rh(bcFLOAT p_radians) noexcept
		{
			rotation_z_lh(p_radians);

			m_entry[i(0, 1)] = -m_entry[i(0, 1)];
			m_entry[i(1, 0)] = -m_entry[i(1, 0)];
		}

		void bc_matrix3f::rotation_xyz_rh(const bc_vector3f& p_rot) noexcept
		{
			bc_matrix3f l_rot1;
			bc_matrix3f l_rot2;

			l_rot1.rotation_z_rh(p_rot.z);
			l_rot2.rotation_x_rh(p_rot.x);
			l_rot1 *= l_rot2;
			l_rot2.rotation_y_rh(p_rot.y);
			l_rot1 *= l_rot2;
			*this = l_rot1;
		}

		void bc_matrix3f::rotation_zyx_rh(const bc_vector3f& p_rot) noexcept
		{
			bc_matrix3f l_rot1;
			bc_matrix3f l_rot2;

			l_rot1.rotation_z_rh(p_rot.z);
			l_rot2.rotation_y_rh(p_rot.y);
			l_rot1 *= l_rot2;
			l_rot2.rotation_x_rh(p_rot.x);
			l_rot1 *= l_rot2;
			*this = l_rot1;
		}

		void bc_matrix3f::rotation_euler_rh(const bc_vector3f& p_axis, bcFLOAT p_radians) noexcept
		{
			rotation_euler_lh(p_axis, p_radians);

			m_entry[i(0, 1)] = -m_entry[i(0, 1)];
			m_entry[i(0, 2)] = -m_entry[i(0, 2)];
			m_entry[i(1, 0)] = -m_entry[i(1, 0)];
			m_entry[i(1, 2)] = -m_entry[i(1, 2)];
			m_entry[i(2, 0)] = -m_entry[i(2, 0)];
			m_entry[i(2, 1)] = -m_entry[i(2, 1)];
		}

		void bc_matrix3f::rotation_between_two_vector_rh(const bc_vector3f& p_v1, const bc_vector3f& p_v2) noexcept
		{
			const bcFLOAT l_angle = std::acos(bc_vector3f::dot(p_v1, p_v2));
			const bc_vector3f l_cross = bc_vector3f::normalize(bc_vector3f::cross(p_v1, p_v2));

			rotation_euler_rh(l_cross, l_angle);
		}

		void bc_matrix3f::rotation_between_two_vector_checked_rh(const bc_vector3f& p_v1, const bc_vector3f& p_v2) noexcept
		{
			const auto l_dot = bc_vector3f::dot(p_v1, p_v2);
			const bcFLOAT l_angle = std::acos(l_dot);
			bc_vector3f l_cross = bc_vector3f::normalize(bc_vector3f::cross(p_v1, p_v2));

			if (bc_vector3f::length_sq(l_cross) == 0)
			{
				l_cross = bc_vector3f::normalize(bc_vector3f::cross(p_v1, p_v2 + bc_vector3f(0, 0.0001f, 0)));
			}

			rotation_euler_rh(l_cross, l_angle);
		}

		void bc_matrix3f::orthonormalize() noexcept
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
			bcFLOAT l_inv_length = static_cast<bcFLOAT>(1.0 / sqrt(static_cast<double>(m_entry[i(0, 0)] * m_entry[i(0, 0)] + m_entry[i(1, 0)] * m_entry[i(1, 0)] + m_entry[i(2, 0)] * m_entry[i(2, 0)])));

			m_entry[i(0, 0)] *= l_inv_length;
			m_entry[i(1, 0)] *= l_inv_length;
			m_entry[i(2, 0)] *= l_inv_length;

			// compute q1
			bcFLOAT l_dot0 = m_entry[i(0, 0)] * m_entry[i(0, 1)] + m_entry[i(1, 0)] * m_entry[i(1, 1)] + m_entry[i(2, 0)] * m_entry[i(2, 1)];

			m_entry[i(0, 1)] -= l_dot0*m_entry[i(0, 0)];
			m_entry[i(1, 1)] -= l_dot0*m_entry[i(1, 0)];
			m_entry[i(2, 1)] -= l_dot0*m_entry[i(2, 0)];

			l_inv_length = static_cast<bcFLOAT>(1.0 / sqrt(static_cast<double>(m_entry[i(0, 1)] * m_entry[i(0, 1)] + m_entry[i(1, 1)] * m_entry[i(1, 1)] + m_entry[i(2, 1)] * m_entry[i(2, 1)])));

			m_entry[i(0, 1)] *= l_inv_length;
			m_entry[i(1, 1)] *= l_inv_length;
			m_entry[i(2, 1)] *= l_inv_length;

			// compute q2
			const bcFLOAT fDot1 = m_entry[i(0, 1)] * m_entry[i(0, 2)] + m_entry[i(1, 1)] * m_entry[i(1, 2)] + m_entry[i(2, 1)] * m_entry[i(2, 2)];

			l_dot0 = m_entry[i(0, 0)] * m_entry[i(0, 2)] + m_entry[i(1, 0)] * m_entry[i(1, 2)] + m_entry[i(2, 0)] * m_entry[i(2, 2)];

			m_entry[i(0, 2)] -= l_dot0*m_entry[i(0, 0)] + fDot1*m_entry[i(0, 1)];
			m_entry[i(1, 2)] -= l_dot0*m_entry[i(1, 0)] + fDot1*m_entry[i(1, 1)];
			m_entry[i(2, 2)] -= l_dot0*m_entry[i(2, 0)] + fDot1*m_entry[i(2, 1)];

			l_inv_length = static_cast<bcFLOAT>(1.0 / sqrt(static_cast<double>(m_entry[i(0, 2)] * m_entry[i(0, 2)] + m_entry[i(1, 2)] * m_entry[i(1, 2)] + m_entry[i(2, 2)] * m_entry[i(2, 2)])));

			m_entry[i(0, 2)] *= l_inv_length;
			m_entry[i(1, 2)] *= l_inv_length;
			m_entry[i(2, 2)] *= l_inv_length;
		}
				
		void bc_matrix3f::make_zero() noexcept
		{
			std::memset(m_entry, 0, 3 * 3 * sizeof(bcFLOAT));
		}
		
		void bc_matrix3f::make_identity() noexcept
		{
			for (auto l_row = 0U; l_row < 3; l_row++)
			{
				for (auto l_col = 0U; l_col < 3; l_col++)
				{
					if (l_row == l_col)
					{
						m_entry[i(l_row, l_col)] = 1.0f;
					}
					else
					{
						m_entry[i(l_row, l_col)] = 0.0f;
					}
				}
			}
		}
		
		void bc_matrix3f::make_transpose() noexcept
		{
			bc_matrix3f l_transpose;

			for (auto l_row = 0U; l_row < 3; l_row++)
			{
				for (auto l_col = 0U; l_col < 3; l_col++)
				{
					l_transpose.m_entry[i(l_row, l_col)] = m_entry[i(l_col, l_row)];
				}
			}

			std::memcpy(m_entry, l_transpose.m_entry, 3 * 3 * sizeof(bcFLOAT));
		}

		void bc_matrix3f::make_neutralize_scale() noexcept
		{
			const auto l_row0 = get_row(0);
			const auto l_row1 = get_row(1);
			const auto l_row2 = get_row(2);
			const auto l_row0_normal = bc_vector3f::normalize(l_row0);
			const auto l_row1_normal = bc_vector3f::normalize(l_row1);
			const auto l_row2_normal = bc_vector3f::normalize(l_row2);

			set_row(0, l_row0_normal);
			set_row(1, l_row1_normal);
			set_row(2, l_row2_normal);
		}

		bc_matrix3f bc_matrix3f::transpose() const noexcept
		{
			bc_matrix3f l_transpose;

			for (auto l_row = 0U; l_row < 3; l_row++)
			{
				for (auto l_col = 0U; l_col < 3; l_col++)
				{
					l_transpose.m_entry[i(l_row, l_col)] = m_entry[i(l_col, l_row)];
				}
			}
			return l_transpose;
		}
		
		bc_vector3f bc_matrix3f::operator* (const bc_vector3f& p_vector) const noexcept
		{
			bc_vector3f l_prod;
			for (auto l_col = 0U; l_col < 3; l_col++)
			{
				l_prod[l_col] = 0.0f;
				for (auto l_row = 0U; l_row < 3; l_row++)
				{
					l_prod[l_col] += m_entry[i(l_row, l_col)] * p_vector[l_row];
				}
			}
			return l_prod;
		}
		
		void bc_matrix3f::set_row(bcINT p_row, const bc_vector3f& p_vector) noexcept
		{
			for (auto l_col = 0U; l_col < 3; l_col++)
			{
				m_entry[i(p_row, l_col)] = p_vector[l_col];
			}
		}
		
		bc_vector3f bc_matrix3f::get_row(bcINT p_row) const noexcept
		{
			bc_vector3f l_row;
			for (auto l_col = 0U; l_col < 3; l_col++)
			{
				l_row[l_col] = m_entry[i(p_row, l_col)];
			}

			return l_row;
		}
		
		void bc_matrix3f::set_column(bcINT p_col, const bc_vector3f& p_vector) noexcept
		{
			for (auto l_row = 0U; l_row < 3; l_row++)
			{
				m_entry[i(l_row, p_col)] = p_vector[l_row];
			}
		}
		
		bc_vector3f bc_matrix3f::get_column(bcINT p_col) const noexcept
		{
			bc_vector3f l_col;
			for (auto l_row = 0U; l_row < 3; l_row++)
			{
				l_col[l_row] = m_entry[i(l_row, p_col)];
			}

			return l_col;
		}

		bcFLOAT bc_matrix3f::operator[] (bcINT p_pos) const noexcept
		{
			return m_entry[i(p_pos)];
		}
		
		bcFLOAT& bc_matrix3f::operator[] (bcINT p_pos) noexcept
		{
			return m_entry[i(p_pos)];
		}
		
		bcFLOAT bc_matrix3f::operator() (bcINT p_row, bcINT p_col) const noexcept
		{
			return m_entry[i(p_row, p_col)];
		}
		
		bcFLOAT& bc_matrix3f::operator() (bcINT p_row, bcINT p_col) noexcept
		{
			return m_entry[i(p_row, p_col)];
		}
		
		bool bc_matrix3f::operator== (const bc_matrix3f& p_other) const noexcept
		{
			return std::memcmp(m_entry, p_other.m_entry, 3 * 3 * sizeof(bcFLOAT)) == 0;
		}

		bool bc_matrix3f::operator!= (const bc_matrix3f& p_other) const noexcept
		{
			return std::memcmp(m_entry, p_other.m_entry, 3 * 3 * sizeof(bcFLOAT)) != 0;
		}

		bc_matrix3f bc_matrix3f::operator* (const bc_matrix3f& p_other) const noexcept
		{
			bc_matrix3f l_prod;

			for (auto l_row = 0U; l_row < 3; l_row++)
			{
				for (auto l_col = 0U; l_col < 3; l_col++)
				{
					const auto l_i = i(l_row, l_col);
					l_prod.m_entry[l_i] = 0.0f;

					for (auto l_mid = 0U; l_mid < 3; l_mid++)
					{
						l_prod.m_entry[l_i] += m_entry[i(l_row, l_mid)] * p_other.m_entry[i(l_mid, l_col)];
					}
				}
			}
			
			return l_prod;
		}

		bc_matrix3f bc_matrix3f::operator+ (const bc_matrix3f& p_other) const noexcept
		{
			bc_matrix3f l_sum;

			for (auto l_i = 0U; l_i < 3 * 3; l_i++)
			{
				l_sum.m_entry[l_i] = m_entry[l_i] + p_other.m_entry[l_i];
			}

			return l_sum;
		}

		bc_matrix3f bc_matrix3f::operator- (const bc_matrix3f& p_other) const noexcept
		{
			bc_matrix3f l_diff;

			for (auto l_i = 0U; l_i < 3 * 3; l_i++)
			{
				l_diff.m_entry[l_i] = m_entry[l_i] - p_other.m_entry[l_i];
			}

			return l_diff;
		}

		bc_matrix3f bc_matrix3f::operator* (bcFLOAT p_scalar) const noexcept
		{
			bc_matrix3f l_prod;

			for (auto l_i = 0U; l_i < 3 * 3; l_i++)
			{
				l_prod.m_entry[l_i] = m_entry[l_i] * p_scalar;
			}

			return l_prod;
		}

		bc_matrix3f bc_matrix3f::operator/ (bcFLOAT p_scalar) const noexcept
		{
			bc_matrix3f l_quot;
			if (p_scalar != 0.0f)
			{
				const bcFLOAT l_inv_scalar = 1.0f / p_scalar;
				for (auto l_i = 0; l_i < 3 * 3; l_i++)
				{
					l_quot.m_entry[l_i] = m_entry[l_i] * l_inv_scalar;
				}
			}
			else
			{
				for (auto l_i = 0; l_i < 3 * 3; l_i++)
				{
					l_quot.m_entry[l_i] = 0;
				}
			}

			return l_quot;
		}

		bc_matrix3f bc_matrix3f::operator- () const noexcept
		{
			bc_matrix3f l_neg;

			for (auto l_i = 0; l_i < 3 * 3; l_i++)
			{
				l_neg.m_entry[l_i] = -m_entry[l_i];
			}

			return l_neg;
		}

		bc_matrix3f& bc_matrix3f::operator+= (const bc_matrix3f& p_other) noexcept
		{
			for (auto l_i = 0; l_i < 3 * 3; l_i++)
			{
				m_entry[l_i] += p_other.m_entry[l_i];
			}

			return*this;
		}

		bc_matrix3f& bc_matrix3f::operator-= (const bc_matrix3f& p_other) noexcept
		{
			for (auto l_i = 0; l_i < 3 * 3; l_i++)
			{
				m_entry[l_i] -= p_other.m_entry[l_i];
			}

			return*this;
		}

		bc_matrix3f& bc_matrix3f::operator*= (bcFLOAT p_scalar) noexcept
		{
			for (auto l_i = 0; l_i < 3 * 3; l_i++)
			{
				m_entry[l_i] *= p_scalar;
			}

			return*this;
		}

		bc_matrix3f& bc_matrix3f::operator*= (const bc_matrix3f& p_other) noexcept
		{
			bc_matrix3f l_prod = *this;

			for (auto l_row = 0U; l_row < 3; l_row++)
			{
				for (auto l_col = 0U; l_col < 3; l_col++)
				{
					const auto l_i = i(l_row, l_col);
					m_entry[l_i] = 0.0f;

					for (auto l_mid = 0U; l_mid < 3; l_mid++)
					{
						m_entry[l_i] += l_prod.m_entry[i(l_row, l_mid)] * p_other.m_entry[i(l_mid, l_col)];
					}
				}
			}
			return*this;
		}

		bc_matrix3f& bc_matrix3f::operator/= (bcFLOAT p_scalar) noexcept
		{
			if (p_scalar != 0.0f)
			{
				const bcFLOAT l_inv_scalar = 1.0f / p_scalar;
				for (auto i = 0U; i < 3 * 3; i++)
				{
					m_entry[i] *= l_inv_scalar;
				}
			}
			else
			{
				for (auto i = 0U; i < 3 * 3; i++)
				{
					m_entry[i] = 0;
				}
			}

			return*this;
		}

		bc_matrix3f bc_matrix3f::zero() noexcept
		{
			bc_matrix3f l_return = bc_matrix3f(true);

			return l_return;
		}

		bc_matrix3f bc_matrix3f::identity() noexcept
		{
			bc_matrix3f l_ident;
			l_ident.make_identity();

			return l_ident;
		}

		bc_matrix3f bc_matrix3f::rotation_matrix_x_lh(bcFLOAT p_radians) noexcept
		{
			bc_matrix3f l_matrix;
			l_matrix.rotation_x_lh(p_radians);

			return l_matrix;
		}

		bc_matrix3f bc_matrix3f::rotation_matrix_y_lh(bcFLOAT p_radians) noexcept
		{
			bc_matrix3f l_matrix;
			l_matrix.rotation_y_lh(p_radians);

			return l_matrix;
		}

		bc_matrix3f bc_matrix3f::rotation_matrix_z_lh(bcFLOAT p_radians) noexcept
		{
			bc_matrix3f l_matrix;
			l_matrix.rotation_z_lh(p_radians);

			return l_matrix;
		}

		bc_matrix3f bc_matrix3f::rotation_matrix_xyz_lh(const bc_vector3f& p_rot) noexcept
		{
			bc_matrix3f l_matrix;
			l_matrix.rotation_xyz_lh(p_rot);

			return l_matrix;
		}

		bc_matrix3f bc_matrix3f::rotation_matrix_zyx_lh(const bc_vector3f& p_rot) noexcept
		{
			bc_matrix3f l_matrix;
			l_matrix.rotation_zyx_lh(p_rot);

			return l_matrix;
		}

		bc_matrix3f bc_matrix3f::rotation_matrix_euler_lh(const bc_vector3f& p_axis, bcFLOAT p_radians) noexcept
		{
			bc_matrix3f l_matrix;
			l_matrix.rotation_euler_lh(p_axis, p_radians);

			return l_matrix;
		}

		bc_matrix3f bc_matrix3f::rotation_matrix_between_two_vector_lh(const bc_vector3f& p_v1, const bc_vector3f& p_v2) noexcept
		{
			bc_matrix3f l_matrix;
			l_matrix.rotation_between_two_vector_lh(p_v1, p_v2);

			return l_matrix;
		}

		bc_matrix3f bc_matrix3f::rotation_matrix_between_two_vector_checked_lh(const bc_vector3f& p_v1, const bc_vector3f& p_v2) noexcept
		{
			bc_matrix3f l_matrix;
			l_matrix.rotation_between_two_vector_checked_lh(p_v1, p_v2);

			return l_matrix;
		}

		bc_matrix3f bc_matrix3f::rotation_matrix_x_rh(bcFLOAT p_radians) noexcept
		{
			bc_matrix3f l_matrix;
			l_matrix.rotation_x_rh(p_radians);

			return l_matrix;
		}

		bc_matrix3f bc_matrix3f::rotation_matrix_y_rh(bcFLOAT p_radians) noexcept
		{
			bc_matrix3f l_matrix;
			l_matrix.rotation_y_rh(p_radians);

			return l_matrix;
		}

		bc_matrix3f bc_matrix3f::rotation_matrix_z_rh(bcFLOAT p_radians) noexcept
		{
			bc_matrix3f l_matrix;
			l_matrix.rotation_z_rh(p_radians);

			return l_matrix;
		}

		bc_matrix3f bc_matrix3f::rotation_matrix_xyz_rh(const bc_vector3f& p_rot) noexcept
		{
			bc_matrix3f l_matrix;
			l_matrix.rotation_xyz_rh(p_rot);

			return l_matrix;
		}

		bc_matrix3f bc_matrix3f::rotation_matrix_zyx_rh(const bc_vector3f& p_rot) noexcept
		{
			bc_matrix3f l_matrix;
			l_matrix.rotation_zyx_rh(p_rot);

			return l_matrix;
		}

		bc_matrix3f bc_matrix3f::rotation_matrix_euler_rh(const bc_vector3f& p_axis, bcFLOAT p_radians) noexcept
		{
			bc_matrix3f l_matrix;
			l_matrix.rotation_euler_rh(p_axis, p_radians);

			return l_matrix;
		}

		bc_matrix3f bc_matrix3f::rotation_matrix_between_two_vector_rh(const bc_vector3f& p_v1, const bc_vector3f& p_v2) noexcept
		{
			bc_matrix3f l_matrix;
			l_matrix.rotation_between_two_vector_rh(p_v1, p_v2);

			return l_matrix;
		}

		bc_matrix3f bc_matrix3f::rotation_matrix_between_two_vector_checked_rh(const bc_vector3f& p_v1, const bc_vector3f& p_v2) noexcept
		{
			bc_matrix3f l_matrix;
			l_matrix.rotation_between_two_vector_checked_rh(p_v1, p_v2);

			return l_matrix;
		}

		constexpr bcUINT32 bc_matrix3f::i(bcUINT32 p_index)
		{
			if constexpr (use_column_major_storage())
			{
				return i(p_index / 3, p_index % 3);
			}
			else
			{
				return p_index;
			}
		}

		constexpr bcUINT32 bc_matrix3f::i(bcUINT32 p_row, bcUINT32 p_col)
		{
			if constexpr (use_column_major_storage())
			{
				return 3 * p_col + p_row;
			}
			else
			{
				return 3 * p_row + p_col;
			}
		}
	}
}