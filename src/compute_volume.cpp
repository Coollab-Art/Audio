#include "compute_volume.hpp"
#include <cstdint>

namespace Cool {

auto compute_volume(RtAudioW::Player const& player, float average_duration_in_seconds /*TODO(Audio) better name*/) -> float
{
    if (!player.has_audio_data())
        return 0.f;

    auto const width = static_cast<int64_t>(static_cast<float>(player.audio_data().sample_rate) * average_duration_in_seconds);

    float volume{0.f};
    for (int64_t i = 0; i < width; ++i)
    {
        int64_t const offset = i - width / 2;
        for (int64_t channel_index = 0; channel_index < player.audio_data().channels_count; ++channel_index)
        {
            // TODO(Audio) When the audio loops, we might start sampling the next play of the audio. But since the width is small, this is probably not a noticeable problem ?
            volume += std::abs(player.sample_unaltered_volume(player.current_frame_index() + offset, channel_index));
        }
    }
    return volume / static_cast<float>(width) / static_cast<float>(player.audio_data().channels_count);
}

auto compute_volume(std::span<float const> data) -> float
{
    float volume{0.f};
    for (float const sample : data)
    {
        volume += std::abs(sample);
    }
    return volume / static_cast<float>(data.size());
}

} // namespace Cool