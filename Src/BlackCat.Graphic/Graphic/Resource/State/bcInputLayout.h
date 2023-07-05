// [12/01/2016 MRB]

#pragma once

#include "Core/Container/bcVector.h"
#include "Graphic/GraphicPCH.h"
#include "Graphic/bcRenderApi.h"
#include "Graphic/bcGraphicDefinition.h"

namespace black_cat::graphic
{
	struct bc_input_element_desc
	{
		const bcCHAR* m_semantic_name;
		bcUINT m_semantic_index;
		bc_format m_format;
		bcUINT m_input_slot;
		bcUINT m_aligned_byte_offset;
		bc_input_classification m_input_slot_class;
		bcUINT m_instance_data_step_rate;
	};

	struct bc_input_layout_config
	{
		core::bc_vector_movable<bc_input_element_desc> m_input_elements;
	};

	/*template<bc_platform_render_api TRenderApi>
		struct bc_platform_input_layout_pack
		{
		};

		template<bc_platform_render_api TRenderApi>
		class bc_platform_input_layout : public platform::bc_no_copy
		{
		public:
			using platform_pack = bc_platform_input_layout_pack<TRenderApi>;

		public:
			platform_pack& get_platform_pack()
			{
				return m_pack;
			}

		protected:

		private:
			platform_pack m_pack;
		};

		class bc_input_layout : public bc_platform_input_layout<g_current_render_api>
		{};*/
}
