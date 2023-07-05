// [18/10/2016 MRB]

#pragma once

#include "Platform/Script/bcScriptContext.h"
#include "Platform/Script/bcScriptPrototypeBuilder.h"

namespace black_cat::platform
{
	template<bc_platform TPlatform>
	struct bc_platform_script_global_prototype_builder_pack
	{
	};

	template<bc_platform TPlatform>
	class bc_platform_script_global_prototype_builder
	{
	public:
		using platform_pack = bc_platform_script_global_prototype_builder_pack<TPlatform>;
		friend bc_script_context;

	public:
		bc_platform_script_global_prototype_builder(bc_platform_script_global_prototype_builder&&) noexcept;

		~bc_platform_script_global_prototype_builder();

		bc_platform_script_global_prototype_builder& operator=(bc_platform_script_global_prototype_builder&&) noexcept;

		/**
			* \brief Define constant property in object prototype.
			* writable		: false
			* enumerable	: false
			* configurable	: false
			* \param p_name
			* \param p_bool
			* \return
			*/
		bc_platform_script_global_prototype_builder& constant(const bcWCHAR* p_name, bc_script_bool p_bool);

		/**
			* \brief Define constant property in object prototype.
			* writable		: false
			* enumerable	: false
			* configurable	: false
			* \param p_name
			* \param p_integer
			* \return
			*/
		bc_platform_script_global_prototype_builder& constant(const bcWCHAR* p_name, bc_script_int p_integer);

		/**
			* \brief Define constant property in object prototype.
			* writable		: false
			* enumerable	: false
			* configurable	: false
			* \param p_name
			* \param p_double
			* \return
			*/
		bc_platform_script_global_prototype_builder& constant(const bcWCHAR* p_name, bc_script_double p_double);

		/**
			* \brief Define constant property in object prototype.
			* writable		: false
			* enumerable	: false
			* configurable	: false
			* \param p_name
			* \param p_string
			* \return
			*/
		bc_platform_script_global_prototype_builder& constant(const bcWCHAR* p_name, bc_script_string& p_string);

		/**
			* \brief Define property in object prototype with default descriptions.
			* writable		: true
			* enumerable	: true
			* configurable	: false
			* getter		: default
			* setter		: default
			* \param p_name
			* \param p_bool
			* \return
			*/
		bc_platform_script_global_prototype_builder& property(const bcWCHAR* p_name, bc_script_bool* p_bool);

		/**
			* \brief Define property in object prototype with default descriptions.
			* writable		: true
			* enumerable	: true
			* configurable	: false
			* getter		: default
			* setter		: default
			* \param p_name
			* \param p_int
			* \return
			*/
		bc_platform_script_global_prototype_builder& property(const bcWCHAR* p_name, bc_script_int* p_int);

		/**
			* \brief Define property in object prototype with default descriptions.
			* writable		: true
			* enumerable	: true
			* configurable	: false
			* getter		: default
			* setter		: default
			* \param p_name
			* \param p_double
			* \return
			*/
		bc_platform_script_global_prototype_builder& property(const bcWCHAR* p_name, bc_script_double* p_double);

		/**
			* \brief Define property in object prototype with default descriptions.
			* writable		: true
			* enumerable	: true
			* configurable	: false
			* getter		: default
			* setter		: default
			* \param p_name
			* \param p_string
			* \return
			*/
		bc_platform_script_global_prototype_builder& property(const bcWCHAR* p_name, bc_script_string* p_string);

		/**
			* \brief Define property in object prototype with default descriptions.
			* writable		: true
			* enumerable	: true
			* configurable	: false
			* getter		: default
			* setter		: default
			* \param p_name
			* \param p_object
			* \return
			*/
		bc_platform_script_global_prototype_builder& property(const bcWCHAR* p_name, bc_script_object* p_object);

		/**
			* \brief Define property in object prototype.
			* \param p_name
			* \param p_descriptor
			* \return
			*/
		bc_platform_script_global_prototype_builder& property(const bcWCHAR* p_name, bc_script_property_descriptor<bc_script_bool>& p_descriptor);

		/**
			* \brief Define property in object prototype.
			* \param p_name
			* \param p_descriptor
			* \return
			*/
		bc_platform_script_global_prototype_builder& property(const bcWCHAR* p_name, bc_script_property_descriptor<bc_script_int>& p_descriptor);

		/**
			* \brief Define property in object prototype.
			* \param p_name
			* \param p_descriptor
			* \return
			*/
		bc_platform_script_global_prototype_builder& property(const bcWCHAR* p_name, bc_script_property_descriptor<bc_script_double>& p_descriptor);

		/**
			* \brief Define property in object prototype.
			* \param p_name
			* \param p_descriptor
			* \return
			*/
		bc_platform_script_global_prototype_builder& property(const bcWCHAR* p_name, bc_script_property_descriptor<bc_script_string>& p_descriptor);

		/**
			* \brief Define property in object prototype.
			* \param p_name
			* \param p_descriptor
			* \return
			*/
		bc_platform_script_global_prototype_builder& property(const bcWCHAR* p_name, bc_script_property_descriptor<bc_script_object>& p_descriptor);

		/**
			* \brief Define function in object prototype.
			* \tparam TR Return type of function
			* \tparam TA Arguments of function
			* \param p_name Name of function
			* \param p_func Callback function
			* \return
			*/
		template<typename TR, typename ...TA>
		bc_platform_script_global_prototype_builder& function(const bcWCHAR* p_name, TR(*p_func)(const TA&...));

		platform_pack& get_platform_pack()
		{
			return m_pack;
		}

	protected:
		explicit bc_platform_script_global_prototype_builder(bc_script_context& p_context);

	private:
		bc_script_context& m_context;
		platform_pack m_pack;
	};

	using bc_script_global_prototype_builder = bc_platform_script_global_prototype_builder<g_current_platform>;
}
