#pragma once
#ifndef REGISTRATING
#define REGISTRATING

//#include <iostream>
//#include <windows.h>
//#include <ImageHlp.h>
//#include <filesystem>
//#include <math.h>
//#include <vector>
//
//#include <opencv2\opencv.hpp>
//#include <highgui.hpp>
//#include <imgproc.hpp>

#include <QString>
#include <QObject>
#include <QThread>

#include "Registration.h"

//using namespace cv;
//using std::ifstream;
//using std::ofstream;
//using std::string;
//using std::vector;
//namespace fs = std::filesystem;
//
//
//extern vector<vector<Point>> pts, rectPts;//存放交点
//extern Mat ref_win, src_win;



class Registrator : public QObject
{
	Q_OBJECT
public:
	//可指定一个线程用于运行
	Registrator() : is_paused(false) {}
	Registrator(const Registrator& src) : is_paused(src.is_paused) {}
	~Registrator() {}
	inline bool isPaused() { return is_paused; }
	inline void worker_pause() { this->is_paused = true; }
	inline void worker_resume() { this->is_paused = false; }
public slots:
	//参数分别为源文件父目录，根箱二值图父目录，目标文件存储目录，是否使用半自动[y/n]，是否使用失败文件列表[y/n]，从第几个文件开始(默认为0，用于暂停)
	void Registrating(const string& srcPattern, const string& binPattern, const string& dstPattern, const char useSemiAuto, const char useFailedImg, const int startPos = 0);
	void GetRectVertices(const string& srcPattern, const string& binPattern, const int startPos = 0);
signals:
	void HandleResults();
	void HandlePause();
	void SendProcess(const QString processingName, const int processingNum, const int allNum);

private:
	static bool handle_pause(Registrator* worker = nullptr);
	static void send_process(const string processingName, const int processingNum, const int allNum, Registrator* worker = nullptr);
	bool is_paused;
};








#endif // !REGISTRATING
