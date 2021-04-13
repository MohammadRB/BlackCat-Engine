// [06/21/2016 MRB]

#include "Core/CorePCH.h"

#include "CorePlatform/bcException.h"
#include "Core/File/bcStream.h"

namespace black_cat
{
	namespace core
	{
		bool bc_read_char(bci_stream_adapter& p_stream, bcCHAR* p_char)
		{
			if(!p_stream.can_read())
			{
				return false;
			}

			return p_stream.read(p_char, sizeof(bcCHAR)) == sizeof(bcCHAR);
		}

		bool bc_read_char(bci_stream_adapter& p_stream, bcWCHAR* p_char)
		{
			if (!p_stream.can_read())
			{
				return false;
			}

			return p_stream.read(reinterpret_cast< bcBYTE* >(p_char), sizeof(bcWCHAR)) == sizeof(bcWCHAR);
		}

		bool bc_read_line(bci_stream_adapter& p_stream, bc_string_frame& p_line)
		{
			bcCHAR l_char;
			bool l_first_read;
			bool l_read;

			l_read = l_first_read = bc_read_char(p_stream, &l_char);

			while (l_read)
			{
				p_line.push_back(l_char);

				if (l_char == '\n')
				{
					break;
				}

				l_read = bc_read_char(p_stream, &l_char);
			}

			return l_first_read;
		}

		bool bc_read_line(bci_stream_adapter& p_stream, bc_wstring_frame& p_line)
		{
			bcWCHAR l_char;
			bool l_first_read;
			bool l_read;

			l_read = l_first_read = bc_read_char(p_stream, &l_char);

			while (l_read)
			{
				p_line.push_back(l_char);

				if (l_char == '\n')
				{
					break;
				}

				l_read = bc_read_char(p_stream, &l_char);
			}

			return l_first_read;
		}

		bool bc_read_all_lines(bci_stream_adapter& p_stream, bc_string_frame& p_content)
		{
			bool l_read = false;
			bc_string_frame l_line;
			
			while (bc_read_line(p_stream, l_line))
			{
				l_read = true;
				p_content.append(l_line);
				l_line.clear();
			}

			return l_read;
		}

		bool bc_read_all_lines(bci_stream_adapter& p_stream, bc_wstring_frame& p_content)
		{
			bool l_read = false;
			bc_wstring_frame l_line;
			
			while (bc_read_line(p_stream, l_line))
			{
				l_read = true;
				p_content.append(l_line);
				l_line.clear();
			}

			return l_read;
		}
	}
}