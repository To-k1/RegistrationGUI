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
#include <QMainWindow>
#include <QFileDialog>
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

class MainWindow;




class Worker:public QObject
{
    Q_OBJECT
public:
    Worker(QObject* parent = nullptr):is_paused(false){}
    inline bool isPaused(){return is_paused;}
    inline void worker_pause(){this->is_paused = true;}
    inline void worker_resume(){this->is_paused = false;}
public slots:
    // doWork定义了线程要执行的操作
    void Registrating(String& srcPattern, String& binPattern, String& dstPattern, char useSemiAuto, char useFailedImg, MainWindow* win, int startPos = 0);
    //暂停和恢复
// 线程完成工作时发送的信号
signals:
    void resultReady();
    void set_labelProcessingName_text(const QString& text);
    void set_labelProgress_text(const QString& text);
    void set_progressBar_val(int val);
    void set_pause();
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

    //intoPoly
    static int RemoveSmall(Mat& src, Mat& dst);
    static void SolvEqu(double a, double b, double c, double d, double e, double f, double& x, double& y);
    //默认void CntPoint(Mat& srcImage, Mat& dstImage, vector<vector<Point>>& pts, double rho_h = 10, double theta_h = CV_PI / 240, double th_h = 1400);
    static void CntPoint(Mat& srcImage, Mat& dstImage, vector<vector<Point>>& pts, double rho_h = 12, double theta_h = CV_PI / 240, double th_h = 1000);
    static void intoPoly(Mat& dstImage, vector<vector<Point>>& pts);

    //Registrating
    //默认bool Registrating1Pic(Mat& M, String FnSrc = "005.jpg", String fnImg1 = "1.png", String FnFi = "fi.png", bool flagM = false, bool succeed = false, String FnMid1 = "0.png", String FnMid2 = "01.png", String fnRefImg = "1_1_1.png");
    static bool Registrating1Pic(Mat& M, String FnSrc = "005.jpg", String fnImg1 = "1.png", String FnFi = "fi.png", bool flagM = false, bool succeed = false, String FnMid1 = "0.png", String FnMid2 = "01.png", String fnRefImg = "1_1_1.png");
    static void renameRoot(String srcPattern);
    static void on_mouse(int event, int x, int y, int flags, void* ustc);
    static bool Registrating1PicSemi(Mat& M, String FnSrc = "005.jpg", String fnImg1 = "1.png", String FnFi = "fi.png", bool flagM = false, bool succeed = false, String FnMid1 = "0.png", String FnMid2 = "01.png", String fnRefImg = "1_1_1.png");

    bool is_paused;
};





#endif // !REGISTRATING
