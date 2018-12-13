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

        reg3d _3D;

        void setRaw(uint8_t reg, uint8_t val);
        void setRegister(uint8_t page, uint8_t reg, uint8_t val);

        void set3D();

        uint8_t _channelStatus = 0x00;

        uint8_t _gainHeadphoneLeft = 0x00;
        uint8_t _gainHeadphoneRight = 0x00;
        uint8_t _gainSpeakerLeft = 0x00;
        uint8_t _gainSpeakerRight = 0x00;

        void updateHeadphone();
        void updateSpeaker();

	public:
        TLV320DAC3101(DTWI *d) : dtwi(d) {}
        TLV320DAC3101(DTWI &d) : dtwi(&d) {}

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

        void enable3D();
        void disable3D();
        void setWide3D();
        void setNarrow3D();
        void set3DDepth(uint8_t w);

        void enableAmplifier();
        void disableAmplifier();

        void setHeadphoneMode();
        void setLineOutMode();

//        void setHeadphoneVolume(uint8_t v);
//        void setHeadphoneVolume(uint8_t l, uint8_t r);
//        void setSpeakerVolume(uint8_t v);
//        void setSpeakerVolume(uint8_t l, uint8_t r);

};

#endif
