#include "AudioData.h"
#include "libnyquist/Common.h"
#include "libnyquist/Decoders.h"

namespace Cool {

void load_audio_file(RtAudioW::Player& player, std::filesystem::path const& path)
{
    nqr::NyquistIO io;
    nqr::AudioData data;
    io.Load(&data, path.string());
    player.open(
        std::move(data.samples),
        static_cast<unsigned int>(data.sampleRate),
        static_cast<unsigned int>(data.channelCount)
    );
}

// AudioData::AudioData(std::filesystem::path const& path)
// {
//     nqr::NyquistIO io;
//     nqr::AudioData data;
//     io.Load(&data, path.string());
//     _player.open(
//         std::move(data.samples),
//         static_cast<unsigned int>(data.sampleRate),
//         static_cast<unsigned int>(data.channelCount)
//     );
// }

// void AudioData::play()
// {
//     _player.play();
// }

} // namespace Cool
