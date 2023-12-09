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


#include "DGLabESTIM01.h"

OpenEStimCtrl::DGLabESTIM01::DGLabESTIM01::DGLabESTIM01(SendDataCallback sendDataCallback): _sendDataCallback(sendDataCallback) {

}

[[maybe_unused]] uint8_t OpenEStimCtrl::DGLabESTIM01::DGLabESTIM01::getBattery() const {
    return _battery;
}

[[maybe_unused]] uint16_t OpenEStimCtrl::DGLabESTIM01::DGLabESTIM01::getStrengthA() const {
    return _strengthA;
}

[[maybe_unused]] uint16_t OpenEStimCtrl::DGLabESTIM01::DGLabESTIM01::getStrengthB() const {
    return _strengthB;
}

void OpenEStimCtrl::DGLabESTIM01::DGLabESTIM01::setOnBatteryChange(
        OpenEStimCtrl::DGLabESTIM01::onBatteryChange onBatteryChange) {
    _onBatteryChange = onBatteryChange;
}

void OpenEStimCtrl::DGLabESTIM01::DGLabESTIM01::setOnStrengthChange(
        OpenEStimCtrl::DGLabESTIM01::onChannelStrengthChange onChannelStrengthChange) {
    _onChannelStrengthChange = onChannelStrengthChange;
}

void OpenEStimCtrl::DGLabESTIM01::DGLabESTIM01::sendWave(DGLabESTIM01Channel channel,uint8_t x, uint16_t y, uint8_t z) {
    DGLabESTIM01EStimWaveData waveData = {0};
    waveData.payload.z = z;
#if BYTE_ORDER == __ORDER_BIG_ENDIAN__
    waveData.payload.y = y;
#else
    waveData.payload.y = ntohs(y);
#endif
    waveData.payload.x = x;
    switch (channel) {
        case CHANNEL_A:
            _sendDataCallback(DGLabESTIM01ServiceEStimUUID, DGLabESTIM01CharacteristicPWMAUUID, waveData.data, sizeof(DGLabESTIM01EStimStrengthData), _userData);
            break;
        case CHANNEL_B:
            _sendDataCallback(DGLabESTIM01ServiceEStimUUID, DGLabESTIM01CharacteristicPWMBUUID, waveData.data, sizeof(DGLabESTIM01EStimStrengthData), _userData);
            break;
    }
}

void OpenEStimCtrl::DGLabESTIM01::DGLabESTIM01::setStrength(uint16_t strengthA, uint16_t strengthB) {
    DGLabESTIM01EStimStrengthData strengthData = {0};
#if BYTE_ORDER == __ORDER_BIG_ENDIAN__
    strengthData.payload.strengthA = strengthA;
    strengthData.payload.strengthB = strengthB;
#else
    strengthData.payload.strengthA = ntohs(strengthA);
    strengthData.payload.strengthB = ntohs(strengthB);
#endif
    _sendDataCallback(DGLabESTIM01ServiceEStimUUID, DGLabESTIM01CharacteristicStrengthUUID, strengthData.data, sizeof(DGLabESTIM01EStimStrengthData), _userData);
}

void
OpenEStimCtrl::DGLabESTIM01::DGLabESTIM01::parseBLEData(const char *service, const char *characteristic, const uint8_t *data,
                                                        int length) {
    if (strcmp(service, DGLabESTIM01ServiceBattUUID) > 0) {
        if (strcmp(characteristic, DGLabESTIM01CharacteristicBattUUID) > 0) {
            DGLabESTIM01BattData battData = {0};
            memcpy(&battData, data, length);
            _battery = battData.level;
            if (_onBatteryChange != nullptr) {
                _onBatteryChange(_battery, _userData);
            }
        }
    } else if (strcmp(service, DGLabESTIM01ServiceEStimUUID) > 0) {
        if (strcmp(characteristic, DGLabESTIM01CharacteristicStrengthUUID) > 0) {
            DGLabESTIM01EStimStrengthData strengthData = {0};
            memcpy(strengthData.data, data, length);
#if BYTE_ORDER == __ORDER_BIG_ENDIAN__
            _strengthA = strengthData.payload.strengthA;
            _strengthB = strengthData.payload.strengthB;
#else
            _strengthA = ntohs(strengthData.payload.strengthA);
            _strengthB = ntohs(strengthData.payload.strengthB);
#endif
            if (_onChannelStrengthChange != nullptr) {
                _onChannelStrengthChange(_strengthA, _strengthB, _userData);
            }
        }
    }
}

void OpenEStimCtrl::DGLabESTIM01::DGLabESTIM01::sendWave(OpenEStimCtrl::DGLabESTIM01::DGLabESTIM01Channel channel,
                                                         OpenEStimCtrl::DGLabESTIM01::DGLabESTIM01EStimWave wave) {
    sendWave(channel, wave.x, wave.y, wave.z);
}

void OpenEStimCtrl::DGLabESTIM01::DGLabESTIM01::setUserData(void *userData) {
    _userData = userData;
}

void* DGLabESTIM01_new(SendDataCallback sendDataCallback) {
    return new OpenEStimCtrl::DGLabESTIM01::DGLabESTIM01(sendDataCallback);
}
void DGLabESTIM01_delete(void* dglab) {
    auto ptr = static_cast<OpenEStimCtrl::DGLabESTIM01::DGLabESTIM01*>(dglab);
    delete ptr;
}
void DGLabESTIM01_parseBLEData(void* dglab, const char* service, const char* characteristic, const uint8_t *data, int length) {
    auto ptr = static_cast<OpenEStimCtrl::DGLabESTIM01::DGLabESTIM01*>(dglab);
    ptr->parseBLEData(service, characteristic, data, length);
}
void DGLabESTIM01_setStrength(void* dglab, uint16_t strengthA, uint16_t strengthB) {
    auto ptr = static_cast<OpenEStimCtrl::DGLabESTIM01::DGLabESTIM01*>(dglab);
    ptr->setStrength(strengthA, strengthB);
}
void DGLabESTIM01_sendWave(void* dglab, OpenEStimCtrl::DGLabESTIM01::DGLabESTIM01Channel channel, uint8_t x, uint16_t y, uint8_t z) {
    auto ptr = static_cast<OpenEStimCtrl::DGLabESTIM01::DGLabESTIM01*>(dglab);
    ptr->sendWave(channel, x, y, z);
}

void DGLabESTIM01_sendWaveStruct(void* dglab, OpenEStimCtrl::DGLabESTIM01::DGLabESTIM01Channel channel, OpenEStimCtrl::DGLabESTIM01::DGLabESTIM01EStimWave wave) {
    auto ptr = static_cast<OpenEStimCtrl::DGLabESTIM01::DGLabESTIM01*>(dglab);
    ptr->sendWave(channel, wave);
}

void DGLabESTIM01_setOnBatteryChange(void* dglab, OpenEStimCtrl::DGLabESTIM01::onBatteryChange onBatteryChange) {
    auto ptr = static_cast<OpenEStimCtrl::DGLabESTIM01::DGLabESTIM01*>(dglab);
    ptr->setOnBatteryChange(onBatteryChange);
}
void DGLabESTIM01_setOnStrengthChange(void* dglab, OpenEStimCtrl::DGLabESTIM01::onChannelStrengthChange onChannelStrengthChange) {
    auto ptr = static_cast<OpenEStimCtrl::DGLabESTIM01::DGLabESTIM01*>(dglab);
    ptr->setOnStrengthChange(onChannelStrengthChange);
}
uint8_t DGLabESTIM01_getBattery(void* dglab) {
    auto ptr = static_cast<OpenEStimCtrl::DGLabESTIM01::DGLabESTIM01*>(dglab);
    return ptr->getBattery();
}
uint16_t DGLabESTIM01_getStrengthA(void* dglab) {
    auto ptr = static_cast<OpenEStimCtrl::DGLabESTIM01::DGLabESTIM01*>(dglab);
    return ptr->getStrengthA();
}
uint16_t DGLabESTIM01_getStrengthB(void* dglab) {
    auto ptr = static_cast<OpenEStimCtrl::DGLabESTIM01::DGLabESTIM01*>(dglab);
    return ptr->getStrengthB();
}
void DGLabESTIM01_setUserData(void* dglab, void* userData) {
    auto ptr = static_cast<OpenEStimCtrl::DGLabESTIM01::DGLabESTIM01*>(dglab);
    ptr->setUserData(userData);
}