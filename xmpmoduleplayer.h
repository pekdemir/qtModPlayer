#ifndef XMPMODULEPLAYER_H
#define XMPMODULEPLAYER_H

#include <QtGlobal>
#include <QtCore>
#include <xmp.h>
#include <QtMultimedia/QAudioBuffer>
#include <QtMultimedia/QAudioOutput>
#include <QtMultimedia/QAudioFormat>
#include <QThread>
#include <QIODevice>
#include <QByteArray>
#include <proxydevice.h>
#include <QThread>

enum PLAY_STATE{IDLE, PLAYING, PAUSED};

class XmpModulePlayer: public QThread
{

Q_OBJECT

public:

    XmpModulePlayer(QObject *parent = 0);

    void run();


    void loadModule(char *);
//    void startModule();
//    void stopModule();
    void pauseModule();
    void resumeModule();
    ProxyDevice* getDevice();
    PLAY_STATE getPlayerState();
    char *getModuleTitle();
    char *getModuleType();
    char *getModuleComment();
    QStringList getInstrumentNames();
    qint32 getModuleLength();
    qint32 getModuleBPM();
    qint32 getModuleSpeed();
    qint32 getChannelNumber();

    void setPosition(qint32 pos);
    void seek(quint32 time);
    qint32 getTotalTime();

    //qint32 getModuleProgress();
    void setLoop(bool loop);
    QAudioOutput* getAudio();
    void setVolume(qint32 volume);


signals:

    void sendFrameInfo(struct xmp_module_info* minfo, struct xmp_frame_info* finfo);



public slots:
    void feedAudioSlot(char* data, qint64 size, qint64& wasRead);
    void startModule();
    void stopModule();

private:
    xmp_context ctx;
    struct xmp_module_info mi;
    struct xmp_frame_info fi;
    QAudioOutput *audio;
    ProxyDevice *ourDevice;
    PLAY_STATE player_state;
    qint8 isLoop;
    QThread *audioThread;

};


#endif // XMPMODULEPLAYER_H
