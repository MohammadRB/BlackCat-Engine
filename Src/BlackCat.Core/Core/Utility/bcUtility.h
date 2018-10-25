// [09/14/2016 MRB]

#pragma once

#include "Core/Utility/bcEnumOperand.h"
#include "Core/Utility/bcTemplateMetaType.h"

namespace black_cat
{
	template< typename T >
	using _dereference_t = std::remove_reference_t< decltype(*std::declval< T >()) >;

	template< typename TObject >
	auto bc_null_default(const TObject& p_object, _dereference_t<TObject> p_default) -> _dereference_t<TObject>
	{
		return p_object == nullptr ? p_default : *p_object;
	}

	inline bool bc_is_power_of_two(bcUINT p_number)
	{
		return p_number && !(p_number & (p_number - 1));
	}
}