/*
  This file is part of Shuriken Beat Slicer.

  Copyright (C) 2014 Andrew M Taylor <a.m.taylor303@gmail.com>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program. If not, see <https://www.gnu.org/licenses/>
  or write to the Free Software Foundation, Inc., 51 Franklin Street,
  Fifth Floor, Boston, MA  02110-1301, USA.

*/

#ifndef SAMPLERAUDIOSOURCE_H
#define SAMPLERAUDIOSOURCE_H

#include "JuceHeader.h"
#include "samplebuffer.h"


class SamplerAudioSource : public PositionableAudioSource
{
public:
    SamplerAudioSource();

    void setSample( const SharedSampleBuffer sampleBuffer, const qreal sampleRate );
    bool setSampleRanges( const QList<SharedSampleRange> sampleRangeList );

    void clearSample();

    void playRange( const SharedSampleRange sampleRange );
    void playAll();

    void stop();

    void prepareToPlay( int /*samplesPerBlockExpected*/, double sampleRate ) override;
    void releaseResources() override {};
    void getNextAudioBlock( const AudioSourceChannelInfo& bufferToFill ) override;

    void setNextReadPosition( int64 newPosition ) override      {}
    int64 getNextReadPosition() const override                  { return 0; }
    int64 getTotalLength() const override                       { return mTotalLength; }
    bool isLooping() const override                             { return false; }

    MidiMessageCollector* getMidiMessageCollector()             { return &mMidiCollector; }

private:
    bool addNewSample( const SharedSampleBuffer sampleBuffer,
                       const SharedSampleRange sampleRange,
                       const qreal sampleRate );
    void clearSampleRanges();

    SharedSampleBuffer mSampleBuffer;
    int mSampleRate;
    MidiMessageCollector mMidiCollector;
    Synthesiser mSynth;
    int mNextFreeKey;
    int mStartKey;
    int64 mTotalLength;
    QList<int> mNoteOnFrameNumList;
    volatile bool mIsPlaySampleSeqEnabled;
    volatile int mNoteCounter;
    volatile int mFrameCounter;

private:
    static const int DEFAULT_KEY = 60;      // MIDI key C4
    static const int MAX_POLYPHONY = 128;
};

#endif // SAMPLERAUDIOSOURCE_H
