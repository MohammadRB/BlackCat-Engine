// [01/22/2022 MRB]

#pragma once

#include "Game/Object/Scene/ActorComponent/bcActorComponent.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		enum class bc_icon_type : bcUBYTE
		{
			editor
		};

		class BC_GAME_DLL bc_icon_component : public bci_actor_component
		{
			BC_COMPONENT(icon, false, false)

		public:
			bc_icon_component(bc_actor_id p_actor_index, bc_actor_component_id p_index);

			bc_icon_component(bc_icon_component&& p_other) noexcept = default;

			~bc_icon_component() override = default;

			bc_icon_component& operator=(bc_icon_component&&) noexcept = default;

			bc_actor get_actor() const noexcept override;

			std::string_view get_name() const noexcept;

			bcUINT16 get_size() const noexcept;

			bc_icon_type get_type() const noexcept;

			void initialize(const bc_actor_component_initialize_context& p_context) override;

		private:
			core::bc_string m_name;
			bcUINT16 m_size;
			bc_icon_type m_type;
		};

		inline std::string_view bc_icon_component::get_name() const noexcept
		{
			return std::string_view(m_name);
		}

		inline bcUINT16 bc_icon_component::get_size() const noexcept
		{
			return m_size;
		}

		inline bc_icon_type bc_icon_component::get_type() const noexcept
		{
			return m_type;
		}
	}	
}
