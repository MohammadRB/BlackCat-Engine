// [22/01/2022 MRB]

#pragma once

#include "Game/Object/Scene/ActorComponent/bcActorComponent.h"
#include "Game/bcExport.h"

namespace black_cat::game
{
	enum class bc_icon_mode : bcUBYTE
	{
		editor
	};

	struct bc_icon_type
	{
		static constexpr const bcCHAR* player = "player";
		static constexpr const bcCHAR* sun = "sun";
		static constexpr const bcCHAR* light = "light";
		static constexpr const bcCHAR* cloud = "cloud";
		static constexpr const bcCHAR* wind = "wind";
		static constexpr const bcCHAR* sound = "sound";
		static constexpr const bcCHAR* particle = "particle";
		static constexpr const bcCHAR* decal = "decal";
		static constexpr const bcCHAR* fog = "fog";
	};

	class BC_GAME_DLL bc_icon_component : public bci_actor_component
	{
		BC_COMPONENT(icon, false, false)

	public:
		static constexpr bcUINT16 s_default_icon_size = 26;

	public:
		bc_icon_component(bc_actor_id p_actor_id, bc_actor_component_id p_id);

		bc_icon_component(bc_icon_component&& p_other) noexcept = default;

		~bc_icon_component() override = default;

		bc_icon_component& operator=(bc_icon_component&&) noexcept = default;

		bc_actor get_actor() const noexcept override;

		bool has_icon() const noexcept;

		std::string_view get_name() const noexcept;

		bcUINT16 get_size() const noexcept;

		bc_icon_mode get_mode() const noexcept;

		void initialize(const bc_actor_component_initialize_context& p_context) override;

		/**
		 * \brief set icon with default size and 'editor' mode
		 * \param p_name 
		 */
		void set_icon(core::bc_string p_name) noexcept;

		void set_icon(core::bc_string p_name, bcUINT16 p_size, bc_icon_mode p_mode) noexcept;

	private:
		core::bc_string m_name;
		bcUINT16 m_size;
		bc_icon_mode m_mode;
	};

	inline bool bc_icon_component::has_icon() const noexcept
	{
		return !m_name.empty();
	}

	inline std::string_view bc_icon_component::get_name() const noexcept
	{
		return std::string_view(m_name);
	}

	inline bcUINT16 bc_icon_component::get_size() const noexcept
	{
		return m_size;
	}

	inline bc_icon_mode bc_icon_component::get_mode() const noexcept
	{
		return m_mode;
	}
}
