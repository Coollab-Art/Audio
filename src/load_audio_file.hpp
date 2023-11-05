#pragma once
#include <filesystem>
#include "Player.hpp"

namespace Audio {

/// Throws an exception if the loading fails (e.g. if the file is not found).
auto load_audio_file(std::filesystem::path const&) -> AudioData;
/// Throws an exception if the loading fails (e.g. if the file is not found).
void load_audio_file(Player&, std::filesystem::path const&);

} // namespace Audio