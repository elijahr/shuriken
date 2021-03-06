/*
  This file is part of Shuriken Beat Slicer.

  Copyright (C) 2015 Andrew M Taylor <a.m.taylor303@gmail.com>

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

#ifndef WAVEGRAPHICSSCENE_H
#define WAVEGRAPHICSSCENE_H

#include <QGraphicsScene>
#include <QTimeLine>
#include <QGraphicsItemAnimation>
#include "JuceHeader.h"
#include "waveformitem.h"
#include "slicepointitem.h"
#include "samplebuffer.h"
#include "wavegraphicsview.h"

class WaveGraphicsView;

typedef QSharedPointer<QGraphicsItem> SharedGraphicsItem;


class WaveGraphicsScene : public QGraphicsScene
{
    Q_OBJECT

public:
    WaveGraphicsScene( qreal x, qreal y, qreal width, qreal height, QObject* parent = NULL );

    enum InteractionMode { SELECT_MOVE_ITEMS, MULTI_SELECT_ITEMS, AUDITION_ITEMS };

    InteractionMode getInteractionMode() const              { return m_interactionMode; }

    void setInteractionMode( InteractionMode mode );

    // Creates a new waveform item and adds it to the scene
    SharedWaveformItem createWaveform( SharedSampleBuffer sampleBuffer,
                                       SharedSampleHeader sampleHeader );

    // Creates new waveform items and adds them to the scene
    QList<SharedWaveformItem> createWaveforms( QList<SharedSampleBuffer> sampleBufferList,
                                               SharedSampleHeader sampleHeader,
                                               int orderPosToInsertAt = 0 );

    void moveWaveforms( QList<int> oldOrderPositions, int numPlacesMoved );

    // Insert waveform items at the order positions at which they have been set to
    void insertWaveforms( QList<SharedWaveformItem> waveformItems );

    // Remove waveform items from the scene
    QList<SharedWaveformItem> removeWaveforms( QList<int> waveformOrderPositions );

    // Get list of currently selected waveform items sorted by order position
    QList<WaveformItem*> getSelectedWaveforms() const;

    QList<int> getSelectedWaveformsOrderPositions() const;

    void selectNextWaveform();

    void selectPreviousWaveform();

    SharedWaveformItem getWaveformAt( int orderPos ) const  { return m_waveformItemList.at( orderPos ); }

    int getNumWaveforms() const                             { return m_waveformItemList.size(); }

    QList<SharedWaveformItem> getWaveformList() const       { return m_waveformItemList; }

    // Stretch waveform items by a ratio of their original size (not necessarily their current size)
    void stretchWaveforms( QList<int> orderPosList, QList<qreal> ratioList );

    QList<qreal> getWaveformStretchRatios( QList<int> orderPositions ) const;

    // Redraw all waveform items
    void redrawWaveforms();

    // Create a new slice point item and add it to the scene
    SharedSlicePointItem createSlicePoint( int frameNum, bool canBeMovedPastOtherSlicePoints );

    // Add a slice point item to the scene
    void addSlicePoint( SharedSlicePointItem slicePoint );

    // Remove a slice point item from the scene
    void removeSlicePoint( SharedSlicePointItem slicePointItem );

    void moveSlicePoint( SharedSlicePointItem slicePointItem, int newFrameNum );

    // Returns the currently selected slice point item
    SharedSlicePointItem getSelectedSlicePoint();

    // Returns a sorted and edited list containing the frame no. of every valid slice point item
    QList<int> getSlicePointFrameNums() const;

    // Returns a list of all slice point items
    QList<SharedSlicePointItem> getSlicePointList() const   { return m_slicePointItemList; }

    void selectNone();
    void selectAll();

    void startPlayhead( bool isLoopingDesired, qreal stretchRatio = 1.0 );
    void startPlayhead( qreal startPosX,
                        qreal endPosX,
                        int numFrames,
                        bool isLoopingDesired,
                        qreal stretchRatio = 1.0 );
    void stopPlayhead();
    bool isPlayheadScrolling() const                        { return m_timer->state() == QTimeLine::Running; }
    void setPlayheadLooping( bool isLoopingDesired );
    void updatePlayheadSpeed( qreal stretchRatio );

    QList<SharedGraphicsItem> getBpmRulerMarks() const      { return m_rulerMarksList; }
    void setBpmRulerMarks( qreal bpm, int timeSigNumerator, int divisionsPerBeat );

    void clearAll();
    void clearWaveform();

    qreal getScenePosX( int frameNum ) const;
    int getFrameNum( qreal scenePosX ) const;
    qreal getNearestFramePosX( qreal scenePosX ) const;

    bool isSceneAtSampleDetailLevel() const                 { return m_isSceneAtSampleDetailLevel; }

    void resizeWaveformItems( qreal scaleFactorX );
    void resizeSlicePointItems( qreal scaleFactorX );
    void resizePlayhead();
    void resizeRuler( qreal scaleFactorX );

    void scaleItems( qreal scaleFactorX );

private:
    WaveGraphicsView* getView() const;

    void connectWaveform( SharedWaveformItem item );

    void createBpmRuler();

    InteractionMode m_interactionMode;

    QList<SharedWaveformItem> m_waveformItemList;
    QList<SharedSlicePointItem> m_slicePointItemList;

    QList<SharedGraphicsItem> m_rulerMarksList;
    ScopedPointer<QGraphicsRectItem> m_rulerBackground;

    SharedSampleHeader m_sampleHeader;

    ScopedPointer<QGraphicsLineItem> m_playhead;
    ScopedPointer<QTimeLine> m_timer;
    ScopedPointer<QGraphicsItemAnimation> m_animation;

    bool m_isSceneAtSampleDetailLevel;

private:
    static int getTotalNumFrames( QList<SharedWaveformItem> waveformItemList );

signals:
    void slicePointPosChanged( SharedSlicePointItem slicePoint,
                               int orderPos,
                               int numFramesFromPrevSlicePoint,
                               int numFramesToNextSlicePoint,
                               int oldFrameNum );
    void playheadFinishedScrolling();

private slots:
    // 'oldOrderPositions' is assumed to be sorted
    // 'numPlacesMoved' should be negative if the items have moved left, or positive if the items have moved right
    void reorderWaveformItems( QList<int> oldOrderPositions, int numPlacesMoved );

    void slideWaveformItemIntoPlace( int orderPos );
    void updateSlicePointOrdering( SlicePointItem* movedItem, int oldFrameNum );
    void removePlayhead();

    void setSceneDetailLevelToSamples();
    void setSceneDetailLevelToSampleBins();

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( WaveGraphicsScene );
};


#endif // WAVEGRAPHICSSCENE_H
