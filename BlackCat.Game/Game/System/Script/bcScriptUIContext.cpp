 // [11/01/2016 MRB]

#include "Game/GamePCH.h"
#include "PlatformImp/Script/bcScriptGlobalPrototypeBuilder.h"
#include "PlatformImp/Script/bcScriptPrototypeBuilder.h"
#include "Game/System/Script/bcScriptUIContext.h"

namespace black_cat
{
	namespace game
	{
		bcINT bc_script_ui_context::s_count = 0;

		struct bc_script_point
		{
		public:
			bc_script_point()
				: m_x(0),
				m_y(0)
			{
			}

			bc_script_point(bcINT p_x, bcINT p_y)
				: m_x(p_x),
				m_y(p_y)
			{
			}

			bcINT get_sum()
			{
				return m_x + m_y;
			}

			void set_sum(const bcINT& p_sum)
			{
				m_x = p_sum / 2;
				m_y = p_sum / 2;
			}

			bcINT sum_with(const platform::bc_script_function<bcINT()>& p_function)
			{
				auto l_this = s_context->create_variable(s_context->get_global());
				return get_sum() + p_function(l_this);
			}

			bcINT m_x;
			bcINT m_y;

			static platform::bc_script_context* s_context;
		};

		platform::bc_script_context* bc_script_point::s_context = nullptr;

		bc_script_ui_context::bc_script_ui_context()
			: m_context()
		{
		}

		bc_script_ui_context::bc_script_ui_context(bc_script_ui_context&& p_other) noexcept
		{
			operator=(std::move(p_other));
		}

		bc_script_ui_context::~bc_script_ui_context()
		{
		}

		bc_script_ui_context& bc_script_ui_context::operator=(bc_script_ui_context&& p_other) noexcept
		{
			m_context = std::move(p_other.m_context);
			s_count = p_other.s_count;

			return *this;
		}

		bcINT bc_script_ui_context::get_count() noexcept
		{
			return s_count;
		}

		void bc_script_ui_context::set_count(const bcINT& p_count) noexcept
		{
			s_count = p_count;
		}

		void bc_script_ui_context::_initialize(platform::bc_script_runtime& p_runtime)
		{
			m_context = p_runtime.create_context();

			{
				platform::bc_script_context::scope l_scope(m_context.get());

				platform::bc_script_global_prototype_builder l_global_builder = m_context->create_global_prototype_builder();

				l_global_builder.constant(L"constCount", 10)
					.property(L"count", &s_count)
					.property(L"count1", platform::bc_script_property_descriptor<bcINT>(&get_count, &set_count))
					.function(L"getCount", &get_count)
					.function(L"setCount", &set_count);

				m_context->register_global_prototype(l_global_builder);

				bc_script_point::s_context = &m_context.get();
				platform::bc_script_prototype_builder< bc_script_point > l_point_builder = m_context->create_prototype_builder< bc_script_point >();

				l_point_builder.constructor<bcINT, bcINT>()
					.property(L"x", &bc_script_point::m_x)
					.property(L"y", &bc_script_point::m_y)
					.function(L"getSum", &bc_script_point::get_sum)
					.function(L"setSum", &bc_script_point::set_sum)
					.function(L"sumWith", &bc_script_point::sum_with);

				auto l_point_prototype = m_context->create_prototype(l_point_builder);
				m_context->register_prototype(L"bcPoint", l_point_prototype);
			}
		}

		void bc_script_ui_context::_destroy()
		{
			m_context.reset();
		}
	}
}