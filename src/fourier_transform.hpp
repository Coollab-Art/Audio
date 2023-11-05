#pragma once
#include <functional>
#include <span>

namespace Audio {

/// Function that takes a function and calls it on each audio sample constituting the data you wish to analyze.
using ForEachSample = std::function<void(std::function<void(float)> const&)>;

struct Spectrum {
    /// Amplitudes of each frequency, where the first frequency is 0 Hz and they are evenly spaced and there is a delta of `frequency_delta_between_values_in_data` between each frequency.
    std::vector<float> data;
    /// In hz
    float frequency_delta_between_values_in_data;

    /// Evaluates the spectrum at the given frequency (in Hertz) and returns its amplitude.
    [[nodiscard]] auto at_frequency(float frequency_in_hertz) const -> float;
};

/// Computes the fourier transform of the given signal.
/// `for_each_sample` is a function that takes a callback and calls it for each of the samples of your audio data (see our tests for an example).
/// Note that you should apply a window function to your audio data, to make sure it is 0 at the beginning and the end: https://digitalsoundandmusic.com/2-3-11-windowing-functions-to-eliminate-spectral-leakage/
/// You can optionally set `max_frequency_in_hz` to tell us the highest frequency that you are interested in, and we will not compute more than that, and return only frequencies up to that value.
/// NB: If the `samples_count` is not a power of two, we will zero-pad the audio data to reach the next power of two: https://mechanicalvibration.com/Zero_Padding_FFTs.html
auto fourier_transform(size_t samples_count, ForEachSample const& for_each_sample, float audio_data_sample_rate, float max_frequency_in_hz = -1.f) -> Spectrum;

/// Computes the fourier transform of the given `audio_data` signal.
/// Note that you should apply a window function to your `audio_data`, to make sure it is 0 at the beginning and the end: https://digitalsoundandmusic.com/2-3-11-windowing-functions-to-eliminate-spectral-leakage/
/// You can optionally set `max_frequency_in_hz` to tell us the highest frequency that you are interested in, and we will not compute more than that, and return only frequencies up to that value.
/// NB: If the `samples_count` is not a power of two, we will zero-pad the `audio_data` to reach the next power of two: https://mechanicalvibration.com/Zero_Padding_FFTs.html
auto fourier_transform(std::span<float const> audio_data, float audio_data_sample_rate, float max_frequency_in_hz = -1.f) -> Spectrum;

} // namespace Audio