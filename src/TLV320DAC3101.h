#ifndef _TLV320DAC3101_H
#define _TLV320DAC3101_H

#include <Arduino.h>
#include <DTWI.h>
#include <I2S.h>

typedef struct {
    union {
        uint8_t val;
        struct {
            unsigned rsv1:1;
            unsigned enable:1;
            unsigned rsv2:1;
            unsigned filter:1;
            unsigned depth:4;
        } __attribute__((packed));
    } __attribute__((packed));
} __attribute__((packed)) reg3d;

class TLV320DAC3101 : public DAC, public Amplifier {
	private:

        DTWI *dtwi;
        uint32_t _sampleFrequency;
        uint8_t _resetPin;

        reg3d _3D;

        void setRaw(uint8_t reg, uint8_t val);
        void setRaw2(uint8_t reg, uint8_t val1, uint8_t val2);
        void setRegister(uint8_t page, uint8_t reg, uint8_t val);
        void setRegister(uint8_t page, uint8_t reg, uint8_t val1, uint8_t val2);

        void set3D();

        uint8_t _channelStatus = 0x00;

        uint8_t _gainHeadphoneLeft = 0x00;
        uint8_t _gainHeadphoneRight = 0x00;
        uint8_t _gainSpeakerLeft = 0x00;
        uint8_t _gainSpeakerRight = 0x00;

        void updateHeadphone();
        void updateSpeaker();

        void setBiquad(uint8_t base, float b0, float b1, float b2, float a0, float a1, float a2);


	public:
        TLV320DAC3101(DTWI *d, uint32_t fs = 44100, uint8_t res = 255) : dtwi(d), _sampleFrequency(fs), _resetPin(res) {}
        TLV320DAC3101(DTWI &d, uint32_t fs = 44100, uint8_t res = 255) : dtwi(&d), _sampleFrequency(fs), _resetPin(res) {}

        static const uint8_t BIQUAD_A = 0x02;
        static const uint8_t BIQUAD_B = 0x0C;
        static const uint8_t BIQUAD_C = 0x16;
        static const uint8_t BIQUAD_D = 0x20;
        static const uint8_t BIQUAD_E = 0x2A;
        static const uint8_t BIQUAD_F = 0x34;

        static const uint8_t LEFT = 0x01;
        static const uint8_t RIGHT = 0x02;
        static const uint8_t STEREO = 0x03;

		void begin();

        void mute();
        void unmute();

        void muteLeft();
        void unmuteLeft();

        void muteRight();
        void unmuteRight();

        void setGain(int8_t vol);
        void setHeadphoneGain(int8_t gain);
        void setHeadphoneGain(int8_t left, int8_t right);
        void setSpeakerGain(int8_t gain);
        void setSpeakerGain(int8_t left, int8_t right);
        void setVolume(uint8_t v);
        void setVolume(uint8_t l, uint8_t r);
        void setBass(int8_t cut);
        void setMid(int8_t cut);
        void setTreble(int8_t cut);

        void setMonoLeft();
        void setMonoRight();
        void setStereo();

        void enable3D();
        void disable3D();
        void setWide3D();
        void setNarrow3D();
        void set3DDepth(uint8_t w);

        void enableAmplifier();
        void disableAmplifier();

        void setHeadphoneMode();
        void setLineOutMode();

        void beep(uint8_t vl, uint8_t vr, uint32_t freq, uint32_t duration);

        void lowPassFilter(uint8_t block, uint8_t channels, uint32_t corner, float q);
        void highPassFilter(uint8_t block, uint8_t channels, uint32_t ciorner, float q);
        void bandPassSkirtFilter(uint8_t block, uint8_t channels, uint32_t center, float q);
        void bandPassZeroFilter(uint8_t block, uint8_t channels, uint32_t center, float q);
        void notchFilter(uint8_t block, uint8_t channels, uint32_t center, float q);
        void allPassFilter(uint8_t block, uint8_t channels, uint32_t center, float q);
        void peakingEQFilter(uint8_t block, uint8_t channels, uint32_t center, float q, float dbgain);
        void lowShelfFilter(uint8_t block, uint8_t channels, uint32_t corner, float q, float dbgain);
        void highShelfFilter(uint8_t block, uint8_t channels, uint32_t corner, float q, float dbgain);

        void updateFilters() {
            setRegister(8, 1, 0x05);
        }

//        void setHeadphoneVolume(uint8_t v);
//        void setHeadphoneVolume(uint8_t l, uint8_t r);
//        void setSpeakerVolume(uint8_t v);
//        void setSpeakerVolume(uint8_t l, uint8_t r);

};

#endif
