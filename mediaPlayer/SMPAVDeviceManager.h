//
// Created by pingkai on 2020/11/2.
//

#ifndef CICADAMEDIA_SMPAVDEVICEMANAGER_H
#define CICADAMEDIA_SMPAVDEVICEMANAGER_H

#include <codec/IDecoder.h>
#include <render/audio/IAudioRender.h>


// TODO: add create lock
namespace Cicada {
    class SMPAVDeviceManager {
    public:
        enum deviceType { DEVICE_TYPE_VIDEO = 1 << 0, DEVICE_TYPE_AUDIO = 1 << 1 };

        struct DecoderHandle {
            std::unique_ptr<IDecoder> decoder{nullptr};
            Stream_meta meta{};
            bool valid{false};
            uint64_t decFlag;
            void *device;

            bool match(const Stream_meta* pMeta,uint64_t flag,void *pDevice)
            {
                return (pDevice == device) && (flag == decFlag) && (pMeta->codec == meta.codec);
            }
        };

    public:
        SMPAVDeviceManager();
        ~SMPAVDeviceManager();

        IDecoder *getDecoder(deviceType type) const
        {
            if (type == DEVICE_TYPE_VIDEO) {
                return mVideoDecoder.decoder.get();
            } else if (type == DEVICE_TYPE_AUDIO) {
                return mAudioDecoder.decoder.get();
            }
            return nullptr;
        }

        bool isDecoderValid(deviceType type) const
        {
            if (type == DEVICE_TYPE_VIDEO) {
                return mVideoDecoder.valid;
            } else if (type == DEVICE_TYPE_AUDIO) {
                return mAudioDecoder.valid;
            }
            return false;
        }
        bool isAudioRenderValid() const
        {
            return mAudioRenderValid;
        }

        void invalidDevices(uint64_t deviceTypes);

        void flushDevice(uint64_t deviceTypes);

        int getFrame(std::unique_ptr<IAFFrame> &frame, deviceType type, uint64_t timeOut);

        int sendPacket(std::unique_ptr<IAFPacket> &packet, deviceType type, uint64_t timeOut);

        int setVolume(float volume);

        void setMute(bool mute);

        uint64_t getAudioRenderQueDuration();

        int renderAudioFrame(std::unique_ptr<IAFFrame> &frame, int timeOut);

        void pauseAudioRender(bool pause);

        int setUpAudioRender(const IAFFrame::audioInfo &info);

        int setSpeed(float speed);

        int64_t getAudioRenderPosition();

        void setAudioRenderListener(IAudioRenderListener *listener);

        void setAudioRenderingCb(renderingFrameCB cb, void *userData);

        int setUpDecoder(uint64_t decFlag, const Stream_meta *meta, void *device, deviceType type);

        uint64_t getVideoDecoderFlags();

    private:
        std::mutex mMutex{};
        DecoderHandle mAudioDecoder;
        DecoderHandle mVideoDecoder;
        std::unique_ptr<IAudioRender> mAudioRender{nullptr};
        IAFFrame::audioInfo mAudioRenderInfo{};
        bool mAudioRenderValid{false};
        DecoderHandle *getDecoderHandle(const deviceType &type);
        bool mMute{false};
    };
}// namespace Cicada


#endif//CICADAMEDIA_SMPAVDEVICEMANAGER_H
