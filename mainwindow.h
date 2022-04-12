#ifndef MAINWINDOW_H
#define MAINWINDOW_H



#include <QMainWindow>
#include <QFileDialog>
#include <QString>
#include <QObject>
#include <QThread>
#include <opencv2/opencv.hpp>
#include "Registrating.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

using namespace cv;

class MainWindow;



class MainWindow : public QMainWindow
{
	Q_OBJECT
		QThread workerThread;
public:
	MainWindow(QWidget* parent = nullptr);
	~MainWindow();
	inline char getSemi() { return semiauto; }
	inline char getFailed() { return failed_list; }

signals:
	void operate(const string& srcPattern, const string& binPattern, const string& dstPattern, const char useSemiAuto, const char useFailedImg, const int startPos = 0);
	//void operate(cv::String& srcPattern, cv::String& binPattern, cv::String& dstPattern, char useSemiAuto, char useFailedImg, int startPos = 0);

public slots:
	void handleResults();
	void handlePause();
	//void set_labelProcessingName_text(const QString& text);
	//void set_labelProgress_text(const QString& text);
	//void set_progressBar_val(int val);
	void sendProcess(const QString processingName, const int processingNum, const int allNum);

private slots:
	void on_pushButtonSrc_clicked();

	void on_pushButtonBin_clicked();

	void on_pushButtonDst_clicked();

	void on_checkBoxSemiAuto_stateChanged(int arg1);

	void on_checkBoxFailedList_stateChanged(int arg1);

	void on_pushButtonRun_clicked();

	void on_pushButtonPause_clicked();

	void on_pushButtonResume_clicked();

private:
	Ui::MainWindow* ui;
	//分别决定是否使用手动配准和失败文件列表，默认为'n'
	char semiauto;
	char failed_list;
	Registrator* worker;
};

#endif // MAINWINDOW_H


