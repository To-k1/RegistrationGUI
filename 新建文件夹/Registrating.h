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
#include <vector>
#include "mainwindow.h"
using namespace cv;
using std::ifstream;
using std::ofstream;
using std::string;
using std::vector;
namespace fs = std::filesystem;






void mkdirAndImwrite(const String& filename, InputArray& img);

//PointSort
bool SetSortRuleX(const Point p1, const Point p2);
//对x方向上的点位进行排序
bool SetSortRuleY(const Point p1, const Point p2);
//用于从上到下从左到右排序
int PointSortL2R(vector<Point>& pts);

//对点进行顺时针排序，保证能正确绘制矩形
void ClockwiseSortPoints(vector<Point>& pts);
void ClockwiseSortPoints(Point2f pts[], int ptsSize);
double KAngle(const Point& a, const Point& center);

//intoPoly
int RemoveSmall(Mat& src, Mat& dst);
void SolvEqu(double a, double b, double c, double d, double e, double f, double& x, double& y);
//默认void CntPoint(Mat& srcImage, Mat& dstImage, vector<vector<Point>>& pts, double rho_h = 10, double theta_h = CV_PI / 240, double th_h = 1400);
void CntPoint(Mat& srcImage, Mat& dstImage, vector<vector<Point>>& pts, double rho_h = 12, double theta_h = CV_PI / 240, double th_h = 1000);
void intoPoly(Mat& dstImage, vector<vector<Point>>& pts);

//Registrating
//默认bool Registrating1Pic(Mat& M, String FnSrc = "005.jpg", String fnImg1 = "1.png", String FnFi = "fi.png", bool flagM = false, bool succeed = false, String FnMid1 = "0.png", String FnMid2 = "01.png", String fnRefImg = "1_1_1.png");
bool Registrating1Pic(Mat& M, String FnSrc = "005.jpg", String fnImg1 = "1.png", String FnFi = "fi.png", bool flagM = false, bool succeed = false, String FnMid1 = "0.png", String FnMid2 = "01.png", String fnRefImg = "1_1_1.png");
void renameRoot(String srcPattern);
void on_mouse(int event, int x, int y, int flags, void* ustc);
bool Registrating1PicSemi(Mat& M, String FnSrc = "005.jpg", String fnImg1 = "1.png", String FnFi = "fi.png", bool flagM = false, bool succeed = false, String FnMid1 = "0.png", String FnMid2 = "01.png", String fnRefImg = "1_1_1.png");
void Registrating(String& srcPattern, String& binPattern, String& dstPattern, char useSemiAuto, char useFailedImg, MainWindow* win, int startPos = 0);

#endif // !REGISTRATING
