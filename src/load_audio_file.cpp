#include "load_audio_file.hpp"
#include "libnyquist/Common.h"
#include "libnyquist/Decoders.h"

namespace Audio {

auto load_audio_file(std::filesystem::path const& path) -> AudioData
{
    nqr::NyquistIO io;
    nqr::AudioData data;
    io.Load(&data, path.string());
    return {
        std::move(data.samples),
        static_cast<unsigned int>(data.sampleRate),
        static_cast<unsigned int>(data.channelCount),
    };
}

void load_audio_file(Player& player, std::filesystem::path const& path)
{
    player.set_audio_data(load_audio_file(path));
}

} // namespace Audio
