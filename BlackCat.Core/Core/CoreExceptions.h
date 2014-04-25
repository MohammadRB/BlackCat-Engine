//=================================================================================================
//
//	MJP's DX11 Sample Framework
//  http://mynameismjp.wordpress.com/
//
//  All code and content licensed under Microsoft Public License (Ms-PL)
//
//=================================================================================================
#pragma once

#ifndef CoreExceptions_H
#define CoreExceptions_H

#include "CorePCH.h"
#include "bcString.h"

namespace BlackCat
{
	namespace Core
	{
		// Generic exception, used as base class for other types
		class Exception
		{
		public:

			Exception()
			{
			}

			// Specify an actual error message
			Exception (const bcWString& exceptionMessage) : 
				mMessage(exceptionMessage)
			{
			}

			// Retrieve that error message
			const bcWString& GetMessage() const throw()
			{
				return mMessage;
			}

			/*void ShowErrorMessage() const throw ()
			{
				MessageBox(NULL, message.c_str(), L"Error", MB_OK|MB_ICONERROR);
			}*/

		protected:

			bcWString mMessage;	
		};
	}
}

#endif