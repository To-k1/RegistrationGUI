#ifndef MAINWINDOW_H
#define MAINWINDOW_H



#include <QMainWindow>
#include <QFileDialog>
#include <QString>
#include <QObject>
#include <QThread>
#include <opencv2/opencv.hpp>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

using namespace cv;

class MainWindow;

class Worker:public QObject
{
    Q_OBJECT
public:
    Worker(QObject* parent = nullptr){}
public slots:
     // doWork定义了线程要执行的操作
    void registrating(cv::String& srcPattern, cv::String& binPattern, cv::String& dstPattern, char useSemiAuto, char useFailedImg, MainWindow* win, int startPos = 0);

// 线程完成工作时发送的信号
signals:
    void resultReady();
    void set_labelProcessingName_text(const QString& text);
    void set_labelProgress_text(const QString& text);
    void set_progressBar_val(int val);
};


class MainWindow : public QMainWindow
{
    Q_OBJECT
    QThread workerThread;
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    inline char getSemi(){return semiauto;}
    inline char getFailed(){return failed_list;}


signals:
    void operate(cv::String& srcPattern, cv::String& binPattern, cv::String& dstPattern, char useSemiAuto, char useFailedImg, MainWindow* win, int startPos = 0);

public slots:
    void handleResults();
    void set_labelProcessingName_text(const QString& text);
    void set_labelProgress_text(const QString& text);
    void set_progressBar_val(int val);

private slots:
    void on_pushButtonSrc_clicked();

    void on_pushButtonBin_clicked();

    void on_pushButtonDst_clicked();

    void on_checkBoxSemiAuto_stateChanged(int arg1);

    void on_checkBoxFailedList_stateChanged(int arg1);

    void on_pushButtonRun_clicked();

private:
    Ui::MainWindow *ui;
    //分别决定是否使用手动配准和失败文件列表，默认为'n'
    char semiauto;
    char failed_list;
};

void Registrating(String& srcPattern, String& binPattern, String& dstPattern, char useSemiAuto, char useFailedImg, MainWindow* win, int startPos);
#endif // MAINWINDOW_H


