// [05/04/2022 MRB]

#pragma once

#include "CorePlatform/bcType.h"

namespace black_cat
{
	namespace game
	{
		/**
		 * \brief Actor identifier which consist of two peace of data, actor index and actor repeat index.
		 * Actor index refer to physical storage of actor and repeat index indicate how many times a new
		 * actor is initialized in physical storage of actor.
		 */
		struct bc_actor_id
		{
		private:
			friend struct std::hash<bc_actor_id>;

			constexpr static bcUINT32 s_repeat_index_mask = 0b111;
			constexpr static bcUINT32 s_repeat_index_bit_size = 3;
			constexpr static bcUINT32 s_index_mask = 0xffffffff & ~s_repeat_index_mask;
			constexpr static bcUINT32 s_index_size = s_index_mask >> s_repeat_index_bit_size;
			constexpr static bcUINT32 s_invalid_index = s_index_size;
			constexpr static bcUINT32 s_invalid_id = s_invalid_index << s_repeat_index_bit_size;

		public:
			/**
			 * \brief Default construction which initialize with invalid id
			 */
			bc_actor_id() = default;

			explicit constexpr bc_actor_id(bcSIZE p_hash) noexcept
				: m_id(static_cast<bcUINT32>(p_hash))
			{
			}

			bc_actor_id(const bc_actor_id&) noexcept = default;

			~bc_actor_id() = default;

			bc_actor_id& operator=(const bc_actor_id&) noexcept = default;

			bool is_valid() const noexcept
			{
				return decompose_id(*this).first != s_invalid_index;
			}

			bool operator==(bc_actor_id p_other) const noexcept
			{
				return m_id == p_other.m_id;
			}

			bool operator!=(bc_actor_id p_other) const noexcept
			{
				return m_id != p_other.m_id;
			}
			
			static bcUINT32 generate_new_repeat_id(bcUINT32 p_old_repeat_id) noexcept;

			/**
			 * \brief Decompose actor id into actor index an actor repeat index
			 * \param p_id 
			 * \return 
			 */
			static std::pair<bcUINT32, bcUINT32> decompose_id(bc_actor_id p_id) noexcept;

			/**
			 * \brief Generate actor id
			 * \param p_index 
			 * \param p_repeat_index 
			 * \return 
			 */
			static bc_actor_id compose_id(bcUINT32 p_index, bcUINT32 p_repeat_index) noexcept;

			/**
			 * \brief Generate actor id with invalid index and provided repeat index
			 * \param p_repeat_index 
			 * \return 
			 */
			static bc_actor_id compose_invalid_index(bcUINT32 p_repeat_index) noexcept;

		private:
			bcUINT32 m_id{ s_invalid_id };
		};

		inline bcUINT32 bc_actor_id::generate_new_repeat_id(bcUINT32 p_old_repeat_id) noexcept
		{
			return (p_old_repeat_id + 1) % s_repeat_index_mask;
		}

		inline std::pair<bcUINT32, bcUINT32> bc_actor_id::decompose_id(bc_actor_id p_id) noexcept
		{
			return { p_id.m_id >> s_repeat_index_bit_size, p_id.m_id & s_repeat_index_mask };
		}

		inline bc_actor_id bc_actor_id::compose_id(bcUINT32 p_index, bcUINT32 p_repeat_index) noexcept
		{
			BC_ASSERT(p_repeat_index <= s_repeat_index_mask);
			BC_ASSERT(p_index <= s_index_size && p_index != s_invalid_index);

			return bc_actor_id{ p_index << 3 | p_repeat_index };
		}

		inline bc_actor_id bc_actor_id::compose_invalid_index(bcUINT32 p_repeat_index) noexcept
		{
			BC_ASSERT(p_repeat_index <= s_repeat_index_mask);

			return bc_actor_id(s_invalid_index << 3 | p_repeat_index);
		}
	}
}

namespace std
{
	template<>
	struct hash<black_cat::game::bc_actor_id>
	{
		using argument_type = black_cat::game::bc_actor_id;
		using result_type = std::size_t;

		result_type operator ()(const argument_type& p_arg) const noexcept
		{
			return p_arg.m_id;
		}
	};
}
