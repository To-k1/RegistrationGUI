#include "mainwindow.h"
#include "ui_mainwindow.h"



MainWindow::MainWindow(QWidget* parent)
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
	string filePattern(ui->lineEditSrc->text().toStdString());
	string binImg(ui->lineEditBin->text().toStdString());
	string dstImg(ui->lineEditDst->text().toStdString());
	//将Registrator初始化到线程workerThread上
	worker = new Registrator();
	worker->moveToThread(&workerThread);

	//用于传递程序进度信息
	connect(worker, &Registrator::SendProcess, this, &MainWindow::SendProcess);
	//connect(worker, &Registrator::set_labelProcessingName_text, this, &MainWindow::set_labelProcessingName_text);
	//connect(worker, &Registrator::set_labelProgress_text, this, &MainWindow::set_labelProgress_text);
	//connect(worker, &Registrator::set_progressBar_val, this, &MainWindow::set_progressBar_val);
	//用于暂停和恢复
	connect(worker, &Registrator::HandlePause, this, &MainWindow::HandlePause);
	//operate信号发射后启动线程工作
	connect(this, &MainWindow::operate, worker, &Registrator::Registrating);
	//线程结束后发送信号，对结果进行处理
	connect(worker, &Registrator::HandleResults, this, &MainWindow::HandleResults);
	//按钮变灰和激活
	ui->pushButtonRun->setEnabled(false);
	ui->pushButtonPause->setEnabled(true);
	ui->checkBoxFailedList->setEnabled(false);
	ui->checkBoxSemiAuto->setEnabled(false);
	ui->lineEditSrc->setEnabled(false);
	ui->lineEditBin->setEnabled(false);
	ui->lineEditDst->setEnabled(false);
	//启动线程
	workerThread.start();
	//发射进程运行信号
	emit operate(filePattern, binImg, dstImg, this->getSemi(), this->getFailed());
}

void MainWindow::on_pushButtonPause_clicked()
{
	ui->pushButtonPause->setEnabled(false);
	worker->worker_pause();
}


void MainWindow::on_pushButtonResume_clicked()
{
	ui->pushButtonResume->setEnabled(false);

	string filePattern(ui->lineEditSrc->text().toStdString());
	string binImg(ui->lineEditBin->text().toStdString());
	string dstImg(ui->lineEditDst->text().toStdString());
	string tmp(ui->labelProgress->text().toStdString());
	int i = 0;
	if (!tmp.empty()) {
		//找到分割点
		for (i = 0; i < tmp.size() && tmp[i] != '/'; ++i);
	}
	worker->worker_resume();
	//重新启动线程
	workerThread.start();
	//发射进程运行信号,从上次终止的点开始,最后一个参数就是上次运行到的断点
	emit operate(filePattern, binImg, dstImg, this->getSemi(), this->getFailed(), std::stoi(string(tmp, 0, i)));
	ui->pushButtonPause->setEnabled(true);
}

//自定义槽函数

//void MainWindow::set_labelProcessingName_text(const QString& text) {
//	ui->labelProcessingName->setText(text);
//}
//
//void MainWindow::set_labelProgress_text(const QString& text) {
//	ui->labelProgress->setText(text);
//}
//void MainWindow::set_progressBar_val(int val) {
//	ui->progressBar->setValue(val);
//}

void MainWindow::HandleResults() {
	workerThread.quit();
	workerThread.wait();
	ui->pushButtonPause->setEnabled(false);
	ui->pushButtonResume->setEnabled(false);
	ui->labelProcessingName->setText("已完成");
}

void MainWindow::HandlePause() {
	workerThread.quit();
	workerThread.wait();
	ui->pushButtonResume->setEnabled(true);
	ui->labelProcessingName->setText(QString::fromStdString("已暂停"));
}

void MainWindow::SendProcess(const QString processingName, const int processingNum, const int allNum) {
	//任务进度
	string progress(std::to_string(processingNum) + "/" + std::to_string(allNum));
	ui->labelProgress->setText(QString::fromStdString(progress));
	//显示百分比进度
	ui->progressBar->setValue(int(100 * double(processingNum) / allNum));
	//输出进度
	ui->labelProcessingName->setText(processingName);
	if (processingNum == allNum) HandleResults();
}


