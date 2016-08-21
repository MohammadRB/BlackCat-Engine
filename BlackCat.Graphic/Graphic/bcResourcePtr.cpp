// [04/16/2016 MRB]

#include "Graphic/GraphicPCH.h"
#include "Graphic/bcResourcePtr.h"
#include "Graphic/Device/bcDevice.h"

namespace black_cat
{
	namespace graphic
	{
		_bc_resource_ptr_deleter::_bc_resource_ptr_deleter() : m_device(nullptr)
		{
		}

		_bc_resource_ptr_deleter::_bc_resource_ptr_deleter(bc_device* p_device) : m_device(p_device)
		{
		}

		void _bc_resource_ptr_deleter::operator()(bc_device_object* p_resource) const
		{
			bcAssert(m_device != nullptr);

			m_device->destroy_resource(p_resource);
		}
	}
}