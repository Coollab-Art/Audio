#include <exe_path/exe_path.h>
#include <Audio/Audio.hpp>
#include <algorithm>
#include <complex>
#include <iterator>
#include <quick_imgui/quick_imgui.hpp>
#include "imgui.h"

#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest/doctest.h>

// Learn how to use Dear ImGui: https://coollibs.github.io/contribute/Programming/dear-imgui

auto main(int argc, char* argv[]) -> int
{
    const int  exit_code              = doctest::Context{}.run();                   // Run all unit tests
    const bool should_run_imgui_tests = argc < 2 || strcmp(argv[1], "-nogpu") != 0; // NOLINT(*pro-bounds-pointer-arithmetic)
    if (
        should_run_imgui_tests
        && exit_code == 0 // Only open the window if the tests passed; this makes it easier to notice when some tests fail
    )
    {
        // Input stream
        Audio::InputStream input_stream{
            [](RtAudioErrorType /* type */, std::string const& error_message) {
                std::cerr << error_message << '\n';
            }};
        // Load the audio file
        Audio::load_audio_file(Audio::player(), exe_path::dir() / "../../tests/res/Monteverdi - L'Orfeo, Toccata.mp3");
        Audio::player().play();

        static constexpr int64_t         N = 1024;  // input size NB: Must be a power of 2 for dj::fft1d
        std::vector<std::complex<float>> myData(N); // input data

        quick_imgui::loop("Audio tests", [&]() { // Open a window and run all the ImGui-related code
            for (int64_t i = 0; i < N; i++)
                myData[static_cast<size_t>(i)] = Audio::player().sample_unaltered_volume(i + Audio::player().current_frame_index(), 0);
            auto const fftData = dj::fft1d(myData, dj::fft_dir::DIR_FWD);
            auto       data    = std::vector<float>{};
            std::transform(fftData.begin(), fftData.end(), std::back_inserter(data), [](auto const x) {
                return std::abs(x);
            });

            ImGui::Begin("Audio tests");
            // Player
            ImGui::SeparatorText("Player");
            if (ImGui::Button(Audio::player().properties().is_muted ? "Unmute" : "Mute"))
                Audio::player().properties().is_muted = !Audio::player().properties().is_muted;
            ImGui::PlotHistogram(
                "Spectrum",
                data.data(),
                static_cast<int>(data.size()) / 2, // The second half is a mirror of the first half, so ne need to display it.
                0, nullptr,
                0.f, 1.f,
                {0.f, 100.f}
            );
            // Input stream
            ImGui::SeparatorText("Input stream");
            auto const input_device_ids = input_stream.device_ids();
            if (ImGui::BeginCombo("Input device", input_stream.current_device_name().c_str()))
            {
                for (unsigned int id : input_device_ids)
                {
                    auto const info        = input_stream.device_info(id);
                    bool const is_selected = info.name == input_stream.current_device_name();
                    if (ImGui::Selectable(info.name.c_str(), is_selected))
                        input_stream.set_device(id);

                    if (is_selected) // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            auto data_from_input_stream = std::vector<float>{};
            input_stream.for_each_sample(512, [&](float const sample) {
                data_from_input_stream.push_back(sample);
            });
            ImGui::PlotHistogram(
                "Input Data",
                data_from_input_stream.data(),
                static_cast<int>(data_from_input_stream.size()),
                0, nullptr,
                -0.1f, 0.1f,
                {0.f, 100.f}
            );
            //
            ImGui::End();
            ImGui::ShowDemoWindow();
        });
    }
    Audio::shut_down();
    return exit_code;
}

TEST_CASE("At least one API has been compiled")
{
    Audio::Player player{}; // This will assert if no API is available, which is something we want to detect.
}

TEST_CASE("Loading a .wav file")
{
    Audio::load_audio_file(Audio::player(), exe_path::dir() / "../../tests/res/10-1000-10000-20000.wav");

    CHECK(Audio::player().audio_data().channels_count == 1);
    CHECK(Audio::player().audio_data().sample_rate == 41000);
    CHECK(Audio::player().audio_data().samples.size() == 164000);
}

TEST_CASE("Loading a .mp3 file")
{
    Audio::load_audio_file(Audio::player(), exe_path::dir() / "../../tests/res/Monteverdi - L'Orfeo, Toccata.mp3");

    CHECK(Audio::player().audio_data().channels_count == 2);
    CHECK(Audio::player().audio_data().sample_rate == 44100);
    CHECK(Audio::player().audio_data().samples.size() == 9819648);
}

TEST_CASE("FFT")
{
    // Load the audio file
    Audio::load_audio_file(Audio::player(), exe_path::dir() / "../../tests/res/10-1000-10000-20000.wav");

    auto fft_input = std::vector<std::complex<float>>{};

    static constexpr int64_t N = 65536; // FFT size. NB: Must be a power of 2
    for (int64_t i = 0; i < N; i++)
        fft_input.emplace_back(Audio::player().sample_unaltered_volume(i, 0)); // Only use 1 channel. This is simple, but ideally you should average the values across all the channels.

    auto const spectrum = dj::fft1d(fft_input, dj::fft_dir::DIR_FWD);

    CHECK(spectrum.size() == N);
    CHECK(std::abs(spectrum[16]) == doctest::Approx(38.669884));
    CHECK(std::abs(spectrum[1598]) == doctest::Approx(27.571739));
    CHECK(std::abs(spectrum[1599]) == doctest::Approx(21.486385));
    CHECK(std::abs(spectrum[15984]) == doctest::Approx(29.728823));
    CHECK(std::abs(spectrum[15985]) == doctest::Approx(18.963114));
    CHECK(std::abs(spectrum[31968]) == doctest::Approx(10.106586));
    CHECK(std::abs(spectrum[31969]) == doctest::Approx(35.716843));
    CHECK(std::abs(spectrum[33567]) == doctest::Approx(35.765961));
    CHECK(std::abs(spectrum[33568]) == doctest::Approx(10.012813));
    CHECK(std::abs(spectrum[49551]) == doctest::Approx(19.058596));
    CHECK(std::abs(spectrum[49552]) == doctest::Approx(29.651283));
    CHECK(std::abs(spectrum[63937]) == doctest::Approx(21.579424));
    CHECK(std::abs(spectrum[63938]) == doctest::Approx(27.487740));
    CHECK(std::abs(spectrum[65520]) == doctest::Approx(38.676113));
}