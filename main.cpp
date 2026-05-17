#define _USE_MATH_DEFINES
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>

#include "AudioSignal.h"
#include "AudioComponents.h"
#include "AudioFile.h"

using namespace std;

void printHeader(const string& title) {
    int w = 60;
    string border(w, '=');
    cout << "\n" << border << "\n";
    int pad = (w - (int)title.size()) / 2;
    cout << string(pad, ' ') << title << "\n";
    cout << border << "\n";
}

void printSignalInfo(const AudioSignal& sig, const string& label = "") {
    cout << "\n";
    if (!label.empty()) cout << "  [" << label << "]\n";
    cout << "  Ten          : " << sig.getName() << "\n";
    cout << "  Sample Rate  : " << sig.getSampleRate() << " Hz\n";
    cout << "  Bit Depth    : " << sig.getBitDepth() << " bit\n";
    cout << "  So mau       : " << sig.getNumSamples() << "\n";
    cout << "  Thoi luong   : " << fixed << setprecision(3) << sig.getDuration() << " s\n";
    cout << "  Bien do dinh : " << setprecision(4) << sig.getPeakAmplitude() << "\n";
    cout << "  RMS          : " << sig.getRMS() << "\n";
    cout << "  Muc (dBFS)   : " << setprecision(1) << sig.getdBFS() << " dBFS\n";
}

void plotWaveform(const AudioSignal& sig, const string& label, int numCols = 72, int numRows = 18, size_t maxSamples = 500) {
    const auto& samples = sig.getSamples();
    if (samples.empty()) return;
    cout << "\n  -- Waveform: " << label << " --\n";
    size_t step = max((size_t)1, samples.size() / maxSamples);
    vector<double> display;
    for (size_t i = 0; i < samples.size() && display.size() < (size_t)numCols; i += step)
        display.push_back(samples[i]);

    for (int row = numRows - 1; row >= 0; row--) {
        double y = (double)(row - numRows / 2) / (numRows / 2);
        cout << "  ";
        if (row == numRows / 2)      cout << "0|";
        else if (row == numRows - 1) cout << "+|";
        else if (row == 0)           cout << "-|";
        else                         cout << " |";

        for (size_t col = 0; col < display.size(); col++) {
            double val = display[col];
            double cellTop    = y + 1.0 / numRows;
            double cellBottom = y - 1.0 / numRows;
            if (val >= cellBottom && val < cellTop)
                cout << "*";
            else
                cout << " ";
        }
        cout << "\n";
    }
    cout << "   +" << string(display.size(), '-') << "\n";
    cout << "    0" << string(display.size() / 2 - 3, ' ') << "t(s)" << "\n";
}

void plotSpectrum(const AudioSignal& sig, int numBins = 20) {
    const auto& samples = sig.getSamples();
    int sr = sig.getSampleRate();
    if (samples.empty()) return;
    cout << "\n  -- Pho bien do (Amplitude Spectrum) --\n";
    int N = min((int)samples.size(), 4096);
    double binSize = (double)sr / N;

    for (int k = 1; k <= numBins; k++) {
        double re = 0, im = 0;
        for (int n = 0; n < N; n++) {
            double angle = 2.0 * M_PI * k * n / N;
            re += samples[n] * cos(angle);
            im -= samples[n] * sin(angle);
        }
        double mag = sqrt(re * re + im * im) / N * 2.0;
        int freq = static_cast<int>(k * binSize);

        cout << "  " << setw(5) << freq << " Hz | ";
        int bars = static_cast<int>(mag * 50);
        bars = min(bars, 40);
        cout << string(bars, '#') << string(40 - bars, ' ') << " | " << fixed << setprecision(4) << mag << "\n";
    }
}

void printStats(const AudioSignal& sig) {
    const auto& s = sig.getSamples();
    if (s.empty()) return;
    double sum = 0, minV = s[0], maxV = s[0];
    for (double x : s) {
        sum += x;
        minV = min(minV, x);
        maxV = max(maxV, x);
    }
    double mean = sum / s.size();
    double var = 0;
    for (double x : s) var += (x - mean) * (x - mean);
    var /= s.size();

    cout << "\n  -- Thong ke thong ke --\n";
    cout << "  Min sample   : " << setprecision(4) << minV << "\n";
    cout << "  Max sample   : " << maxV << "\n";
    cout << "  Mean         : " << mean << "\n";
    cout << "  Variance     : " << var << "\n";
    cout << "  Std Dev      : " << sqrt(var) << "\n";
    cout << "  Dynamic Range: " << (maxV - minV) << "\n";
}

int main() {
    cout << "\n";
    cout << "  +==============================================+\n";
    cout << "  |   HE THONG XU LY TIN HIEU AM THANH SO      |\n";
    cout << "  |        Digital Audio Signal System          |\n";
    cout << "  |   C++ OOP Demo - Project 1                  |\n";
    cout << "  +==============================================+\n";

    AudioProcessor  processor;
    AudioEffect     effect(0.5, 100.0);
    AudioFile       audioFile("output.wav");

    printHeader("1. TAO TIN HIEU SIN 440 Hz (La/A4)");
    AudioSignal sineA4 = AudioGenerator::generateSine(440.0, 0.1, 0.8);
    printSignalInfo(sineA4, "Sine Wave 440 Hz");
    plotWaveform(sineA4, "Sine 440 Hz", 72, 18, 200);
    printStats(sineA4);

    printHeader("2. SO SANH DANG SONG");
    auto sine   = AudioGenerator::generateSine(200.0, 0.05);
    auto square = AudioGenerator::generateSquare(200.0, 0.05);
    auto tri    = AudioGenerator::generateTriangle(200.0, 0.05);
    auto noise  = AudioGenerator::generateWhiteNoise(0.05, 0.3);
    plotWaveform(sine,   "Sine     200 Hz", 72, 12, 150);
    plotWaveform(square, "Square   200 Hz", 72, 12, 150);
    plotWaveform(tri,    "Triangle 200 Hz", 72, 12, 150);
    plotWaveform(noise,  "White Noise",      72, 12, 150);

    printHeader("3. KHUYECH DAI & CHUAN HOA");
    AudioSignal sigAmp = AudioGenerator::generateSine(300.0, 0.05, 0.3);
    cout << "\n  Truoc khi khuech dai:\n";
    cout << "    Peak = " << sigAmp.getPeakAmplitude()
         << "  RMS = " << sigAmp.getRMS() << " dBFS = " << sigAmp.getdBFS() << "\n";
    processor.amplify(sigAmp, 2.5);
    cout << "  Sau khi khuech dai x2.5:\n";
    cout << "    Peak = " << sigAmp.getPeakAmplitude()
         << "  RMS = " << sigAmp.getRMS() << " dBFS = " << sigAmp.getdBFS() << "\n";
    processor.normalize(sigAmp);
    cout << "  Sau khi chuan hoa:\n";
    cout << "    Peak = " << sigAmp.getPeakAmplitude()
         << "  RMS = " << sigAmp.getRMS() << " dBFS = " << sigAmp.getdBFS() << "\n";

    printHeader("4. BO LOC SO (DIGITAL FILTER)");
    AudioSignal noisy = AudioGenerator::generateSine(100.0, 0.05, 0.6);
    AudioSignal hfNoise = AudioGenerator::generateSine(3000.0, 0.05, 0.3);
    AudioSignal mixed = processor.mix(noisy, hfNoise, 0.7);
    mixed.setName("Mixed 100Hz + 3kHz noise");
    cout << "\n  Tin hieu truoc loc:\n";
    cout << "    Peak = " << setprecision(4) << mixed.getPeakAmplitude()
         << "  RMS = " << mixed.getRMS() << "\n";
    plotWaveform(mixed, "Truoc loc - 100Hz + 3kHz noise", 72, 12, 150);

    LowPassFilter  lpf(500.0, 44100.0);
    HighPassFilter hpf(80.0,  44100.0);
    BandPassFilter bpf(200.0, 2000.0, 44100.0);

    AudioSignal filtered = mixed;
    lpf.apply(filtered);
    filtered.setName("After LPF 500Hz");
    cout << "\n  Sau Low-Pass Filter (cutoff=500Hz):\n";
    cout << "    Peak = " << filtered.getPeakAmplitude()
         << "  RMS = " << filtered.getRMS() << "\n";
    plotWaveform(filtered, "Sau LPF 500Hz", 72, 12, 150);
    cout << "\n  Bo loc: " << lpf.getFilterType()
         << " | Cutoff: " << lpf.getCutoff() << " Hz\n";

    printHeader("5. HIEU UNG AM THANH (AUDIO EFFECTS)");
    AudioSignal drySignal = AudioGenerator::generateSine(440.0, 0.2, 0.7);
    drySignal.setName("Dry Signal 440Hz");
    AudioSignal echoSig = drySignal;
    effect.echo(echoSig, 0.05, 0.45);
    echoSig.setName("Echo (delay=50ms)");
    cout << "\n  -- Echo Effect --\n";
    cout << "  Delay  : 50 ms   Feedback: 0.45\n";
    cout << "  Dry Peak: " << drySignal.getPeakAmplitude() << "\n";
    cout << "  Wet Peak: " << echoSig.getPeakAmplitude() << "\n";
    plotWaveform(echoSig, "Echo Effect", 72, 12, 200);

    AudioSignal reverbSig = drySignal;
    effect.reverb(reverbSig, 0.6);
    reverbSig.setName("Reverb (room=0.6)");
    cout << "\n  -- Reverb Effect --\n";
    cout << "  Room Size: 0.6   Wet/Dry: 0.5\n";
    cout << "  Wet Peak: " << reverbSig.getPeakAmplitude() << "\n";

    AudioSignal distSig = drySignal;
    effect.distort(distSig, 0.4);
    distSig.setName("Distortion (threshold=0.4)");
    cout << "\n  -- Distortion Effect --\n";
    cout << "  Threshold: 0.4   Type: Hard Clipping\n";
    plotWaveform(distSig, "Distortion", 72, 12, 200);

    AudioSignal tremSig = drySignal;
    effect.tremolo(tremSig, 5.0, 0.6);
    tremSig.setName("Tremolo (5Hz, depth=0.6)");
    plotWaveform(tremSig, "Tremolo 5Hz", 72, 12, 200);

    printHeader("6. PHO TAN SO (FREQUENCY SPECTRUM)");
    AudioSignal specSig = AudioGenerator::generateSine(440.0, 0.1, 0.8);
    plotSpectrum(specSig, 15);

    printHeader("7. DTMF TONE (DIEN THOAI)");
    cout << "\n  Cac phim DTMF:\n";
    struct { const char* key; double f1; double f2; } dtmfKeys[] = {
        { "1", 697, 1209 }, { "2", 697, 1336 }, { "3", 697, 1477 },
        { "4", 770, 1209 }, { "5", 770, 1336 }, { "6", 770, 1477 },
        { "7", 852, 1209 }, { "8", 852, 1336 }, { "9", 852, 1477 },
        { "*", 941, 1209 }, { "0", 941, 1336 }, { "#", 941, 1477 },
    };
    for (auto& d : dtmfKeys) {
        cout << "  Phim [" << d.key << "] : " << setw(4) << (int)d.f1 << " Hz + "
             << setw(4) << (int)d.f2 << " Hz\n";
    }
    AudioSignal dtmf5 = AudioGenerator::generateDTMF(770, 1336, 0.05);
    plotWaveform(dtmf5, "DTMF Phim 5 (770+1336 Hz)", 72, 12, 150);

    printHeader("8. TRON TIN HIEU (MIXING)");
    AudioSignal bass   = AudioGenerator::generateSine(80.0, 0.1, 0.7);
    AudioSignal melody = AudioGenerator::generateSine(523.25, 0.1, 0.5);
    AudioSignal drum   = AudioGenerator::generateWhiteNoise(0.1, 0.2);
    AudioSignal mix1 = processor.mix(bass, melody, 0.6);
    AudioSignal mix2 = processor.mix(mix1, drum, 0.8);
    mix2.setName("Bass + Melody + Drum mix");
    cout << "\n  Bass     (80 Hz)  : Peak=" << bass.getPeakAmplitude() << "\n";
    cout << "  Melody   (523 Hz) : Peak=" << melody.getPeakAmplitude() << "\n";
    cout << "  Drum     (noise)  : Peak=" << drum.getPeakAmplitude() << "\n";
    cout << "  Mix ket qua       : Peak=" << mix2.getPeakAmplitude()
         << "  RMS=" << setprecision(4) << mix2.getRMS() << "\n";
    plotWaveform(mix2, "Final Mix", 72, 16, 300);

    printHeader("9. FADE IN / FADE OUT");
    AudioSignal fadeSig = AudioGenerator::generateSine(440.0, 0.15, 0.8);
    cout << "\n  Truoc fade:\n";
    plotWaveform(fadeSig, "Truoc Fade", 72, 12, 200);
    processor.fadeIn(fadeSig, 2000);
    processor.fadeOut(fadeSig, 2000);
    fadeSig.setName("Fade In + Out");
    cout << "\n  Sau Fade In (2000 mau) + Fade Out (2000 mau):\n";
    plotWaveform(fadeSig, "Sau Fade In+Out", 72, 12, 200);

    printHeader("10. GHI FILE WAV");
    AudioSignal finalOut = AudioGenerator::generateSine(440.0, 1.0, 0.8);
    effect.reverb(finalOut, 0.4);
    processor.fadeIn(finalOut, 4000);
    processor.fadeOut(finalOut, 4000);
    bool saved = audioFile.save(finalOut, "output_demo.wav");
    if (saved) {
        cout << "\n  [OK] Da luu file: output_demo.wav\n";
        cout << "  Thoi luong: " << finalOut.getDuration() << " s\n";
        cout << "  So mau   : " << finalOut.getNumSamples() << "\n";
        cout << "  Format   : WAV / 16-bit PCM / 44100 Hz\n";
    } else {
        cout << "\n  [WARN] Khong the ghi file.\n";
    }

    printHeader("TOM TAT HE THONG");
    cout << "\n  Cac lop da cai dat:\n";
    cout << "  + AudioSignal    - Luu tru & cung cap thong tin tin hieu\n";
    cout << "  + AudioProcessor - Khuyech dai, chuan hoa, fade, tron\n";
    cout << "  + AudioFilter    - Lop truu tuong cho bo loc\n";
    cout << "    - LowPassFilter  (IIR bac 1)\n";
    cout << "    - HighPassFilter (IIR bac 1)\n";
    cout << "    - BandPassFilter (LPF + HPF ket hop)\n";
    cout << "  + AudioEffect    - Echo, Reverb, Chorus, Distortion, Tremolo\n";
    cout << "  + AudioGenerator - Sine, Square, Triangle, Noise, DTMF\n";
    cout << "  + AudioFile      - Doc/Ghi WAV 16-bit PCM\n\n";
    cout << "  OK - Tat ca usecase demo thanh cong!\n\n";

    return 0;
}