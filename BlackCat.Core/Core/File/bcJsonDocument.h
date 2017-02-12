// [05/12/2016 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "Core/Container/bcString.h"
#include "Core/Container/bcList.h"
#include "Core/bcException.h"
#include "Core/Utility/bcParameterPack.h"
#include "Core/Utility/bcExpressionParameter.h"

#include "3rdParty/RapidJSON/include/rapidjson/document.h"

namespace black_cat
{
	namespace core
	{
#define BC_JSON_VALUE(type, name)	black_cat::core::bc_json_value< type > m_##name { #name, this }
#define BC_JSON_OBJECT(type, name)	black_cat::core::bc_json_object< type > m_##name { #name, this }
#define BC_JSON_ARRAY(type, name)	black_cat::core::bc_json_array< type > m_##name { #name, this }
#define BC_JSON_STRUCTURE(name)		struct name : public black_cat::core::bc_ijson_structure

		using bc_json_parse_object = rapidjson::Document::ValueType;

		class bc_ijson_value;

		class bc_ijson_structure
		{
		public:
			virtual ~bc_ijson_structure() = default;

			void parse(bc_json_parse_object& p_value);

			void add_parser(bc_ijson_value* p_jparser);

		protected:

		private:
			std::vector<bc_ijson_value*> m_parsers;
		};

		class bc_ijson_value
		{
		public:
			explicit bc_ijson_value(bc_ijson_structure* p_jstructure);

			virtual ~bc_ijson_value() = default;

			virtual void parse(bc_json_parse_object& p_value) = 0;
		};

		inline bc_ijson_value::bc_ijson_value(bc_ijson_structure* p_jstructure)
		{
			if (p_jstructure)
			{
				p_jstructure->add_parser(this);
			}
		}

		inline void bc_ijson_structure::parse(bc_json_parse_object& p_value)
		{
			for (auto l_parser : m_parsers)
			{
				l_parser->parse(p_value);
			}
		}

		inline void bc_ijson_structure::add_parser(bc_ijson_value* p_jparser)
		{
			m_parsers.push_back(p_jparser);
		}

		struct bc_json_key_value
		{
		public:
			using key_value_array_t = bc_vector< std::pair< bc_string, bc_any > >;

		public:
			key_value_array_t m_key_values;
		};

		// T can be fundamental types like bool, bcINT, bcUINT, bcFLOAT32, bc_string, bc_string_program, bc_string_level, 
		// bc_string_frame as well bc_expression_parameter and bc_parameter_pack that last one can hold all of previous 
		// types.
		template< typename T >
		class bc_json_value : public bc_ijson_value
		{
		public:
			bc_json_value(const bcCHAR* p_name, bc_ijson_structure* p_jstructure);

			bc_json_value(const bc_json_value&) noexcept(std::is_nothrow_copy_constructible<T>::value) = delete;

			bc_json_value(bc_json_value&&) noexcept(std::is_nothrow_move_constructible<T>::value) = delete;

			~bc_json_value() = default;

			bc_json_value& operator=(const bc_json_value&) noexcept(std::is_nothrow_copy_assignable<T>::value) = delete;

			bc_json_value& operator=(bc_json_value&&) noexcept(std::is_nothrow_move_assignable<T>::value) = delete;

			void parse(bc_json_parse_object& p_value) override;

			T& get() noexcept;

			const T& get() const noexcept;

			void set(T p_value) noexcept;

			T* operator->() noexcept;

			const T* operator->() const noexcept;

			T& operator*() noexcept;

			const T& operator*() const noexcept;

		protected:

		private:
			void _parse(bool* p_value, bc_json_parse_object& p_jvalue);

			void _parse(bcINT* p_value, bc_json_parse_object& p_jvalue);

			void _parse(bcUINT* p_value, bc_json_parse_object& p_jvalue);

			void _parse(bcFLOAT* p_value, bc_json_parse_object& p_jvalue);

			void _parse(bc_string* p_value, bc_json_parse_object& p_jvalue);

			void _parse(bc_string_program* p_value, bc_json_parse_object& p_jvalue);

			void _parse(bc_string_level* p_value, bc_json_parse_object& p_jvalue);

			void _parse(bc_string_frame* p_value, bc_json_parse_object& p_jvalue);

			void _parse(bc_expression_parameter* p_value, bc_json_parse_object& p_jvalue);

			void _parse(bc_parameter_pack* p_value, bc_json_parse_object& p_jvalue);

			void _parse(bc_json_key_value* p_value, bc_json_parse_object& p_jvalue);

			const bcCHAR* m_name;
			T m_value;
		};

		template< class T >
		class bc_json_object : public bc_ijson_value
		{
		public:
			bc_json_object(const bcCHAR* p_name, bc_ijson_structure* p_jstructure);

			bc_json_object(const bc_json_object&) noexcept(std::is_nothrow_copy_constructible<T>::value) = delete;

			bc_json_object(bc_json_object&&) noexcept(std::is_nothrow_move_constructible<T>::value) = delete;

			~bc_json_object() = default;

			bc_json_object& operator=(const bc_json_object&) noexcept(std::is_nothrow_copy_assignable<T>::value) = delete;

			bc_json_object& operator=(bc_json_object&&) noexcept(std::is_nothrow_move_assignable<T>::value) = delete;

			void parse(bc_json_parse_object& p_value) override;

			T& get() noexcept;

			const T& get() const noexcept;

			void set(T p_value) noexcept;

			T* operator->() noexcept;

			const T* operator->() const noexcept;

			T& operator*() noexcept;

			const T& operator*() const noexcept;

		protected:

		private:
			const bcCHAR* m_name;
			T m_value;
		};

		template< typename T, typename T1 = T >
		class bc_json_array : public bc_ijson_value
		{
		public:
			bc_json_array(const char* p_name, bc_ijson_structure* p_jstructure);

			bc_json_array(const bc_json_array&) noexcept(std::is_nothrow_copy_constructible<T>::value) = default;

			bc_json_array(bc_json_array&&) noexcept(std::is_nothrow_move_constructible<T>::value) = default;
			
			~bc_json_array() = default;

			bc_json_array& operator=(const bc_json_array&) noexcept(std::is_nothrow_copy_assignable<T>::value) = default;

			bc_json_array& operator=(bc_json_array&&) noexcept(std::is_nothrow_move_assignable<T>::value) = default;

			void parse(bc_json_parse_object& p_value) override;

			bc_list< bc_json_object< T > >* operator->() noexcept;

			const bc_list< bc_json_object< T > >* operator->() const noexcept;

			bc_list< bc_json_object< T > >& operator*() noexcept;

			const bc_list< bc_json_object< T > >& operator*() const noexcept;

		protected:

		private:
			const bcCHAR* m_name;
			bc_list< bc_json_object< T > > m_value; // Because json objects are non-copy and movable we used list instead of vector
		};

		/*template< typename T >
		struct _bc_json_array_remap { using type = T; };

		template<>
		struct _bc_json_array_remap<bool> { using type = void; };

		template<>
		struct _bc_json_array_remap<bcINT> { using type = void; };

		template<>
		struct _bc_json_array_remap<bcUINT> { using type = void; };

		template<>
		struct _bc_json_array_remap<bcFLOAT32> { using type = void; };

		template<>
		struct _bc_json_array_remap<bc_string> { using type = void; };

		template<>
		struct _bc_json_array_remap<bc_string_program> { using type = void; };

		template<>
		struct _bc_json_array_remap<bc_string_level> { using type = void; };

		template<>
		struct _bc_json_array_remap<bc_string_frame> { using type = void; };

		template<>
		struct _bc_json_array_remap<bc_expression_parameter> { using type = void; };

		template<>
		struct _bc_json_array_remap<bc_parameter_pack> { using type = void; };*/

		template< typename T >
		class bc_json_array
		<
			T,
			typename std::enable_if
			<
				std::is_same< bool, typename std::decay< T >::type >::value ||
				std::is_same< bcINT, typename std::decay< T >::type >::value ||
				std::is_same< bcUINT, typename std::decay< T >::type >::value ||
				std::is_same< bcFLOAT, typename std::decay< T >::type >::value ||
				std::is_same< bc_string, typename std::decay< T >::type >::value ||
				std::is_same< bc_string_program, typename std::decay< T >::type >::value ||
				std::is_same< bc_string_level, typename std::decay< T >::type >::value ||
				std::is_same< bc_string_frame, typename std::decay< T >::type >::value ||
				std::is_same< bc_expression_parameter, typename std::decay< T >::type >::value ||
				std::is_same< bc_parameter_pack, typename std::decay< T >::type >::value
			>::type
		> : public bc_ijson_value
		{
		public:
			bc_json_array(const char* p_name, bc_ijson_structure* p_jstructure);

			bc_json_array(const bc_json_array&) noexcept(std::is_nothrow_copy_constructible<T>::value) = delete;

			bc_json_array(bc_json_array&&) noexcept(std::is_nothrow_move_constructible<T>::value) = delete;

			~bc_json_array() = default;

			bc_json_array& operator=(const bc_json_array&) noexcept(std::is_nothrow_copy_assignable<T>::value) = delete;

			bc_json_array& operator=(bc_json_array&&) noexcept(std::is_nothrow_move_assignable<T>::value) = delete;

			void parse(bc_json_parse_object& p_value) override;

			bc_list< bc_json_value< T > >* operator->() noexcept;

			const bc_list< bc_json_value< T > >* operator->() const noexcept;

			bc_list< bc_json_value< T > >& operator*() noexcept;

			const bc_list< bc_json_value< T > >& operator*() const noexcept;

		protected:

		private:
			const bcCHAR* m_name;
			bc_list< bc_json_value< T > > m_value; // Because json objects are non-copy and movable we used list instead of vector
		};

		template< typename T >
		class bc_json_document
		{
		public:
			bc_json_document() = default;

			bc_json_document(const bc_json_document&) noexcept(std::is_nothrow_copy_constructible<T>::value) = delete;

			bc_json_document(bc_json_document&&) noexcept(std::is_nothrow_move_constructible<T>::value) = delete;

			~bc_json_document() = default;

			bc_json_document& operator=(const bc_json_document&) noexcept(std::is_nothrow_copy_assignable<T>::value) = delete;

			bc_json_document& operator=(bc_json_document&&) noexcept(std::is_nothrow_move_assignable<T>::value) = delete;

			void parse(const bcCHAR* p_json);

			T& get() noexcept;

			const T& get() const noexcept;

			void set(T p_value) noexcept;

			T* operator->() noexcept;

			const T* operator->() const noexcept;

			T& operator*() noexcept;

			const T& operator*() const noexcept;

		protected:

		private:
			T m_value;
		};

		// -- Json value --------------------------------------------------------------------------------

		template< typename T >
		bc_json_value< T >::bc_json_value(const bcCHAR* p_name, bc_ijson_structure* p_jstructure) : bc_ijson_value(p_jstructure),
			m_name(p_name),
			m_value()
		{
		}

		template< typename T >
		void bc_json_value< T >::parse(bc_json_parse_object& p_value)
		{
			// If we use json value within json array m_name will be null
			auto& l_jvalue = m_name != nullptr ? p_value[m_name] : p_value;

			_parse(&m_value, l_jvalue);
		}

		template< typename T >
		T& bc_json_value< T >::get() noexcept
		{
			return m_value;
		}

		template< typename T >
		const T& bc_json_value< T >::get() const noexcept
		{
			return const_cast<bc_json_value&>(*this).get();
		}

		template< typename T >
		void bc_json_value< T >::set(T p_value) noexcept
		{
			m_value = p_value;
		}

		template< typename T >
		T* bc_json_value< T >::operator->() noexcept
		{
			return &get();
		}

		template< typename T >
		const T* bc_json_value< T >::operator->() const noexcept
		{
			return operator->();
		}

		template< typename T >
		T& bc_json_value< T >::operator*() noexcept
		{
			return get();
		}

		template< typename T >
		const T& bc_json_value< T >::operator*() const noexcept
		{
			return operator*();
		}

		template< typename T >
		void bc_json_value< T >::_parse(bool* p_value, bc_json_parse_object& p_jvalue)
		{
			if (!p_jvalue.IsBool())
				throw bc_io_exception("bad json format");

			*p_value = p_jvalue.GetBool();
		}

		template< typename T >
		void bc_json_value< T >::_parse(bcINT* p_value, bc_json_parse_object& p_jvalue)
		{
			if (!p_jvalue.IsNumber())
				throw bc_io_exception("bad json format");

			*p_value = p_jvalue.GetInt();
		}

		template< typename T >
		void bc_json_value< T >::_parse(bcUINT* p_value, bc_json_parse_object& p_jvalue)
		{
			if (!p_jvalue.IsNumber())
				throw bc_io_exception("bad json format");

			*p_value = p_jvalue.GetUint();
		}

		template< typename T >
		void bc_json_value< T >::_parse(bcFLOAT* p_value, bc_json_parse_object& p_jvalue)
		{
			if (!p_jvalue.IsNumber())
				throw bc_io_exception("bad json format");

			*p_value = p_jvalue.GetFloat();
		}

		template< typename T >
		void bc_json_value< T >::_parse(bc_string* p_value, bc_json_parse_object& p_jvalue)
		{
			if (!p_jvalue.IsString())
				throw bc_io_exception("bad json format");

			*p_value = p_jvalue.GetString();
		}

		template< typename T >
		void bc_json_value< T >::_parse(bc_string_program* p_value, bc_json_parse_object& p_jvalue)
		{
			if (!p_jvalue.IsString())
				throw bc_io_exception("bad json format");

			*p_value = p_jvalue.GetString();
		}

		template< typename T >
		void bc_json_value< T >::_parse(bc_string_level* p_value, bc_json_parse_object& p_jvalue)
		{
			if (!p_jvalue.IsString())
				throw bc_io_exception("bad json format");

			*p_value = p_jvalue.GetString();
		}

		template< typename T >
		void bc_json_value< T >::_parse(bc_string_frame* p_value, bc_json_parse_object& p_jvalue)
		{
			if (!p_jvalue.IsString())
				throw bc_io_exception("bad json format");

			*p_value = p_jvalue.GetString();
		}

		template< typename T >
		void bc_json_value<T>::_parse(bc_expression_parameter* p_value, bc_json_parse_object& p_jvalue)
		{
			if (!p_jvalue.IsString())
				throw bc_io_exception("bad json format");

			p_value->compile(p_jvalue.GetString());
		}

		template< typename T >
		void bc_json_value< T >::_parse(bc_parameter_pack* p_value, bc_json_parse_object& p_jvalue)
		{
			if (p_jvalue.IsBool())
			{
				p_value->set_value(p_jvalue.GetBool());
			}
			else if (p_jvalue.IsDouble())
			{
				p_value->set_value(p_jvalue.GetDouble());
			}
			else if (p_jvalue.IsFloat())
			{
				p_value->set_value(p_jvalue.GetFloat());
			}
			else if (p_jvalue.IsInt())
			{
				p_value->set_value(p_jvalue.GetInt());
			}
			else if (p_jvalue.IsInt64())
			{
				p_value->set_value(p_jvalue.GetInt64());
			}
			else if (p_jvalue.IsUint())
			{
				p_value->set_value(p_jvalue.GetUint());
			}
			else if (p_jvalue.IsUint64())
			{
				p_value->set_value(p_jvalue.GetUint64());
			}
			else if (p_jvalue.IsString())
			{
				if(bc_expression_parameter::is_expression_parameter(p_jvalue.GetString()))
				{
					bc_expression_parameter l_expression;
					l_expression.compile(p_jvalue.GetString());

					p_value->set_value(std::move(l_expression));
				}
				else
				{
					p_value->set_value(bc_string(p_jvalue.GetString()));
				}
			}
			else
			{
				throw bc_io_exception("bad json format");
			}
		}

		template< typename T >
		void bc_json_value<T>::_parse(bc_json_key_value* p_value, bc_json_parse_object& p_jvalue)
		{
			if (p_jvalue.IsObject())
			{
				p_value->m_key_values.reserve(p_jvalue.GetObjectW().MemberCount());

				for (auto& l_jobject_memeber : p_jvalue.GetObjectW())
				{
					bc_string l_key = l_jobject_memeber.name.GetString();
					bc_any l_value;

					_parse(&l_value, l_jobject_memeber.value);

					p_value->m_key_values.push_back(std::make_pair(std::move(l_key), std::move(l_value)));
				}
			}
			else
			{
				throw bc_io_exception("bad json format");
			}
		}

		// -- Json object --------------------------------------------------------------------------------

		template< class T >
		bc_json_object< T >::bc_json_object(const bcCHAR* p_name, bc_ijson_structure* p_jstructure) : bc_ijson_value(p_jstructure),
			m_name(p_name),
			m_value()
		{
		}

		template< class T >
		void bc_json_object< T >::parse(bc_json_parse_object& p_value)
		{
			// If we use json object within json array m_name will be null
			auto& l_jvalue = m_name != nullptr ? p_value[m_name] : p_value;

			if (!l_jvalue.IsObject())
				throw bc_io_exception("bad json format");

			m_value.parse(l_jvalue);
		}

		template< class T >
		T& bc_json_object< T >::get() noexcept
		{
			return m_value;
		}

		template< class T >
		const T& bc_json_object<T>::get() const noexcept
		{
			return const_cast<bc_json_object&>(*this).get();
		}

		template< class T >
		void bc_json_object< T >::set(T p_value) noexcept
		{
			m_value = p_value;
		}

		template< class T >
		T* bc_json_object< T >::operator->() noexcept
		{
			return &get();
		}

		template< class T >
		const T* bc_json_object< T >::operator->() const noexcept
		{
			return operator->();
		}

		template< class T >
		T& bc_json_object< T >::operator*() noexcept
		{
			return get();
		}

		template< class T >
		const T& bc_json_object< T >::operator*() const noexcept
		{
			return operator*();
		}

		// -- Json array --------------------------------------------------------------------------------

		template< typename T, typename T1 >
		bc_json_array< T, T1 >::bc_json_array(const char* p_name, bc_ijson_structure* p_jstructure) 
			: bc_ijson_value(p_jstructure),
			m_name(p_name),
			m_value()
		{
		}

		template< typename T, typename T1 >
		void bc_json_array< T, T1 >::parse(bc_json_parse_object& p_value)
		{
			auto& l_jvalue = p_value[m_name];

			if (!l_jvalue.IsArray())
				throw bc_io_exception("bad json format");

			auto l_jarray = l_jvalue.GetArray();
			for (auto& l_jarray_value : l_jarray)
			{
				// Because json objects are non-copy and movable we emplace one in array and then parse it in-place
				m_value.emplace_back(nullptr, nullptr);

				auto l_value = m_value.rbegin();

				l_value->parse(l_jarray_value);
			}
		}

		template< typename T, typename T1 >
		bc_list< bc_json_object< T > >* bc_json_array< T, T1 >::operator->() noexcept
		{
			return &m_value;
		}

		template< typename T, typename T1 >
		const bc_list< bc_json_object< T > >* bc_json_array< T, T1 >::operator->() const noexcept
		{
			return operator->();
		}

		template< typename T, typename T1 >
		bc_list< bc_json_object< T > >& bc_json_array< T, T1 >::operator*() noexcept
		{
			return m_value;
		}

		template< typename T, typename T1 >
		const bc_list< bc_json_object< T > >& bc_json_array< T, T1 >::operator*() const noexcept
		{
			return operator*();
		}

		template< typename T >
		bc_json_array<T, typename std::enable_if<std::is_same<bool, typename std::decay<T>::type>::value || std::is_same<bcINT, typename std::decay<T>::type>::value || std::is_same<bcUINT, typename std::decay<T>::type>::value || std::is_same<bcFLOAT, typename std::decay<T>::type>::value || std::is_same<bc_string, typename std::decay<T>::type>::value || std::is_same<bc_string_program, typename std::decay<T>::type>::value || std::is_same<bc_string_level, typename std::decay<T>::type>::value || std::is_same<bc_string_frame, typename std::decay<T>::type>::value || std::is_same<bc_expression_parameter, typename std::decay<T>::type>::value || std::is_same<bc_parameter_pack, typename std::decay<T>::type>::value>::type>::bc_json_array(const char* p_name, bc_ijson_structure* p_jstructure)
			: bc_ijson_value(p_jstructure),
			m_name(p_name),
			m_value()
		{
		}

		template< typename T >
		void bc_json_array<T, typename std::enable_if<std::is_same<bool, typename std::decay<T>::type>::value || std::is_same<bcINT, typename std::decay<T>::type>::value || std::is_same<bcUINT, typename std::decay<T>::type>::value || std::is_same<bcFLOAT, typename std::decay<T>::type>::value || std::is_same<bc_string, typename std::decay<T>::type>::value || std::is_same<bc_string_program, typename std::decay<T>::type>::value || std::is_same<bc_string_level, typename std::decay<T>::type>::value || std::is_same<bc_string_frame, typename std::decay<T>::type>::value || std::is_same<bc_expression_parameter, typename std::decay<T>::type>::value || std::is_same<bc_parameter_pack, typename std::decay<T>::type>::value>::type>::parse(bc_json_parse_object& p_value)
		{
			auto& l_jvalue = p_value[m_name];

			if (!l_jvalue.IsArray())
				throw bc_io_exception("bad json format");

			auto l_jarray = l_jvalue.GetArray();
			for (auto& l_jarray_value : l_jarray)
			{
				// Because json objects are non-copy and movable we emplace one in array and then parse it in-place
				m_value.emplace_back(nullptr, nullptr);

				auto l_value = m_value.rbegin();

				l_value->parse(l_jarray_value);
			}
		}

		template< typename T >
		bc_list< bc_json_value< T > >* bc_json_array<T, typename std::enable_if<std::is_same<bool, typename std::decay<T>::type>::value || std::is_same<bcINT, typename std::decay<T>::type>::value || std::is_same<bcUINT, typename std::decay<T>::type>::value || std::is_same<bcFLOAT, typename std::decay<T>::type>::value || std::is_same<bc_string, typename std::decay<T>::type>::value || std::is_same<bc_string_program, typename std::decay<T>::type>::value || std::is_same<bc_string_level, typename std::decay<T>::type>::value || std::is_same<bc_string_frame, typename std::decay<T>::type>::value || std::is_same<bc_expression_parameter, typename std::decay<T>::type>::value || std::is_same<bc_parameter_pack, typename std::decay<T>::type>::value>::type>::operator->() noexcept
		{
			return &m_value;
		}

		template< typename T >
		const bc_list< bc_json_value< T > >* bc_json_array<T, typename std::enable_if<std::is_same<bool, typename std::decay<T>::type>::value || std::is_same<bcINT, typename std::decay<T>::type>::value || std::is_same<bcUINT, typename std::decay<T>::type>::value || std::is_same<bcFLOAT, typename std::decay<T>::type>::value || std::is_same<bc_string, typename std::decay<T>::type>::value || std::is_same<bc_string_program, typename std::decay<T>::type>::value || std::is_same<bc_string_level, typename std::decay<T>::type>::value || std::is_same<bc_string_frame, typename std::decay<T>::type>::value || std::is_same<bc_expression_parameter, typename std::decay<T>::type>::value || std::is_same<bc_parameter_pack, typename std::decay<T>::type>::value>::type>::operator->() const noexcept
		{
			return operator->();
		}

		template< typename T >
		bc_list< bc_json_value< T > >& bc_json_array<T, typename std::enable_if<std::is_same<bool, typename std::decay<T>::type>::value || std::is_same<bcINT, typename std::decay<T>::type>::value || std::is_same<bcUINT, typename std::decay<T>::type>::value || std::is_same<bcFLOAT, typename std::decay<T>::type>::value || std::is_same<bc_string, typename std::decay<T>::type>::value || std::is_same<bc_string_program, typename std::decay<T>::type>::value || std::is_same<bc_string_level, typename std::decay<T>::type>::value || std::is_same<bc_string_frame, typename std::decay<T>::type>::value || std::is_same<bc_expression_parameter, typename std::decay<T>::type>::value || std::is_same<bc_parameter_pack, typename std::decay<T>::type>::value>::type>::operator*() noexcept
		{
			return m_value;
		}

		template< typename T >
		const bc_list< bc_json_value< T > >& bc_json_array<T, typename std::enable_if<std::is_same<bool, typename std::decay<T>::type>::value || std::is_same<bcINT, typename std::decay<T>::type>::value || std::is_same<bcUINT, typename std::decay<T>::type>::value || std::is_same<bcFLOAT, typename std::decay<T>::type>::value || std::is_same<bc_string, typename std::decay<T>::type>::value || std::is_same<bc_string_program, typename std::decay<T>::type>::value || std::is_same<bc_string_level, typename std::decay<T>::type>::value || std::is_same<bc_string_frame, typename std::decay<T>::type>::value || std::is_same<bc_expression_parameter, typename std::decay<T>::type>::value || std::is_same<bc_parameter_pack, typename std::decay<T>::type>::value>::type>::operator*() const noexcept
		{
			return operator*();
		}

		// -- Json document --------------------------------------------------------------------------------

		template< typename T >
		void bc_json_document< T >::parse(const bcCHAR* p_json)
		{
			rapidjson::Document l_json;
			l_json.Parse(p_json);

			if (!l_json.IsObject())
				throw bc_io_exception("bad json format");

			m_value.parse(l_json);
		}

		template< typename T >
		T& bc_json_document< T >::get() noexcept
		{
			return m_value;
		}

		template< typename T >
		const T& bc_json_document<T>::get() const noexcept
		{
			return const_cast<bc_json_document&>(*this).get();
		}

		template< typename T >
		void bc_json_document< T >::set(T p_value) noexcept
		{
			m_value = p_value;
		}

		template< typename T >
		T* bc_json_document< T >::operator->() noexcept
		{
			return &get();
		}

		template< typename T >
		const T* bc_json_document< T >::operator->() const noexcept
		{
			return operator->();
		}

		template< typename T >
		T& bc_json_document< T >::operator*() noexcept
		{
			return get();
		}

		template< typename T >
		const T& bc_json_document< T >::operator*() const noexcept
		{
			return operator*();
		}
	}
}
