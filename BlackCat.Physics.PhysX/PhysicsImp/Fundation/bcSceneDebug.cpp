// [12/16/2016 MRB]

#include "PhysicsImp/PhysicsImpPCH.h"
#include "PhysicsImp/bcExport.h"
#include "PhysicsImp/Fundation/bcSceneDebug.h"

namespace black_cat
{
	namespace physics
	{
		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_scene_debug<g_api_physx>::bc_platform_scene_debug() noexcept
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_scene_debug<g_api_physx>::bc_platform_scene_debug(bc_platform_scene_debug&& p_other) noexcept
		{
			operator=(std::move(p_other));
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_scene_debug<g_api_physx>::~bc_platform_scene_debug()
		{
		}

		template<>
		BC_PHYSICSIMP_DLL
		bc_platform_scene_debug<g_api_physx>& bc_platform_scene_debug<g_api_physx>::operator=(bc_platform_scene_debug&& p_other) noexcept
		{
			m_pack.m_px_debug = p_other.m_pack.m_px_debug;
			p_other.m_pack.m_px_debug = nullptr;

			return *this;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bcUINT32 bc_platform_scene_debug<g_api_physx>::get_line_count() const noexcept
		{
			return m_pack.m_px_debug->getNbLines();
		}

		template<>
		BC_PHYSICSIMP_DLL
		core::bc_vector< bc_scene_debug_line > bc_platform_scene_debug<g_api_physx>::get_lines() const noexcept
		{
			core::bc_vector< bc_scene_debug_line > result;
			auto* l_px_lines = m_pack.m_px_debug->getLines();
			auto l_line_count = get_line_count();

			result.reserve(l_line_count);

			for (bcUINT32 i = 0; i < l_line_count; ++i)
			{
				result.push_back(bc_scene_debug_line
				(
					core::bc_vector3f(l_px_lines->pos0.x, l_px_lines->pos0.y, l_px_lines->pos0.z),
					l_px_lines->color0,
					core::bc_vector3f(l_px_lines->pos1.x, l_px_lines->pos1.y, l_px_lines->pos1.z),
					l_px_lines->color1
				));
			}

			return result;
		}

		template<>
		BC_PHYSICSIMP_DLL
		bcUINT32 bc_platform_scene_debug<g_api_physx>::get_text_count() const noexcept
		{
			return m_pack.m_px_debug->getNbTexts();
		}

		template<>
		BC_PHYSICSIMP_DLL
		core::bc_vector< bc_scene_debug_text > bc_platform_scene_debug<g_api_physx>::get_texts() const noexcept
		{
			core::bc_vector< bc_scene_debug_text > result;
			auto* l_px_texts = m_pack.m_px_debug->getTexts();
			auto l_text_count = get_text_count();

			result.reserve(l_text_count);

			for (bcUINT32 i = 0; i < l_text_count; ++i)
			{
				result.push_back(bc_scene_debug_text
				(
					core::bc_vector3f(l_px_texts->position.x, l_px_texts->position.y, l_px_texts->position.z),
					l_px_texts->color,
					l_px_texts->size,
					l_px_texts->string
				));
			}

			return result;
		}
	}
}