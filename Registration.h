#pragma once
#ifndef REGISTRATOR
#define REGISTRATOR

#include <iostream>
#include <windows.h>
#include <ImageHlp.h>
#include <filesystem>
#include <math.h>
#include <vector>

#include <opencv2\opencv.hpp>
#include <highgui.hpp>
#include <imgproc.hpp>


using namespace cv;
using std::ifstream;
using std::ofstream;
using std::string;
using std::vector;
namespace fs = std::filesystem;


extern vector<vector<Point>> pts, rectPts;//存放交点
extern Mat ref_win, src_win;

namespace Registration
{
	//完整的配准流程
	template<typename T>
	void Registrating(const string& srcPattern, const string& binPattern, const string& dstPattern, const char useSemiAuto, const char useFailedImg,
		bool (*handle_pause)(T* worker), void (*send_process)(const string processingName, const int processingNum, const int allNum, T* worker), T* worker = nullptr, const int startPos = 0);


	void MkdirAndImwrite(const String& filename, const InputArray& img);

	//PointSort
	bool SetSortRuleX(const Point p1, const Point p2);
	//对x方向上的点位进行排序
	bool SetSortRuleY(const Point p1, const Point p2);
	//用于从上到下从左到右排序
	int PointSortL2R(vector<Point>& pts);

	//对点进行顺时针排序，保证能正确绘制矩形
	void ClockwiseSortPoints(vector<Point>& pts);
	void ClockwiseSortPoints(Point2f pts[], const int ptsSize);
	double KAngle(const Point& a, const Point& center);

	//输入一个数组，返回他是否有4个不同顶点,后两个参数分别是两个点被判断为至少需要有的x，y轴距离 
	bool Has4Points(vector<Point>& pts, const int th_x, const int th_y);
	bool Has4Points(Point2f pts[], const int ptsSize, const int th_x, const int th_y);

	//intoPoly
	int RemoveSmall(Mat& src, Mat& dst);
	void SolvEqu(const double a, const double b, const double c, const double d, const double e, const double f, double& x, double& y);
	//默认static void CntPoint(Mat& srcImage, Mat& dstImage, vector<vector<Point>>& pts, double rho_h = 10, double theta_h = CV_PI / 240, double th_h = 1400);
	//static void CntPoint(Mat& srcImage, Mat& dstImage, vector<vector<Point>>& pts, double rho_h = 12, double theta_h = CV_PI / 240, double th_h = 1000);

	void CntPoint(Mat& srcImage, Mat& dstImage, vector<vector<Point>>& pts, const double rho_h = 1, const double theta_h = CV_PI / 360, const double th_h = 450, const double srn = 0, const double stn = 0);
	void IntoPoly(Mat& dstImage, vector<vector<Point>>& pts);

	//处理文件路径
	void GetFilePath(vector<String>& srcNames, const string& srcPattern, const char useFailedImg = 'n');

	//Registrating
	//默认bool Registrating1Pic(Mat& M, String FnSrc = "005.jpg", String fnImg1 = "1.png", String FnFi = "fi.png", bool flagM = false, bool succeed = false, String FnMid1 = "0.png", String FnMid2 = "01.png", String fnRefImg = "refImg.png");
	bool Registrating1Pic(Mat& M, const String FnSrc = "005.jpg", const String fnBinImg = "1.png", const String FnFi = "fi.png", const bool flagM = false, const bool succeed = false, const String FnMid1 = "srcLines.png", const String FnMid2 = "refLines.png", const String fnRefImg = "refImg.png");
	void RenameRoot(const String srcPattern);
	void on_mouse(int event, int x, int y, int flags, void* ustc);
	bool Registrating1PicSemi(Mat& M, const String FnSrc = "005.jpg", const String fnBinImg = "1.png", const String FnFi = "fi.png", const bool flagM = false, const bool succeed = false, const String FnMid1 = "srcLines.png", const String FnMid2 = "refLines.png", const String fnRefImg = "refImg.png");

};


//完成整个配准过程
template<typename T>
void Registration::Registrating(const string& srcPattern, const string& binPattern, const string& dstPattern, const char useSemiAuto, const char useFailedImg,
	bool (*handle_pause)(T* worker), void (*send_process)(const string processingName, const int processingNum, const int allNum, T* worker), T* worker, const int startPos) {
	//qDebug() << "12311111111111111111111111111111111111111111111";
	vector<String> srcNames;//保存图名
	//String srcPattern = "E:\\root\\";
	//String srcPattern = "srcImg";
	//char useSemiAuto = 'y';//如果为y则使用半自动配准
	//std::cout << "是否使用半自动配准？[y/n]" << std::endl;
	//cin >> useSemiAuto;
	//char useFailedImg = 'y';//如果为y则使用failedImg.txt作为文件列表
	//std::cout << "是否使用上次失败的文件列表？[y/n]" << std::endl;
	//cin >> useFailedImg;

	////得到源文件目录
	//if (useFailedImg == 'y') {
	//	ifstream failedImg("failedImg.txt");
	//	string line;
	//	while (getline(failedImg, line)) {
	//		srcNames.push_back(line);
	//	}
	//}
	//else {
	//	renameRoot(srcPattern);
	//	glob(srcPattern, srcNames, true);
	//}
	GetFilePath(srcNames, srcPattern, useFailedImg);


	//仅自动配准且从头开始时用于清空失败的图片列表
	if (useSemiAuto != 'y' || startPos == 0) {
		ofstream failedImg("failedImg.txt");
		failedImg.close();
	}
	String binName, dstName;
	Mat M;//变换矩阵
	fs::path fnPath;

	for (int i = startPos; i < srcNames.size(); ++i) {
		//若暂停了就退出，此时进度上显示的序号正好是下一个文件名的下标
		//if (this->isPaused()) {
		//	//emit set_pause();
		//	emit HandlePause();
		//	return;
		//}
		if (handle_pause(worker)) return;


		//emit SendProcess(QString::fromStdString(srcNames[i]), i, srcNames.size());
		send_process(srcNames[i], i, srcNames.size(), worker);


		bool flagM = true;
		int startChar = srcPattern.size() + 1;//除去floderPattern外的第一个字符下标
		String fn(srcNames[i], startChar, srcNames[i].size() - startChar);//从startChar开始,得到相对于srcNames[i]的路径
		fnPath = fn.c_str();
		//获得后缀如jpg,png
		String fnTail(fnPath.extension().string());
		//去掉fn的后缀名如.jpg
		fn = fnPath.parent_path().string() + "\\" + fnPath.stem().string();
		if (fnTail != ".png" && fnTail != ".jpg") continue;
		//用于分割后2个根的路径(短，如052_1.png
		if (fn[fn.size() - 2] == '-' || fn[fn.size() - 2] == '_') {
			binName = binPattern + "\\" + String(fn, 0, fn.size() - 2) + ".png";
		}
		//用于分割后2个根的路径(长，如001_Right_F0_01.png
		else if ((fn[fn.size() - 3] == '-' || fn[fn.size() - 3] == '_')) {
			if (fn[fn.size() - 5] != 'F' && fn[fn.size() - 5] != 'R') continue;
			binName = binPattern + "\\" + String(fn, 0, fn.size() - 12) + ".png";
		}
		//一般原图路径如052.png
		else {
			binName = binPattern + "\\" + fn + ".png";
			flagM = false;
			std::cout << "原图" << std::endl;
		}


		dstName = dstPattern + "\\" + fn + ".png";



		if (fnTail != ".png" && fnTail != ".jpg") continue;

		if (useSemiAuto != 'y') {
			if (!Registrating1Pic(M, srcNames[i], binName, dstName, flagM))//失败
				Registrating1Pic(M, srcNames[i], binName, dstName, flagM, true);
		}
		else {
			if (!Registrating1PicSemi(M, srcNames[i], binName, dstName, flagM))//失败
				Registrating1PicSemi(M, srcNames[i], binName, dstName, flagM, true);
		}

		pts[0].clear(), rectPts[0].clear();


	}
	//仅在全自动程序成功运行结束时把这次的失败列表备份
	if (useSemiAuto != 'y')
		CopyFile(L"failedImg.txt", L"failedImg.txt.bak", FALSE);
	// 先更新进度到完成再发送结束信号
	//emit SendProcess(QString::fromStdString(srcNames.back()), srcNames.size(), srcNames.size());
	send_process(srcNames.back(), srcNames.size(), srcNames.size(), worker);

}

#endif // !REGISTRATOR



