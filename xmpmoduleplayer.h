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

enum PLAY_STATE{IDLE, PLAYING, PAUSED};

class XmpModulePlayer: public QObject
{

Q_OBJECT

public:

    XmpModulePlayer(QObject* parent = 0);
    void loadModule(char *);
    void startModule();
    void stopModule();
    void pauseModule();
    void resumeModule();
    ProxyDevice* getDevice();
    PLAY_STATE getPlayerState();
    char *getModuleTitle();
    char *getModuleType();
    char *getModuleComment();
    qint32 getModuleLength();
    qint32 getModuleBPM();
    qint32 getModuleSpeed();

    void setPosition(qint32 pos);

    //qint32 getModuleProgress();
    void setLoop(bool loop);
    QAudioOutput* getAudio();


signals:

    void sendFrameInfo(struct xmp_module_info* minfo, struct xmp_frame_info* finfo);



public slots:
    void feedAudioSlot(char* data, qint64 size, qint64& wasRead);


private:
    xmp_context ctx;
    struct xmp_module_info mi;
    struct xmp_frame_info fi;
    QAudioOutput *audio;
    ProxyDevice *ourDevice;
    PLAY_STATE player_state;
    qint8 isLoop;

};


#endif // XMPMODULEPLAYER_H
