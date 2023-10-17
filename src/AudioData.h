#pragma once
#include <filesystem>
#include "RtAudioWrapper/RtAudioWrapper.hpp"
// #include <optional>
// #include <vector>

namespace Cool {

void load_audio_file(RtAudioW::Player&, std::filesystem::path const&);

// class AudioData {
// public:
//     /// Throws an exception if the file is not found.
//     explicit AudioData(std::filesystem::path const&);

//     /// All the audio samples.
//     [[nodiscard]] auto samples() const -> std::vector<float> const& { return _player.samples(); }
//     /// The number of channels (mono vs stereo). When using 2 channels, they will be interleaved: every other sample will correspond to the first channel, and others to the second channel.
//     [[nodiscard]] auto channels_count() const -> unsigned int { return _player.get_data_channels(); }
//     /// The number of samples per second.
//     [[nodiscard]] auto sample_rate() const -> unsigned int { return _player.sample_rate(); }

//     // Does nothing if this audio instance is already playing.
//     void play();

// private:
//     RtAudioW::Player _player{};
// };

} // namespace Cool