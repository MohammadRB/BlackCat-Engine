// [12/05/2016 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "Core/Container/bcString.h"
#include "Core/Container/bcStringStream.h"
#include "Core/Container/bcVector.h"
#include "Core/Container/bcList.h"
#include "Core/Container/bcIteratorAdapter.h"
#include "Core/bcException.h"
#include "Core/Utility/bcParameterPack.h"
#include "Core/Utility/bcTemplateMetaType.h"
#include "Core/File/bcJsonParse.h"
#include "3rdParty/RapidJSON/Include/writer.h"
#include "3rdParty/RapidJSON/Include/prettywriter.h"

namespace black_cat
{
	namespace core
	{
#define BC_JSON_VALUE(type, name)			black_cat::core::bc_json_value<type>	m_##name { #name, this }
#define BC_JSON_VALUE_OP(type, name)		black_cat::core::bc_json_value<type>	m_##name { #name, this, true }
#define BC_JSON_OBJECT(type, name)			black_cat::core::bc_json_object<type>	m_##name { #name, this }
#define BC_JSON_OBJECT_OP(type, name)		black_cat::core::bc_json_object<type>	m_##name { #name, this, true }
#define BC_JSON_ARRAY(type, name)			black_cat::core::bc_json_array<type>	m_##name { #name, this }
#define BC_JSON_ARRAY_OP(type, name)		black_cat::core::bc_json_array<type>	m_##name { #name, this, true }
#define BC_JSON_STRUCTURE(type)				struct type : public black_cat::core::bci_json_structure

		class bci_json_value;

		class bci_json_structure
		{
		public:
			virtual ~bci_json_structure() = default;

			void load(bc_json_value_object& p_json_value);

			void write(bc_json_document_object& p_document, bc_json_value_object& p_json_value);

			void _add_json_value(bci_json_value* p_parser);

		private:
			bc_vector<bci_json_value*> m_json_fields;
		};

		class bci_json_value
		{
		public:
			virtual ~bci_json_value() = default;

			bool is_optional() const noexcept;

			bool has_value() const noexcept;

			virtual void load(bc_json_value_object& p_json_value) = 0;

			virtual void write(bc_json_document_object& p_document, bc_json_value_object& p_json_value) = 0;

		protected:
			bci_json_value(bci_json_structure* p_structure, bool p_optional);

			bc_json_value_object* get_json_field(bc_json_value_object& p_json, const bcCHAR* p_name) const;

			bc_json_value_object* set_json_field(bc_json_document_object& p_document, bc_json_value_object& p_json, const bcCHAR* p_name) const;

			bool m_optional;
			bool m_had_value;
		};

		inline void bci_json_structure::load(bc_json_value_object& p_json_value)
		{
			for (auto* l_field : m_json_fields)
			{
				l_field->load(p_json_value);
			}
		}

		inline void bci_json_structure::write(bc_json_document_object& p_document, bc_json_value_object& p_json_value)
		{
			for (auto* l_field : m_json_fields)
			{
				l_field->write(p_document, p_json_value);
			}
		}

		inline void bci_json_structure::_add_json_value(bci_json_value* p_parser)
		{
			m_json_fields.push_back(p_parser);
		}

		inline bool bci_json_value::is_optional() const noexcept
		{
			return m_optional;
		}

		inline bool bci_json_value::has_value() const noexcept
		{
			return m_had_value;
		}

		inline bci_json_value::bci_json_value(bci_json_structure* p_structure, bool p_optional)
			: m_optional(p_optional),
			m_had_value(false)
		{
			if (p_structure)
			{
				p_structure->_add_json_value(this);
			}
		}

		inline bc_json_value_object* bci_json_value::get_json_field(bc_json_value_object& p_json, const bcCHAR* p_name) const
		{
			auto* l_value = p_json.HasMember(p_name) ? &p_json[p_name] : nullptr;

			if (!l_value && !m_optional)
			{
				bc_string_frame l_msg = "json key for non optional field '";
				l_msg += p_name;
				l_msg += "' not found";

				throw bc_io_exception(l_msg.c_str());
			}

			return l_value;
		}

		inline bc_json_value_object* bci_json_value::set_json_field(bc_json_document_object& p_document, bc_json_value_object& p_json, const bcCHAR* p_name) const
		{
			bc_json_value_object* l_result;

			if (p_json.HasMember(p_name))
			{
				l_result = &p_json[p_name];
			}
			else
			{
				bc_json_value_object l_name(rapidjson::kStringType);
				bc_json_value_object l_value(rapidjson::kNullType);
				l_name.SetString(p_name, std::strlen(p_name));

				p_json.AddMember(l_name, l_value, p_document.GetAllocator());
				l_result = &p_json[p_name];
			}

			return l_result;
		}

		/**
		 * \brief Store json values as key value pair.
		 * If value is a json array, stored value will be bc_vector<bc_any>.
		 * If value is a json object, stored value will be bc_json_key_value.
		 */
		class bc_json_key_value
		{
		public:
			using key_value_array = bc_vector<std::pair<bc_string, bc_any>>;
			using value_type = key_value_array::value_type;
			using pointer = key_value_array::pointer;
			using const_pointer = key_value_array::const_pointer;
			using reference = key_value_array::reference;
			using const_reference = key_value_array::const_reference;
			using difference_type = key_value_array::difference_type;
			using size_type = key_value_array::size_type;
			using iterator = key_value_array::iterator;
			using const_iterator = key_value_array::const_iterator;

			bc_json_key_value() = default;

			bc_json_key_value(const bc_json_key_value&) noexcept(bc_type_traits<key_value_array>::is_no_throw_copy) = default;

			bc_json_key_value(bc_json_key_value&&) noexcept(bc_type_traits<key_value_array>::is_no_throw_move) = default;

			~bc_json_key_value() = default;

			bc_json_key_value& operator=(const bc_json_key_value&) noexcept(bc_type_traits<key_value_array>::is_no_throw_copy) = default;

			bc_json_key_value& operator=(bc_json_key_value&&) noexcept(bc_type_traits<key_value_array>::is_no_throw_move) = default;

			iterator begin() noexcept
			{
				return m_key_values.begin();
			}

			const_iterator begin() const noexcept
			{
				return m_key_values.begin();
			}

			const_iterator cbegin() const noexcept
			{
				return m_key_values.cbegin();
			}

			iterator end() noexcept
			{
				return m_key_values.end();
			}

			const_iterator end() const noexcept
			{
				return m_key_values.end();
			}

			const_iterator cend() const noexcept
			{
				return m_key_values.cend();
			}

			iterator find(bc_string_view p_key) noexcept
			{
				auto l_current = std::begin(m_key_values);
				const auto l_end = std::end(m_key_values);
				for (; l_current != l_end; ++l_current)
				{
					if (l_current->first == p_key)
					{
						break;
					}
				}

				return l_current;
			}

			const_iterator find(bc_string_view p_key) const noexcept
			{
				return const_cast<bc_json_key_value*>(this)->find(p_key);
			}

			iterator add_or_update(value_type p_value)
			{
				const auto l_ite = find(p_value.first);
				if(l_ite != end())
				{
					l_ite->second = std::move(p_value.second);
					return l_ite;
				}

				m_key_values.push_back(std::move(p_value));
				return m_key_values.rbegin().base();
			}

			iterator add_or_update(bc_string p_key, bc_any p_value)
			{
				return add_or_update(std::make_pair(std::move(p_key), std::move(p_value)));
			}

			void remove(const bcCHAR* p_key)
			{
				auto l_current = std::begin(m_key_values);
				const auto l_end = std::end(m_key_values);
				for (; l_current != l_end; ++l_current)
				{
					if (l_current->first == p_key)
					{
						break;
					}
				}

				m_key_values.erase(l_current);
			}

			void clear()
			{
				m_key_values.clear();
			}

			void reserve(const size_type p_new_capacity)
			{
				m_key_values.reserve(p_new_capacity);
			}

			bcSIZE size() const noexcept
			{
				return m_key_values.size();
			}

		private:
			key_value_array m_key_values;
		};

		/**
		 * \brief T can be fundamental types bool, bcINT8, bcUINT8, bcINT16, bcUINT16, bcINT32, bcUINT32, bcINT64, bcUINT64, bcFLOAT, bcDOUBLE,
		 * bc_string, bc_string_program, bc_string_frame, bc_json_key_value and bc_parameter_pack which last one can hold all of previous types.
		 * \tparam T
		 */
		template<typename T>
		class bc_json_value : public bci_json_value
		{
		public:
			bc_json_value(const bcCHAR* p_name, bci_json_structure* p_structure, bool p_optional = false)
				: bci_json_value(p_structure, p_optional),
				m_name(p_name),
				m_value()
			{
				static_assert(std::is_default_constructible_v<T>, "T must be default constructable");
			}

			bc_json_value(const bc_json_value&) noexcept(std::is_nothrow_copy_constructible_v<T>) = delete;

			bc_json_value(bc_json_value&&) noexcept(std::is_nothrow_move_constructible_v<T>) = delete;

			~bc_json_value() override = default;

			bc_json_value& operator=(const bc_json_value&) noexcept(std::is_nothrow_copy_assignable_v<T>) = delete;

			bc_json_value& operator=(bc_json_value&&) noexcept(std::is_nothrow_move_assignable_v<T>) = delete;

			void load(bc_json_value_object& p_json_value) override
			{
				// If we use json value within json array m_name will be null
				auto* l_value = m_name != nullptr ? get_json_field(p_json_value, m_name) : &p_json_value;
				if (!l_value)
				{
					m_had_value = false;
					return;
				}

				if (m_optional && l_value->IsNull())
				{
					m_had_value = false;
					return;
				}

				_load(*l_value, m_value);
				m_had_value = true;
			}

			void write(bc_json_document_object& p_document, bc_json_value_object& p_json_value) override
			{
				// If we use json value within json array m_name will be null
				auto* l_json = m_name != nullptr ? set_json_field(p_document, p_json_value, m_name) : &p_json_value;
				/*if (!l_json)
				{
					return;
				}*/

				_write(p_document, *l_json, m_value);
			}

			T& get() noexcept
			{
				return m_value;
			}

			const T& get() const noexcept
			{
				return m_value;
			}

			void set(const T& p_value)
			{
				m_value = p_value;
			}

			void set(T&& p_value)
			{
				m_value = std::move(p_value);
			}

			T* operator->() noexcept
			{
				return &get();
			}

			const T* operator->() const noexcept
			{
				return &get();
			}

			T& operator*() noexcept
			{
				return get();
			}

			const T& operator*() const noexcept
			{
				return get();
			}

		private:
			void _load(const bc_json_value_object& p_json_value, bool& p_value)
			{
				if (!p_json_value.IsBool())
				{
					throw bc_io_exception("bad json format. expected bool value.");
				}

				p_value = p_json_value.GetBool();
			}

			void _write(bc_json_document_object& p_document, bc_json_value_object& p_json_value, const bool& p_value)
			{
				p_json_value.SetBool(p_value);
			}

			void _load(const bc_json_value_object& p_json_value, bcINT32& p_value)
			{
				if (p_json_value.IsNumber())
				{
					p_value = static_cast<bcINT32>(p_json_value.GetInt());
					return;
				}
				if (p_json_value.IsFloat())
				{
					p_value = static_cast<bcINT32>(p_json_value.GetFloat());
					return;
				}
				if (p_json_value.IsDouble())
				{
					p_value = static_cast<bcINT32>(p_json_value.GetDouble());
					return;
				}

				throw bc_io_exception("bad json format. expected int value.");
			}

			void _write(bc_json_document_object& p_document, bc_json_value_object& p_json_value, const bcINT32& p_value)
			{
				p_json_value.SetInt(p_value);
			}

			void _load(const bc_json_value_object& p_json_value, bcUINT32& p_value)
			{
				if (p_json_value.IsNumber())
				{
					p_value = static_cast<bcUINT32>(p_json_value.GetUint());
					return;
				}
				if (p_json_value.IsFloat())
				{
					p_value = static_cast<bcUINT32>(p_json_value.GetFloat());
					return;
				}
				if (p_json_value.IsDouble())
				{
					p_value = static_cast<bcUINT32>(p_json_value.GetDouble());
					return;
				}

				throw bc_io_exception("bad json format. expected uint value.");
			}

			void _write(bc_json_document_object& p_document, bc_json_value_object& p_json_value, const bcUINT32& p_value)
			{
				p_json_value.SetUint(p_value);
			}

			void _load(const bc_json_value_object& p_json_value, bcINT64& p_value)
			{
				if (p_json_value.IsNumber())
				{
					p_value = static_cast<bcINT64>(p_json_value.GetInt64());
					return;
				}
				if (p_json_value.IsFloat())
				{
					p_value = static_cast<bcINT64>(p_json_value.GetFloat());
					return;
				}
				if (p_json_value.IsDouble())
				{
					p_value = static_cast<bcINT64>(p_json_value.GetDouble());
					return;
				}

				throw bc_io_exception("bad json format. expected int value.");
			}

			void _write(bc_json_document_object& p_document, bc_json_value_object& p_json_value, const bcINT64& p_value)
			{
				p_json_value.SetInt64(p_value);
			}

			void _load(const bc_json_value_object& p_json_value, bcUINT64& p_value)
			{
				if (p_json_value.IsNumber())
				{
					p_value = static_cast<bcUINT64>(p_json_value.GetUint64());
					return;
				}
				if (p_json_value.IsFloat())
				{
					p_value = static_cast<bcUINT64>(p_json_value.GetFloat());
					return;
				}
				if (p_json_value.IsDouble())
				{
					p_value = static_cast<bcUINT64>(p_json_value.GetDouble());
					return;
				}

				throw bc_io_exception("bad json format. expected uint value.");
			}

			void _write(bc_json_document_object& p_document, bc_json_value_object& p_json_value, const bcUINT64& p_value)
			{
				p_json_value.SetUint64(p_value);
			}

			void _load(const bc_json_value_object& p_json_value, bcFLOAT& p_value)
			{
				if (p_json_value.IsFloat())
				{
					p_value = p_json_value.GetFloat();
					return;
				}
				if (p_json_value.IsDouble())
				{
					p_value = static_cast<bcFLOAT>(p_json_value.GetDouble());
					return;
				}
				if (p_json_value.IsNumber())
				{
					p_value = static_cast<bcFLOAT>(p_json_value.GetInt());
					return;
				}

				throw bc_io_exception("bad json format. expected float value.");
			}

			void _write(bc_json_document_object& p_document, bc_json_value_object& p_json_value, const bcFLOAT& p_value)
			{
				p_json_value.SetFloat(p_value);
			}

			void _load(const bc_json_value_object& p_json_value, bcDOUBLE& p_value)
			{
				if (p_json_value.IsDouble())
				{
					p_value = p_json_value.GetDouble();
					return;
				}
				if (p_json_value.IsFloat())
				{
					p_value = static_cast<bcDOUBLE>(p_json_value.GetFloat());
					return;
				}
				if (p_json_value.IsNumber())
				{
					p_value = static_cast<bcDOUBLE>(p_json_value.GetInt());
					return;
				}

				throw bc_io_exception("bad json format. expected double value.");
			}

			void _write(bc_json_document_object& p_document, bc_json_value_object& p_json_value, const bcDOUBLE& p_value)
			{
				p_json_value.SetDouble(p_value);
			}

			void _load(const bc_json_value_object& p_json_value, bc_string& p_value)
			{
				if (!p_json_value.IsString())
				{
					throw bc_io_exception("bad json format. expected string value.");
				}

				p_value = p_json_value.GetString();
			}

			void _write(bc_json_document_object& p_document, bc_json_value_object& p_json_value, const bc_string& p_value)
			{
				p_json_value.SetString(p_value.c_str(), p_value.length());
			}

			void _load(const bc_json_value_object& p_json_value, bc_string_program& p_value)
			{
				if (!p_json_value.IsString())
				{
					throw bc_io_exception("bad json format. expected string value.");
				}

				p_value = p_json_value.GetString();
			}

			void _write(bc_json_document_object& p_document, bc_json_value_object& p_json_value, const bc_string_program& p_value)
			{
				p_json_value.SetString(p_value.c_str(), p_value.length());
			}

			void _load(const bc_json_value_object& p_json_value, bc_string_frame& p_value)
			{
				if (!p_json_value.IsString())
				{
					throw bc_io_exception("bad json format. expected string value.");
				}

				p_value = p_json_value.GetString();
			}

			void _write(bc_json_document_object& p_document, bc_json_value_object& p_json_value, const bc_string_frame& p_value)
			{
				p_json_value.SetString(p_value.c_str(), p_value.length());
			}

			void _load(const bc_json_value_object& p_json_value, bc_parameter_pack& p_value)
			{
				if (p_json_value.IsBool())
				{
					p_value.set_value(p_json_value.GetBool());
				}
				else if (p_json_value.IsFloat())
				{
					p_value.set_value(p_json_value.GetFloat());
				}
				else if (p_json_value.IsDouble())
				{
					p_value.set_value(p_json_value.GetDouble());
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
				else if (p_json_value.IsObject())
				{
					bc_json_key_value l_value;
					_load(p_json_value, l_value);

					p_value.set_value(std::move(l_value));
				}
				else if (p_json_value.IsArray())
				{
					const auto l_array = p_json_value.GetArray();
					bc_vector<bc_any> l_values;

					l_values.reserve(l_array.Size());

					for (auto& l_array_item : l_array)
					{
						bc_any l_value;

						_load(l_array_item, l_value);

						l_values.push_back(std::move(l_value));
					}

					p_value.set_value(std::move(l_values));
				}
				else
				{
					throw bc_io_exception("bad json format. cannot deserialize 'any' type.");
				}
			}

			void _write(bc_json_document_object& p_document, bc_json_value_object& p_json_value, const bc_parameter_pack& p_value)
			{
				if (const auto* l_bool = p_value.as<bool>())
				{
					p_json_value.SetBool(*l_bool);
				}
				else if (const auto* l_float = p_value.as<bcFLOAT>())
				{
					p_json_value.SetFloat(*l_float);
				}
				else if (const auto* l_double = p_value.as<bcDOUBLE>())
				{
					p_json_value.SetDouble(*l_double);
				}
				else if (const auto* l_int32 = p_value.as<bcINT32>())
				{
					p_json_value.SetInt(*l_int32);
				}
				else if (const auto* l_int64 = p_value.as<bcINT64>())
				{
					p_json_value.SetInt64(*l_int64);
				}
				else if (const auto* l_uint32 = p_value.as<bcUINT32>())
				{
					p_json_value.SetUint(*l_uint32);
				}
				else if (const auto* l_uint64 = p_value.as<bcUINT64>())
				{
					p_json_value.SetUint64(*l_uint64);
				}
				else if (const auto* l_string = p_value.as<bc_string>())
				{
					p_json_value.SetString(l_string->c_str(), l_string->length());
				}
				else if (const auto* l_string_frame = p_value.as<bc_string_frame>())
				{
					p_json_value.SetString(l_string_frame->c_str(), l_string_frame->length());
				}
				else if (const auto* l_string_program = p_value.as<bc_string_program>())
				{
					p_json_value.SetString(l_string_program->c_str(), l_string_program->length());
				}
				else if (const auto* l_key_value = p_value.as<bc_json_key_value>())
				{
					_write(p_document, p_json_value, *l_key_value);
				}
				else if (const auto* l_vector = p_value.as<bc_vector<bc_any>>())
				{
					p_json_value.SetArray();

					for (const auto& l_value : *l_vector)
					{
						bc_json_value_object l_json_value;
						_write(p_document, l_json_value, l_value);

						p_json_value.PushBack(l_json_value, p_document.GetAllocator());
					}
				}
				else
				{
					throw bc_io_exception("bad json format. cannot serialize 'any' type.");
				}
			}

			void _load(const bc_json_value_object& p_json_value, bc_any& p_value)
			{
				_load(p_json_value, static_cast<bc_parameter_pack&>(p_value));
			}

			void _write(bc_json_document_object& p_document, bc_json_value_object& p_json_value, const bc_any& p_value)
			{
				_write(p_document, p_json_value, static_cast<const bc_parameter_pack&>(p_value));
			}

			void _load(const bc_json_value_object& p_json_value, bc_json_key_value& p_value)
			{
				if (p_json_value.IsObject())
				{
					p_value.reserve(p_json_value.GetObjectW().MemberCount());

					for (auto& l_object_member : p_json_value.GetObjectW())
					{
						bc_string l_key = l_object_member.name.GetString();
						bc_any l_value;

						_load(l_object_member.value, l_value);

						p_value.add_or_update(std::make_pair(std::move(l_key), std::move(l_value)));
					}
				}
				else
				{
					throw bc_io_exception("bad json format. expected object value.");
				}
			}

			void _write(bc_json_document_object& p_document, bc_json_value_object& p_json_value, const bc_json_key_value& p_value)
			{
				p_json_value.SetObject(); // Mark json value as an object

				for (const std::pair<bc_string, bc_any>& l_value : p_value)
				{
					auto* l_json_value = set_json_field(p_document, p_json_value, l_value.first.c_str());
					_write(p_document, *l_json_value, l_value.second);
				}
			}

			template<typename T1>
			void _load(const bc_json_value_object& p_json_value, T1& p_value)
			{
				json_parse::bc_load(p_json_value, p_value);
			}

			template<typename T1>
			void _write(bc_json_document_object& p_document, bc_json_value_object& p_json_value, const T1& p_value)
			{
				json_parse::bc_write(p_document, p_json_value, p_value);
			}

			const bcCHAR* m_name;
			T m_value;
		};

		template<class T>
		class bc_json_object : public bci_json_value
		{
		public:
			bc_json_object(const bcCHAR* p_name, bci_json_structure* p_structure, bool p_optional = false)
				: bci_json_value(p_structure, p_optional),
				m_name(p_name),
				m_value()
			{
			}

			bc_json_object(const bc_json_object&) noexcept(std::is_nothrow_copy_constructible_v<T>) = delete;

			bc_json_object(bc_json_object&&) noexcept(std::is_nothrow_move_constructible_v<T>) = delete;

			~bc_json_object() override = default;

			bc_json_object& operator=(const bc_json_object&) noexcept(std::is_nothrow_copy_assignable_v<T>) = delete;

			bc_json_object& operator=(bc_json_object&&) noexcept(std::is_nothrow_move_assignable_v<T>) = delete;

			void load(bc_json_value_object& p_json_value) override
			{
				// If we use json object within json array m_name will be null
				auto* l_value = m_name != nullptr ? get_json_field(p_json_value, m_name) : &p_json_value;
				if (!l_value)
				{
					m_had_value = false;
					return;
				}

				if (m_optional && l_value->IsNull())
				{
					m_had_value = false;
					return;
				}
				
				if (!l_value->IsObject())
				{
					throw bc_io_exception("bad json format. expected object value.");
				}

				m_value.load(*l_value);
				m_had_value = true;
			}

			void write(bc_json_document_object& p_document, bc_json_value_object& p_json_value) override
			{
				// If we use json object within json array m_name will be null
				auto* l_json = m_name != nullptr ? set_json_field(p_document, p_json_value, m_name) : &p_json_value;
				/*if (!l_json)
				{
					return;
				}*/
				l_json->SetObject(); // Mark this json object as an object

				m_value.write(p_document, *l_json);
			}

			T& get() noexcept
			{
				return m_value;
			}

			const T& get() const noexcept
			{
				return m_value;
			}

			void set(const T& p_value)
			{
				m_value = p_value;
			}

			void set(T&& p_value)
			{
				m_value = std::move(p_value);
			}

			T* operator->() noexcept
			{
				return &get();
			}

			const T* operator->() const noexcept
			{
				return &get();
			}

			T& operator*() noexcept
			{
				return get();
			}

			const T& operator*() const noexcept
			{
				return get();
			}

		private:
			const bcCHAR* m_name;
			T m_value;
		};

		template<typename T, typename T1 = void>
		class bc_json_array : public bci_json_value, public bc_iterator_adapter<bc_list<bc_json_object<T>>>
		{
		public:
			using list_t = bc_list<bc_json_object<T>>;
			using value_type = typename list_t::value_type;
			using pointer = typename list_t::pointer;
			using reference = typename list_t::reference;
			using difference_type = typename list_t::difference_type;
			using iterator = typename list_t::iterator;
			using const_iterator = typename list_t::const_iterator;
			using size_type = typename list_t::size_type;

		public:
			bc_json_array(const char* p_name, bci_json_structure* p_structure, bool p_optional = false)
				: bci_json_value(p_structure, p_optional),
				bc_iterator_adapter<list_t>(m_value),
				m_name(p_name),
				m_value()
			{
			}

			bc_json_array(const bc_json_array& p_other) noexcept(std::is_nothrow_copy_constructible_v<T>)
				: bci_json_value(p_other),
				bc_iterator_adapter(m_value),
				m_name(p_other.m_name),
				m_value(p_other.m_value)
			{
			}

			bc_json_array(bc_json_array&& p_other) noexcept(std::is_nothrow_move_constructible<T>::value)
				: bci_json_value(std::move(p_other)),
				bc_iterator_adapter(m_value),
				m_name(p_other.m_name),
				m_value(std::move(p_other.m_value))
			{
			}

			~bc_json_array() override = default;

			bc_json_array& operator=(const bc_json_array&) noexcept(std::is_nothrow_copy_assignable_v<T>) = default;

			bc_json_array& operator=(bc_json_array&&) noexcept(std::is_nothrow_move_assignable_v<T>) = default;

			void load(bc_json_value_object& p_json_value) override
			{
				auto* l_value = get_json_field(p_json_value, m_name);
				if (!l_value)
				{
					m_had_value = false;
					return;
				}

				if (m_optional && l_value->IsNull())
				{
					m_had_value = false;
					return;
				}
				
				if (!l_value->IsArray())
				{
					throw bc_io_exception("bad json format. expected array value.");
				}

				const auto l_array = l_value->GetArray();
				for (auto& l_array_value : l_array)
				{
					// Because json objects are non-copy and movable we emplace one in array and then load it in-place
					m_value.emplace_back(nullptr, nullptr);

					auto l_inserted_value = m_value.rbegin();
					l_inserted_value->load(l_array_value);
				}
				m_had_value = true;
			}

			void write(bc_json_document_object& p_document, bc_json_value_object& p_json_value) override
			{
				auto* l_json_value = set_json_field(p_document, p_json_value, m_name);
				l_json_value->SetArray(); // Mark this json object as an array

				for (auto& l_value : m_value)
				{
					bc_json_value_object l_json_item(rapidjson::kObjectType);
					l_value.write(p_document, l_json_item);

					l_json_value->PushBack(l_json_item, p_document.GetAllocator());
				}
			}

			bc_json_object<T>& new_entry()
			{
				m_value.emplace_back(nullptr, nullptr);

				auto l_inserted_value = m_value.rbegin();
				return *l_inserted_value;
			}

			void remove_entry(const_iterator p_iterator)
			{
				m_value.erase(p_iterator);
			}
		
		private:
			const bcCHAR* m_name;
			list_t m_value;			// Because json objects are not copyable and movable we have used list instead of vector
		};

		template<typename T>
		class bc_json_array
		<
			T,
			std::enable_if_t
			<
				std::is_same_v<bool, std::decay_t<T>> ||
				std::is_same_v<bcINT, std::decay_t<T>> ||
				std::is_same_v<bcUINT, std::decay_t<T>> ||
				std::is_same_v<bcFLOAT, std::decay_t<T>> ||
				std::is_same_v<bc_string, std::decay_t<T>> ||
				std::is_same_v<bc_string_program, std::decay_t<T>> ||
				std::is_same_v<bc_string_frame, std::decay_t<T>> ||
				std::is_same_v<bc_parameter_pack, std::decay_t<T>> ||
				std::is_same_v<bc_any, std::decay_t<T>>
			>
		> : public bci_json_value, public bc_iterator_adapter<bc_list<bc_json_value<T>>>
		{
		public:
			using list_t = bc_list<bc_json_value<T>>;
			using value_type = typename list_t::value_type;
			using pointer = typename list_t::pointer;
			using reference = typename list_t::reference;
			using difference_type = typename list_t::difference_type;
			using iterator = typename list_t::iterator;
			using const_iterator = typename list_t::const_iterator;
			using size_type = typename list_t::size_type;

		public:
			bc_json_array(const bcCHAR* p_name, bci_json_structure* p_structure, bool p_optional = false)
				: bci_json_value(p_structure, p_optional),
				bc_iterator_adapter<list_t>(m_value),
				m_name(p_name),
				m_value()
			{
			}

			bc_json_array(const bc_json_array& p_other) noexcept(std::is_nothrow_copy_constructible<T>::value)
				: bci_json_value(p_other),
				bc_iterator_adapter(m_value),
				m_name(p_other.m_name),
				m_value(p_other.m_value)
			{
			}

			bc_json_array(bc_json_array&& p_other) noexcept(std::is_nothrow_move_constructible<T>::value)
				: bci_json_value(std::move(p_other)),
				bc_iterator_adapter(m_value),
				m_name(p_other.m_name),
				m_value(std::move(p_other.m_value))
			{
			}

			~bc_json_array() override = default;

			bc_json_array& operator=(const bc_json_array&) noexcept(std::is_nothrow_copy_assignable_v<T>) = delete;

			bc_json_array& operator=(bc_json_array&&) noexcept(std::is_nothrow_move_assignable_v<T>) = delete;

			void load(bc_json_value_object& p_json_value) override
			{
				auto* l_value = get_json_field(p_json_value, m_name);
				if (!l_value)
				{
					m_had_value = false;
					return;
				}

				if (m_optional && l_value->IsNull())
				{
					m_had_value = false;
					return;
				}
				
				if (!l_value->IsArray())
				{
					throw bc_io_exception("bad json format. expected array value.");
				}

				auto l_array = l_value->GetArray();
				for (auto& l_array_value : l_array)
				{
					// Because json objects are non-copy and movable we emplace one in array and then load it in-place
					m_value.emplace_back(nullptr, nullptr);

					auto l_inserted_value = m_value.rbegin();
					l_inserted_value->load(l_array_value);
				}
				m_had_value = true;
			}

			void write(bc_json_document_object& p_document, bc_json_value_object& p_json_value) override
			{
				auto* l_json_value = set_json_field(p_document, p_json_value, m_name);
				l_json_value->SetArray(); // Mark this json object as an array

				for (auto& l_value : m_value)
				{
					bc_json_value_object l_json_item;
					l_value.write(p_document, l_json_item);

					l_json_value->PushBack(l_json_item, p_document.GetAllocator());
				}
			}

			bc_json_value<T>& new_entry()
			{
				m_value.emplace_back(nullptr, nullptr);

				auto l_inserted_value = m_value.rbegin();
				return *l_inserted_value;
			}

			void remove_entry(const_iterator p_iterator)
			{
				m_value.erase(p_iterator);
			}

		private:
			const bcCHAR* m_name;
			list_t m_value;			// Because json objects are not copyable and movable we have used list instead of vector
		};

		template<typename T>
		class bc_json_document
		{
		public:
			bc_json_document() = default;

			bc_json_document(const bc_json_document&) = delete;

			bc_json_document(bc_json_document&&) = delete;

			~bc_json_document() = default;

			bc_json_document& operator=(const bc_json_document&) = delete;

			bc_json_document& operator=(bc_json_document&&) = delete;

			void load(bc_string_view p_json)
			{
				bc_json_document_object l_json_document;
				l_json_document.Parse(p_json.data(), p_json.size());

				if (l_json_document.HasParseError())
				{
					const auto l_parse_error = l_json_document.GetParseError();
					const auto l_parse_error_offset = l_json_document.GetErrorOffset();
					const auto l_parse_error_msg = bc_string_stream_frame() << "bad json format. error: " << l_parse_error << " error location: " << l_parse_error_offset;

					throw bc_io_exception(l_parse_error_msg.str().c_str());
				}

				if (!l_json_document.IsObject())
				{
					throw bc_io_exception("bad json format. expected object value.");
				}

				m_value.load(l_json_document);
			}

			bc_string write_pretty()
			{
				bc_string l_result = _write<rapidjson::PrettyWriter<rapidjson::StringBuffer>>().GetString();
				return l_result;
			}

			bc_string_frame write_pretty_frame()
			{
				bc_string_frame l_result = _write<rapidjson::PrettyWriter<rapidjson::StringBuffer>>().GetString();
				return l_result;
			}

			bc_string write()
			{
				bc_string l_result = _write<rapidjson::Writer<rapidjson::StringBuffer>>().GetString();
				return l_result;
			}

			bc_string_frame write_frame()
			{
				bc_string_frame l_result = _write<rapidjson::Writer<rapidjson::StringBuffer>>().GetString();
				return l_result;
			}

			T& get() noexcept
			{
				return m_value;
			}

			const T& get() const noexcept
			{
				return m_value;
			}

			T* operator->() noexcept
			{
				return &get();
			}

			const T* operator->() const noexcept
			{
				return &get();
			}

			T& operator*() noexcept
			{
				return get();
			}

			const T& operator*() const noexcept
			{
				return get();
			}

			bcUINT32 get_max_decimal_places() const noexcept
			{
				return m_max_decimal_places;
			}
			
			void set_max_decimal_places(bcUINT32 p_decimal_places) noexcept
			{
				m_max_decimal_places = p_decimal_places;
			}

		private:
			template<typename TWriter>
			rapidjson::StringBuffer _write()
			{
				bc_json_document_object l_json_document(rapidjson::kObjectType);
				m_value.write(l_json_document, l_json_document);

				rapidjson::StringBuffer l_json_buffer;
				TWriter l_json_writer(l_json_buffer);
				l_json_writer.SetMaxDecimalPlaces(m_max_decimal_places);
				l_json_document.Accept(l_json_writer);

				return l_json_buffer;
			}

			T m_value;
			bcUINT32 m_max_decimal_places{7};
		};
	}
}