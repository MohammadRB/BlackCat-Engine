// [05/12/2016 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "Core/Container/bcString.h"
#include "Core/Container/bcVector.h"
#include "Core/Container/bcList.h"
#include "Core/bcException.h"
#include "Core/Utility/bcParameterPack.h"
#include "Core/File/bcJsonParse.h"

#include "3rdParty/RapidJSON/include/rapidjson/document.h"

namespace black_cat
{
	namespace core
	{
#define BC_JSON_VALUE(type, name)			black_cat::core::bc_json_value< type > m_##name { #name, this }
#define BC_JSON_VALUE_OP(type, name)		black_cat::core::bc_json_value< type > m_##name { #name, this, true }
#define BC_JSON_OBJECT(type, name)			black_cat::core::bc_json_object< type > m_##name { #name, this }
#define BC_JSON_OBJECT_OP(type, name)		black_cat::core::bc_json_object< type > m_##name { #name, this, true }
#define BC_JSON_ARRAY(type, name)			black_cat::core::bc_json_array< type > m_##name { #name, this }
#define BC_JSON_ARRAY_OP(type, name)		black_cat::core::bc_json_array< type > m_##name { #name, this, true }
#define BC_JSON_STRUCTURE(name)				struct name : public black_cat::core::bc_ijson_structure

		using bc_json_parse_object = rapidjson::Document::ValueType;

		class bc_ijson_value;

		class bc_ijson_structure
		{
		public:
			virtual ~bc_ijson_structure() = default;

			void parse(bc_json_parse_object& p_value);

			void add_parser(bc_ijson_value* p_parser);

		protected:

		private:
			bc_vector<bc_ijson_value*> m_parsers;
		};

		class bc_ijson_value
		{
		public:
			explicit bc_ijson_value(bc_ijson_structure* p_structure, bool p_optional);

			virtual ~bc_ijson_value() = default;

			virtual void parse(bc_json_parse_object& p_value) = 0;

		protected:
			bc_json_parse_object* _get_json_field(bc_json_parse_object& p_value, const bcCHAR* p_name) const;

			bool m_optional;
		};

		inline void bc_ijson_structure::parse(bc_json_parse_object& p_value)
		{
			for (auto l_parser : m_parsers)
			{
				l_parser->parse(p_value);
			}
		}

		inline void bc_ijson_structure::add_parser(bc_ijson_value* p_parser)
		{
			m_parsers.push_back(p_parser);
		}

		inline bc_ijson_value::bc_ijson_value(bc_ijson_structure* p_structure, bool p_optional)
			: m_optional(p_optional)
		{
			if (p_structure)
			{
				p_structure->add_parser(this);
			}
		}

		inline bc_json_parse_object* bc_ijson_value::_get_json_field(bc_json_parse_object& p_value, const bcCHAR* p_name) const
		{
			auto* l_value = p_value.HasMember(p_name) ? &p_value[p_name] : nullptr;

			if (!l_value && !m_optional)
			{
				bc_string_frame l_msg = "json key for non optional field '";
				l_msg += p_name;
				l_msg += "' not found";

				throw bc_io_exception(l_msg.c_str());
			}

			return l_value;
		}

		/**
		 * \brief Store json values as key value pair.
		 * If value is a json array, stored value will be bc_vector<bc_any>.
		 * If value is a json object, stored value will be bc_json_key_value.
		 */
		struct bc_json_key_value
		{
		public:
			using key_value_array_t = bc_vector< std::pair< bc_string, bc_any > >;

		public:
			key_value_array_t m_key_values;
		};

		/**
		 * \brief T can be fundamental types like bool, bcINT, bcUINT, bcFLOAT32, bc_string, bc_string_program, bc_string_level, 
		 * bc_string_frame, bc_json_key_value as well bc_expression_parameter and bc_parameter_pack that last one can hold all of 
		 * previous types.
		 * \tparam T 
		 */
		template< typename T >
		class bc_json_value : public bc_ijson_value
		{
		public:
			bc_json_value(const bcCHAR* p_name, bc_ijson_structure* p_structure, bool p_optional = false);

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
			void _parse(bool& p_value, bc_json_parse_object& p_json_value);

			void _parse(bcINT& p_value, bc_json_parse_object& p_json_value);

			void _parse(bcUINT& p_value, bc_json_parse_object& p_json_value);

			void _parse(bcFLOAT& p_value, bc_json_parse_object& p_json_value);

			void _parse(bc_string& p_value, bc_json_parse_object& p_json_value);

			void _parse(bc_string_program& p_value, bc_json_parse_object& p_json_value);

			void _parse(bc_string_frame& p_value, bc_json_parse_object& p_json_value);

			void _parse(bc_parameter_pack& p_value, bc_json_parse_object& p_json_value);

			void _parse(bc_any& p_value, bc_json_parse_object& p_json_value);

			void _parse(bc_json_key_value& p_value, bc_json_parse_object& p_json_value);

			template<typename T1>
			void _parse(T1& p_value, bc_json_parse_object& p_json_value);

			const bcCHAR* m_name;
			T m_value;
		};

		template< class T >
		class bc_json_object : public bc_ijson_value
		{
		public:
			bc_json_object(const bcCHAR* p_name, bc_ijson_structure* p_structure, bool p_optional = false);

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
			bc_json_array(const char* p_name, bc_ijson_structure* p_structure, bool p_optional = false);

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
				std::is_same< bc_string_frame, typename std::decay< T >::type >::value ||
				std::is_same< bc_parameter_pack, typename std::decay< T >::type >::value
			>::type
		> : public bc_ijson_value
		{
		public:
			bc_json_array(const bcCHAR* p_name, bc_ijson_structure* p_structure, bool p_optional = false);

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
		bc_json_value< T >::bc_json_value(const bcCHAR* p_name, bc_ijson_structure* p_structure, bool p_optional)
			: bc_ijson_value(p_structure, p_optional),
			m_name(p_name),
			m_value()
		{
			static_assert(std::is_default_constructible_v<T>, "T must be default constructible");
		}

		template< typename T >
		void bc_json_value< T >::parse(bc_json_parse_object& p_value)
		{
			// If we use json value within json array m_name will be null
			auto* l_value = m_name != nullptr ? _get_json_field(p_value, m_name) : &p_value;

			if(!l_value)
			{
				return;
			}

			_parse(m_value, *l_value);
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
		void bc_json_value< T >::_parse(bool& p_value, bc_json_parse_object& p_json_value)
		{
			if (!p_json_value.IsBool())
			{
				throw bc_io_exception("bad json format");
			}

			p_value = p_json_value.GetBool();
		}

		template< typename T >
		void bc_json_value< T >::_parse(bcINT& p_value, bc_json_parse_object& p_json_value)
		{
			if (!p_json_value.IsNumber())
			{
				throw bc_io_exception("bad json format");
			}

			p_value = p_json_value.GetInt();
		}

		template< typename T >
		void bc_json_value< T >::_parse(bcUINT& p_value, bc_json_parse_object& p_json_value)
		{
			if (!p_json_value.IsNumber())
			{
				throw bc_io_exception("bad json format");
			}

			p_value = p_json_value.GetUint();
		}

		template< typename T >
		void bc_json_value< T >::_parse(bcFLOAT& p_value, bc_json_parse_object& p_json_value)
		{
			if (!p_json_value.IsNumber())
			{
				throw bc_io_exception("bad json format");
			}

			p_value = p_json_value.GetFloat();
		}

		template< typename T >
		void bc_json_value< T >::_parse(bc_string& p_value, bc_json_parse_object& p_json_value)
		{
			if (!p_json_value.IsString())
			{
				throw bc_io_exception("bad json format");
			}

			p_value = p_json_value.GetString();
		}

		template< typename T >
		void bc_json_value< T >::_parse(bc_string_program& p_value, bc_json_parse_object& p_json_value)
		{
			if (!p_json_value.IsString())
			{
				throw bc_io_exception("bad json format");
			}

			p_value = p_json_value.GetString();
		}

		template< typename T >
		void bc_json_value< T >::_parse(bc_string_frame& p_value, bc_json_parse_object& p_json_value)
		{
			if (!p_json_value.IsString())
			{
				throw bc_io_exception("bad json format");
			}

			p_value = p_json_value.GetString();
		}

		template< typename T >
		void bc_json_value< T >::_parse(bc_parameter_pack& p_value, bc_json_parse_object& p_json_value)
		{
			if (p_json_value.IsBool())
			{
				p_value.set_value(p_json_value.GetBool());
			}
			else if (p_json_value.IsDouble())
			{
				p_value.set_value(p_json_value.GetDouble());
			}
			else if (p_json_value.IsFloat())
			{
				p_value.set_value(p_json_value.GetFloat());
			}
			else if (p_json_value.IsInt())
			{
				p_value.set_value(p_json_value.GetInt());
			}
			else if (p_json_value.IsInt64())
			{
				p_value.set_value(p_json_value.GetInt64());
			}
			else if (p_json_value.IsUint())
			{
				p_value.set_value(p_json_value.GetUint());
			}
			else if (p_json_value.IsUint64())
			{
				p_value.set_value(p_json_value.GetUint64());
			}
			else if (p_json_value.IsString())
			{
					p_value.set_value(bc_string(p_json_value.GetString()));
			}
			else if(p_json_value.IsObject())
			{
				bc_json_key_value l_value;
				_parse(l_value, p_json_value);

				p_value.set_value(std::move(l_value));
			}
			else if(p_json_value.IsArray())
			{
				auto l_array = p_json_value.GetArray();
				bc_vector<bc_any> l_values;

				l_values.reserve(l_array.Size());

				for(auto& l_array_item : l_array)
				{
					bc_any l_value;
					
					_parse(l_value, l_array_item);

					l_values.push_back(std::move(l_value));
				}

				p_value.set_value(std::move(l_values));
			}
			else
			{
				throw bc_io_exception("bad json format");
			}
		}

		template< typename T >
		void bc_json_value<T>::_parse(bc_any& p_value, bc_json_parse_object& p_json_value)
		{
			_parse(static_cast< bc_parameter_pack& >(p_value), p_json_value);
		}

		template< typename T >
		void bc_json_value<T>::_parse(bc_json_key_value& p_value, bc_json_parse_object& p_json_value)
		{
			if (p_json_value.IsObject())
			{
				p_value.m_key_values.reserve(p_json_value.GetObjectW().MemberCount());

				for (auto& l_object_member : p_json_value.GetObjectW())
				{
					bc_string l_key = l_object_member.name.GetString();
					bc_any l_value;

					_parse(l_value, l_object_member.value);

					p_value.m_key_values.push_back(std::make_pair(std::move(l_key), std::move(l_value)));
				}
			}
			else
			{
				throw bc_io_exception("bad json format");
			}
		}

		template< typename T >
		template< typename T1 >
		void bc_json_value<T>::_parse(T1& p_value, bc_json_parse_object& p_json_value)
		{
			json_parse::bc_parse(p_value, p_json_value);
		}

		// -- Json object --------------------------------------------------------------------------------

		template< class T >
		bc_json_object< T >::bc_json_object(const bcCHAR* p_name, bc_ijson_structure* p_structure, bool p_optional) 
			: bc_ijson_value(p_structure, p_optional),
			m_name(p_name),
			m_value()
		{
		}

		template< class T >
		void bc_json_object< T >::parse(bc_json_parse_object& p_value)
		{
			// If we use json object within json array m_name will be null
			auto* l_value = m_name != nullptr ? _get_json_field(p_value, m_name) : &p_value;

			if (!l_value)
			{
				return;
			}

			if (!l_value->IsObject())
			{
				throw bc_io_exception("bad json format");
			}

			m_value.parse(*l_value);
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
		bc_json_array< T, T1 >::bc_json_array(const char* p_name, bc_ijson_structure* p_structure, bool p_optional)
			: bc_ijson_value(p_structure, p_optional),
			m_name(p_name),
			m_value()
		{
		}

		template< typename T, typename T1 >
		void bc_json_array< T, T1 >::parse(bc_json_parse_object& p_value)
		{
			auto* l_value = _get_json_field(p_value, m_name);

			if (!l_value)
			{
				return;
			}

			if (!l_value->IsArray())
			{
				throw bc_io_exception("bad json format");
			}

			auto l_array = l_value->GetArray();
			for (auto& l_array_value : l_array)
			{
				// Because json objects are non-copy and movable we emplace one in array and then parse it in-place
				m_value.emplace_back(nullptr, nullptr);

				auto l_value = m_value.rbegin();

				l_value->parse(l_array_value);
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
		bc_json_array
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
				std::is_same< bc_string_frame, typename std::decay< T >::type >::value ||
				std::is_same< bc_parameter_pack, typename std::decay< T >::type >::value
			>::type
		>
		::bc_json_array(const char* p_name, bc_ijson_structure* p_structure, bool p_optional)
			: bc_ijson_value(p_structure, p_optional),
			m_name(p_name),
			m_value()
		{
		}

		template< typename T >
		void bc_json_array
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
				std::is_same< bc_string_frame, typename std::decay< T >::type >::value ||
				std::is_same< bc_parameter_pack, typename std::decay< T >::type >::value
			>::type
		>
		::parse(bc_json_parse_object& p_value)
		{
			auto* l_value = _get_json_field(p_value, m_name);

			if (!l_value)
			{
				return;
			}

			if (!l_value->IsArray())
			{
				throw bc_io_exception("bad json format");
			}

			auto l_array = l_value->GetArray();
			for (auto& l_array_value : l_array)
			{
				// Because json objects are non-copy and movable we emplace one in array and then parse it in-place
				m_value.emplace_back(nullptr, nullptr);

				auto l_value = m_value.rbegin();

				l_value->parse(l_array_value);
			}
		}

		template< typename T >
		bc_list< bc_json_value< T > >* bc_json_array
		<
			T,
			typename std::enable_if< std::is_same< bool, typename std::decay< T >::type >::value ||
				std::is_same< bcINT, typename std::decay< T >::type >::value ||
				std::is_same< bcUINT, typename std::decay< T >::type >::value ||
				std::is_same< bcFLOAT, typename std::decay< T >::type >::value ||
				std::is_same< bc_string, typename std::decay< T >::type >::value ||
				std::is_same< bc_string_program, typename std::decay< T >::type >::value ||
				std::is_same< bc_string_frame, typename std::decay< T >::type >::value ||
				std::is_same< bc_parameter_pack, typename std::decay< T >::type >::value
			>::type
		>
		::operator->() noexcept
		{
			return &m_value;
		}

		template< typename T >
		const bc_list< bc_json_value< T > >* bc_json_array
		<
			T,
			typename std::enable_if< std::is_same< bool, typename std::decay< T >::type >::value ||
				std::is_same< bcINT, typename std::decay< T >::type >::value ||
				std::is_same< bcUINT, typename std::decay< T >::type >::value ||
				std::is_same< bcFLOAT, typename std::decay< T >::type >::value ||
				std::is_same< bc_string, typename std::decay< T >::type >::value ||
				std::is_same< bc_string_program, typename std::decay< T >::type >::value ||
				std::is_same< bc_string_frame, typename std::decay< T >::type >::value ||
				std::is_same< bc_parameter_pack, typename std::decay< T >::type >::value
			>::type
		>
		::operator->() const noexcept
		{
			return operator->();
		}

		template< typename T >
		bc_list< bc_json_value< T > >& bc_json_array
		<
			T,
			typename std::enable_if< std::is_same< bool, typename std::decay< T >::type >::value ||
				std::is_same< bcINT, typename std::decay< T >::type >::value ||
				std::is_same< bcUINT, typename std::decay< T >::type >::value ||
				std::is_same< bcFLOAT, typename std::decay< T >::type >::value ||
				std::is_same< bc_string, typename std::decay< T >::type >::value ||
				std::is_same< bc_string_program, typename std::decay< T >::type >::value ||
				std::is_same< bc_string_frame, typename std::decay< T >::type >::value ||
				std::is_same< bc_parameter_pack, typename std::decay< T >::type >::value
			>::type
		>
		::operator*() noexcept
		{
			return m_value;
		}

		template< typename T >
		const bc_list< bc_json_value< T > >& bc_json_array
		<
			T,
			typename std::enable_if< std::is_same< bool, typename std::decay< T >::type >::value ||
				std::is_same< bcINT, typename std::decay< T >::type >::value ||
				std::is_same< bcUINT, typename std::decay< T >::type >::value ||
				std::is_same< bcFLOAT, typename std::decay< T >::type >::value ||
				std::is_same< bc_string, typename std::decay< T >::type >::value ||
				std::is_same< bc_string_program, typename std::decay< T >::type >::value ||
				std::is_same< bc_string_frame, typename std::decay< T >::type >::value ||
				std::is_same< bc_parameter_pack, typename std::decay< T >::type >::value
			>::type
		>
		::operator*() const noexcept
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
			{
				throw bc_io_exception("bad json format");
			}

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