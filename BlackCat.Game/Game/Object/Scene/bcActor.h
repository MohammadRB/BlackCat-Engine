// [04/27/2016 MRB]

#pragma once

#include "CorePlatform/bcType.h"

namespace black_cat
{
	namespace game
	{
		class bc_actor_component_manager;
		using bc_actor_index = bcINT32;
		constexpr bc_actor_index g_actor_invalid_index = bc_actor_index(-1);

		class bc_actor
		{
		private:
			friend class bc_actor_component_manager;

		public:
			bc_actor();

			bc_actor(const bc_actor&) noexcept;

			~bc_actor();

			bc_actor& operator=(const bc_actor&) noexcept;

			template< class TComponent >
			void create_component();

			template< class TComponent >
			void remove_component();

			template< class TComponent >
			bool has_component() const;

			template< class TComponent >
			TComponent* get_component();

			template< class TComponent >
			const TComponent* get_component() const;

			void destroy();

			bool operator==(const bc_actor& p_other) const noexcept;

			bool operator!=(const bc_actor& p_other) const noexcept;

		protected:
			bc_actor_index get_index() const noexcept;

		private:
			bc_actor(bc_actor_component_manager* p_manager, bc_actor_index p_index);

			bc_actor_component_manager* m_manager;
			bc_actor_index m_index;
		};		
	}
}