#include "load_audio_file.hpp"
#include "libnyquist/Common.h"
#include "libnyquist/Decoders.h"

namespace Cool {

void load_audio_file(RtAudioW::Player& player, std::filesystem::path const& path)
{
    nqr::NyquistIO io;
    nqr::AudioData data;
    io.Load(&data, path.string());
    player.set_audio_data({
        std::move(data.samples),
        static_cast<unsigned int>(data.sampleRate),
        static_cast<unsigned int>(data.channelCount),
    });
}

} // namespace Cool
