#include <TLV320DAC3101.h>

void TLV320DAC3101::begin() {
    dtwi->beginMaster();

    setRegister(0, 1, 1); // Soft reset
    delay(10); // 10ms startup delay


    setRegister(0, 0x0b, 0x81); 
    setRegister(0, 0x0c, 0x82); 
    setRegister(0, 0x0d, 0x00);
    setRegister(0, 0x0e, 0x80);
    setRegister(0, 0x1b, 0b00110000);
    setRegister(0, 0x3c, 0x0b);
    setRegister(8, 0x01, 0x04);
    setRegister(0, 0x74, 0x00);

    setRegister(1, 0x1f, 0b000010100);
    setRegister(1, 0x21, 0x4e);
    setRegister(1, 0x23, 0x44);
    setRegister(1, 0x28, 0x06);
    setRegister(1, 0x29, 0x06);
    setRegister(1, 0x2A, 0x1C);
    setRegister(1, 0x2B, 0x1C);
    setRegister(1, 0x1F, 0b11000100);
    setRegister(1, 0x20, 0b11000110);

    setGain(0);
    unmute();
    
    setRegister(0, 0x3F, 0xD4);
    setRegister(0, 0x41, 0xD4);
    setRegister(0, 0x42, 0xD4);
    setRegister(0, 0x40, 0x00);
}

void TLV320DAC3101::setGain(int8_t g) {
    setHeadphoneGain(g);
    setSpeakerGain(g);
}

void TLV320DAC3101::setRegister(uint8_t page, uint8_t reg, uint8_t val) {
    setRaw(0, page);
    setRaw(reg, val);
}

void TLV320DAC3101::setRaw(uint8_t reg, uint8_t val) {
    uint8_t state = 0;
    uint32_t ts = millis();
    while (1) {
        if (millis() - ts >= 1000) {
            dtwi->stopMaster();
            return;
        }
        switch (state) {
            case 0: // begin write
                if (dtwi->startMasterWrite(0x18)) {
                    state = 1;
                }
                break;
            case 1: // Send register address
                if (dtwi->write(&reg, 1) == 1) {
                    state = 2;
                }
                break;
            case 2: // Send register address
                if (dtwi->write(&val, 1) == 1) {
                    state = 3;
                }
                break;
            case 3: // Stop Master
                if (dtwi->stopMaster()) {
                    return;
                }
                break;
        }
    }
}

void TLV320DAC3101::setBass(int8_t cut) {
}

void TLV320DAC3101::setMid(int8_t cut) {
}

void TLV320DAC3101::setTreble(int8_t cut) {
}

void TLV320DAC3101::setVolume(uint8_t v) {
    setVolume(v, v);
}

void TLV320DAC3101::setVolume(uint8_t l, uint8_t r) {
    float lRange = -63.5 + ((l / 255.0) * 87.5);
    float rRange = -63.5 + ((r / 255.0) * 87.5);

    int8_t lInt = (int)lRange;
    int8_t rInt = (int)rRange;


    if ((lRange - lInt) >= 0.5) {
        lInt <<= 1;
        lInt |= 1;
    } else {
        lInt <<= 1;
    }

    if ((rRange - rInt) >= 0.5) {
        rInt <<= 1;
        rInt |= 1;
    } else {
        rInt <<= 1;
    }

    setRegister(0, 0x41, lInt);
    setRegister(0, 0x42, rInt);
}

void TLV320DAC3101::setHeadphoneGain(int8_t v) {
    setHeadphoneGain(v, v);
}

void TLV320DAC3101::setHeadphoneGain(int8_t l, int8_t r) {
    setRegister(1, 0x24, 0x80 | (127 - (l & 0x7f)));
    setRegister(1, 0x25, 0x80 | (127 - (r & 0x7f)));
}

void TLV320DAC3101::setSpeakerGain(int8_t v) {
    setSpeakerGain(v, v);
}

void TLV320DAC3101::setSpeakerGain(int8_t l, int8_t r) {
    setRegister(1, 0x26, 0x80 | (127 - (l & 0x7f)));
    setRegister(1, 0x27, 0x80 | (127 - (r & 0x7f)));
}

void TLV320DAC3101::set3D() {
}

void TLV320DAC3101::enable3D() {
}

void TLV320DAC3101::disable3D() {
}

void TLV320DAC3101::setWide3D() {
}

void TLV320DAC3101::setNarrow3D() {
}

void TLV320DAC3101::set3DDepth(uint8_t w) {
}

void TLV320DAC3101::enableAmplifier() {
}

void TLV320DAC3101::disableAmplifier() {
}

void TLV320DAC3101::muteLeft() {
    _channelStatus |= 0x08;
    setRegister(0, 0x40, _channelStatus);
}

void TLV320DAC3101::muteRight() {
    _channelStatus |= 0x04;
    setRegister(0, 0x40, _channelStatus);
}

void TLV320DAC3101::mute() {
    _channelStatus |= 0x0C;
    setRegister(0, 0x40, _channelStatus);
}

void TLV320DAC3101::unmuteLeft() {
    _channelStatus &= 0xF7;
    setRegister(0, 0x40, _channelStatus);
}

void TLV320DAC3101::unmuteRight() {
    _channelStatus &= 0xFB;
    setRegister(0, 0x40, _channelStatus);
}

void TLV320DAC3101::unmute() {
    _channelStatus &= 0xF3;
    setRegister(0, 0x40, _channelStatus);
}

void TLV320DAC3101::setHeadphoneMode() {
    setRegister(1, 0x2C, 0b00000000);
}

void TLV320DAC3101::setLineOutMode() {
    setRegister(1, 0x2C, 0b00000110);
}

void TLV320DAC3101::setMonoLeft() {
    setRegister(1, 0x20, 0b10000110); // Turn on left and turn off right
    setRegister(0, 0x3F, 0b11110000);
}

void TLV320DAC3101::setMonoRight() {
    setRegister(1, 0x20, 0b01000110); // Turn off left and turn on right
    setRegister(0, 0x3F, 0b11001100);
}

void TLV320DAC3101::setStereo() {
    setRegister(1, 0x20, 0b11000110); // Turn on left and turn off right
    setRegister(0, 0x3F, 0b11010100);
}




