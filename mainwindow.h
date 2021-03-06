#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <xmpmoduleplayer.h>
#include <QAudio>
#include <QListWidgetItem>
#include <QTreeWidgetItem>
#include <QProgressBar>
#include "trackview.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void addChildren(QTreeWidgetItem* item,QString filePath);
    void initLoadModule(QString path);

    void fillTreeWidget(QString path);

signals:
    void startPlayerSignal();
    void stopPlayerSignal();


private slots:
    void on_playPauseButton_clicked();
    void on_stopButton_clicked();
    void guiUpdate(xmp_module_info *minfo, xmp_frame_info *finfo);
    void handleStateChanged(QAudio::State newState);
//    void on_posListItemSelected(QListWidgetItem* item);
    void showDirectory(QTreeWidgetItem* item, int /*column*/);

    void on_nextButton_clicked();

    void on_prevButton_clicked();

    void on_loopButton_toggled(bool checked);

    void on_progessSlider_sliderPressed();

    void on_progessSlider_sliderReleased();

    void on_actionOpen_triggered();

    void on_actionExit_triggered();

    void on_actionAbout_triggered();

private:
    bool isSliderPressed;
    Ui::MainWindow *ui;
    XmpModulePlayer *player;
    QStringList supportedFileTypes;
    QString settingsPath;
    TrackView trackview;

};

#endif // MAINWINDOW_H
