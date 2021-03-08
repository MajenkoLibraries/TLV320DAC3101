#include <TLV320DAC3101.h>

void TLV320DAC3101::begin() {
    dtwi->beginMaster();

    delay(10);

    if (_resetPin != 255) {
        pinMode(_resetPin, OUTPUT);
        digitalWrite(_resetPin, HIGH);
        delay(10);
        digitalWrite(_resetPin, LOW);
        delay(10);
        digitalWrite(_resetPin, HIGH);
        delay(10);
    }

    setRegister(0, 1, 1); // Soft reset
    delay(20); // 10ms startup delay


    setRegister(0, 0x0b, 0x81); 
    setRegister(0, 0x0c, 0x82); 
    setRegister(0, 0x0d, 0x00);
    setRegister(0, 0x0e, 0x80);
    setRegister(0, 0x1b, 0b00110000);


    setRegister(0, 0x3c, 0x0b); // Block 25
    setRegister(8, 0x01, 0x04);
/*
    setLeftBiquadB(
        0.9190440523145926,
        1.8380881046291853,
        0.9190440523145926,
        1.3857551277146498,
        1.6761762092583707,
        0.6142448722853502
    );
    setRightBiquadB(
        0.9190440523145926,
        1.8380881046291853,
        0.9190440523145926,
        1.3857551277146498,
        1.6761762092583707,
        0.6142448722853502
    );
*/

    setRegister(0, 0x74, 0x00);

    setRegister(1, 0x1f, 0b00010100);
    setRegister(1, 0x21, 0x4e);
    setRegister(1, 0x23, 0x44);
    setRegister(1, 0x28, 0x06);
    setRegister(1, 0x29, 0x06);
    setRegister(1, 0x2A, 0x1C);
    setRegister(1, 0x2B, 0x1C);
    setRegister(1, 0x1F, 0b11010100);
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

void TLV320DAC3101::setRegister(uint8_t page, uint8_t reg, uint8_t val1, uint8_t val2) {
    setRaw(0, page);
    setRaw2(reg, val1, val2);
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

void TLV320DAC3101::setRaw2(uint8_t reg, uint8_t val1, uint8_t val2) {
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
            case 2: 
                if (dtwi->write(&val1, 1) == 1) {
                    state = 3;
                }
                break;
            case 3: 
                if (dtwi->write(&val2, 1) == 1) {
                    state = 4;
                }
                break;
            case 4: // Stop Master
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

void TLV320DAC3101::beep(uint8_t vl, uint8_t vr, uint32_t freq, uint32_t duration) {
    
    uint32_t s = round(sin(2*PI*freq/_sampleFrequency)*32768);
    uint32_t c = round(cos(2*PI*freq/_sampleFrequency)*32768);


    setRegister(0, 0x47, 0x3f - (vl & 0x3F));
    setRegister(0, 0x48, 0x3f - (vr & 0x3F));
    setRegister(0, 0x49, duration >> 16);
    setRegister(0, 0x4A, duration >> 8);
    setRegister(0, 0x4B, duration);
    setRegister(0, 0x4C, s >> 8);
    setRegister(0, 0x4D, s);
    setRegister(0, 0x4E, c >> 8);
    setRegister(0, 0x4F, c);

    setRegister(0, 0x47, (0x3f - (vl & 0x3F)) | 0x80);
}


void TLV320DAC3101::setBiquad(uint8_t base, float b0, float b1, float b2, float a0, float a1, float a2) {

    float b0n = b0 / a0;
    float b1n = b1 / a0;
    float b2n = b2 / a0;
    float a1n = a1 / a0;
    float a2n = a2 / a0;


    float n0_real = b0/a0;
    float n1_real = (b1/a0)/2;
    float n2_real = b2/a0;

    float fact = fabs(max(n0_real, max(n1_real, n2_real)));

    if (fact > 1) {
        int16_t n0 = ((b0 / a0) / fact) * 32767;
        int16_t n1 = ((b1 / a0) / (2 * fact)) * 32767;
        int16_t n2 = ((b2 / a0) / fact) * 32767;
        int16_t d1 = (a1/ (-2 * a0)) * 32767;
        int16_t d2 = (-(a2 / a0)) * 32767;

        setRegister(8, base, n0 >> 8, n0);
        setRegister(8, base + 2, n1 >> 8, n1);
        setRegister(8, base + 4, n2 >> 8, n2);
        setRegister(8, base + 6, d1 >> 8, d1);
        setRegister(8, base + 8, d2 >> 8, d2);

        setRegister(12, base, n0 >> 8, n0);
        setRegister(12, base + 2, n1 >> 8, n1);
        setRegister(12, base + 4, n2 >> 8, n2);
        setRegister(12, base + 6, d1 >> 8, d1);
        setRegister(12, base + 8, d2 >> 8, d2);
    } else {
        int16_t n0 = (b0 / a0) * 32767;
        int16_t n1 = ((b1 / a0) / 2) * 32767;
        int16_t n2 = (b2 / a0)  * 32767;
        int16_t d1 = (a1/ (-2 * a0)) * 32767;
        int16_t d2 = (-(a2 / a0)) * 32767;

        setRegister(8, base, n0 >> 8, n0);
        setRegister(8, base + 2, n1 >> 8, n1);
        setRegister(8, base + 4, n2 >> 8, n2);
        setRegister(8, base + 6, d1 >> 8, d1);
        setRegister(8, base + 8, d2 >> 8, d2);

        setRegister(12, base, n0 >> 8, n0);
        setRegister(12, base + 2, n1 >> 8, n1);
        setRegister(12, base + 4, n2 >> 8, n2);
        setRegister(12, base + 6, d1 >> 8, d1);
        setRegister(12, base + 8, d2 >> 8, d2);
    }
}


void TLV320DAC3101::lowPassFilter(uint8_t block, uint8_t channels, uint32_t corner, float q) {
    float w0 = 2 * 3.141592653 * ((float)corner / (float)_sampleFrequency);
    float cosw0 = cos(w0);
    float sinw0 = sin(w0);
    float a = sinw0 / (2 * q);


    float b0 = (1 - cosw0) / 2;
    float b1 = 1 - cosw0;
    float b2 = (1 - cosw0) / 2;

    float a0 = 1 + a;
    float a1 = -2 * cosw0;
    float a2 = 1 - a;

    if (channels & LEFT) {
        setBiquad(block, b0, b1, b2, a0, a1, a2);
    }

    if (channels & RIGHT) {
        setBiquad(block + 0x40, b0, b1, b2, a0, a1, a2);
    }
}
    
void TLV320DAC3101::highPassFilter(uint8_t block, uint8_t channels, uint32_t corner, float q) {
    float w0 = 2 * 3.141592653 * ((float)corner / (float)_sampleFrequency);
    float cosw0 = cos(w0);
    float sinw0 = sin(w0);
    float a = sinw0 / (2 * q);

    float b0 = (1 + cosw0) / 2;
    float b1 = - (1 + cosw0);
    float b2 = (1 + cosw0) / 2;

    float a0 = 1 + a;
    float a1 = -2 * cosw0;
    float a2 = 1 - a;

    if (channels & LEFT) {
        setBiquad(block, b0, b1, b2, a0, a1, a2);
    }

    if (channels & RIGHT) {
        setBiquad(block + 0x40, b0, b1, b2, a0, a1, a2);
    }
}
    
void TLV320DAC3101::bandPassSkirtFilter(uint8_t block, uint8_t channels, uint32_t center, float q) {
    float w0 = 2 * 3.141592653 * ((float)center / (float)_sampleFrequency);
    float cosw0 = cos(w0);
    float sinw0 = sin(w0);
    float a = sinw0 / (2 * q);

    float b0 = q * a;
    float b1 = 0;
    float b2 = (-q) * a;

    float a0 = 1 + a;
    float a1 = -2 * cosw0;
    float a2 = 1 - a;

    if (channels & LEFT) {
        setBiquad(block, b0, b1, b2, a0, a1, a2);
    }

    if (channels & RIGHT) {
        setBiquad(block + 0x40, b0, b1, b2, a0, a1, a2);
    }
}
    
void TLV320DAC3101::bandPassZeroFilter(uint8_t block, uint8_t channels, uint32_t center, float q) {
    float w0 = 2 * 3.141592653 * ((float)center / (float)_sampleFrequency);
    float cosw0 = cos(w0);
    float sinw0 = sin(w0);
    float a = sinw0 / (2 * q);

    float b0 = a;
    float b1 = 0;
    float b2 = -a;

    float a0 = 1 + a;
    float a1 = -2 * cosw0;
    float a2 = 1 - a;

    if (channels & LEFT) {
        setBiquad(block, b0, b1, b2, a0, a1, a2);
    }

    if (channels & RIGHT) {
        setBiquad(block + 0x40, b0, b1, b2, a0, a1, a2);
    }
}

void TLV320DAC3101::notchFilter(uint8_t block, uint8_t channels, uint32_t center, float q) {
    float w0 = 2 * 3.141592653 * ((float)center / (float)_sampleFrequency);
    float cosw0 = cos(w0);
    float sinw0 = sin(w0);
    float a = sinw0 / (2 * q);

    float b0 = 1;
    float b1 = -2 * cosw0;
    float b2 = 1;

    float a0 = 1 + a;
    float a1 = -2 * cosw0;
    float a2 = 1 - a;

    if (channels & LEFT) {
        setBiquad(block, b0, b1, b2, a0, a1, a2);
    }

    if (channels & RIGHT) {
        setBiquad(block + 0x40, b0, b1, b2, a0, a1, a2);
    }
}

void TLV320DAC3101::allPassFilter(uint8_t block, uint8_t channels, uint32_t center, float q) {
    float w0 = 2 * 3.141592653 * ((float)center / (float)_sampleFrequency);
    float cosw0 = cos(w0);
    float sinw0 = sin(w0);
    float a = sinw0 / (2 * q);

    float b0 = 1 - a;
    float b1 = -2 * cosw0;
    float b2 = 1 + a;

    float a0 = 1 + a;
    float a1 = -2 * cosw0;
    float a2 = 1 - a;

    if (channels & LEFT) {
        setBiquad(block, b0, b1, b2, a0, a1, a2);
    }

    if (channels & RIGHT) {
        setBiquad(block + 0x40, b0, b1, b2, a0, a1, a2);
    }
}

void TLV320DAC3101::peakingEQFilter(uint8_t block, uint8_t channels, uint32_t center, float q, float dbgain) {
    float w0 = 2 * 3.141592653 * ((float)center / (float)_sampleFrequency);
    float cosw0 = cos(w0);
    float sinw0 = sin(w0);
    float a = sinw0 / (2 * q);
    float A = pow(10, dbgain/40);

    float b0 = 1 + a * A;
    float b1 = -2 * cosw0;
    float b2 = 1 - a * A;

    float a0 = 1 + a/A;
    float a1 = -2 * cosw0;
    float a2 = 1 - a/A;

    if (channels & LEFT) {
        setBiquad(block, b0, b1, b2, a0, a1, a2);
    }

    if (channels & RIGHT) {
        setBiquad(block + 0x40, b0, b1, b2, a0, a1, a2);
    }
}

void TLV320DAC3101::lowShelfFilter(uint8_t block, uint8_t channels, uint32_t corner, float q, float dbgain) {
    float w0 = 2 * 3.141592653 * ((float)corner / (float)_sampleFrequency);
    float cosw0 = cos(w0);
    float sinw0 = sin(w0);
    float a = sinw0 / (2 * q);
    float A = pow(10, dbgain/40);

    float b0 = A * ((A + 1) - (A - 1) * cosw0 + 2 * sqrt(A) * a);
    float b1 = 2 * A * ((A - 1) - (A + 1) * cosw0);
    float b2 = A * ((A + 1) - (A - 1) * cosw0 - 2 * sqrt(A) * a);

    float a0 = (A + 1) + (A - 1) * cosw0 + 2 * sqrt(A) * a;
    float a1 = -2 * ((A - 1) + (A + 1) * cosw0);
    float a2 = (A + 1) + (A - 1) * cosw0 - 2 * sqrt(A) * a;

    if (channels & LEFT) {
        setBiquad(block, b0, b1, b2, a0, a1, a2);
    }

    if (channels & RIGHT) {
        setBiquad(block + 0x40, b0, b1, b2, a0, a1, a2);
    }
}

void TLV320DAC3101::highShelfFilter(uint8_t block, uint8_t channels, uint32_t corner, float q, float dbgain) {
    float w0 = 2 * 3.141592653 * ((float)corner / (float)_sampleFrequency);
    float cosw0 = cos(w0);
    float sinw0 = sin(w0);
    float a = sinw0 / (2 * q);
    float A = pow(10, dbgain/40);

    float b0 = A * ((A + 1) + (A - 1) * cosw0 + 2 * sqrt(A) * a);
    float b1 = -2 * A * ((A - 1) + (A + 1) * cosw0);
    float b2 = A * ((A + 1) + (A - 1) * cosw0 - 2 * sqrt(A) * a);

    float a0 = (A + 1) - (A - 1) * cosw0 + 2 * sqrt(A) * a;
    float a1 = 2 * ((A - 1) - (A + 1) * cosw0);
    float a2 = (A + 1) - (A - 1) * cosw0 - 2 * sqrt(A) * a;

    if (channels & LEFT) {
        setBiquad(block, b0, b1, b2, a0, a1, a2);
    }

    if (channels & RIGHT) {
        setBiquad(block + 0x40, b0, b1, b2, a0, a1, a2);
    }
}

