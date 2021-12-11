/*
 *  Copyright (C) 2020 the_hunter
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include <adminskit/utils.h>
#include <core/console.h>
#include <cssdk/public/os_defs.h>
#include <algorithm>
#include <cstring>

using namespace core;
using namespace cssdk;

namespace adminskit
{
    Color24 StringToColor24(const std::string& str)
    {
        constexpr auto* format = "%i %i %i";
        int r{}, g{}, b{};

#ifdef MSVC_COMPILER
        sscanf_s(str.c_str(), format, &r, &g, &b);
#else
        std::sscanf(str.c_str(), format, &r, &g, &b);
#endif
        return Color24{static_cast<byte>(std::clamp(r, 0, 255)),
                       static_cast<byte>(std::clamp(g, 0, 255)),
                       static_cast<byte>(std::clamp(b, 0, 255))};
    }

    PlayerBase* GetObservablePlayer(const PlayerBase* const observer) noexcept
    {
        assert(IsValidEntity(observer));

        if (IsClient(observer->vars->i_user2) && observer->observer_target.IsValid()) {
            if (auto* const player = EntityPrivateData<PlayerBase>(observer->observer_target.Get());
                player != observer && IsPlayerAlive(player)) {
                return player;
            }
        }

        return nullptr;
    }

    bool ValidateCvarValue(const CVar* const cvar, const std::string_view old_value, std::string& new_value,
                           const std::add_pointer_t<bool(std::string&)> policy)
    {
        assert(cvar != nullptr);

        if (policy(new_value)) {
            return true;
        }

        console::AlertMessage("%s: Attempting to initialize with an invalid value \"%s\".", cvar->name, new_value.c_str());
        new_value = old_value;

        return false;
    }

    bool ColorCvarPolicy(std::string& new_value)
    {
        if (!str::IsEmptyOrWhiteSpace(new_value)) {
            constexpr auto* format = "%i %i %i";
            int r{}, g{}, b{};

#ifdef MSVC_COMPILER
            const auto args = sscanf_s(new_value.c_str(), format, &r, &g, &b);
#else
            const auto args = std::sscanf(new_value.c_str(), format, &r, &g, &b);
#endif
            if (args == 3) {
                new_value = str::Format(format, std::clamp(r, 0, 255), std::clamp(g, 0, 255), std::clamp(b, 0, 255));
                return true;
            }
        }

        return false;
    }

    bool FlagsCvarPolicy(std::string& new_value)
    {
        new_value = str::Trim(new_value);
        str::ToLower(new_value);

        return new_value.empty() ||
            std::all_of(new_value.cbegin(), new_value.cend(), [](const int ch) { return std::isalpha(ch); });
    }

    bool HudPosCvarPolicy(std::string& new_value)
    {
        if (str::IsEmptyOrWhiteSpace(new_value)) {
            return false;
        }

        constexpr auto* format = "%lf %lf";
        constexpr auto min = 0., max = 1.;
        double x{}, y{};

#ifdef MSVC_COMPILER
        const auto args = sscanf_s(new_value.c_str(), format, &x, &y);
#else
        const auto args = std::sscanf(new_value.c_str(), format, &x, &y);
#endif
        if (args != 2) {
            return false;
        }

        if ((x < min && std::abs(x - -1.) > DOUBLE_EPSILON) || x > max) {
            return false;
        }

        if ((y < min && std::abs(y - -1.) > DOUBLE_EPSILON) || y > max) {
            return false;
        }

        {
            new_value = str::Format(format, x, y);
        }

        return true;
    }
}

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN // NOLINT(clang-diagnostic-unused-macros)
#include <Windows.h>
#include <Psapi.h>
#pragma comment(lib, "kernel32.Lib")

namespace
{
    DWORD g_number_of_proc{1};
    HANDLE g_current_process{};
    ULARGE_INTEGER g_last_cpu{}, g_last_sys_cpu{}, g_last_user_cpu{};

    // This will be called after the library is loaded (equivalent of DllMain)
    AT_LOADED(Init) // cppcheck-suppress syntaxError
    {
        SYSTEM_INFO sys_info{};
        FILETIME time{}, sys{}, user{};

        GetSystemInfo(&sys_info);
        g_number_of_proc = sys_info.dwNumberOfProcessors;

        GetSystemTimeAsFileTime(&time);
        memcpy(&g_last_cpu, &time, sizeof(FILETIME));

        g_current_process = GetCurrentProcess();
        GetProcessTimes(g_current_process, &time, &time, &sys, &user);
        memcpy(&g_last_sys_cpu, &sys, sizeof(FILETIME));
        memcpy(&g_last_user_cpu, &user, sizeof(FILETIME));
    }

    [[nodiscard]] PROCESS_MEMORY_COUNTERS_EX* GetProcessMemoryInfoInternal()
    {
        static PROCESS_MEMORY_COUNTERS_EX pmc{};

        if (GetProcessMemoryInfo(g_current_process, reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&pmc), sizeof(pmc))) {
            return &pmc;
        }

        return nullptr;
    }
}

namespace adminskit
{
    std::size_t GetPhysicalMemoryUsed()
    {
        const auto* const pmc = GetProcessMemoryInfoInternal();
        return pmc ? static_cast<std::size_t>(pmc->WorkingSetSize) : 0;
    }

    std::size_t GetVirtualMemoryUsed()
    {
        const auto* const pmc = GetProcessMemoryInfoInternal();
        return pmc ? static_cast<std::size_t>(pmc->PrivateUsage) : 0;
    }

    double GetCpuUsed()
    {
        static FILETIME ftime{}, fsys{}, fuser{};
        static ULARGE_INTEGER now{}, sys{}, user{};

        GetSystemTimeAsFileTime(&ftime);
        std::memcpy(static_cast<void*>(&now), &ftime, sizeof(FILETIME));

        if (!GetProcessTimes(g_current_process, &ftime, &ftime, &fsys, &fuser)) {
            return 0.0;
        }

        std::memcpy(static_cast<void*>(&sys), &fsys, sizeof(FILETIME));
        std::memcpy(static_cast<void*>(&user), &fuser, sizeof(FILETIME));

        auto percent = static_cast<double>((sys.QuadPart - g_last_sys_cpu.QuadPart) +
                                           (user.QuadPart - g_last_user_cpu.QuadPart));

        percent /= static_cast<double>((now.QuadPart - g_last_cpu.QuadPart));
        percent /= g_number_of_proc;

        g_last_cpu = now;
        g_last_sys_cpu = sys;
        g_last_user_cpu = user;

        return percent * 100.0;
    }
}
#elif defined(__linux__)
#include <sys/times.h>

namespace
{
    int g_number_of_proc{};
    clock_t g_last_cpu{}, g_last_sys_cpu{}, g_last_user_cpu{};

    // This will be called after the library is loaded (equivalent of DllMain)
    AT_LOADED(Init) // cppcheck-suppress syntaxError
    {
        std::FILE* stream{};

        if ((stream = std::fopen("/proc/cpuinfo", "r"))) {
            char buffer[128];

            while (std::fgets(buffer, sizeof buffer, stream)) {
                if (str::StartsWith(buffer, "processor")) {
                    ++g_number_of_proc;
                }
            }

            std::fclose(stream);
        }
        else {
            g_number_of_proc = 1;
        }

        tms time_sample{};
        g_last_cpu = times(&time_sample);
        g_last_sys_cpu = time_sample.tms_stime;
        g_last_user_cpu = time_sample.tms_utime;
    }

    std::string GetStatusLine(const char* const prefix)
    {
        std::FILE* stream{};
        std::string result{};

        if ((stream = std::fopen("/proc/self/status", "r"))) {
            static char buffer[128];

            while (std::fgets(buffer, sizeof buffer, stream)) {
                if (str::StartsWith(buffer, prefix)) {
                    result = buffer;
                    break;
                }
            }

            std::fclose(stream);
        }

        return result;
    }

    std::size_t GetStatusValue(const char* const prefix)
    {
        const auto& line = GetStatusLine(prefix);

        if (!str::IsEmptyOrWhiteSpace(line)) {
            if (const auto pos = line.find_first_of("0123456789"); pos != std::string::npos) {
                return static_cast<std::size_t>(std::atoi(line.c_str() + pos));
            }
        }

        return 0;
    }
}

namespace adminskit
{
    std::size_t GetPhysicalMemoryUsed()
    {
        return GetStatusValue("VmRSS:") * 1024;
    }

    std::size_t GetVirtualMemoryUsed()
    {
        return GetStatusValue("VmSize:") * 1024;
    }

    double GetCpuUsed()
    {
        double percent{};
        tms time_sample{};
        auto now = times(&time_sample);

        if (now <= g_last_cpu || time_sample.tms_stime < g_last_sys_cpu || time_sample.tms_utime < g_last_user_cpu) {
            percent = -1.0; // Overflow detection. Just skip this value.
        }
        else {
            percent = (time_sample.tms_stime - g_last_sys_cpu) + (time_sample.tms_utime - g_last_user_cpu);
            percent /= (now - g_last_cpu);
            //percent /= g_number_of_proc;
            percent *= 100;
        }

        g_last_cpu = now;
        g_last_sys_cpu = time_sample.tms_stime;
        g_last_user_cpu = time_sample.tms_utime;

        return percent;
    }
}
#elif defined(__APPLE__)
namespace adminskit
{
    std::size_t GetPhysicalMemoryUsed()
    {
        return 0;
    }

    std::size_t GetVirtualMemoryUsed()
    {
        return 0;
    }

    double GetCpuUsed()
    {
        return 0.0;
    }
}
#endif
