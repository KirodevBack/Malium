#include "pch.h"
#include "Unreal.h"
#include "Redirection.h"
#include "Options.h"

namespace Malium
{
    namespace Unreal 
    {
        FString::FString(const char* Other)
        {
            if (Other)
            {
                MaxSize = Length = (int)strlen(Other) + 1;

                AllocString();

                size_t ConvertedChars = 0;
                mbstowcs_s(&ConvertedChars, String, Length, Other, _TRUNCATE);
            }
        }

        FString::FString(wchar_t* Other)
        {
            if (Other) {
                MaxSize = Length = (int)wcslen(Other) + 1;

                AllocString();

                __movsb((PBYTE)String, (const PBYTE)Other, Length * sizeof(wchar_t));
            }
        }

        FString::FString(uint32_t len)
        {
            MaxSize = Length = len + 1;
            AllocString();
        }

        FString FString::operator+(const FString& other)
        {
            if (!String || !other.String) return *this;
            auto sLen = Length - 1;
            auto oLen = other.Length - 1;

            FString nStr((uint32_t)(sLen + oLen));
            __movsb((PBYTE)nStr.String, (const PBYTE)String, sLen * sizeof(wchar_t));
            __movsb((PBYTE)nStr.String + sLen * sizeof(wchar_t), (const PBYTE)other.String, oLen * sizeof(wchar_t));
            nStr.String[nStr.Length - 1] = 0;

            return nStr;
        }

        void FString::operator+=(const FString& other) 
        {
            if (!String || !other.String) return;
            const auto sLen = Length - 1;
            const auto oLen = other.Length - 1;
            const auto newLength = static_cast<uint32_t>(sLen + oLen + 1);
            const auto oldString = String;
            const auto newString = FMemory::MallocForType<wchar_t>(newLength);

            __movsb(reinterpret_cast<PBYTE>(newString), reinterpret_cast<const PBYTE>(oldString), sLen * sizeof(wchar_t));
            __movsb(reinterpret_cast<PBYTE>(newString + sLen), reinterpret_cast<const PBYTE>(other.String), oLen * sizeof(wchar_t));
            newString[newLength - 1] = L'\0';

            FMemory::Free(oldString);
            String = newString;
            Length = MaxSize = newLength;
        }

        FString FString::substr(size_t off, size_t count) 
        {
            if (count == -1) count = Length - off - 1;
            else if (count > Length) return *this;
            FString nStr((uint32_t)count);

            __movsb((PBYTE)nStr.String, (const PBYTE)(String + off), count * sizeof(wchar_t));
            nStr.String[count] = 0;

            return nStr;
        }

        size_t FString::find(wchar_t c) 
        {
            if (!String || Length == 0)
                return npos;

            const auto result = wcschr(String, c);
            return result ? static_cast<size_t>(result - String) : npos;
        }

        size_t FString::find(char c) 
        {
            return find((wchar_t)c);
        }

        size_t FString::find(const wchar_t* c) 
        {
            if (!String || Length == 0)
                return npos;

            const auto result = wcsstr(String, c);
            return result ? static_cast<size_t>(result - String) : npos;
        }

        bool FString::contains(wchar_t c) 
        {
            return find(c) != npos;
        }

        bool FString::contains(const wchar_t* c) 
        {
            return find(c) != npos;
        }

        bool FString::starts_with(const wchar_t* c)
        {
            const auto cLen = wcslen(c);
            return cLen == 0 || (String && wcsncmp(String, c, cLen) == 0);
        }

        bool FString::ends_with(const wchar_t* c) {
            const auto cLen = wcslen(c);
            if (cLen == 0)
                return true;
            if (!String || Length == 0)
                return false;

            const auto length = static_cast<size_t>(Length) - 1;
            return cLen <= length && wcsncmp(String + length - cLen, c, cLen) == 0;
        }

        size_t FString::find_first_of(char c) 
        {
            return find(c);
        }

        size_t FString::find_first_of(wchar_t c) 
        {
            return find(c);
        }

        wchar_t* FString::c_str() 
        {
            return String;
        }

        FString::operator wchar_t* () 
        {
            return String;
        }

        void FString::Dealloc() 
        {
            if (!String)
                return;

            FMemory::Free(String);
            String = nullptr;
            Length = MaxSize = 0;
        }

        inline void FString::AllocString() 
        {
            String = FMemory::MallocForType<wchar_t>(Length);
        }

        FString FCurlHttpRequest::GetURL()
        {
            return ((FString & (*&)(FCurlHttpRequest*, FString)) VTable[0])(this, FString());
        }

        int setupMemLeak = 0;
        void FCurlHttpRequest::RedirectRequest(bool bEOS)
        {
            if (!FCurlHttpRequest::SetURLIdx && !bEOS)
                InitializeURLIndex();
            else if (++setupMemLeak == 5 && FixMemLeak)
            {
                constexpr static auto pattern = Malium::Patchfinder::Pattern<"48 89 5C 24 ?? 57 48 83 EC ?? 48 8B 01 4C 8B C2 48 8D 54 24 ? 48 8B D9 FF 50 30">();
                constexpr static auto pattern2 = Malium::Patchfinder::Pattern<"48 8B 01 4C 8D 41 08 48 FF 60 20">();

                auto MemLeakPatch = pattern.Scan();

                if (!MemLeakPatch)
                    MemLeakPatch = pattern2.Scan();

                if (MemLeakPatch)
                {
                    DWORD oldProt;
                    VirtualProtect(LPVOID(MemLeakPatch), 1, PAGE_EXECUTE_READWRITE, &oldProt);

                    *(uint8_t*)MemLeakPatch = 0xC3;

                    VirtualProtect(LPVOID(MemLeakPatch), 1, oldProt, &oldProt);
                }
            }

            auto URL = GetURL();
            Malium::URL RequestURL = URL;

            if (Malium::Redirection::ShouldRedirect(RequestURL))
            {
                if constexpr (UseBackendParam)
                    RequestURL.SetHost(backend);
                else
                    __URL_SetHost(RequestURL, Backend);

                Malium::Unreal::FString str = RequestURL;
                ((void (*&)(Malium::Unreal::FCurlHttpRequest*, Malium::Unreal::FString)) VTable[bEOS ? 10 : FCurlHttpRequest::SetURLIdx])(this, str);
                str.Dealloc();

                UseBackendParam ? RequestURL.Dealloc() : RequestURL.DeallocPathQuery();
            }
            else
                RequestURL.Dealloc();

            URL.Dealloc();
        }

    }
}