// [11/09/2020 MRB]

#pragma once

#include "Core/Container/bcString.h"
#include "Core/Container/bcUnorderedMap.h"
#include "Core/Event/bcEvent.h"
#include "Core/Event/bcEventListenerHandle.h"
#include "Core/Utility/bcServiceManager.h"
#include "Core/bcExport.h"

namespace black_cat
{
	namespace core
	{
		/**
		 * \brief Store global counter and watch values. \n
		 * At the end of every frame values become available for read. \n
		 * <b>ThreadSafe</b>
		 */
		class BC_CORE_DLL bc_counter_value_manager : public bc_iservice
		{
			BC_SERVICE(cv_mng)

		private:
			using container_t = bc_unordered_map<const bcCHAR*, bc_wstring>;

		public:
			using value_type = container_t::value_type;
			using pointer = container_t::pointer;
			using const_pointer = container_t::const_pointer;
			using reference = container_t::reference;
			using const_reference = container_t::const_reference;
			using difference_type = container_t::difference_type;
			using size_type = container_t::size_type;
			using iterator = container_t::iterator;
			using const_iterator = container_t::const_iterator;
			
		public:
			bc_counter_value_manager();

			bc_counter_value_manager(bc_counter_value_manager&&) = delete;

			~bc_counter_value_manager();

			bc_counter_value_manager& operator=(bc_counter_value_manager&&) = delete;
			
			const_iterator begin() const noexcept;

			const_iterator cbegin() const noexcept;

			const_iterator end() const noexcept;

			const_iterator cend() const noexcept;

			size_type size() const noexcept;
			
			void add_counter(const bcCHAR* p_name, bc_wstring p_value);

			const_iterator find(const bcCHAR* p_name) const noexcept;
			
		private:
			bool _event_handler(bc_ievent& p_event);
			
			bcUINT32 m_read_container;
			bcUINT32 m_write_container;
			container_t m_container[2];

			bc_event_listener_handle m_swap_frame_event_handle;
		};
	}
}