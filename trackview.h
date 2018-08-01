#ifndef TRACKVIEW_H
#define TRACKVIEW_H
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QProgressBar>
#include <QListWidget>
#include <xmp.h>


class TrackView
{
public:
    TrackView();
    void buildNewTrackView(QHBoxLayout *parent, int noOfChannel);
    void clearTrackView(QHBoxLayout *parent);
    void updateTrackView(struct xmp_module_info* minfo,struct xmp_frame_info* finfo);

private:
    struct WidgetPack
    {
        QProgressBar *pb;
        QListWidget *lw;
        quint32 pbMax;
        QVBoxLayout *vb;

    }widgetPack;

    QList<WidgetPack> widgets;
    QSpacerItem *sp = NULL;
    quint32 chn;

};

#endif // TRACKVIEW_H
