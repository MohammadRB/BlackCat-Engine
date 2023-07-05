// [21/02/2021 MRB]

#pragma once

#include "Core/Utility/bcRefCountPtr.h"
#include "GraphicImp/Resource/Buffer/bcBuffer.h"
#include "Game/System/Render/Material/bcMeshMaterial.h"
#include "Game/System/Render/State/bcRenderState.h"
#include "Game/System/Render/bcRenderInstance.h"

namespace black_cat::game
{
	class bc_decal;
	class bc_decal_manager;

	class bc_decal_deleter
	{
	public:
		bc_decal_deleter();
			
		explicit bc_decal_deleter(bc_decal_manager& p_manager);
			
		void operator()(bc_decal* p_ptr) const;

	private:
		bc_decal_manager* m_manager;
	};

	/**
		 * \brief Basic info of a decal. Custom cbuffer and render state can be associated with every instance.
		 */
	class bc_decal : public core::bc_ref_count
	{
	public:
		bc_decal(core::bc_string_view p_name,
		         bc_mesh_material_ptr p_material, 
		         bcFLOAT p_u0, 
		         bcFLOAT p_v0, 
		         bcFLOAT p_u1, 
		         bcFLOAT p_v1,
		         bcFLOAT p_width,
		         bcFLOAT p_height,
		         bcFLOAT p_depth,
		         bcFLOAT p_view_distance,
		         bc_actor_render_group p_group,
		         bool p_auto_remove);

		bc_decal(bc_decal&&) noexcept;

		~bc_decal();

		bc_decal& operator=(bc_decal&&) noexcept;

		core::bc_string_view get_name() const noexcept;
			
		const bc_mesh_material& get_material() const noexcept;

		bcFLOAT get_u0() const noexcept;
			
		bcFLOAT get_v0() const noexcept;
			
		bcFLOAT get_u1() const noexcept;
			
		bcFLOAT get_v1() const noexcept;

		bcFLOAT get_width() const noexcept;

		bcFLOAT get_height() const noexcept;
			
		bcFLOAT get_depth() const noexcept;
			
		bcFLOAT get_view_distance() const noexcept;

		bc_actor_render_group get_group() const noexcept;

		bool get_auto_remove() const noexcept;

	private:
		core::bc_string_view m_name;
		bc_mesh_material_ptr m_material;
		bcFLOAT m_u0;
		bcFLOAT m_v0;
		bcFLOAT m_u1;
		bcFLOAT m_v1;
		bcFLOAT m_width;
		bcFLOAT m_height;
		bcFLOAT m_depth;
		bcFLOAT m_view_distance;
		bc_actor_render_group m_group;
		bool m_auto_remove;
	};

	using bc_decal_ptr = core::bc_ref_count_ptr<bc_decal, bc_decal_deleter>;
		
	inline bc_decal_deleter::bc_decal_deleter()
		: m_manager(nullptr)
	{
	}

	inline bc_decal_deleter::bc_decal_deleter(bc_decal_manager& p_manager)
		: m_manager(&p_manager)
	{
	}

	inline bc_decal::bc_decal(core::bc_string_view p_name,
	                          bc_mesh_material_ptr p_material,
	                          bcFLOAT p_u0,
	                          bcFLOAT p_v0,
	                          bcFLOAT p_u1,
	                          bcFLOAT p_v1,
	                          bcFLOAT p_width,
	                          bcFLOAT p_height,
	                          bcFLOAT p_depth,
	                          bcFLOAT p_view_distance,
	                          bc_actor_render_group p_group,
	                          bool p_auto_remove)
		: m_name(p_name),
		  m_material(std::move(p_material)),
		  m_u0(p_u0),
		  m_v0(p_v0),
		  m_u1(p_u1),
		  m_v1(p_v1),
		  m_width(p_width),
		  m_height(p_height),
		  m_depth(p_depth),
		  m_view_distance(p_view_distance),
		  m_group(p_group),
		  m_auto_remove(p_auto_remove)
	{
	}

	inline bc_decal::bc_decal(bc_decal&&) noexcept = default;

	inline bc_decal::~bc_decal() = default;

	inline bc_decal& bc_decal::operator=(bc_decal&&) noexcept = default;

	inline core::bc_string_view bc_decal::get_name() const noexcept
	{
		return m_name;
	}
		
	inline const bc_mesh_material& bc_decal::get_material() const noexcept
	{
		return *m_material;
	}

	inline bcFLOAT bc_decal::get_width() const noexcept
	{
		return m_width;
	}

	inline bcFLOAT bc_decal::get_height() const noexcept
	{
		return m_height;
	}

	inline bcFLOAT bc_decal::get_depth() const noexcept
	{
		return m_depth;
	}

	inline bcFLOAT bc_decal::get_view_distance() const noexcept
	{
		return m_view_distance;
	}
		
	inline bcFLOAT bc_decal::get_u0() const noexcept
	{
		return m_u0;
	}

	inline bcFLOAT bc_decal::get_v0() const noexcept
	{
		return m_v0;
	}

	inline bcFLOAT bc_decal::get_u1() const noexcept
	{
		return m_u1;
	}

	inline bcFLOAT bc_decal::get_v1() const noexcept
	{
		return m_v1;
	}

	inline bc_actor_render_group bc_decal::get_group() const noexcept
	{
		return m_group;
	}

	inline bool bc_decal::get_auto_remove() const noexcept
	{
		return m_auto_remove;
	}
}
