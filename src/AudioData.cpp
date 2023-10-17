#include "AudioData.h"
#include "libnyquist/Common.h"
#include "libnyquist/Decoders.h"

namespace Cool {

AudioData::AudioData(std::filesystem::path const& path)
{
    nqr::NyquistIO io;
    nqr::AudioData data;
    io.Load(&data, path.string());
    _samples        = std::move(data.samples);
    _channels_count = std::move(data.channelCount); // NOLINT(*-move-const-arg)
    _sample_rate    = std::move(data.sampleRate);   // NOLINT(*-move-const-arg)
}

} // namespace Cool
