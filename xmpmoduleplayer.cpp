#include <xmpmoduleplayer.h>
#include <QMessageBox>
#include "mainwindow.h"


XmpModulePlayer::XmpModulePlayer(QObject* parent):
    QThread(parent)
{

    ctx = xmp_create_context();

    QAudioFormat format;

    // Set up the format, eg.
    format.setSampleRate(44100);
    format.setChannelCount(2);
    format.setSampleSize(16);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::UnSignedInt);

    audio = new QAudioOutput(format, NULL);

    ourDevice = new ProxyDevice();
    ourDevice->open(QIODevice::ReadWrite);

    player_state = IDLE;
    isLoop = 0;

    connect(ourDevice, &ProxyDevice::feedAudio, this, &XmpModulePlayer::feedAudioSlot, Qt::DirectConnection);

}

void XmpModulePlayer::run()
{

}

ProxyDevice* XmpModulePlayer::getDevice()
{
    return ourDevice;
}

void XmpModulePlayer::loadModule(char * filename)
{

    if(xmp_load_module(ctx, filename) != 0)
    {
        //printf("Error on module loading!\n");
        throw "Error on module loading!\n";
        return;
    }
    xmp_get_module_info(ctx, &mi);

}

PLAY_STATE XmpModulePlayer::getPlayerState()
{
    return player_state;
}

void XmpModulePlayer::startModule()
{
    if(player_state == IDLE)
    {

        xmp_start_player(ctx, 44100, 0);
        audio->start(ourDevice);
        player_state = PLAYING;

    }else if(player_state == PLAYING)
    {
        audio->suspend();
        player_state = PAUSED;
    }else if(player_state == PAUSED)
    {
        audio->resume();
        player_state = PLAYING;
    }
}

void XmpModulePlayer::pauseModule()
{

    audio->suspend();
}

void XmpModulePlayer::resumeModule()
{

    audio->resume();
}

char* XmpModulePlayer::getModuleTitle()
{
    return mi.mod->name;
}

char *XmpModulePlayer::getModuleType()
{
    return mi.mod->type;
}

char *XmpModulePlayer::getModuleComment()
{
    return mi.comment;
}

QStringList XmpModulePlayer::getInstrumentNames()
{
    QStringList ilist;
    //qDebug() << "" << mi.mod->ins;
    for(int i=1; i <= mi.mod->ins; i++)
    {
        QString str;
        ilist.append(str.sprintf("%02X|%s", i, mi.mod->xxi[i].name));
    }

    return ilist;
}

qint32 XmpModulePlayer::getModuleLength()
{
    return mi.mod->len;
}

qint32 XmpModulePlayer::getModuleBPM()
{
    return mi.mod->bpm;
}

qint32 XmpModulePlayer::getModuleSpeed()
{
    return mi.mod->spd;
}

qint32 XmpModulePlayer::getChannelNumber()
{
    return mi.mod->chn;
}

void XmpModulePlayer::setPosition(qint32 pos)
{
    xmp_set_position(ctx, pos);
}

void XmpModulePlayer::seek(quint32 time)
{
    xmp_seek_time(ctx, time);
}

qint32 XmpModulePlayer::getTotalTime()
{
    xmp_get_frame_info(ctx, &fi);
    return fi.total_time;
}


//qint32 XmpModulePlayer::getModuleProgress()
//{
//    xmp_get_frame_info(ctx, &fi);
//    qint32 progress = fi.pos * 100 / mi.mod->len;
//    return progress;
//}

void XmpModulePlayer::setLoop(bool loop)
{
    isLoop = loop==true ? 1 : 0;
}


QAudioOutput* XmpModulePlayer::getAudio()
{
    return audio;
}

void XmpModulePlayer::setVolume(qint32 volume)
{
    qreal linearVolume = QAudio::convertVolume(volume / qreal(100.0),
                                                     QAudio::LogarithmicVolumeScale,
                                                     QAudio::LinearVolumeScale);
    qDebug() << linearVolume;
//    audio->setVolume(qRound(linearVolume * 100));
    audio->setVolume(linearVolume + 1);
    //    audio->setVolume(qreal(100.0) / volume );
}

void XmpModulePlayer::feedAudioSlot(char* data, qint64 size, qint64& wasRead)
{

    //if(xmp_play_buffer(ctx, data, size, isLoop) == 0)
    if(xmp_play_frame(ctx) == 0)
    {
        xmp_get_frame_info(ctx, &fi);
        memcpy(data, fi.buffer, fi.buffer_size);
        //wasRead = size;
        wasRead = fi.buffer_size;

        emit sendFrameInfo(&mi, &fi);
    }else
        wasRead = 0;

}

void XmpModulePlayer::stopModule()
{
    if(player_state == PLAYING || player_state == PAUSED)
    {
        audio->stop();
        xmp_end_player(ctx);
        player_state = IDLE;
    }
}
