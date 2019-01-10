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
			bc_initializable() = default;

			bc_initializable(bc_initializable&& p_other) noexcept
			{
				operator=(std::move(p_other));
			}

			~bc_initializable() = default;

			bc_initializable& operator=(bc_initializable&& p_other) noexcept
			{
				m_initialized = p_other.m_initialized;
				p_other.m_initialized = false;

				return *this;
			}

			virtual void _initialize(TArgs... pArgs) = 0;

			virtual void _destroy() = 0;

		protected:
			bool m_initialized = false;

		private:
		};
	}
}