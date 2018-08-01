#include "trackview.h"

TrackView::TrackView()
{

}

void TrackView::buildNewTrackView(QHBoxLayout *parent, int noOfChannel)
{
    chn = noOfChannel;
    for(int i = 0; i < chn; i++)
    {
        WidgetPack wp;
        QProgressBar *pb = new QProgressBar();
        pb->setFormat(QString("Channel %1").arg(i + 1));
        pb->setMaximum(256);
        pb->setFixedSize(150, 15);
        pb->setValue(0);
        wp.pb = pb;
        wp.pbMax = 0;

        QVBoxLayout *vb = new QVBoxLayout();
        vb->setSizeConstraint(QVBoxLayout::SetFixedSize);
        vb->addWidget(pb);
        wp.vb = vb;

        QListWidget *lw = new QListWidget();
        lw->setFixedSize(150, 290);
        vb->addWidget(lw);
        parent->addLayout(vb);
        wp.lw = lw;
        widgets.append(wp);
        //ui->tracksHLayout->setAlignment(vb, Qt::AlignLeft);
    }
    sp = new QSpacerItem(500, 100, QSizePolicy::Maximum);
    parent->addSpacerItem(sp);
}

void TrackView::clearTrackView(QHBoxLayout *parent)
{
    foreach (WidgetPack wp, widgets) {
        parent->removeItem(wp.vb);
        delete wp.lw;
        delete wp.pb;
        delete wp.vb;
    }
    if(sp)
    {
        parent->removeItem(sp);
        delete sp;
    }
    widgets.clear();
}

static char *note_name[] = {
    "C-", "C#", "D-", "D#", "E-", "F-", "F#", "G-", "G#", "A-", "A#", "B-"
};

void TrackView::updateTrackView(xmp_module_info *minfo, xmp_frame_info *finfo)
{
    //add current pattern
    for (int i = 0; i < minfo->mod->chn; i++)
    {
        QListWidget *lw = widgets.at(i).lw;
        lw->clear();

        //update channel progressbars
        QProgressBar* cpb = widgets.at(i).pb;
        quint32 max = widgets.at(i).pbMax;
        int flg = minfo->mod->xxs[finfo->channel_info[i].sample].flg;
        if(flg & XMP_SAMPLE_16BIT)
        {
            unsigned short *sampleData = (unsigned short*) minfo->mod->xxs[finfo->channel_info[i].sample].data;
            unsigned short val = sampleData[finfo->channel_info[i].position];
//            if((unsigned short) max < val)
//            {
//                widgets[i].pbMax = val;
//                max = val;
//            }
//            cpb->setValue(max);
//            widgets[i].pbMax = max == 0 ? 0: max - 1;
            if(val > 0)
                cpb->setValue(val);
        }else
        {
            unsigned char *sampleData = minfo->mod->xxs[finfo->channel_info[i].sample].data;
            unsigned char val = sampleData[finfo->channel_info[i].position];
//            if((unsigned char)max < val)
//            {
//                widgets[i].pbMax = val;
//                max = val;
//            }
//            cpb->setValue(max);
//            widgets[i].pbMax = max == 0 ? 0: max - 1;
            if(val > 0)
                cpb->setValue(val);
        }

        for(int j = 0; j < finfo->num_rows; j++)
        {

            QString row;


            int track = minfo->mod->xxp[finfo->pattern]->index[i];

            struct xmp_event *event = &minfo->mod->xxt[track]->event[j];

            if (event->note > 0x80) {
                row.append("=== ");
            } else if (event->note > 0) {
                int note = event->note - 1;
                row.append(row.asprintf("%s%X ", note_name[note % 12], note / 12));
            } else {
                row.append("--- ");
            }

            if (event->ins > 0) {
                row.append(row.asprintf("%02X ", event->ins));
            } else {
                row.append("-- ");
            }

            if (event->vol > 0) {
                row.append(row.asprintf("v%02X ", event->vol));
            } else {
                row.append(" -- ");
            }

            if (event->fxt > 0) {
                row.append(row.asprintf("%2X%02X", event->fxt, event->fxp));
            } else {
                row.append("----");
            }


            lw->addItem(row);
        }

        lw->setCurrentRow(finfo->row);

    }

}
