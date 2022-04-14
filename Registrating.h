#pragma once
#ifndef REGISTRATING
#define REGISTRATING

#include <iostream>
#include <windows.h>
#include <ImageHlp.h>
#include <filesystem>
#include <math.h>
#include <opencv2\opencv.hpp>
#include <highgui.hpp>
#include <imgproc.hpp>
#include <QString>
#include <QObject>
#include <QThread>
#include <vector>

using namespace cv;
using std::ifstream;
using std::ofstream;
using std::string;
using std::vector;
namespace fs = std::filesystem;


extern vector<vector<Point>> pts, rectPts;//存放交点
extern Mat ref_win, src_win;




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
	void registrating(const string& srcPattern, const string& binPattern, const string& dstPattern, const char useSemiAuto, const char useFailedImg, const int startPos = 0);
signals:
	void handleResults();
	void handlePause();
	void sendProcess(const QString processingName, const int processingNum, const int allNum);

private:
	static void mkdirAndImwrite(const String& filename, InputArray& img);

	//PointSort
	static bool SetSortRuleX(const Point p1, const Point p2);
	//对x方向上的点位进行排序
	static bool SetSortRuleY(const Point p1, const Point p2);
	//用于从上到下从左到右排序
	static int PointSortL2R(vector<Point>& pts);

	//对点进行顺时针排序，保证能正确绘制矩形
	static void ClockwiseSortPoints(vector<Point>& pts);
	static void ClockwiseSortPoints(Point2f pts[], int ptsSize);
	static double KAngle(const Point& a, const Point& center);

	//输入一个数组，返回他是否有4个不同顶点,后两个参数分别是两个点被判断为至少需要有的x，y轴距离
	static bool has4Points(vector<Point>& pts, int th_x, int th_y);
	static bool has4Points(Point2f pts[], int ptsSize, int th_x, int th_y);

	//intoPoly
	static int RemoveSmall(Mat& src, Mat& dst);
	static void SolvEqu(double a, double b, double c, double d, double e, double f, double& x, double& y);
	//默认static void CntPoint(Mat& srcImage, Mat& dstImage, vector<vector<Point>>& pts, double rho_h = 10, double theta_h = CV_PI / 240, double th_h = 1400);
	//static void CntPoint(Mat& srcImage, Mat& dstImage, vector<vector<Point>>& pts, double rho_h = 12, double theta_h = CV_PI / 240, double th_h = 1000);

	static void CntPoint(Mat& srcImage, Mat& dstImage, vector<vector<Point>>& pts, double rho_h = 1, double theta_h = CV_PI / 360, double th_h = 450, double srn = 0, double stn = 0);
	static void intoPoly(Mat& dstImage, vector<vector<Point>>& pts);

	//Registrating
	//默认bool Registrating1Pic(Mat& M, String FnSrc = "005.jpg", String fnImg1 = "1.png", String FnFi = "fi.png", bool flagM = false, bool succeed = false, String FnMid1 = "0.png", String FnMid2 = "01.png", String fnRefImg = "refImg.png");
	static bool Registrating1Pic(Mat& M, String FnSrc = "005.jpg", String fnBinImg = "1.png", String FnFi = "fi.png", bool flagM = false, bool succeed = false, String FnMid1 = "srcLines.png", String FnMid2 = "refLines.png", String fnRefImg = "refImg.png");
	static void renameRoot(String srcPattern);
	static void on_mouse(int event, int x, int y, int flags, void* ustc);
	static bool Registrating1PicSemi(Mat& M, String FnSrc = "005.jpg", String fnBinImg = "1.png", String FnFi = "fi.png", bool flagM = false, bool succeed = false, String FnMid1 = "srcLines.png", String FnMid2 = "refLines.png", String fnRefImg = "refImg.png");

	bool is_paused;
};





#endif // !REGISTRATING
