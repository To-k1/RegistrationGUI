#include "Registrating.h"


vector<vector<Point>> pts(1), rectPts(1);//存放交点
Mat ref_win, src_win;





//若有目录就保存，没有目录就创建并保存

void Registrator::mkdirAndImwrite(const String& filename, InputArray& img) {
	fs::path fp(filename.c_str());
	fp = fp.parent_path();
	if (!fp.string().empty())
		fs::create_directories(fp);
	imwrite(filename, img);
}


//以下为点的顺时针排序过程
//以下为点的顺时针排序过程
//以下为点的顺时针排序过程

//对y方向上的点位进行排序


bool Registrator::SetSortRuleX(const Point p1, const Point p2)
{
	if (p1.x < p2.x)
		return true;
	else
		return false;
}
//对x方向上的点位进行排序

bool Registrator::SetSortRuleY(const Point p1, const Point p2)
{
	if (p1.y < p2.y)
		return true;
	else
		return false;
}
//用于从上到下从左到右排序

int Registrator::PointSortL2R(vector<Point>& pts)
{
	std::cout << pts << "before:" << std::endl;
	//先按y，再按x排序
	sort(pts.begin(), pts.end(), SetSortRuleY);
	sort(pts.begin(), pts.end(), SetSortRuleX);
	std::cout << pts << "aft:" << std::endl;



	return 0;
}


//计算该点与中心连线的斜率角

double Registrator::KAngle(const Point& a, const Point& center) {
	return atan2((a.y - center.y), (a.x - center.x));
}

//对点进行顺时针排序，保证能正确绘制矩形

void Registrator::ClockwiseSortPoints(vector<Point>& pts)
{
	//计算重心
	Point center;
	double x = 0, y = 0;
	for (int i = 0; i < pts.size(); i++)
	{
		x += pts[i].x;
		y += pts[i].y;
	}
	if (pts.size()) {//防止出现除0的错误
		center.x = (int)x / pts.size();
		center.y = (int)y / pts.size();
	}
	else {
		center.x = 2000;
		center.y = 1500;
	}




	//冒泡排序
	for (int i = 0; i < int(pts.size()) - 1; i++)
	{
		for (int j = 0; j < int(pts.size()) - i - 1; j++)
		{
			if (KAngle(pts[j], center) > KAngle(pts[j + 1], center))
			{
				Point tmp = pts[j];
				pts[j] = pts[j + 1];
				pts[j + 1] = tmp;
			}
		}
	}

}

//重载

void Registrator::ClockwiseSortPoints(Point2f pts[], int ptsSize)
{
	//计算重心
	Point center;
	double x = 0, y = 0;
	for (int i = 0; i < ptsSize; i++)
	{
		x += pts[i].x;
		y += pts[i].y;
	}
	if (ptsSize) {//防止出现除0的错误
		center.x = (int)x / ptsSize;
		center.y = (int)y / ptsSize;
	}
	else {
		center.x = 2000;
		center.y = 1500;
	}




	//冒泡排序
	for (int i = 0; i < int(ptsSize) - 1; i++)
	{
		for (int j = 0; j < int(ptsSize) - i - 1; j++)
		{
			if (KAngle(pts[j], center) > KAngle(pts[j + 1], center))
			{
				Point tmp = pts[j];
				pts[j] = pts[j + 1];
				pts[j + 1] = tmp;
			}
		}
	}

}



//以下为画多边形过程
//以下为画多边形过程
//以下为画多边形过程

//去除小连通区域

int Registrator::RemoveSmall(Mat& src, Mat& dst) {

	inRange(src, Scalar(0, 100, 0), Scalar(255, 255, 255), dst);
	mkdirAndImwrite("original.png", dst);

	// 提取连通区域，并剔除小面积联通区域
	vector<vector<Point>> contours;
	findContours(dst, contours, RETR_LIST, CHAIN_APPROX_NONE);
	contours.erase(remove_if(contours.begin(), contours.end(),
		[](const vector<Point>& c) {return contourArea(c) < 3000000; }), contours.end());

	// 显示图像并保存
	dst.setTo(0);
	drawContours(dst, contours, -1, Scalar(255), FILLED);
	mkdirAndImwrite("remove.png", dst);
	return 0;
}



//给二元一次方程组求解(带过程){ax+by+c=0 dx+ey+f=0}

void Registrator::SolvEqu(double a, double b, double c, double d, double e, double f, double& x, double& y)
{
	y = (double)((-f * 1.0 + d * c * 1.0 / a) * 1.0 / (-d * b * 1.0 / a + e * 1.0));
	x = (double)((-c * 1.0 - b * y * 1.0) * 1.0 / a);
}



//获得直线交点,第4,5,6个参数是HoughLines的rho精度，theta精度和长度阈值

void Registrator::CntPoint(Mat& srcImage, Mat& dstImage, vector<vector<Point>>& pts, double rho_h, double theta_h, double th_h)
{
	//【1】载入原始图和Mat变量定义   
	//Mat srcImage = imread("remove.png");  //工程目录下应该有一张名为remove.png的素材图
	Mat midImage;//临时变量和目标图的定义
	//vector<vector<Point>> pts(1);//存放交点

	//【2】进行边缘检测和转化为灰度图
	Canny(srcImage, midImage, 50, 200, 3);//进行一此canny边缘检测
	cvtColor(midImage, dstImage, COLOR_GRAY2BGR);//转化边缘检测后的图为灰度图

	//【3】进行霍夫线变换
	vector<Vec2f> lines;//定义一个矢量结构lines用于存放得到的线段矢量集合
	HoughLines(midImage, lines, rho_h, theta_h, th_h, 0, 0);

	//【4】依次在图中绘制出每条线段,并且两两求交点
	for (size_t i = 0; i < lines.size(); i++)
	{
		float rho = lines[i][0], theta = lines[i][1];
		Point pt1, pt2;
		double a = cos(theta), b = sin(theta);
		double x0 = a * rho, y0 = b * rho;
		pt1.x = cvRound(x0 + 1000 * (-b));
		pt1.y = cvRound(y0 + 1000 * (a));
		pt2.x = cvRound(x0 - 1000 * (-b));
		pt2.y = cvRound(y0 - 1000 * (a));
		//画线
		line(dstImage, pt1, pt2, Scalar(55, 100, 195), 1, LINE_AA);
		//两两求交点
		for (size_t j = i + 1; j < lines.size(); ++j)
		{
			double rho2 = lines[j][0], theta2 = lines[j][1];
			double c = cos(theta2), d = sin(theta2), x, y;
			SolvEqu(a, b, -rho, c, d, -rho2, x, y);
			if ((0 <= x && x <= 4096 && 0 <= y && y <= 3000) && !((x > 1000 && x < 3000) || (y > 1050 && y < 1950)))
				pts[0].push_back(Point(x, y));
		}
	}

}


void Registrator::intoPoly(Mat& dstImage, vector<vector<Point>>& pts)
{
	//将交点排序并连成多边形并填充
	ClockwiseSortPoints(pts[0]);
	dstImage = Mat::zeros(Size(dstImage.size()), CV_8UC3);
	fillPoly(dstImage, pts, Scalar(255, 255, 255));
	mkdirAndImwrite("1_1.png", dstImage);


}







//以下为配准过程
//以下为配准过程
//以下为配准过程

//从前到后分别为变换矩阵，原图，二值图，结果图，中间图1,2，参照图，flagM表示是否直接用传进来的变换矩阵为1用,succeed表示是否失败过，为1表示失败过要变更参数

bool Registrator::Registrating1Pic(Mat& M, String FnSrc, String fnImg1, String FnFi, bool flagM, bool failed, String FnMid1, String FnMid2, String fnRefImg)
{
	//用于记录失败的图片
	ofstream failedImg; failedImg.open("failedImg.txt", ofstream::app);
	//IMG1:二值图，refimg：参考二值图，src：原图，dst：结果图
	Mat Img1 = imread(fnImg1), refImg = imread(fnRefImg), src = imread(FnSrc), dst(src.size(), CV_8UC3);
	//没有二值图(png)返回
	if (Img1.empty()) {
		return 1;
	}
	resize(refImg, refImg, Size(src.cols, src.rows));
	Mat midImg(src.size(), CV_8UC1);
	//vector<vector<Point>> pts(1), rectPts(1);//存放交点
	Point2f pts2F[4], rectPts2F[4];
	if (flagM && !M.empty()) warpPerspective(src, dst, M, Size(src.cols, src.rows));
	else {

		//1,2
		//去除小区域
		RemoveSmall(Img1, midImg);
		try {
			//获得多边形顶点1（重复3次减少误差默认failed ? CntPoint(midImg, Img1, pts, 15, CV_PI / 240, 800) : CntPoint(midImg, Img1, pts);
			failed ? CntPoint(midImg, Img1, pts, 15, CV_PI / 240, 700) : CntPoint(midImg, Img1, pts);
			std::cout << pts[0] << std::endl;
			//由顶点填充多边形1
			intoPoly(Img1, pts);
		}
		catch (...) {
			//若已经失败过，记录文件名到txt文件方便以后处理
			if (failed)
				failedImg << string(FnSrc.c_str()) << std::endl;
			std::cout << "该图失败" << std::endl;
			failedImg.close();
			//删除失败的结果图，方便下一步处理
			remove(FnFi.c_str());
			return false;
		}


		//3.1获取待配准图的顶点，存入0.png
		pts = vector<vector<Point>>(1);
		//获得填充好的矩形的顶点
		CntPoint(Img1, midImg, pts, 7, CV_PI / 180, 700);
		std::cout << pts[0] << std::endl;
		mkdirAndImwrite(FnMid1, midImg);//中间图1
		if (pts[0].size() < 4 || pts[0].size() > 100) {
			std::cout << "识别不到4个顶点" << std::endl;
			Mat temp = midImg;
			mkdirAndImwrite(FnFi, temp);
			return false;
		}

		//3.1获取基准图顶点，存入01.png
		CntPoint(refImg, midImg, rectPts);
		std::cout << rectPts[0] << std::endl;
		mkdirAndImwrite(FnMid2, midImg);//中间图2

		//顶点排序
		ClockwiseSortPoints(pts[0]); ClockwiseSortPoints(rectPts[0]);
		//删除多余顶点(仅仅将不多余的放入2f)
		for (int i = 1, j = 1; i < pts[0].size(); ++i) {
			//不为多余顶点
			if (abs(pts[0][i - 1].x - pts[0][i].x) > 1000 || abs(pts[0][i - 1].y - pts[0][i].y) > 800)
				pts2F[j++] = pts[0][i];
		}
		pts2F[0] = pts[0][0];
		ClockwiseSortPoints(pts2F, 4);
		for (int i = 0; i < 4; ++i) {
			rectPts2F[i] = rectPts[0][i];
			std::cout << pts2F[i] << std::endl;
			if (pts2F[i] == Point2f(0, 0)) {
				//若已经失败过，记录文件名到txt文件方便以后处理
				if (failed)
					failedImg << string(FnSrc.c_str()) << std::endl;
				std::cout << "该图失败" << std::endl;
				failedImg.close();
				//删除失败的结果图，方便下一步处理
				remove(FnFi.c_str());
				return false;
			}
		}

		M = getPerspectiveTransform(pts2F, rectPts2F);
		src.convertTo(src, CV_32F);
		dst.convertTo(dst, CV_32F);
		warpPerspective(src, dst, M, Size(src.cols, src.rows));
	}
	Mat maskedDst;
	dst.copyTo(maskedDst, refImg);//抠图
	mkdirAndImwrite(FnFi, maskedDst);//最终结果
	failedImg.close();



	return true;
}





void Registrator::renameRoot(String srcPattern) {
	vector<String> srcNames;//保存图名
	glob(srcPattern, srcNames, true);
	for (int i = 0; i < srcNames.size(); ++i) {
		bool flagM = true;
		String fn(srcNames[i], 7, srcNames[i].size() - 11);//从7开始13个
		if (srcNames[i].size() < 23) continue;
		String fnTail(srcNames[i], srcNames[i].size() - 3, 3);//后缀
		//if (fnTail != "png" && fnTail != "jpg") continue;
		//用于分割后2个根的路径(短
		if (fn[fn.size() - 2] == '-') {
			String temp = srcNames[i]; string temp1 = srcNames[i];
			temp1[temp1.size() - 6] = '_';
			srcNames[i] = temp1;
			rename(temp.c_str(), srcNames[i].c_str());
			std::cout << "rename:" << temp.c_str() << " to " << srcNames[i].c_str() << std::endl;
		}
	}
}


//以下为半自动
//鼠标回调函数

void Registrator::on_mouse(int event, int x, int y, int flags, void* ustc) //event鼠标事件代号，x,y鼠标坐标，flags拖拽和键盘操作的代号
{
	if (event == CV_EVENT_LBUTTONDOWN)//左键按下，读取初始坐标，并在图像上该点处打点
	{
		Point  p = Point(x, y);
		circle(src_win, p, 1, Scalar(0, 0, 255), -1);
		imshow("待配准图", src_win);
		pts[0].push_back(p);   //将选中的点存起来
		std::cout << "待配准图: " << p << std::endl;
	}
}


bool Registrator::Registrating1PicSemi(Mat& M, String FnSrc, String fnImg1, String FnFi, bool flagM, bool succeed, String FnMid1, String FnMid2, String fnRefImg)
{
	//IMG1:二值图，refimg：参考二值图，src：原图，dst：结果图
	Mat refImg = imread(fnRefImg), src = imread(FnSrc), dst(src.size(), CV_8UC3);
	resize(refImg, refImg, Size(src.cols, src.rows));
	Mat midImg(src.size(), CV_8UC1);
	src_win = src.clone();

	Point2f pts2F[4], rectPts2F[4];

	if (flagM && !M.empty()) warpPerspective(src, dst, M, Size(src.cols, src.rows));
	else {
		//3.1获取待配准图的顶点，存入0.png
		namedWindow("待配准图", 0);
		resizeWindow("待配准图", src.cols >> 2, src.rows >> 2);
		imshow("待配准图", src_win);
		setMouseCallback("待配准图", on_mouse);
		waitKey();
		destroyWindow("待配准图");

		//3.1获取基准图顶点，存入01.png
		CntPoint(refImg, midImg, rectPts);
		std::cout << rectPts[0] << std::endl;
		//mkdirAndImwrite(FnMid2, midImg);//中间图2

		//顶点排序
		ClockwiseSortPoints(pts[0]); ClockwiseSortPoints(rectPts[0]);
		//删除多余顶点(仅仅将不多余的放入2f)
		for (int i = 1, j = 1; i < pts[0].size(); ++i) {
			//不为多余顶点
			if (abs(pts[0][i - 1].x - pts[0][i].x) > 1000 || abs(pts[0][i - 1].y - pts[0][i].y) > 800)
				pts2F[j++] = pts[0][i];
		}
		pts2F[0] = pts[0][0];
		for (int i = 0; i < 4; ++i) {
			rectPts2F[i] = rectPts[0][i];
			std::cout << pts2F[i] << std::endl;
		}

		M = getPerspectiveTransform(pts2F, rectPts2F);
	}
	src.convertTo(src, CV_32F);
	dst.convertTo(dst, CV_32F);
	warpPerspective(src, dst, M, Size(src.cols, src.rows));
	Mat maskedDst;
	dst.copyTo(maskedDst, refImg);//抠图
	mkdirAndImwrite(FnFi, maskedDst);//最终结果



	return true;
}




//完成整个配准过程

void Registrator::Registrating(String& srcPattern, String& binPattern, String& dstPattern, char useSemiAuto, char useFailedImg, QMainWindow* win, int startPos) {
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
	if (useFailedImg == 'y') {
		ifstream failedImg("failedImg.txt");
		string line;
		while (getline(failedImg, line)) {
			srcNames.push_back(line);
		}
	}
	else {
		renameRoot(srcPattern);
		glob(srcPattern, srcNames, true);
	}
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
		if (this->isPaused()) {
			//emit set_pause();
			emit handlePause();
			return;
		}

		////任务进度
		//string progress(std::to_string(i) + "/" + std::to_string(srcNames.size()));
		////win->set_labelProgress_text(QString::fromStdString(progress));
		//emit set_labelProgress_text(QString::fromStdString(progress));
		////显示百分比进度
		////win->set_progressBar_val(int(100*double(i+1)/srcNames.size()));
		//emit set_progressBar_val(int(100 * double(i) / srcNames.size()));
		////输出进度
		////std::cout << srcNames[i] << std::endl; std::cout << binName << std::endl; std::cout << dstName << std::endl;
		////win->set_labelProcessingName_text(QString::fromStdString(srcNames[i]));
		//emit set_labelProcessingName_text(QString::fromStdString(srcNames[i]));
		emit sendProcess(QString::fromStdString(srcNames[i]), i, srcNames.size());




		bool flagM = true;
		int startChar = srcPattern.size() + 1;//除去floderPattern外的第一个字符下标
		String fn(srcNames[i], startChar, srcNames[i].size() - startChar);//从startChar开始,得到相对于srcNames[i]的路径
		fnPath = fn.c_str();
		//获得后缀如jpg,png
		//String fnTail(srcNames[i], srcNames[i].size() - 4, 4);
		String fnTail(fnPath.extension().string());
		//去掉fn的后缀名如.jpg
		fn = fnPath.parent_path().string() + "\\" + fnPath.stem().string();
		//if (srcNames[i].size() < 23) continue;
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


		//dstName = String("F:\\RootImg\\") + fn + ".png";
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
	//// 先更新进度到完成再发送结束信号
	//string progress(std::to_string(srcNames.size()) + "/" + std::to_string(srcNames.size()));
	////win->set_labelProgress_text(QString::fromStdString(progress));
	//emit set_labelProgress_text(QString::fromStdString(progress));
	////显示百分比进度
	////win->set_progressBar_val(int(100*double(i+1)/srcNames.size()));
	//emit set_progressBar_val(100);
	//emit resultReady();
	emit sendProcess(QString::fromStdString(srcNames.back()), srcNames.size(), srcNames.size());

}




