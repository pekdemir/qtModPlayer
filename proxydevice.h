#ifndef PROXYDEVICE_H
#define PROXYDEVICE_H

#include <QtGlobal>
#include <QtCore>

class ProxyDevice : public QIODevice
{

Q_OBJECT

public:
    /** Constructor does almost nothing, just initializes size of fake buffer. */
    ProxyDevice(QObject* parent = 0) : QIODevice(parent), m_bufferSize(2048) {}

    /** In fact, there is no any buffer!
    * That value controls size of data to get by feedAudio()
    * instead of maxlen value sending in readData(data, maxlen) */
    void setBufferSize(qint64 s) { m_bufferSize = s; }
    qint64 bufferSize() const { return m_bufferSize; }

signals:
    /** char* is pointer to data, qint64 is its size
    * and qint64& is reference where connected slot will return size of data has been written.  */
    void feedAudio(char*, qint64, qint64&);

protected:

    /** This is heart of the class:
    * feedAudio(data, m_bufferSize, wasRead) signal is emitted
    * to get m_bufferSize bytes into data
    * and into reference of wasRead connected slot
    * returns amout of bytes were put. */
    virtual qint64 readData(char *data, qint64 maxlen) {
        Q_UNUSED(maxlen);
        qint64 wasRead = 0;
        emit feedAudio(data, m_bufferSize, wasRead);
        return wasRead;
    }

    /** Dummy - does nothing */
    virtual qint64 writeData(const char *data, qint64 len) { return 0; }

    private:
    qint64          m_bufferSize;

};

#endif // PROXYDEVICE_H
