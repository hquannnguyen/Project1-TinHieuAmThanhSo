#pragma once

#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <iostream>

using namespace std;

class AudioSignal {
private:
    vector<double> samples;
    int sampleRate;
    int numChannels;
    int bitDepth;
    string name;

public:
    AudioSignal(int sr = 44100, int ch = 1, int bd = 16)
        : sampleRate(sr), numChannels(ch), bitDepth(bd), name("Untitled") {}

    AudioSignal(const vector<double>& s, int sr = 44100, int ch = 1, int bd = 16)
        : samples(s), sampleRate(sr), numChannels(ch), bitDepth(bd), name("Signal") {}

    const vector<double>& getSamples() const { return samples; }
    int getSampleRate() const { return sampleRate; }
    int getNumChannels() const { return numChannels; }
    int getBitDepth() const { return bitDepth; }
    const string& getName() const { return name; }

    double getDuration() const {
        if (sampleRate == 0 || numChannels == 0) return 0.0;
        return static_cast<double>(samples.size()) / (sampleRate * numChannels);
    }

    size_t getNumSamples() const { return samples.size(); }

    void setSamples(const vector<double>& s) { samples = s; }
    void setName(const string& n) { name = n; }
    void addSample(double s) { samples.push_back(s); }
    void clear() { samples.clear(); }

    double getPeakAmplitude() const {
        if (samples.empty()) return 0.0;
        double peak = 0.0;
        for (double s : samples) peak = max(peak, abs(s));
        return peak;
    }

    double getRMS() const {
        if (samples.empty()) return 0.0;
        double sum = 0.0;
        for (double s : samples) sum += s * s;
        return sqrt(sum / samples.size());
    }

    double getdBFS() const {
        double rms = getRMS();
        if (rms == 0.0) return -96.0;
        return 20.0 * log10(rms);
    }

    void printInfo() const;
};

inline void AudioSignal::printInfo() const {
    cout << "AudioSignal[" << name << "] "
         << sampleRate << "Hz " << numChannels << "ch "
         << bitDepth << "bit " << getDuration() << "s\n";
}