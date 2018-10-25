// [1/27/2015 MRB]

#pragma once

#include "CorePlatform/bcException.h"
#include "Core/CorePCH.h"

namespace black_cat
{
	class bc_thread_interrupted_exception : public std::exception
	{
	public:
		bc_thread_interrupted_exception() = default;

		bc_thread_interrupted_exception(const bc_thread_interrupted_exception&) = default;

		~bc_thread_interrupted_exception() = default;

		bc_thread_interrupted_exception& operator =(const bc_thread_interrupted_exception&) = default;

	protected:

	private:
	};

	class bc_io_exception : public std::runtime_error
	{
	public:
		explicit bc_io_exception(const bcCHAR* p_message);

		explicit bc_io_exception(std::string& p_message);

		bc_io_exception(const bc_io_exception&) = default;

		bc_io_exception(bc_io_exception&&) = default;

		~bc_io_exception() = default;

		bc_io_exception& operator=(const bc_io_exception&) = default;

		bc_io_exception& operator=(bc_io_exception&&) = default;
	protected:

	private:
	};

	inline bc_io_exception::bc_io_exception(const bcCHAR* p_message)
		: runtime_error(p_message)
	{
	}

	inline bc_io_exception::bc_io_exception(std::string& p_message)
		: runtime_error(p_message)
	{
	}
}
