/*
OpenEStimCtrl
Copyright (C) 2023 Personal/SoundReload

This program is free software: you can redistribute it and/or modify
        it under the terms of the GNU Affero General Public License as published by
        the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
        but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
        along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "DGLabESTIM01WaveGen.h"

namespace OpenEStimCtrl::DGLabESTIM01WaveGen {
    static DGLabESTIM01::DGLabESTIM01EStimWave calcXYZByFrequency(double frequency, int z) {
        DGLabESTIM01::DGLabESTIM01EStimWave wave = {0};
        if (frequency < 10) frequency = 10;
        if (frequency > 1000) frequency = 1000;
        if (z > 31) z = 31;
        if (z < 0) z = 0;
        double x = sqrt(frequency / 1000) * 15;
        double y = frequency - x;
        if (x > 31) x = 31.0;
        if (y > 1023) y = 1023.0;
        wave.x = (uint8_t)(x + 0.5 - (x<0));
        wave.y = (uint16_t)(y + 0.5 - (x<0));
        wave.z = z;
        return wave;
    }

    static DGLabESTIM01::DGLabESTIM01EStimWave calcTouchWaveByPosition(double x, double y) {
        DGLabESTIM01::DGLabESTIM01EStimWave wave = {0};
        if (x > 1.0 || x < 0.0 || y > 1.0 || y < 0.0) {
            return wave;
        }
        double p1 = pow(10.0, y * 2.5 + 0.5);
        if (p1 < 10.0) p1 = 10.0;
        double p2 = 20.0 - pow(abs(x * 2.0 - 1.0), 1.65) * 20.0;
        int p3 = (int)(pow(p1/1000, 0.5) * 8.0);
        if (p3 < 1) p3 = 1;
        int i2 = (p1 - p3);
        int i3 = (int)p2;
        wave.x = p3;
        wave.y = i2;
        wave.z = i3;
        return wave;
    }

    static DGLabESTIM01::DGLabESTIM01EStimWave calcTouchWaveByTimeSpan(int timeSpan, DGLabESTIM01EStimTouchWave* touchWave, size_t length) {
        int time = timeSpan % length;
        uint8_t x = touchWave[time].x;
        uint16_t y = touchWave[time].y;
        uint8_t z = touchWave[time].z;
        DGLabESTIM01::DGLabESTIM01EStimWave wave = {x, y, z};
        return wave;
    }

} // OpenEStimCtrl.DGLabESTIM01WaveGen
void DGLabESTIM01WaveGen_calcXYZByFrequency(double frequency, int z, OpenEStimCtrl::DGLabESTIM01::DGLabESTIM01EStimWave* wave){
    OpenEStimCtrl::DGLabESTIM01::DGLabESTIM01EStimWave w = OpenEStimCtrl::DGLabESTIM01WaveGen::calcXYZByFrequency(frequency, z);
    memcpy(wave, &w, sizeof(OpenEStimCtrl::DGLabESTIM01::DGLabESTIM01EStimWave));
}
void DGLabESTIM01WaveGen_calcTouchWaveByPosition(double x, double y, OpenEStimCtrl::DGLabESTIM01::DGLabESTIM01EStimWave* wave) {
    OpenEStimCtrl::DGLabESTIM01::DGLabESTIM01EStimWave w = OpenEStimCtrl::DGLabESTIM01WaveGen::calcTouchWaveByPosition(x, y);
    memcpy(wave, &w, sizeof(OpenEStimCtrl::DGLabESTIM01::DGLabESTIM01EStimWave));
}

void DGLabESTIM01WaveGen_calcTouchWaveByTimeSpan(int timeSpan, OpenEStimCtrl::DGLabESTIM01WaveGen::DGLabESTIM01EStimTouchWave* touchWave, size_t length, OpenEStimCtrl::DGLabESTIM01::DGLabESTIM01EStimWave* wave) {
    OpenEStimCtrl::DGLabESTIM01::DGLabESTIM01EStimWave w = OpenEStimCtrl::DGLabESTIM01WaveGen::calcTouchWaveByTimeSpan(timeSpan, touchWave, length);
    memcpy(wave, &w, sizeof(OpenEStimCtrl::DGLabESTIM01::DGLabESTIM01EStimWave));
}
void DGLabESTIM01WaveGen_calcAutoWave(OpenEStimCtrl::DGLabESTIM01::DGLabESTIM01EStimWave* wave) {

}