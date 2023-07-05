// [04/05/2022 MRB]

#pragma once

#include "CorePlatform/bcType.h"

namespace black_cat::game
{
	struct bc_actor_id_data
	{
		bcUINT32 m_index;
		bcUINT32 m_manager_index;
		bcUINT32 m_repeat_index;
	};

	/**
		 * \brief Actor identifier which consist of three peace of data.
		 * \n First 3 bits is actor repeat index which indicate how many times a new actor is initialized in physical storage of actor.
		 * \n Second 2 bits is manager index which refers to actor component manager that owns this actor.
		 * \n Last 27 bits is actor index which refers to physical storage of actor.
		 */
	struct bc_actor_id
	{
	private:
		friend struct std::hash<bc_actor_id>;

		constexpr static bcUINT32 s_repeat_index_mask = 0b111;
		constexpr static bcUINT32 s_repeat_index_bit_size = 3;
		constexpr static bcUINT32 s_manager_index_mask = 0b11000;
		constexpr static bcUINT32 s_manager_index_bit_size = 2;
		constexpr static bcUINT32 s_index_mask = 0xffffffff & ~s_manager_index_mask & ~s_repeat_index_mask;
		constexpr static bcUINT32 s_invalid_index = s_index_mask >> (s_repeat_index_bit_size + s_manager_index_bit_size);
		constexpr static bcUINT32 s_invalid_id = s_invalid_index << (s_repeat_index_bit_size + s_manager_index_bit_size);

	public:
		constexpr static bcUINT32 s_repeat_index_size = s_repeat_index_mask + 1;
		constexpr static bcUINT32 s_manager_index_size = (s_manager_index_mask >> s_repeat_index_bit_size) + 1;
		constexpr static bcUINT32 s_index_size = s_invalid_index;

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
			return decompose_id(*this).m_index != s_invalid_index;
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

		static bcUINT32 generate_new_manager_id(bcUINT32 p_old_manager_id) noexcept;

		/**
			 * \brief Decompose actor id into actor index, actor manager id and actor repeat index
			 * \param p_id 
			 * \return 
			 */
		static bc_actor_id_data decompose_id(bc_actor_id p_id) noexcept;

		/**
			 * \brief Generate actor id from actor index, actor manager id and actor repeat index
			 * \param p_index
			 * \param p_manager_index 
			 * \param p_repeat_index 
			 * \return 
			 */
		static bc_actor_id compose_id(bcUINT32 p_index, bcUINT32 p_manager_index, bcUINT32 p_repeat_index) noexcept;

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
		return (p_old_repeat_id + 1) % s_repeat_index_size;
	}

	inline bcUINT32 bc_actor_id::generate_new_manager_id(bcUINT32 p_old_manager_id) noexcept
	{
		return (p_old_manager_id + 1) % s_manager_index_size;
	}

	inline bc_actor_id_data bc_actor_id::decompose_id(bc_actor_id p_id) noexcept
	{
		return
		{
			p_id.m_id >> (s_manager_index_bit_size + s_repeat_index_bit_size),
			(p_id.m_id & s_manager_index_mask) >> s_repeat_index_bit_size,
			p_id.m_id & s_repeat_index_mask
		};
	}

	inline bc_actor_id bc_actor_id::compose_id(bcUINT32 p_index, bcUINT32 p_manager_index, bcUINT32 p_repeat_index) noexcept
	{
		BC_ASSERT(p_index < s_index_size && p_index != s_invalid_index);
		BC_ASSERT(p_manager_index < s_manager_index_size);
		BC_ASSERT(p_repeat_index < s_repeat_index_size);
			
		return bc_actor_id((p_index << (s_repeat_index_bit_size + s_manager_index_bit_size)) | (p_manager_index << s_repeat_index_bit_size) | p_repeat_index);
	}

	inline bc_actor_id bc_actor_id::compose_invalid_index(bcUINT32 p_repeat_index) noexcept
	{
		BC_ASSERT(p_repeat_index < s_repeat_index_size);

		return bc_actor_id((s_invalid_index << (s_repeat_index_bit_size + s_manager_index_bit_size)) | p_repeat_index);
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
