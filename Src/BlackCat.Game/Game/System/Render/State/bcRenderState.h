// [09/06/2016 MRB]

#pragma once

#include "Core/Container/bcArray.h"
#include "Core/Utility/bcRefCountPtr.h"
#include "GraphicImp/Resource/Buffer/bcBuffer.h"
#include "GraphicImp/Resource/Texture/bcTexture2d.h"
#include "GraphicImp/Resource/View/bcResourceView.h"
#include "Game/System/Render/State/bcVertexLayout.h"
#include "Game/System/Render/bcRenderSystemParameter.h"
#include "Game/bcExport.h"

namespace black_cat::game
{
	class bc_render_system;
	class bc_render_thread;
	class bc_render_state;
		
	class BC_GAME_DLL _bc_render_state_handle_deleter
	{
	public:
		_bc_render_state_handle_deleter();

		_bc_render_state_handle_deleter(bc_render_system* p_render_system);

		void operator ()(bc_render_state* p_render_state) const;

	private:
		bc_render_system* m_render_system;
	};

	/**
	 * \brief Non-mutable object that represent part of device pipeline states which are specific to render instance
	 * \n Shader parameters will be mapped to device registers as they appear in their array
	 */
	class BC_GAME_DLL bc_render_state 
			: public core::bc_ref_count,
			  platform::bc_no_copy
	{
	public:
		template< class T, class TDeleter >
		friend class core::bc_ref_count_handle;
		friend class bc_render_system;
		friend class bc_render_thread;

	public:
		bc_render_state(bc_render_state&& p_other) noexcept;

		~bc_render_state() = default;

		bc_render_state& operator=(bc_render_state&& p_other) noexcept;

		graphic::bc_primitive get_primitive() const
		{
			return m_primitive;
		}

		const graphic::bc_buffer& get_vertex_buffer() const
		{
			return m_vertex_buffer;
		}

		bcUINT32 get_vertex_buffer_stride() const
		{
			return m_vertex_buffer_stride;
		}

		bcUINT32 get_vertex_buffer_offset() const
		{
			return m_vertex_buffer_offset;
		}

		const graphic::bc_buffer& get_index_buffer() const
		{
			return m_index_buffer;
		}

		bc_index_type get_index_type() const
		{
			return m_index_type;
		}

		bcUINT32 get_index_count() const
		{
			return m_index_count;
		}

		bcUINT32 get_index_buffer_offset() const
		{
			return m_index_buffer_offset;
		}

		const bc_render_state_resource_view_array& get_shader_views() const
		{
			return m_resource_views;
		}

		const bc_render_state_constant_buffer_array& get_shader_buffers() const
		{
			return m_shader_cbuffers;
		}

	private:
		bc_render_state(graphic::bc_primitive p_primitive,
		                graphic::bc_buffer p_vertex_buffer,
		                bcUINT32 p_vertex_buffer_stride,
		                bcUINT32 p_vertex_buffer_offset,
		                graphic::bc_buffer p_index_buffer,
		                bc_index_type p_index_type,
		                bcUINT32 p_index_count,
		                bcUINT32 p_index_buffer_offset,
		                bc_render_state_resource_view_array&& p_shader_views,
		                bc_render_state_constant_buffer_array&& p_shader_buffers);

		graphic::bc_primitive m_primitive;
		graphic::bc_buffer m_vertex_buffer;
		bcUINT32 m_vertex_buffer_stride;
		bcUINT32 m_vertex_buffer_offset;
		graphic::bc_buffer m_index_buffer;
		bc_index_type m_index_type;
		bcUINT32 m_index_count;
		bcUINT32 m_index_buffer_offset;
		bc_render_state_resource_view_array m_resource_views;
		bc_render_state_constant_buffer_array m_shader_cbuffers;
	};

	using bc_render_state_ptr = core::bc_ref_count_ptr< bc_render_state, _bc_render_state_handle_deleter >;
}
