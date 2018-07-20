#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "xmp.h"
#include <QtMultimedia/QAudioBuffer>
#include <QtMultimedia/QAudioOutput>
#include <QtMultimedia/QAudioFormat>
#include <QThread>
#include <QIODevice>
#include <QByteArray>
#include <QTimer>
#include <QtWidgets>



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    player = new XmpModulePlayer(this);
    //player->loadModule("test.it");
   // player->loadModule("theme_for_sound_1.mod");
    //player->loadModule("db_ab.xm");

    //ui->TitleLabel->setText(player->getModuleTitle());

    //connect((QObject*)player->getDevice(), SIGNAL(&ProxyDevice::feedAudio), (QObject*)player, SLOT(&XmpModulePlayer::feedAudioSlot), Qt::DirectConnection);
    connect(player->getDevice(), &ProxyDevice::feedAudio, player, &XmpModulePlayer::feedAudioSlot, Qt::DirectConnection);

    //connect player to mainwindow to update gui
    connect(player, &XmpModulePlayer::sendFrameInfo, this, &MainWindow::guiUpdate);

//    connect(ui->posList, SIGNAL(itemClicked(QListWidgetItem*)),
//                this, SLOT(on_posListItemSelected(QListWidgetItem*)));

//    QTimer *guiUpdateTimer = new QTimer(this);
//    connect(guiUpdateTimer, SIGNAL(timeout()), this, SLOT(guiUpdate()));
//    guiUpdateTimer->start(1000);
    isSliderPressed = false;

//    supportedFileTypes << "*.it" << "*.mod" << "*.xm";
//    supportedFileTypes.append(QString("*.it"));
//    supportedFileTypes.append(QString("*.mod"));
//    supportedFileTypes.append(QString("*.xm"));

//    foreach (const QAudioDeviceInfo &deviceInfo, QAudioDeviceInfo::availableDevices(QAudio::AudioOutput))
//        qDebug() << "Device name: " << deviceInfo.deviceName();

    //audio output state change
    connect(player->getAudio(), SIGNAL(stateChanged(QAudio::State)), this, SLOT(handleStateChanged(QAudio::State)));

    connect(ui->treeWidget , SIGNAL(itemClicked(QTreeWidgetItem*,int)),this,SLOT(showDirectory(QTreeWidgetItem*,int)));

    //Set QTreeWidget Column Header
    QTreeWidgetItem* headerItem = new QTreeWidgetItem();
    headerItem->setText(0,QString("File Name"));
    headerItem->setText(1,QString("Size (Bytes)"));
    //headerItem->setText(2,QString("Type"));
    //headerItem->setText(3,QString("Path"));
    ui->treeWidget->setHeaderItem(headerItem);

    QDir* rootDir = new QDir("/home/ubuntu/Music");
    QFileInfoList filesList = rootDir->entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs |QDir::Files);

    foreach(QFileInfo fileInfo, filesList)
    {
      if(fileInfo.isFile() && fileInfo.suffix() != QString("mod") && fileInfo.suffix() != QString("xm") && fileInfo.suffix() != QString("it") && fileInfo.suffix() != QString("s3m"))
        continue;

      QTreeWidgetItem* item = new QTreeWidgetItem();
      item->setText(0,fileInfo.completeBaseName());

      if(fileInfo.isFile())
      {
        item->setText(1,QString::number(fileInfo.size()));
        //item->setText(2,fileInfo.suffix());
        if(fileInfo.suffix() == QString("mod"))
            item->setIcon(0,QIcon(":/icon/music_file_mod_icon.png"));
        else if(fileInfo.suffix() == QString("it"))
            item->setIcon(0,QIcon(":/icon/music_file_it_icon.png"));
        else if(fileInfo.suffix() == QString("s3m"))
            item->setIcon(0,QIcon(":/icon/music_file_s3m_icon.png"));
        else
            item->setIcon(0,QIcon(":/icon/music_file_xm_icon.png"));

        item->setData(0, Qt::UserRole, fileInfo.filePath());
      }

      if(fileInfo.isDir())
      {
        item->setIcon(0,QIcon(":/icon/folder.png"));
        addChildren(item,fileInfo.filePath());
      }

      ui->treeWidget->addTopLevelItem(item);
    }
    ui->treeWidget->resizeColumnToContents(0);
}

void MainWindow::initLoadModule(QString path)
{
    player->stopModule();
    //ui->playPauseButton->setText(tr("Pause"));
    ui->playPauseButton->setIcon(QIcon(":/icon/pause.png"));
    QByteArray by = path.toLocal8Bit();
    player->loadModule(by.data());
    player->startModule();
    //MainWindow::setWindowTitle(QString("QtModPlayer [%1]").arg(player->getModuleTitle()));

    ui->titleEdit->setPlainText(QString("%1\n%2").arg(player->getModuleTitle(), player->getModuleType()));
   // ui->typeEdit->setPlainText(player->getInstrumentNames());

    ui->instrumentsList->clear();
    ui->instrumentsList->addItems(player->getInstrumentNames());

    // fill pos list
//    ui->posList->clear();
//    qint32 pos = player->getModuleLength();
//    for(qint32 i=1; i<=pos; i++)
//        if(i < 10)
//            ui->posList->addItem(QString("0").append(QString::number(i)));
//        else
//            ui->posList->addItem(QString::number(i));
//    ui->posList->setCurrentRow(0);

    //set progressslider maximum
    ui->progessSlider->setMaximum(player->getTotalTime());

}

void MainWindow::addChildren(QTreeWidgetItem* item,QString filePath)
{
    QDir* rootDir = new QDir(filePath);
    QFileInfoList filesList = rootDir->entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs |QDir::Files);

    foreach(QFileInfo fileInfo, filesList)
    {
        if(fileInfo.isFile() && fileInfo.suffix() != QString("mod") && fileInfo.suffix() != QString("xm") && fileInfo.suffix() != QString("it") && fileInfo.suffix() != QString("s3m"))
            continue;

        QTreeWidgetItem* child = new QTreeWidgetItem();
        child->setText(0,fileInfo.completeBaseName());
        child->setData(0, Qt::UserRole, fileInfo.filePath());

        if(fileInfo.isFile())
        {
          child->setText(1,QString::number(fileInfo.size()));
          //child->setText(2,fileInfo.suffix());
          //child->setText(3,fileInfo.filePath());
          if(fileInfo.suffix() == QString("mod"))
              child->setIcon(0,QIcon(":/icon/music_file_mod_icon.png"));
          else if(fileInfo.suffix() == QString("it"))
              child->setIcon(0,QIcon(":/icon/music_file_it_icon.png"));
          else if(fileInfo.suffix() == QString("s3m"))
              child->setIcon(0,QIcon(":/icon/music_file_s3m_icon.png"));
          else
              child->setIcon(0,QIcon(":/icon/music_file_xm_icon.png"));
        }

        if(fileInfo.isDir())
        {
          child->setIcon(0,QIcon(":/icon/folder.png"));
          //child->setText(3,fileInfo.filePath());
          //recursively adding children is closed (too slow)
            //addChildren(child,fileInfo.filePath());
        }


        item->addChild(child);
    }
    ui->treeWidget->resizeColumnToContents(0);
}

void MainWindow::showDirectory(QTreeWidgetItem* item, int /*column*/)
{
    QString filepath = item->data(0, Qt::UserRole).toString();
    // if a file is clicked (it has a type)
    if(filepath.endsWith("xm") || filepath.endsWith("it") || filepath.endsWith("s3m") || filepath.endsWith("mod"))
    {
        initLoadModule(filepath);
        return;
    }

    // if dir item has child dont fill again!!!
    if(item->childCount() > 0)
        return;

    QDir* rootDir = new QDir(filepath);
    QFileInfoList filesList = rootDir->entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs |QDir::Files);

    foreach(QFileInfo fileInfo, filesList)
    {
        if(fileInfo.isFile() && fileInfo.suffix() != QString("mod") && fileInfo.suffix() != QString("xm") && fileInfo.suffix() != QString("it") && fileInfo.suffix() != QString("s3m"))
            continue;

        QTreeWidgetItem* child = new QTreeWidgetItem();
        child->setText(0,fileInfo.completeBaseName());
        child->setData(0, Qt::UserRole, fileInfo.filePath());

        if(fileInfo.isFile())
        {
          child->setText(1,QString::number(fileInfo.size()));
          //child->setText(2,fileInfo.suffix());
          //child->setText(3,fileInfo.filePath());
          if(fileInfo.suffix() == QString("mod"))
              child->setIcon(0,QIcon(":/icon/music_file_mod_icon.png"));
          else if(fileInfo.suffix() == QString("it"))
              child->setIcon(0,QIcon(":/icon/music_file_it_icon.png"));
          else if(fileInfo.suffix() == QString("s3m"))
              child->setIcon(0,QIcon(":/icon/music_file_s3m_icon.png"));
          else
              child->setIcon(0,QIcon(":/icon/music_file_xm_icon.png"));
        }

        if(fileInfo.isDir())
        {
          child->setIcon(0,QIcon(":/icon/folder.png"));
          //child->setText(3,fileInfo.filePath());
        }

        item->addChild(child);
    }
    item->setExpanded(true);
    ui->treeWidget->resizeColumnToContents(0);

}

MainWindow::~MainWindow()
{
    delete ui;
}

static void display_data(struct xmp_module_info *mi, struct xmp_frame_info *fi)
{
    printf("%3d/%3d %3d/%3d\r",
           fi->pos, mi->mod->len, fi->row, fi->num_rows);

    fflush(stdout);
}

static char *note_name[] = {
    "C-", "C#", "D-", "D#", "E-", "F-", "F#", "G-", "G#", "A-", "A#", "B-"
};

void MainWindow::guiUpdate(struct xmp_module_info* minfo,struct xmp_frame_info* finfo)
{
    qint32 currTimeM = (finfo->time / 1000) / 60;
    qint32 currTimeS = (finfo->time / 1000) % 60;

    qint32 totalTimeM = (finfo->total_time / 1000) / 60;
    qint32 totalTimeS = (finfo->total_time / 1000) % 60;

    QString timeInfo;
    timeInfo.sprintf("%02d:%02d/%02d:%02d", currTimeM, currTimeS, totalTimeM, totalTimeS);
    ui->timeLabel->setText(timeInfo);

    //qint32 progress = ((finfo->pos * finfo->num_rows) + finfo->row) * 100 / (minfo->mod->len * finfo->num_rows);
    //qint32 progress = ((float)finfo->time / (float)finfo->total_time) * 100;
   // ui->progessSlider->setMaximum(finfo->total_time);

    if(!isSliderPressed)
        ui->progessSlider->setValue(finfo->time);
//    printf("%d ", finfo->row);
//    fflush(stdout);
    QString moduleInfo;
    moduleInfo.sprintf("PAT:%02x ROW:%02x BPM:%03d SPD:%02x CHN:%02d", finfo->pattern, finfo->row, finfo->bpm, finfo->speed, finfo->virt_used);

    ui->StatusLabel->setText(moduleInfo.toUpper());




    static int patternIndex = -1;
    if(patternIndex != finfo->pattern)
    {
        QStringList tracks;

        for(int j = 0; j < finfo->num_rows / 2; j++)
        {
            // add dummy rows
            QString row;
            tracks.append(row);
        }


        for(int j = 0; j < finfo->num_rows; j++)
        {

            QString row;
            for (int i = 0; i < minfo->mod->chn; i++)
            {
                int track = minfo->mod->xxp[finfo->pattern]->index[i];

                struct xmp_event *event = &minfo->mod->xxt[track]->event[j];

                if (event->note > 0x80) {
                    row.append("=== ");
                } else if (event->note > 0) {
                    int note = event->note - 1;
                    row.append(row.asprintf("%s%d ", note_name[note % 12], note / 12));
                } else {
                    row.append("--- ");
                }

                if (event->ins > 0) {
                    row.append(row.asprintf("%02X ", event->ins));
                } else {
                    row.append("-- ");
                }

                if (event->fxt > 0) {
                    row.append(row.asprintf("%X%02X", event->fxt, event->fxp));
                } else {
                    row.append("---");
                }

                row.append("|");
            }
            tracks.append(row);
        }

        for(int j = 0; j < finfo->num_rows / 2; j++)
        {
            // add dummy rows
            QString row;
            tracks.append(row);
        }

        ui->noteList->clear();
        ui->noteList->addItems(tracks);
    }else
    {
        patternIndex = finfo->pattern;

    }
    ui->noteList->setCurrentRow(finfo->row + finfo->num_rows / 2);

    //adjust notelist scroll
    QScrollBar *sb = ui->noteList->verticalScrollBar();
    //int scrollpos = ((sb->maximum() + sb->minimum()) / 4) + (sb->maximum() + sb->minimum()) * ((qreal)finfo->row / (finfo->num_rows * 2));
    int scrollpos = sb->singleStep() *  finfo->num_rows / 4;
    scrollpos += sb->singleStep()  * finfo->row;
    sb->setValue(scrollpos);

}

void MainWindow::on_playPauseButton_clicked()
{

    player->startModule();

    PLAY_STATE state = player->getPlayerState();
    if(state == IDLE || state == PAUSED)
        //ui->playPauseButton->setText(tr("Play"));
        ui->playPauseButton->setIcon(QIcon(":/icon/play-button.png"));
    else
        //ui->playPauseButton->setText(tr("Pause"));
        ui->playPauseButton->setIcon(QIcon(":/icon/pause.png"));
}

void MainWindow::on_stopButton_clicked()
{
    player->stopModule();
    //ui->playPauseButton->setText(tr("Play"));
    ui->playPauseButton->setIcon(QIcon(":/icon/play-button.png"));
}

//void MainWindow::on_posListItemSelected(QListWidgetItem* item)
//{
//    qint32 selection = ui->posList->currentRow();
//    player->setPosition(selection);
//}


void MainWindow::handleStateChanged(QAudio::State newState)
{
    if(newState == QAudio::IdleState || newState == QAudio::StoppedState)
    {
        player->stopModule();
        //ui->playPauseButton->setText(tr("Play"));
        ui->playPauseButton->setIcon(QIcon(":/icon/play-button.png"));
    }
}

void MainWindow::on_nextButton_clicked()
{
    QModelIndex currIndex = ui->treeWidget->currentIndex();
    QModelIndex nextIndex = currIndex.sibling(currIndex.row() + 1, currIndex.column());
    if(nextIndex.isValid())
    {
        ui->treeWidget->setCurrentIndex(nextIndex);
        ui->treeWidget->clicked(nextIndex);
    }else
    {
        QModelIndex parent = currIndex.parent();
        QModelIndex nextParent = parent.sibling(parent.row() + 1, parent.column());
        nextIndex = nextParent.child(0, currIndex.column());
        if(nextIndex.isValid())
        {
            ui->treeWidget->setCurrentIndex(nextIndex);
            ui->treeWidget->clicked(nextIndex);
        }
    }

}

void MainWindow::on_prevButton_clicked()
{
    QModelIndex currIndex = ui->treeWidget->currentIndex();
    QModelIndex prevIndex = currIndex.sibling(currIndex.row() - 1, currIndex.column());
    if(prevIndex.isValid())
    {
        ui->treeWidget->setCurrentIndex(prevIndex);
        ui->treeWidget->clicked(prevIndex);
    }else
    {
        QModelIndex parent = currIndex.parent();
        QModelIndex prevParent = parent.sibling(parent.row() - 1, parent.column());
        prevIndex = prevParent.child(prevParent.model()->rowCount(prevParent) - 1, currIndex.column());
        if(prevIndex.isValid())
        {
            ui->treeWidget->setCurrentIndex(prevIndex);
            ui->treeWidget->clicked(prevIndex);
        }
    }
}

void MainWindow::on_loopButton_toggled(bool checked)
{
    //qDebug() << "Checked: " << checked;
    player->setLoop(checked);
}


void MainWindow::on_progessSlider_sliderPressed()
{
    isSliderPressed = true;
}

void MainWindow::on_progessSlider_sliderReleased()
{

    quint32 sliderPos = ui->progessSlider->value();
    player->seek(sliderPos);



    isSliderPressed = false;

}
