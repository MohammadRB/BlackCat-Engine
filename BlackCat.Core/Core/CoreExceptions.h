// [1/27/2015 MRB]

#pragma once

#include "CorePlatform/bcCorePlatformException.h"
#include "Core/CorePCH.h"
#include "Core/Container/bcString.h"

namespace black_cat
{
	namespace core
	{
		// TODO remove this class when all references has been removed
		// Generic exception, used as base class for other types
		class Exception
		{
		public:

			Exception()
			{
			}

			// Specify an actual error message
			Exception(const bc_wstring& exceptionMessage) :
				mMessage(exceptionMessage)
			{
			}

			// Retrieve that error message
			const bc_wstring& GetMessage() const noexcept(true)
			{
				return mMessage;
			}

			/*void ShowErrorMessage() const throw ()
			{
			MessageBox(nullptr, message.c_str(), L"Error", MB_OK|MB_ICONERROR);
			}*/

		protected:
			bc_wstring mMessage;
		};

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
	}
}