#pragma once
#include "pch.h"
#include "Unreal.h"

namespace Malium
{
    class URL
    {
    public:
        using StrType = Unreal::FString;
        StrType Protocol, Seperator, Domain, Port, Path, Query;

        __forceinline URL(StrType& url)
        {
            const std::wstring_view text(url.String, url.Length ? url.Length - 1 : 0);

            const auto ProtoEnd = text.find(':');
            if (ProtoEnd == std::wstring_view::npos)
                return;

            Protocol = Slice(text.data(), ProtoEnd);

            const auto ProtoSize = (ProtoEnd + 2 < text.size() && text[ProtoEnd + 1] == '/' && text[ProtoEnd + 2] == '/') ? 3 : 1;
            Seperator = Slice(text.data() + ProtoEnd, ProtoSize);

            const auto DomainStart = ProtoEnd + ProtoSize;
            const auto PathEnd = text.find('/', DomainStart);
            const auto AuthorityEnd = PathEnd == std::wstring_view::npos ? text.size() : PathEnd;
            const auto AuthorityQuery = text.substr(0, AuthorityEnd).find('?', DomainStart);
            const auto DomainPortEnd = AuthorityQuery == std::wstring_view::npos ? AuthorityEnd : AuthorityQuery;

            const auto PortOff = text.find(':', DomainStart);
            const auto PortStart = PortOff < DomainPortEnd ? PortOff : std::wstring_view::npos;
            const auto DomainEnd = PortStart == std::wstring_view::npos ? DomainPortEnd : PortStart;
            Domain = Slice(text.data() + DomainStart, DomainEnd - DomainStart);

            if (PortStart != std::wstring_view::npos)
                Port = Slice(text.data() + PortStart, DomainPortEnd - PortStart);

            const auto QueryOff = text.find('?', DomainPortEnd);
            const auto PathLength = QueryOff == std::wstring_view::npos ? text.size() - DomainPortEnd : QueryOff - DomainPortEnd;
            Path = Slice(text.data() + DomainPortEnd, PathLength);

            if (QueryOff != std::wstring_view::npos)
                Query = Slice(text.data() + QueryOff, text.size() - QueryOff);
        }

        __forceinline URL& SetHost(StrType host)
        {
            const std::wstring_view text(host.String, host.Length ? host.Length - 1 : 0);
            const auto ProtoEnd = text.find(':');
            if (ProtoEnd == std::wstring_view::npos)
                return *this;

            Protocol.Dealloc();
            Protocol = Slice(text.data(), ProtoEnd);

            const auto ProtoSize = (ProtoEnd + 2 < text.size() && text[ProtoEnd + 1] == '/' && text[ProtoEnd + 2] == '/') ? 3 : 1;
            Seperator.Dealloc();
            Seperator = Slice(text.data() + ProtoEnd, ProtoSize);

            const auto DomainStart = ProtoEnd + ProtoSize;
            const auto PathEnd = text.find('/', DomainStart);
            const auto AuthorityEnd = PathEnd == std::wstring_view::npos ? text.size() : PathEnd;
            const auto AuthorityQuery = text.substr(0, AuthorityEnd).find('?', DomainStart);
            const auto DomainPortEnd = AuthorityQuery == std::wstring_view::npos ? AuthorityEnd : AuthorityQuery;

            const auto PortOff = text.find(':', DomainStart);
            const auto PortStart = PortOff < DomainPortEnd ? PortOff : std::wstring_view::npos;
            const auto DomainEnd = PortStart == std::wstring_view::npos ? DomainPortEnd : PortStart;

            Domain.Dealloc();
            Domain = Slice(text.data() + DomainStart, DomainEnd - DomainStart);

            Port.Dealloc();
            if (PortStart != std::wstring_view::npos)
                Port = Slice(text.data() + PortStart, DomainPortEnd - PortStart);

            return *this;
        }

        template <StrType host>
        __forceinline URL& SetHost()
        {
            constexpr auto ProtoEnd = Unreal::FStringUtil::find_const(host, ':');
            Protocol.Dealloc();
            constexpr static auto ProtocolS = Unreal::FStringUtil::substr<host, 0, ProtoEnd>();
            constexpr StrType ProtocolV = ProtocolS.data();
            Protocol = ProtocolV;

            constexpr auto ProtoSize = (host.String[ProtoEnd + 1] == '/' && host.String[ProtoEnd + 2] == '/') ? 3 : 1;
            Seperator.Dealloc();
            constexpr static auto SeperatorS = Unreal::FStringUtil::substr<host, ProtoEnd, ProtoSize>();
            constexpr StrType SeperatorV = SeperatorS.data();
            Seperator = SeperatorV;

            constexpr auto DomainAndPortStart = Unreal::FStringUtil::substr<host, ProtoEnd + ProtoSize>();
            constexpr auto PathEnd = Unreal::CPPArrayUtil::find<DomainAndPortStart.size(), DomainAndPortStart>('/');
            constexpr auto DomainAndPort = Unreal::CPPArrayUtil::substr<DomainAndPortStart.size(), DomainAndPortStart, 0, PathEnd>();
            constexpr auto PortOff = Unreal::CPPArrayUtil::find<DomainAndPort.size(), DomainAndPort>(':');
            Domain.Dealloc();

            constexpr static auto DomainS = Unreal::CPPArrayUtil::substr<DomainAndPort.size(), DomainAndPort, 0, PortOff>();
            constexpr StrType DomainV = DomainS.data();
            Domain = DomainV;

            Port.Dealloc();
            if (PortOff != StrType::npos) 
            {
                constexpr static auto PortS = Unreal::CPPArrayUtil::substr<DomainAndPort.size(), DomainAndPort, PortOff>();
                constexpr StrType PortV = PortS.data();
                Port = PortV;
            }

            return *this;
        }

        __forceinline StrType GetUrl()
        {
            const auto contentLength = [](const StrType& value)
            {
                return value.String ? value.Length - 1 : 0;
            };
            auto OutStr = StrType(contentLength(Protocol) + contentLength(Seperator) + contentLength(Domain) + contentLength(Port) + contentLength(Path) + contentLength(Query));
            auto output = OutStr.String;

            auto append = [&output](const StrType& value)
            {
                if (!value.String)
                    return;

                const auto length = value.Length - 1;
                __movsb(reinterpret_cast<PBYTE>(output), reinterpret_cast<const PBYTE>(value.String), length * sizeof(wchar_t));
                output += length;
            };

            append(Protocol);
            append(Seperator);
            append(Domain);
            append(Port);
            append(Path);
            append(Query);
            *output = L'\0';

            return OutStr;
        }

        __forceinline operator StrType()
        {
            return GetUrl();
        }

        void DeallocPathQuery() 
        {
            Path.Dealloc();
            Query.Dealloc();
        }

        void Dealloc() 
        {
            Protocol.Dealloc();
            Seperator.Dealloc();
            Domain.Dealloc();
            Port.Dealloc();
            Path.Dealloc();
            Query.Dealloc();
        }
    private:
        static StrType Slice(const wchar_t* source, size_t length)
        {
            StrType value(static_cast<uint32_t>(length));
            if (length)
                __movsb(reinterpret_cast<PBYTE>(value.String), reinterpret_cast<const uint8_t*>(source), length * sizeof(wchar_t));
            value.String[length] = L'\0';
            return value;
        }
    };

}
#ifdef __clang__
#define __URL_SetHost(url, host) url.SetHost(host)
#else
#define __URL_SetHost(url, host) url.SetHost<host>()
#endif