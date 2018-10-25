// [06/21/2016 MRB]

#include "Core/CorePCH.h"

#include "CorePlatform/bcException.h"
#include "Core/File/bcStream.h"

namespace black_cat
{
	namespace core
	{
		bool bc_get_char(bc_istream_adapter& p_stream, bcCHAR* p_char)
		{
			if(!p_stream.can_read())
			{
				return false;
			}

			return p_stream.read(p_char, sizeof(bcCHAR)) == sizeof(bcCHAR);
		}

		bool bc_get_wchar(bc_istream_adapter& p_stream, bcWCHAR* p_char)
		{
			if (!p_stream.can_read())
			{
				return false;
			}

			return p_stream.read(reinterpret_cast< bcBYTE* >(p_char), sizeof(bcWCHAR)) == sizeof(bcWCHAR);
		}

		bool bc_get_line(bc_istream_adapter& p_stream, bc_string_frame& p_line)
		{
			bcCHAR l_char;
			bool l_first_read;
			bool l_read;

			l_read = l_first_read = bc_get_char(p_stream, &l_char);

			if(l_first_read)
			{
				p_line.clear();
			}

			while (l_read)
			{
				p_line.push_back(l_char);

				if (l_char == '\n')
				{
					break;
				}

				l_read = bc_get_char(p_stream, &l_char);
			}

			return l_first_read;
		}

		bool bc_get_wline(bc_istream_adapter& p_stream, bc_wstring_frame& p_line)
		{
			bcWCHAR l_char;
			bool l_first_read;
			bool l_read;

			l_read = l_first_read = bc_get_wchar(p_stream, &l_char);

			if (l_first_read)
			{
				p_line.clear();
			}

			while (l_read)
			{
				p_line.push_back(l_char);

				if (l_char == '\n')
				{
					break;
				}

				l_read = bc_get_wchar(p_stream, &l_char);
			}

			return l_first_read;
		}
	}
}