// [03/23/2016 MRB]

#pragma once

namespace black_cat
{
	namespace core
	{
		template< typename ...TArgs >
		class bc_initializable
		{
		public:
			void initialize(TArgs... p_args)
			{
				// Because TArgs can be rvalue reference we must forward it
				_initialize(std::forward<TArgs>(p_args)...);
				m_initialized = true;
			}

			void destroy()
			{
				_destroy();
				m_initialized = false;
			}

			bool is_initialized() const
			{
				return m_initialized;
			}

		protected:
			bc_initializable()
				: m_initialized(false)
			{
			}

			~bc_initializable() = default;

			virtual void _initialize(TArgs... pArgs) = 0;

			virtual void _destroy() = 0;

		protected:
			bool m_initialized;

		private:
		};
	}
}