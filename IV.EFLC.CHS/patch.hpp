#pragma once
#include <windows.h>
#include <cstdint>

namespace patch
{
	namespace internal
	{
		struct string_info
		{
			
		};

		void parse_string(const char *, string_info &result)
		{

		}
	}

	class scoped_unprotect
	{
		DWORD m_address;
		SIZE_T m_size;
		DWORD m_old_protection;
		bool m_unprotected;

	public:
		scoped_unprotect()
		{
			this->m_address = 0;
			this->m_size = 0;
			this->m_unprotected = false;
		}

		scoped_unprotect(DWORD address, SIZE_T size, bool do_unprotect) :scoped_unprotect()
		{
			if (do_unprotect && size > 0)
			{

			}
		}

		scoped_unprotect(const char *section)
		{

		}

		scoped_unprotect(scoped_unprotect &&rv)
		{
			this->m_address = rv.m_address;
			this->m_size = rv.m_size;
			this->m_unprotected = rv.m_unprotected;
			rv.m_unprotected = false;
		}

		~scoped_unprotect()
		{
			DWORD old_protection;

			if (m_unprotected)
			{
				VirtualProtect((LPVOID)m_address, m_size, m_old_protection, &old_protection);
			}
		}
	};

	inline DWORD get_aslr_address(DWORD imagine_address, DWORD imagine_base = 0x400000)
	{
		static DWORD real_base = (DWORD)::GetModuleHandle(NULL);

		return (imagine_address + real_base - imagine_base);
	}

	template <typename T>
	inline void patch_any(DWORD address, T value, bool vp = false)
	{
		scoped_unprotect raii_unprotect(address, sizeof(value), vp);

		*(T *)address = value;
	}


}
