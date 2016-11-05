// [10/09/2016 MRB]

#pragma once

#include "CorePlatform/bcPlatform.h"
#include "CorePlatform/bcType.h"
#include "Core/Container/bcString.h"
#include "Platform/Script/bcScriptContext.h"
#include "Platform/Script/bcScriptObject.h"

namespace black_cat
{
	namespace platform
	{
		template< typename TM >
		using bc_script_getter = TM(*)();

		template< typename T, typename TM >
		using bc_script_member_getter = TM(T::*)();

		template< typename TM >
		using bc_script_setter = void(*)(const TM&);

		template< typename T, typename TM >
		using bc_script_member_setter = void(T::*)(const TM&);

		template< typename TR, typename ...TA >
		using bc_script_free_function = TR(*)(const TA&...);

		template< typename T, typename TR, typename ...TA >
		using bc_script_memeber_function = TR(T::*)(const TA&...);

		template< typename TM >
		class bc_script_property_descriptor
		{
		public:
			bc_script_property_descriptor(TM p_value, bool p_writable = true, bool p_enumerable = true, bool p_configurable = false)
				: m_value(p_value),
				m_writable(p_writable),
				m_enumerable(p_enumerable),
				m_configurable(p_configurable),
				m_getter(),
				m_setter()
			{
			}

			bc_script_property_descriptor(bc_script_getter< TM > p_getter, bc_script_setter< TM > p_setter, bool p_enumerable = true, bool p_configurable = false)
				: m_value(),
				m_writable(false),
				m_enumerable(p_enumerable),
				m_configurable(p_configurable),
				m_getter(std::move(p_getter)),
				m_setter(std::move(p_setter))
			{
			}

			TM m_value;
			bool m_writable;
			bool m_enumerable;
			bool m_configurable;
			bc_script_getter< TM > m_getter;
			bc_script_setter< TM > m_setter;
		};

		template< typename T, typename TM >
		class bc_script_member_property_descriptor
		{
		public:
			bc_script_member_property_descriptor(TM p_value, bool p_writable = true, bool p_enumerable = true, bool p_configurable = false)
				: m_value(p_value),
				m_writable(p_writable),
				m_enumerable(p_enumerable),
				m_configurable(p_configurable),
				m_getter(),
				m_setter()
			{
			}

			bc_script_member_property_descriptor(bc_script_member_getter< T, TM > p_getter, bc_script_member_setter< T, TM > p_setter, bool p_enumerable = true, bool p_configurable = false)
				: m_value(),
				m_writable(false),
				m_enumerable(p_enumerable),
				m_configurable(p_configurable),
				m_getter(std::move(p_getter)),
				m_setter(std::move(p_setter))
			{
			}

			TM m_value;
			bool m_writable;
			bool m_enumerable;
			bool m_configurable;
			bc_script_member_getter< T, TM > m_getter;
			bc_script_member_setter< T, TM > m_setter;
		};

		template< core_platform::bc_platform TPlatform, typename T >
		struct bc_platform_script_prototype_builder_pack
		{
		};

		template< core_platform::bc_platform TPlatform, typename T >
		class bc_platform_script_prototype_builder
		{
		public:
			using platform_pack = bc_platform_script_prototype_builder_pack< TPlatform, T >;
			using type = T;
			friend bc_script_context;

		public:
			bc_platform_script_prototype_builder(bc_platform_script_prototype_builder&&) noexcept;

			~bc_platform_script_prototype_builder();

			bc_platform_script_prototype_builder& operator=(bc_platform_script_prototype_builder&&) noexcept;

			/**
			 * \brief Specify constructor function signature. Calling this function more than one time 
			 * will throw bc_invalid_operation_exception
			 * \tparam TA Arguments to constructor function
			 * \return 
			 */
			template< typename ...TA >
			bc_platform_script_prototype_builder& constructor();

			/**
			* \brief Define constant property in object prototype.
			* writable		: false
			* enumerable	: false
			* configurable	: false
			* \param p_name
			* \param p_bool
			* \return
			*/
			bc_platform_script_prototype_builder& constant(const bcWCHAR* p_name, bool p_bool);

			/**
			* \brief Define constant property in object prototype.
			* writable		: false
			* enumerable	: false
			* configurable	: false
			* \param p_name
			* \param p_integer
			* \return
			*/
			bc_platform_script_prototype_builder& constant(const bcWCHAR* p_name, bcINT p_integer);

			/**
			* \brief Define constant property in object prototype.
			* writable		: false
			* enumerable	: false
			* configurable	: false
			* \param p_name
			* \param p_double
			* \return
			*/
			bc_platform_script_prototype_builder& constant(const bcWCHAR* p_name, bcDOUBLE p_double);

			/**
			* \brief Define constant property in object prototype.
			* writable		: false
			* enumerable	: false
			* configurable	: false
			* \param p_name
			* \param p_string
			* \return
			*/
			bc_platform_script_prototype_builder& constant(const bcWCHAR* p_name, const core::bc_string& p_string);

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
			bc_platform_script_prototype_builder& property(const bcWCHAR* p_name, bool type::* p_bool);

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
			bc_platform_script_prototype_builder& property(const bcWCHAR* p_name, bcINT type::* p_int);

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
			bc_platform_script_prototype_builder& property(const bcWCHAR* p_name, bcDOUBLE type::* p_double);

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
			bc_platform_script_prototype_builder& property(const bcWCHAR* p_name, core::bc_string type::* p_string);

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
			bc_platform_script_prototype_builder& property(const bcWCHAR* p_name, bc_script_object type::* p_object);

			/**
			 * \brief Define property in object prototype.
			 * \param p_name 
			 * \param p_descriptor 
			 * \return 
			 */
			bc_platform_script_prototype_builder& property(const bcWCHAR* p_name, bc_script_member_property_descriptor<type, bool>& p_descriptor);

			/**
			* \brief Define property in object prototype.
			* \param p_name
			* \param p_descriptor
			* \return
			*/
			bc_platform_script_prototype_builder& property(const bcWCHAR* p_name, bc_script_member_property_descriptor<type, bcINT>& p_descriptor);

			/**
			* \brief Define property in object prototype.
			* \param p_name
			* \param p_descriptor
			* \return
			*/
			bc_platform_script_prototype_builder& property(const bcWCHAR* p_name, bc_script_member_property_descriptor<type, bcDOUBLE>& p_descriptor);

			/**
			* \brief Define property in object prototype.
			* \param p_name
			* \param p_descriptor
			* \return
			*/
			bc_platform_script_prototype_builder& property(const bcWCHAR* p_name, bc_script_member_property_descriptor<type, core::bc_string>& p_descriptor);

			/**
			* \brief Define property in object prototype.
			* \param p_name
			* \param p_descriptor
			* \return
			*/
			bc_platform_script_prototype_builder& property(const bcWCHAR* p_name, bc_script_member_property_descriptor<type, bc_script_object>& p_descriptor);

			/**
			 * \brief Define function in object prototype. Callback function must be a member function of binding object.
			 * \tparam TR Return type of function
			 * \tparam TA Arguments of function
			 * \param p_name Name of function
			 * \param member_func Callback function that is a member function of binding object
			 * \return 
			 */
			template< typename TR, typename ...TA >
			bc_platform_script_prototype_builder& function(const bcWCHAR* p_name, bc_script_memeber_function<type, TR, TA...> p_member_func);

			platform_pack& get_platform_pack()
			{
				return m_pack;
			}

		protected:
			bc_platform_script_prototype_builder(bc_script_context& p_context);

		private:
			bc_script_context& m_context;
			platform_pack m_pack;
		};

		template< typename T >
		using bc_script_prototype_builder = bc_platform_script_prototype_builder< core_platform::g_current_platform, T >;
	}
}