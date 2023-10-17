#pragma once
#include <filesystem>
#include <vector>

namespace Cool {

class AudioData {
public:
    /// Throws an exception if the file is not found.
    explicit AudioData(std::filesystem::path const&);

    /// All the audio samples.
    [[nodiscard]] auto samples() const -> std::vector<float> const& { return _samples; }
    /// The number of channels (mono vs stereo). When using 2 channels, they will be interleaved: every other sample will correspond to the first channel, and others to the second channel.
    [[nodiscard]] auto channels_count() const -> int { return _channels_count; }
    /// The number of samples per second.
    [[nodiscard]] auto sample_rate() const -> int { return _sample_rate; }

private:
    std::vector<float> _samples{};
    int                _channels_count{};
    int                _sample_rate{};
};

} // namespace Cool