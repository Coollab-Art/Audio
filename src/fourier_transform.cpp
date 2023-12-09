#include "fourier_transform.hpp"
#include <algorithm>
#include <complex>
#include <vector>
#include "dj_fft.h"

namespace Audio {

template<std::integral T>
static auto next_power_of_two(T n) -> T
{
    if (n != 0 && !(n & (n - 1)))
        return n; // n is already a power of 2

    T power = 1;
    while (power < n)
    {
        power <<= 1;
    }

    return power;
}

/// Since the FFT requires a size that is a power of two, we add 0s at the end of the data.
/// https://mechanicalvibration.com/Zero_Padding_FFTs.html
static void zero_pad(std::vector<std::complex<float>>& data)
{
    data.resize(next_power_of_two(data.size()));
}

auto fourier_transform(size_t samples_count, ForEachSample const& for_each_sample, float audio_data_sample_rate, float max_frequency_in_hz) -> Spectrum
{
    // Create a vector of complex numbers containing the audio data
    auto fft_input = std::vector<std::complex<float>>{};
    fft_input.reserve(next_power_of_two(samples_count));
    for_each_sample([&](float const sample) {
        fft_input.emplace_back(sample);
    });

    // Make sure the size of fft_input is a power of 2.
    zero_pad(fft_input);

    // Compute the fft
    auto        fft_output = dj::fft1d(fft_input, dj::fft_dir::DIR_FWD);
    float const delta_between_frequencies{audio_data_sample_rate / static_cast<float>(fft_output.size())}; // The values in the `fft_output` correspond to frequencies between 0 and sample_rate, evenly spaced.

    // TODO(Audio) Instead of computing the fft on a signal with many samples, and then resizing it to fit the requested `max_output_frequency_in_hz`, we could reduce it's sample rate before computing the fft, to minimize the number of frequencies that are computed for nothing (since they will be discarded afterwards anyways).

    // Ignore the redondant information and the frequencies that are above max_frequency_in_hz.
    {
        size_t final_size = fft_output.size() / 2; // The second half is a mirror of the first half so we don't need it.
        if (max_frequency_in_hz != -1.f)
        {
            final_size = std::min(
                final_size,
                static_cast<size_t>(max_frequency_in_hz / delta_between_frequencies)
            );
        }
        fft_output.resize(final_size);
    }

    // Compute the amplitude corresponding to each frequency
    auto spectrum = std::vector<float>{};
    spectrum.reserve(fft_output.size());
    std::transform(fft_output.begin(), fft_output.end(), std::back_inserter(spectrum), [](std::complex<float> const z) {
        return std::abs(z);
    });

    return {spectrum, delta_between_frequencies};
}

auto fourier_transform(std::span<float const> audio_data, float audio_data_sample_rate, float max_frequency_in_hz) -> Spectrum
{
    return fourier_transform(
        audio_data.size(), [&](std::function<void(float)> const& callback) {
            for (float const sample : audio_data)
                callback(sample);
        },
        audio_data_sample_rate, max_frequency_in_hz
    );
}

auto Spectrum::at_frequency(float frequency_in_hertz) const -> float
{
    assert(frequency_in_hertz >= 0.f);
    auto const idx = static_cast<size_t>(frequency_in_hertz / frequency_delta_between_values_in_data);
    if (idx >= data.size())
        return 0.f;
    return data[idx];
}

} // namespace Audio