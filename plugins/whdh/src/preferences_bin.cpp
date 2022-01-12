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

#include "preferences_bin.h"
#include <cista/serialization.h>
#include <core/system.h>
#include <whdh/cvars.h>
#include <cassert>
#include <ctime>
#include <stdexcept>
#include <vector>

using namespace core;
using namespace cista;

namespace
{
    bool IsValidAuthId(const char* const auth_id)
    {
        return !str::IsNullOrWhiteSpace(auth_id) &&
            !str::Equals(auth_id, "BOT") &&
            !str::Equals(auth_id, "HLTV") &&
            !str::Equals(auth_id, "STEAM_ID_LAN") &&
            !str::Equals(auth_id, "STEAM_ID_PENDING") &&
            !str::Equals(auth_id, "VALVE_ID_LAN") &&
            !str::Equals(auth_id, "VALVE_ID_PENDING");
    }

    int UnixTimestamp()
    {
        return static_cast<int>(std::time(nullptr));
    }
}

namespace whdh
{
    void PreferencesBin::Serialize()
    {
        EraseOutdated();

        if (preferences_map_.empty()) {
            const auto& bin_path = Path();

            if (std::remove(bin_path.c_str()) != 0) {
                // Ignore
            }

            return;
        }

        const auto stream = Open("wb");
        const auto buffer = serialize(preferences_map_);
        assert(stream != nullptr);

        if (std::fwrite(buffer.data(), sizeof(byte_buf::value_type), buffer.size(), stream.get()) == 0) {
            throw std::runtime_error{"File write error."};
        }
    }

    void PreferencesBin::Deserialize()
    {
        const auto stream = Open("rb");

        if (stream == nullptr) {
            preferences_map_.clear();
            return;
        }

        auto* const fp = stream.get();
        std::fseek(fp, 0, SEEK_END);
        const auto position = std::ftell(fp);

        if (position < 0) {
            throw std::runtime_error{"File read error."};
        }

        const auto size = static_cast<byte_buf::size_type>(position);
        byte_buf buffer(size);
        std::rewind(fp);

        if (const auto count = std::fread(buffer.data(), sizeof(byte_buf::value_type), buffer.size(), fp);
            count != size) {
#ifdef _WIN32
            const auto error_code = GetLastError();
            except_msg_ = str::Format("Failed to read file. Error %lu: %s", error_code, system::StrError(error_code));
#else
            except_msg_ = str::Format("Failed to read file. Error %d: %s", errno, system::StrError(errno));
#endif
            throw std::runtime_error{except_msg_};
        }

        preferences_map_ = *cista::deserialize<raw::hash_map<raw::string, UserPreferences>>(buffer);
        EraseOutdated();
    }

    void PreferencesBin::EraseOutdated()
    {
        const auto save_prefs = cvars::SavePrefs();

        if (save_prefs <= 0.F) {
            return;
        }

        const auto now = UnixTimestamp();
        const auto save_prefs_seconds = static_cast<int>(save_prefs * 24.F * 60.F * 60.F);
        std::vector<raw::string> outdated{};

        for (const auto& [auth, prefs] : preferences_map_) {
            if (now - prefs.timestamp > save_prefs_seconds) {
                outdated.emplace_back(auth);
            }
        }

        for (const auto& auth : outdated) {
            preferences_map_.erase(auth);
        }
    }

    UserPreferences PreferencesBin::Load(const char* const auth_id)
    {
        EraseOutdated();

        if (!preferences_map_.empty() && IsValidAuthId(auth_id)) {
            if (const auto& it = preferences_map_.find(raw::string{auth_id}); it != preferences_map_.end()) {
                it->second.timestamp = UnixTimestamp();
                return it->second;
            }
        }

        return UserPreferences{};
    }

    void PreferencesBin::Save(const char* const auth_id, UserPreferences& preferences)
    {
        if (cvars::SavePrefs() > 0.F && IsValidAuthId(auth_id)) {
            if (preferences.IsDefault()) {
                if (const auto& it = preferences_map_.find(raw::string{auth_id}); it != preferences_map_.end()) {
                    preferences_map_.erase(it);
                }
            }
            else {
                preferences.timestamp = UnixTimestamp();
                preferences_map_[auth_id] = preferences;
            }
        }
    }

#ifdef _WIN32
    std::unique_ptr<std::FILE, int (*)(std::FILE*)> PreferencesBin::Open(const char* const mode)
    {
        const auto& filepath = Path();

        std::FILE* stream{};
        const auto error_code = fopen_s(&stream, filepath.c_str(), mode);

        if (error_code == 2) {
            // Return nullptr if the specified file was not found.
            return std::unique_ptr<std::FILE, int (*)(std::FILE*)>{nullptr, +[](std::FILE* const) { return EOF; }};
        }

        if (error_code != 0) {
            const auto& error = system::StrError(error_code);
            except_msg_ = str::Format(R"(Could not open "%s". Mode: "%s". Error %d: %s)",
                                      filepath.c_str(), mode, error_code, error.c_str());

            throw std::runtime_error{except_msg_};
        }

        if (stream == nullptr) {
            except_msg_ = str::Format(R"(Could not open "%s". Mode: "%s".)", filepath.c_str(), mode);
            throw std::runtime_error{except_msg_};
        }

        return std::unique_ptr<std::FILE, int (*)(std::FILE*)>{
            stream, +[](std::FILE* const fp) {
                return fp ? std::fclose(fp) : EOF;
            }};
    }
#else
    std::unique_ptr<std::FILE, int (*)(std::FILE*)> PreferencesBin::Open(const char* const mode)
    {
        const auto& filepath = Path();
        auto* const stream = std::fopen(filepath.c_str(), mode);

        if (stream == nullptr && errno != 2) {
            except_msg_ = str::Format(R"(Could not open "%s". Mode: "%s". Error %d: %s)",
                                      filepath.c_str(), mode, errno, system::StrError(errno)); // NOLINT(concurrency-mt-unsafe)

            throw std::runtime_error{except_msg_};
        }

        return std::unique_ptr<std::FILE, int (*)(std::FILE*)>{
            stream, +[](std::FILE* const fp) {
                return fp ? std::fclose(fp) : EOF;
            }};
    }
#endif
}
