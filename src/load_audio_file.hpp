#pragma once
#include <filesystem>
#include "RtAudioWrapper/RtAudioWrapper.hpp"

namespace Cool {

/// Throws an exception if the loading fails (e.g. if the file is not found).
void load_audio_file(RtAudioW::Player&, std::filesystem::path const&);

} // namespace Cool