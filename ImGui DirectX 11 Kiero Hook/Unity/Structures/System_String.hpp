#pragma once

namespace Unity
{
	struct System_String : il2cppObject
	{
		int m_iLength;
		wchar_t m_wString[1024];

		void Clear()
		{
			if (!this) return;

			memset(m_wString, 0, static_cast<size_t>(m_iLength) * 2);
			m_iLength = 0;
		}

		void StripColorTag()
		{
			if (m_iLength == 0) return;

			// Find the end of the color tag
			const wchar_t* tagEnd = L"</color>";
			wchar_t* pos = wcsstr(m_wString, tagEnd);
			if (pos != nullptr)
			{
				// Find the position just after the end tag
				wchar_t* nameStart = pos + wcslen(tagEnd);
				while (*nameStart == L' ') nameStart++; // Skip any spaces

				// Shift characters to remove the tag
				int newLength = static_cast<int>(wcslen(nameStart));
				memmove(m_wString, nameStart, (newLength + 1) * sizeof(wchar_t));
				m_iLength = newLength;
			}
		}

		std::string ToString()
		{
		    if (!this) return "";

			StripColorTag();

		    std::string sRet(static_cast<size_t>(m_iLength + 1) * 4, '\0');
		    WideCharToMultiByte(CP_UTF8, 0, m_wString, m_iLength, &sRet[0], static_cast<int>(sRet.size()), 0, 0);
		    return sRet;
		}
	};
}
