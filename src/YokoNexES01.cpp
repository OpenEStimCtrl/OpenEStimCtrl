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


#include "YokoNexES01.h"
OpenEStimCtrl::YokoNexES01::YokoNexES01::YokoNexES01(SendDataCallback sendCallback): _sendCallback(sendCallback) {

}

[[maybe_unused]] void OpenEStimCtrl::YokoNexES01::YokoNexES01::parseBLEData(uint8_t *data, int length) {
    YokoNexES01DataReceive dataReceive = {0};
    memcpy(dataReceive.data, data, length);
    switch (dataReceive.packet.type) {
        case YokoNexES01DataReceiveType::TYPE_CHANNEL_A:
        {
            channelA.enabled = dataReceive.packet.payload.channel.enabled != 0x00;
            channelA.strength = dataReceive.packet.payload.channel.strength;
            channelA.mode = dataReceive.packet.payload.channel.mode;
            channelA.connection = dataReceive.packet.payload.channel.connection;
            if (_onChannelAStatusChange != nullptr) {
                _onChannelAStatusChange(channelA);
            }
            break;
        }
        case YokoNexES01DataReceiveType::TYPE_CHANNEL_B:
        {
            channelB.enabled = dataReceive.packet.payload.channel.enabled != 0x00;
            channelB.strength = dataReceive.packet.payload.channel.strength;
            channelB.mode = dataReceive.packet.payload.channel.mode;
            channelB.connection = dataReceive.packet.payload.channel.connection;
            if (_onChannelBStatusChange != nullptr) {
                _onChannelBStatusChange(channelB);
            }
            break;
        }
        case YokoNexES01DataReceiveType::TYPE_MOTOR:
        {
            motor = dataReceive.packet.payload.motor.mode;
            if (_onMotorStatusChange != nullptr) {
                _onMotorStatusChange(motor);
            }
            break;
        }
        case YokoNexES01DataReceiveType::TYPE_BATTERY:
        {
            battery = dataReceive.packet.payload.battery.level;
            if (_onBatteryStatusChange != nullptr) {
                _onBatteryStatusChange(battery);
            }
            break;
        }
        case YokoNexES01DataReceiveType::TYPE_STEP:
        {
            step = (uint16_t)dataReceive.packet.payload.step.step;
            if (_onStepStatusChange != nullptr) {
                _onStepStatusChange(step);
            }
            break;
        }
        case YokoNexES01DataReceiveType::TYPE_ANGLE:
        {
            accel.accX = dataReceive.packet.payload.angle.accX;
            accel.accY = dataReceive.packet.payload.angle.accY;
            accel.accZ = dataReceive.packet.payload.angle.accZ;
            accel.gyroX = dataReceive.packet.payload.angle.gyroX;
            accel.gyroY = dataReceive.packet.payload.angle.gyroY;
            accel.gyroZ = dataReceive.packet.payload.angle.gyroZ;
            if (_onAngleStatusChange != nullptr) {
                _onAngleStatusChange(accel);
            }
            break;
        }
        case YokoNexES01DataReceiveType::TYPE_EXCEPTION:
        {
            lastException = dataReceive.packet.payload.exception.code;
            if (_onException != nullptr) {
                _onException(lastException);
            }
            break;
        }
    }
}

void OpenEStimCtrl::YokoNexES01::YokoNexES01::setEStim(OpenEStimCtrl::YokoNexES01::YokoNexES01Channel channel, bool enabled, uint16_t strength,
                                          OpenEStimCtrl::YokoNexES01::YokoNexES01Mode mode, uint8_t frequency, uint8_t pulseWidth) {
    if (strength > 0x114) {
        strength = 0x114;
    }
    if (strength < 0x01) {
        strength = 0x01;
    }
    YokoNexES01DataChannel dataChannel;
    dataChannel.channel = channel;
    dataChannel.enabled = enabled ? 0x01 : 0x00;
    dataChannel.strength = be_uint16_t(strength);
    dataChannel.mode = mode;
    if (mode == OpenEStimCtrl::YokoNexES01::YokoNexES01Mode::MODE_CUSTOM) {
        if (frequency > 0xC8) {
            frequency = 0xC8;
        }
        if (frequency < 0x01) {
            frequency = 0x01;
        }
        if (pulseWidth > 0x64) {
            pulseWidth = 0x64;
        }
        dataChannel.frequency = frequency;
        dataChannel.pulseWidth = pulseWidth;
    } else {
        dataChannel.frequency = 0x00;
        dataChannel.pulseWidth = 0x00;
    }
    dataChannel.checksum = dataChannel.header + dataChannel.command + dataChannel.channel + dataChannel.enabled + (uint16_t)dataChannel.strength + dataChannel.mode + dataChannel.frequency + dataChannel.pulseWidth;
    _sendCallback(YokonexES01ServiceUUID, YokonexES01CharacteristicSendUUID, (uint8_t*)&dataChannel, sizeof(dataChannel));
}

void OpenEStimCtrl::YokoNexES01::YokoNexES01::triggerMotor(OpenEStimCtrl::YokoNexES01::YokoNexES01Motor mode) {
    YokoNexES01DataMotor dataMotor;
    dataMotor.mode = mode;
    dataMotor.checksum = dataMotor.header + dataMotor.command + dataMotor.mode;
    _sendCallback(YokonexES01ServiceUUID, YokonexES01CharacteristicSendUUID, (uint8_t*)&dataMotor, sizeof(dataMotor));
}

void OpenEStimCtrl::YokoNexES01::YokoNexES01::setStep(OpenEStimCtrl::YokoNexES01::YokoNexES01Step mode) {
    YokoNexES01DataStep dataStep;
    dataStep.mode = mode;
    dataStep.checksum = dataStep.header + dataStep.command + dataStep.mode;
    _sendCallback(YokonexES01ServiceUUID, YokonexES01CharacteristicSendUUID, (uint8_t*)&dataStep, sizeof(dataStep));
}

void OpenEStimCtrl::YokoNexES01::YokoNexES01::setAngle(OpenEStimCtrl::YokoNexES01::YokoNexES01Angle mode) {
    YokoNexES01DataAngle dataAngle;
    dataAngle.mode = mode;
    dataAngle.checksum = dataAngle.header + dataAngle.command + dataAngle.mode;
    _sendCallback(YokonexES01ServiceUUID, YokonexES01CharacteristicSendUUID, (uint8_t*)&dataAngle, sizeof(dataAngle));
}

void OpenEStimCtrl::YokoNexES01::YokoNexES01::query(OpenEStimCtrl::YokoNexES01::YokoNexES01Query query) {
    YokoNexES01DataQuery dataQuery;
    dataQuery.query = query;
    dataQuery.checksum = dataQuery.header + dataQuery.command + dataQuery.query;
    _sendCallback(YokonexES01ServiceUUID, YokonexES01CharacteristicSendUUID, (uint8_t*)&dataQuery, sizeof(dataQuery));
}

uint8_t OpenEStimCtrl::YokoNexES01::YokoNexES01::getLastException() const {
    return lastException;
}

uint8_t OpenEStimCtrl::YokoNexES01::YokoNexES01::getBattery() const {
    return battery;
}

uint16_t OpenEStimCtrl::YokoNexES01::YokoNexES01::getStep() const {
    return step;
}

OpenEStimCtrl::YokoNexES01::YokoNexES01Accel OpenEStimCtrl::YokoNexES01::YokoNexES01::getAccel() const {
    return accel;
}

OpenEStimCtrl::YokoNexES01::YokoNexES01Status OpenEStimCtrl::YokoNexES01::YokoNexES01::getChannelA() const {
    return channelA;
}

OpenEStimCtrl::YokoNexES01::YokoNexES01Status OpenEStimCtrl::YokoNexES01::YokoNexES01::getChannelB() const {
    return channelB;
}

OpenEStimCtrl::YokoNexES01::YokoNexES01Motor OpenEStimCtrl::YokoNexES01::YokoNexES01::getMotor() const {
    return motor;
}

void OpenEStimCtrl::YokoNexES01::YokoNexES01::setOnChannelAStatusChange(OpenEStimCtrl::YokoNexES01::onChannelStatusChange callback) {
    _onChannelAStatusChange = callback;
}

void OpenEStimCtrl::YokoNexES01::YokoNexES01::setOnChannelBStatusChange(OpenEStimCtrl::YokoNexES01::onChannelStatusChange callback) {
    _onChannelBStatusChange = callback;
}

void OpenEStimCtrl::YokoNexES01::YokoNexES01::setOnMotorStatusChange(OpenEStimCtrl::YokoNexES01::onMotorStatusChange callback) {
    _onMotorStatusChange = callback;
}

void OpenEStimCtrl::YokoNexES01::YokoNexES01::setOnBatteryStatusChange(OpenEStimCtrl::YokoNexES01::onBatteryStatusChange callback) {
    _onBatteryStatusChange = callback;
}

void OpenEStimCtrl::YokoNexES01::YokoNexES01::setOnStepStatusChange(OpenEStimCtrl::YokoNexES01::onStepStatusChange callback) {
    _onStepStatusChange = callback;
}

void OpenEStimCtrl::YokoNexES01::YokoNexES01::setOnAngleStatusChange(OpenEStimCtrl::YokoNexES01::onAngleStatusChange callback) {
    _onAngleStatusChange = callback;
}

void OpenEStimCtrl::YokoNexES01::YokoNexES01::setOnException(OpenEStimCtrl::YokoNexES01::onException callback) {
    _onException = callback;
}

void* YokoNexES01_new(SendDataCallback sendDataCallback) {
    return new OpenEStimCtrl::YokoNexES01::YokoNexES01(sendDataCallback);
}
void YokoNexES01_delete(void* yokoNexES01) {
    auto* ptr = static_cast<OpenEStimCtrl::YokoNexES01::YokoNexES01 *>(yokoNexES01);
    delete ptr;
}

void YokoNexES01_parseBLEData(void* yokoNexES01, uint8_t* data, int length) {
    auto* ptr = static_cast<OpenEStimCtrl::YokoNexES01::YokoNexES01 *>(yokoNexES01);
    ptr->parseBLEData(data, length);
}
void YokoNexES01_setEStim(void* yokoNexES01, OpenEStimCtrl::YokoNexES01::YokoNexES01Channel channel, bool enabled, uint16_t strength, OpenEStimCtrl::YokoNexES01::YokoNexES01Mode mode, uint8_t frequency, uint8_t pulseWidth) {
    auto* ptr = static_cast<OpenEStimCtrl::YokoNexES01::YokoNexES01 *>(yokoNexES01);
    ptr->setEStim(channel, enabled, strength, mode, frequency, pulseWidth);
}
void YokoNexES01_triggerMotor(void* yokoNexES01, OpenEStimCtrl::YokoNexES01::YokoNexES01Motor mode) {
    auto* ptr = static_cast<OpenEStimCtrl::YokoNexES01::YokoNexES01 *>(yokoNexES01);
    ptr->triggerMotor(mode);
}
void YokoNexES01_setStep(void* yokoNexES01, OpenEStimCtrl::YokoNexES01::YokoNexES01Step mode) {
    auto* ptr = static_cast<OpenEStimCtrl::YokoNexES01::YokoNexES01 *>(yokoNexES01);
    ptr->setStep(mode);
}
void YokoNexES01_setAngle(void* yokoNexES01, OpenEStimCtrl::YokoNexES01::YokoNexES01Angle mode) {
    auto* ptr = static_cast<OpenEStimCtrl::YokoNexES01::YokoNexES01 *>(yokoNexES01);
    ptr->setAngle(mode);
}
void YokoNexES01_query(void* yokoNexES01, OpenEStimCtrl::YokoNexES01::YokoNexES01Query query) {
    auto* ptr = static_cast<OpenEStimCtrl::YokoNexES01::YokoNexES01 *>(yokoNexES01);
    ptr->query(query);
}

void YokoNexES01_setOnChannelAStatusChange(void* yokoNexES01, OpenEStimCtrl::YokoNexES01::onChannelStatusChange callback) {
    auto* ptr = static_cast<OpenEStimCtrl::YokoNexES01::YokoNexES01 *>(yokoNexES01);
    ptr->setOnChannelAStatusChange(callback);
}
void YokoNexES01_setOnChannelBStatusChange(void* yokoNexES01, OpenEStimCtrl::YokoNexES01::onChannelStatusChange callback) {
    auto* ptr = static_cast<OpenEStimCtrl::YokoNexES01::YokoNexES01 *>(yokoNexES01);
    ptr->setOnChannelBStatusChange(callback);
}
void YokoNexES01_setOnMotorStatusChange(void* yokoNexES01, OpenEStimCtrl::YokoNexES01::onMotorStatusChange callback) {
    auto* ptr = static_cast<OpenEStimCtrl::YokoNexES01::YokoNexES01 *>(yokoNexES01);
    ptr->setOnMotorStatusChange(callback);
}
void YokoNexES01_setOnBatteryStatusChange(void* yokoNexES01, OpenEStimCtrl::YokoNexES01::onBatteryStatusChange callback) {
    auto* ptr = static_cast<OpenEStimCtrl::YokoNexES01::YokoNexES01 *>(yokoNexES01);
    ptr->setOnBatteryStatusChange(callback);
}
void YokoNexES01_setOnStepStatusChange(void* yokoNexES01, OpenEStimCtrl::YokoNexES01::onStepStatusChange callback) {
    auto* ptr = static_cast<OpenEStimCtrl::YokoNexES01::YokoNexES01 *>(yokoNexES01);
    ptr->setOnStepStatusChange(callback);
}
void YokoNexES01_setOnAngleStatusChange(void* yokoNexES01, OpenEStimCtrl::YokoNexES01::onAngleStatusChange callback) {
    auto* ptr = static_cast<OpenEStimCtrl::YokoNexES01::YokoNexES01 *>(yokoNexES01);
    ptr->setOnAngleStatusChange(callback);
}
void YokoNexES01_setOnException(void* yokoNexES01, OpenEStimCtrl::YokoNexES01::onException callback) {
    auto* ptr = static_cast<OpenEStimCtrl::YokoNexES01::YokoNexES01 *>(yokoNexES01);
    ptr->setOnException(callback);
}

uint8_t YokoNexES01_getLastException(OpenEStimCtrl::YokoNexES01::YokoNexES01* yokoNexES01) {
    auto* ptr = static_cast<OpenEStimCtrl::YokoNexES01::YokoNexES01 *>(yokoNexES01);
    return yokoNexES01->getLastException();
}
uint8_t YokoNexES01_getBattery(OpenEStimCtrl::YokoNexES01::YokoNexES01* yokoNexES01) {
    auto* ptr = static_cast<OpenEStimCtrl::YokoNexES01::YokoNexES01 *>(yokoNexES01);
    return yokoNexES01->getBattery();
}
uint16_t YokoNexES01_getStep(OpenEStimCtrl::YokoNexES01::YokoNexES01* yokoNexES01) {
    auto* ptr = static_cast<OpenEStimCtrl::YokoNexES01::YokoNexES01 *>(yokoNexES01);
    return yokoNexES01->getStep();
}
void YokoNexES01_getAccel(OpenEStimCtrl::YokoNexES01::YokoNexES01* yokoNexES01, OpenEStimCtrl::YokoNexES01::YokoNexES01Accel* accel) {
    auto* ptr = static_cast<OpenEStimCtrl::YokoNexES01::YokoNexES01 *>(yokoNexES01);
    OpenEStimCtrl::YokoNexES01::YokoNexES01Accel cAccel = yokoNexES01->getAccel();
    memcpy(accel, &cAccel, sizeof(OpenEStimCtrl::YokoNexES01::YokoNexES01Accel));
}
void YokoNexES01_getChannelA(OpenEStimCtrl::YokoNexES01::YokoNexES01* yokoNexES01, OpenEStimCtrl::YokoNexES01::YokoNexES01Status* status) {
    auto* ptr = static_cast<OpenEStimCtrl::YokoNexES01::YokoNexES01 *>(yokoNexES01);
    OpenEStimCtrl::YokoNexES01::YokoNexES01Status cStatus = yokoNexES01->getChannelA();
    memcpy(status, &cStatus, sizeof(OpenEStimCtrl::YokoNexES01::YokoNexES01ChannelStatus));
}
void YokoNexES01_getChannelB(OpenEStimCtrl::YokoNexES01::YokoNexES01* yokoNexES01, OpenEStimCtrl::YokoNexES01::YokoNexES01Status* status) {
    auto* ptr = static_cast<OpenEStimCtrl::YokoNexES01::YokoNexES01 *>(yokoNexES01);
    OpenEStimCtrl::YokoNexES01::YokoNexES01Status cStatus = yokoNexES01->getChannelB();
    memcpy(status, &cStatus, sizeof(OpenEStimCtrl::YokoNexES01::YokoNexES01ChannelStatus));
}
OpenEStimCtrl::YokoNexES01::YokoNexES01Motor YokoNexES01_getMotor(OpenEStimCtrl::YokoNexES01::YokoNexES01* yokoNexES01) {
    auto* ptr = static_cast<OpenEStimCtrl::YokoNexES01::YokoNexES01 *>(yokoNexES01);
    return yokoNexES01->getMotor();
}
