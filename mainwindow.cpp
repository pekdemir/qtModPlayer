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

    connect(ui->posList, SIGNAL(itemClicked(QListWidgetItem*)),
                this, SLOT(on_posListItemSelected(QListWidgetItem*)));

//    QTimer *guiUpdateTimer = new QTimer(this);
//    connect(guiUpdateTimer, SIGNAL(timeout()), this, SLOT(guiUpdate()));
//    guiUpdateTimer->start(1000);

    supportedFileTypes << "*.it" << "*.mod" << "*.xm";
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
    headerItem->setText(2,QString("Type"));
    headerItem->setText(3,QString("Path"));
    ui->treeWidget->setHeaderItem(headerItem);

    QDir* rootDir = new QDir("/home/ubuntu/Music");
    QFileInfoList filesList = rootDir->entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs |QDir::Files);

    foreach(QFileInfo fileInfo, filesList)
    {
      QTreeWidgetItem* item = new QTreeWidgetItem();
      item->setText(0,fileInfo.fileName());

      if(fileInfo.isFile())
      {
        item->setText(1,QString::number(fileInfo.size()));
        item->setText(2,fileInfo.suffix());
        //item->setIcon(0,*(new QIcon("file.jpg")));
      }

      if(fileInfo.isDir())
      {
        //item->setIcon(0,*(new QIcon("folder.jpg")));
        addChildren(item,fileInfo.filePath());
      }

      item->setText(3,fileInfo.filePath());
      ui->treeWidget->addTopLevelItem(item);
    }
}

void MainWindow::initLoadModule(QString path)
{
    player->stopModule();
    //ui->playPauseButton->setText(tr("Pause"));
    ui->playPauseButton->setIcon(QIcon(":/icon/pause.png"));
    QByteArray by = path.toLocal8Bit();
    player->loadModule(by.data());
    player->startModule();
    MainWindow::setWindowTitle(QString("QtModPlayer [%1]").arg(player->getModuleTitle()));

    ui->titleEdit->setText(QString("[%1] [%2]").arg(player->getModuleTitle(), player->getModuleType()));
    ui->typeEdit->setPlainText(QString(player->getModuleComment()));

    // fill pos list
    ui->posList->clear();
    qint32 pos = player->getModuleLength();
    for(qint32 i=1; i<=pos; i++)
        if(i < 10)
            ui->posList->addItem(QString("0").append(QString::number(i)));
        else
            ui->posList->addItem(QString::number(i));
    ui->posList->setCurrentRow(0);

}

void MainWindow::addChildren(QTreeWidgetItem* item,QString filePath)
{
    QDir* rootDir = new QDir(filePath);
    QFileInfoList filesList = rootDir->entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs |QDir::Files);

    foreach(QFileInfo fileInfo, filesList)
    {
        QTreeWidgetItem* child = new QTreeWidgetItem();
        child->setText(0,fileInfo.fileName());


        if(fileInfo.isFile())
        {
          child->setText(1,QString::number(fileInfo.size()));
          child->setText(2,fileInfo.suffix());
          child->setText(3,fileInfo.filePath());
        }

        if(fileInfo.isDir())
        {
          //child->setIcon(0,*(new QIcon("folder.jpg")));
          child->setText(3,fileInfo.filePath());

        }

        item->addChild(child);
    }
}

void MainWindow::showDirectory(QTreeWidgetItem* item, int /*column*/)
{
    // if a file is clicked (it has a type)
    if(item->text(2) != QString(""))
    {
        initLoadModule(item->text(3));
        return;
    }

    QDir* rootDir = new QDir(item->text(3));
    QFileInfoList filesList = rootDir->entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs |QDir::Files);

    foreach(QFileInfo fileInfo, filesList)
    {
        QTreeWidgetItem* child = new QTreeWidgetItem();
        child->setText(0,fileInfo.fileName());

        if(fileInfo.isFile())
        {
          child->setText(1,QString::number(fileInfo.size()));
          child->setText(2,fileInfo.suffix());
          child->setText(3,fileInfo.filePath());
          //child->setIcon(0,*(new QIcon("file.jpg")));
        }

        if(fileInfo.isDir())
        {
          //child->setIcon(0,*(new QIcon("folder.jpg")));
          child->setText(3,fileInfo.filePath());
        }

        if(fileInfo.isFile() && fileInfo.suffix() != QString("mod") && fileInfo.suffix() != QString("xm") && fileInfo.suffix() != QString("it") && fileInfo.suffix() != QString("s3m"))
            continue;
        else
            item->addChild(child);

        ui->treeWidget->resizeColumnToContents(0);
    }
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

void MainWindow::guiUpdate(struct xmp_module_info* minfo,struct xmp_frame_info* finfo)
{
    qint32 progress = ((finfo->pos * finfo->num_rows) + finfo->row) * 100 / (minfo->mod->len * finfo->num_rows);
    ui->progessSlider->setValue(progress);
//    printf("%d ", finfo->row);
//    fflush(stdout);
    QString rowInfo;
    rowInfo.sprintf("Row: %02d/%02d", finfo->row, finfo->num_rows);

    ui->rowInfoLabel->setText(rowInfo);    

    ui->posList->setCurrentRow(finfo->pos);

    QString bpm;
    bpm.sprintf("BPM: %03d", finfo->bpm);
    ui->bpmLabel->setText(bpm);

    QString spd;
    spd.sprintf("SPD: %02d", finfo->speed);
    ui->speedLabel->setText(spd);
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

void MainWindow::on_posListItemSelected(QListWidgetItem* item)
{
    qint32 selection = ui->posList->currentRow();
    player->setPosition(selection);
}


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
    }
}

void MainWindow::on_loopButton_toggled(bool checked)
{
    //qDebug() << "Checked: " << checked;
    player->setLoop(checked);
}

