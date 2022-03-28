#include "mainwindow.h"
#include "ui_mainwindow.h"

void Worker::registrating(String& srcPattern, String& binPattern, String& dstPattern, char useSemiAuto, char useFailedImg, MainWindow* win, int startPos)
{
    // 工作
    Registrating(srcPattern, binPattern, dstPattern, useSemiAuto, useFailedImg, win, startPos);
    // 发送结束信号
    emit resultReady();
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , semiauto('n')
    , failed_list('n')
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButtonSrc_clicked()
{
    ui->lineEditSrc->setText(QFileDialog::getExistingDirectory(this, QString::fromLocal8Bit("请选择文件夹路径..."), QString::fromLocal8Bit("./")));
}


void MainWindow::on_pushButtonBin_clicked()
{
    ui->lineEditBin->setText(QFileDialog::getExistingDirectory(this, QString::fromLocal8Bit("请选择文件夹路径..."), QString::fromLocal8Bit("./")));
}


void MainWindow::on_pushButtonDst_clicked()
{
    ui->lineEditDst->setText(QFileDialog::getExistingDirectory(this, QString::fromLocal8Bit("请选择文件夹路径..."), QString::fromLocal8Bit("./")));
}


void MainWindow::on_checkBoxSemiAuto_stateChanged(int arg1)
{
    (arg1 == 2) ? (this->semiauto = 'y') : (this->semiauto = 'n');
}


void MainWindow::on_checkBoxFailedList_stateChanged(int arg1)
{
    (arg1 == 2) ? (this->failed_list = 'y') : (this->failed_list = 'n');
}


void MainWindow::on_pushButtonRun_clicked()
{
    String filePattern(ui->lineEditSrc->text().toStdString());
    String binImg(ui->lineEditBin->text().toStdString());
    String dstImg(ui->lineEditDst->text().toStdString());
    Worker *worker = new Worker;
    //调用moveToThread将该任务交给workThread
    worker->moveToThread(&workerThread);
    //用于传递程序进度信息
    connect(worker, &Worker::set_labelProcessingName_text, this, &MainWindow::set_labelProcessingName_text);
    connect(worker, &Worker::set_labelProgress_text, this, &MainWindow::set_labelProgress_text);
    connect(worker, &Worker::set_progressBar_val, this, &MainWindow::set_progressBar_val);
    //operate信号发射后启动线程工作
    connect(this, &MainWindow::operate, worker, &Worker::registrating);
    //线程结束后发送信号，对结果进行处理
    connect(worker, &Worker::resultReady, this, &MainWindow::handleResults);
    //按钮变灰和激活
    ui->pushButtonRun->setEnabled(false);
    ui->pushButtonPause->setEnabled(true);
    ui->checkBoxFailedList->setEnabled(false);
    ui->checkBoxSemiAuto->setEnabled(false);
    //启动线程
    workerThread.start();
    //发射进程运行信号
    emit operate(filePattern, binImg, dstImg, this->getSemi(), this->getFailed(), this);
}

void MainWindow::set_labelProcessingName_text(const QString& text){
    ui->labelProcessingName->setText(text);
}

void MainWindow::set_labelProgress_text(const QString& text){
    ui->labelProgress->setText(text);
}
void MainWindow::set_progressBar_val(int val){
    ui->progressBar->setValue(val);
}

void MainWindow::handleResults(){
    workerThread.quit();
    workerThread.wait();
    ui->pushButtonPause->setEnabled(false);
    ui->pushButtonContinue->setEnabled(false);
    ui->labelProcessingName->setText("已完成");
}

