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
		template<bcSIZE TArgCount>
		struct _bc_script_free_function;

		template<>
		struct _bc_script_free_function<0>
		{
			using func = bc_script_variable(*)();
		};

		template<>
		struct _bc_script_free_function<1>
		{
			using member_func = bc_script_variable(*)(const bc_script_variable&);
		};

		template<>
		struct _bc_script_free_function<2>
		{
			using member_func = bc_script_variable(*)(const bc_script_variable&, const bc_script_variable&);
		};

		template<>
		struct _bc_script_free_function<3>
		{
			using member_func = bc_script_variable(*)(const bc_script_variable&, const bc_script_variable&, const bc_script_variable&);
		};

		template<>
		struct _bc_script_free_function<4>
		{
			using member_func = bc_script_variable(*)(const bc_script_variable&, const bc_script_variable&, const bc_script_variable&, const bc_script_variable&);
		};

		template<>
		struct _bc_script_free_function<5>
		{
			using member_func = bc_script_variable(*)(const bc_script_variable&, const bc_script_variable&, const bc_script_variable&, const bc_script_variable&, const bc_script_variable&);
		};

		template<>
		struct _bc_script_free_function<6>
		{
			using member_func = bc_script_variable(*)(const bc_script_variable&, const bc_script_variable&, const bc_script_variable&, const bc_script_variable&, const bc_script_variable&, const bc_script_variable&);
		};

		template<>
		struct _bc_script_free_function<7>
		{
			using member_func = bc_script_variable(*)(const bc_script_variable&, const bc_script_variable&, const bc_script_variable&, const bc_script_variable&, const bc_script_variable&, const bc_script_variable&, const bc_script_variable&);
		};

		template<>
		struct _bc_script_free_function<8>
		{
			using member_func = bc_script_variable(*)(const bc_script_variable&, const bc_script_variable&, const bc_script_variable&, const bc_script_variable&, const bc_script_variable&, const bc_script_variable&, const bc_script_variable&, const bc_script_variable&);
		};

		template<>
		struct _bc_script_free_function<9>
		{
			using member_func = bc_script_variable(*)(const bc_script_variable&, const bc_script_variable&, const bc_script_variable&, const bc_script_variable&, const bc_script_variable&, const bc_script_variable&, const bc_script_variable&, const bc_script_variable&, const bc_script_variable&);
		};
		
		template<typename T, bcSIZE TArgCount>
		struct _bc_script_member_function;

		template<typename T>
		struct _bc_script_member_function<T, 0>
		{
			using member_func = bc_script_variable(T::*)();
		};

		template<typename T>
		struct _bc_script_member_function<T, 1>
		{
			using member_func = bc_script_variable(T::*)(const bc_script_variable&);
		};

		template<typename T>
		struct _bc_script_member_function<T, 2>
		{
			using member_func = bc_script_variable(T::*)(const bc_script_variable&, const bc_script_variable&);
		};

		template<typename T>
		struct _bc_script_member_function<T, 3>
		{
			using member_func = bc_script_variable(T::*)(const bc_script_variable&, const bc_script_variable&, const bc_script_variable&);
		};

		template<typename T>
		struct _bc_script_member_function<T, 4>
		{
			using member_func = bc_script_variable(T::*)(const bc_script_variable&, const bc_script_variable&, const bc_script_variable&, const bc_script_variable&);
		};

		template<typename T>
		struct _bc_script_member_function<T, 5>
		{
			using member_func = bc_script_variable(T::*)(const bc_script_variable&, const bc_script_variable&, const bc_script_variable&, const bc_script_variable&, const bc_script_variable&);
		};

		template<typename T>
		struct _bc_script_member_function<T, 6>
		{
			using member_func = bc_script_variable(T::*)(const bc_script_variable&, const bc_script_variable&, const bc_script_variable&, const bc_script_variable&, const bc_script_variable&, const bc_script_variable&);
		};

		template<typename T>
		struct _bc_script_member_function<T, 7>
		{
			using member_func = bc_script_variable(T::*)(const bc_script_variable&, const bc_script_variable&, const bc_script_variable&, const bc_script_variable&, const bc_script_variable&, const bc_script_variable&, const bc_script_variable&);
		};

		template<typename T>
		struct _bc_script_member_function<T, 8>
		{
			using member_func = bc_script_variable(T::*)(const bc_script_variable&, const bc_script_variable&, const bc_script_variable&, const bc_script_variable&, const bc_script_variable&, const bc_script_variable&, const bc_script_variable&, const bc_script_variable&);
		};

		template<typename T>
		struct _bc_script_member_function<T, 9>
		{
			using member_func = bc_script_variable(T::*)(const bc_script_variable&, const bc_script_variable&, const bc_script_variable&, const bc_script_variable&, const bc_script_variable&, const bc_script_variable&, const bc_script_variable&, const bc_script_variable&, const bc_script_variable&);
		};
		
		template<typename T, typename TM>
		using bc_script_member_ptr = TM T::*;
		
		using bc_script_getter = bc_script_variable(*)();

		template<typename T>
		using bc_script_member_getter = bc_script_variable(T::*)();

		using bc_script_setter = bc_script_variable(*)(const bc_script_variable&);

		template<typename T>
		using bc_script_member_setter = bc_script_variable(T::*)(const bc_script_variable&);

		template<bcSIZE TArgCount>
		using bc_script_free_function = typename _bc_script_free_function<TArgCount>::func;

		template<typename T, bcSIZE TArgCount>
		using bc_script_member_function = typename _bc_script_member_function<T, TArgCount>::member_func;

		template<typename TM>
		class bc_script_property_descriptor
		{
		public:
			explicit bc_script_property_descriptor(TM* p_value, bool p_writable = true, bool p_enumerable = true, bool p_configurable = false)
				: m_value(p_value),
				m_writable(p_writable),
				m_enumerable(p_enumerable),
				m_configurable(p_configurable),
				m_getter(nullptr),
				m_setter(nullptr)
			{
			}

			bc_script_property_descriptor(bc_script_getter p_getter, bc_script_setter p_setter, bool p_enumerable = true, bool p_configurable = false)
				: m_value(nullptr),
				m_writable(false),
				m_enumerable(p_enumerable),
				m_configurable(p_configurable),
				m_getter(std::move(p_getter)),
				m_setter(std::move(p_setter))
			{
			}

			TM* m_value;
			bool m_writable;
			bool m_enumerable;
			bool m_configurable;
			bc_script_getter m_getter;
			bc_script_setter m_setter;
		};

		template<typename T, typename TM>
		class bc_script_member_property_descriptor
		{
		public:
			explicit bc_script_member_property_descriptor(TM T::* p_value, bool p_writable = true, bool p_enumerable = true, bool p_configurable = false)
				: m_value(p_value),
				m_writable(p_writable),
				m_enumerable(p_enumerable),
				m_configurable(p_configurable),
				m_getter(nullptr),
				m_setter(nullptr)
			{
			}

			bc_script_member_property_descriptor(bc_script_member_getter<T> p_getter, bc_script_member_setter<T> p_setter, bool p_enumerable = true, bool p_configurable = false)
				: m_value(nullptr),
				m_writable(false),
				m_enumerable(p_enumerable),
				m_configurable(p_configurable),
				m_getter(std::move(p_getter)),
				m_setter(std::move(p_setter))
			{
			}

			bc_script_member_ptr<T, TM> m_value;
			bool m_writable;
			bool m_enumerable;
			bool m_configurable;
			bc_script_member_getter<T> m_getter;
			bc_script_member_setter<T> m_setter;
		};

		template<core_platform::bc_platform TPlatform, typename T>
		struct bc_platform_script_prototype_builder_pack
		{
		};

		template<core_platform::bc_platform TPlatform, typename T>
		class bc_platform_script_prototype_builder
		{
		public:
			using platform_pack = bc_platform_script_prototype_builder_pack<TPlatform, T>;
			using type = T;
			friend bc_script_context;

		public:
			bc_platform_script_prototype_builder(bc_platform_script_prototype_builder&&) noexcept;

			~bc_platform_script_prototype_builder();

			bc_platform_script_prototype_builder& operator=(bc_platform_script_prototype_builder&&) noexcept;

			/**
			 * \brief Specify constructor function signature. 
			 * Defining constructor signature for new types is mandatory, but for objects that won't create
			 * in script runtime(eg global objects) there is no need to define constructor signature.
			 * Calling this function more than one time will throw bc_invalid_operation_exception.
			 * \tparam TArgCount Number of arguments which constructor accepts
			 * \return 
			 */
			template<bcSIZE TArgCount>
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
			bc_platform_script_prototype_builder& constant(const bcWCHAR* p_name, bc_script_bool p_bool);

			/**
			* \brief Define constant property in object prototype.
			* writable		: false
			* enumerable	: false
			* configurable	: false
			* \param p_name
			* \param p_integer
			* \return
			*/
			bc_platform_script_prototype_builder& constant(const bcWCHAR* p_name, bc_script_int p_integer);

			/**
			* \brief Define constant property in object prototype.
			* writable		: false
			* enumerable	: false
			* configurable	: false
			* \param p_name
			* \param p_double
			* \return
			*/
			bc_platform_script_prototype_builder& constant(const bcWCHAR* p_name, bc_script_double p_double);

			/**
			* \brief Define constant property in object prototype.
			* writable		: false
			* enumerable	: false
			* configurable	: false
			* \param p_name
			* \param p_string
			* \return
			*/
			bc_platform_script_prototype_builder& constant(const bcWCHAR* p_name, bc_script_string& p_string);

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
			bc_platform_script_prototype_builder& property(const bcWCHAR* p_name, bc_script_bool type::* p_bool);

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
			bc_platform_script_prototype_builder& property(const bcWCHAR* p_name, bc_script_int type::* p_int);

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
			bc_platform_script_prototype_builder& property(const bcWCHAR* p_name, bc_script_double type::* p_double);

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
			bc_platform_script_prototype_builder& property(const bcWCHAR* p_name, bc_script_string type::* p_string);

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
			bc_platform_script_prototype_builder& property(const bcWCHAR* p_name, bc_script_member_property_descriptor<type, bc_script_bool>& p_descriptor);

			/**
			* \brief Define property in object prototype.
			* \param p_name
			* \param p_descriptor
			* \return
			*/
			bc_platform_script_prototype_builder& property(const bcWCHAR* p_name, bc_script_member_property_descriptor<type, bc_script_int>& p_descriptor);

			/**
			* \brief Define property in object prototype.
			* \param p_name
			* \param p_descriptor
			* \return
			*/
			bc_platform_script_prototype_builder& property(const bcWCHAR* p_name, bc_script_member_property_descriptor<type, bc_script_double>& p_descriptor);

			/**
			* \brief Define property in object prototype.
			* \param p_name
			* \param p_descriptor
			* \return
			*/
			bc_platform_script_prototype_builder& property(const bcWCHAR* p_name, bc_script_member_property_descriptor<type, bc_script_string>& p_descriptor);

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
			 * \param p_member_func Callback function that is a member function of binding object
			 * \return 
			 */
			template<typename TR, typename ...TA>
			bc_platform_script_prototype_builder& function(const bcWCHAR* p_name, TR(T::* p_member_func)(const TA&...));

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

		template<typename T>
		using bc_script_prototype_builder = bc_platform_script_prototype_builder<core_platform::g_current_platform, T>;
	}
}