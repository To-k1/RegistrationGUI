#include "Registration.h"


vector<vector<Point>> pts(1), rectPts(1);//存放交点
Mat ref_win, src_win;


//若有目录就保存，没有目录就创建并保存

void Registration::MkdirAndImwrite(const String& filename, const InputArray& img) {
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


bool Registration::SetSortRuleX(const Point p1, const Point p2)
{
	if (p1.x < p2.x)
		return true;
	else
		return false;
}
//对x方向上的点位进行排序

bool Registration::SetSortRuleY(const Point p1, const Point p2)
{
	if (p1.y < p2.y)
		return true;
	else
		return false;
}
//用于从上到下从左到右排序

int Registration::PointSortL2R(vector<Point>& pts)
{
	std::cout << pts << "before:" << std::endl;
	//先按y，再按x排序
	sort(pts.begin(), pts.end(), SetSortRuleY);
	sort(pts.begin(), pts.end(), SetSortRuleX);
	std::cout << pts << "aft:" << std::endl;



	return 0;
}


//计算该点与中心连线的斜率角

double Registration::KAngle(const Point& a, const Point& center) {
	return atan2((a.y - center.y), (a.x - center.x));
}

//对点进行顺时针排序，保证能正确绘制矩形

void Registration::ClockwiseSortPoints(vector<Point>& pts)
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

void Registration::ClockwiseSortPoints(Point2f pts[], const int ptsSize)
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


//输入一个数组，返回他是否有4个不同顶点
bool Registration::Has4Points(vector<Point>& pts, const int th_x, const int th_y) {
	//用来表示有多少个点
	int pointNum = pts.size();
	ClockwiseSortPoints(pts);
	for (int i = 1; i < pts.size(); ++i) {
		//有两点挨在一起,pointNum-1
		if (abs(pts[i - 1].x - pts[i].x) < th_x && abs(pts[i - 1].y - pts[i].y) < th_y)
			--pointNum;
	}
	if (pointNum >= 4) return true;
	return false;
}
bool Registration::Has4Points(Point2f pts[], const int ptsSize, const int th_x, const int th_y) {
	//用来表示有多少个点
	int pointNum = ptsSize;
	ClockwiseSortPoints(pts, ptsSize);
	for (int i = 1; i < ptsSize; ++i) {
		//有两点挨在一起,pointNum-1
		if (abs(pts[i - 1].x - pts[i].x) < th_x && abs(pts[i - 1].y - pts[i].y) < th_y)
			--pointNum;
	}
	if (pointNum >= 4) return true;
	return false;
}




//以下为画多边形过程
//以下为画多边形过程
//以下为画多边形过程

//去除小连通区域

int Registration::RemoveSmall(Mat& src, Mat& dst) {

	//将二值图转化为黑白存储到dst中
	inRange(src, Scalar(0, 100, 0), Scalar(255, 255, 255), dst);
	MkdirAndImwrite("original.png", dst);

	// 提取连通区域，并剔除小面积联通区域
	vector<vector<Point>> contours;
	findContours(dst, contours, RETR_LIST, CHAIN_APPROX_NONE);
	//图片总面积(像素)
	int areaSize = src.cols * src.rows;
	//小于图片四分之一的视为小区域
	contours.erase(remove_if(contours.begin(), contours.end(),
		[areaSize](const vector<Point>& c) {return contourArea(c) < areaSize / 4; }), contours.end());

	// 显示图像并保存
	dst.setTo(0);
	drawContours(dst, contours, -1, Scalar(255), FILLED);
	MkdirAndImwrite("remove.png", dst);
	return 0;
}



//给二元一次方程组求解(带过程){ax+by+c=0 dx+ey+f=0}

void Registration::SolvEqu(const double a, const double b, const double c, const double d, const double e, const double f, double& x, double& y)
{
	y = (double)((-f * 1.0 + d * c * 1.0 / a) * 1.0 / (-d * b * 1.0 / a + e * 1.0));
	x = (double)((-c * 1.0 - b * y * 1.0) * 1.0 / a);
}



//获得直线交点,第4,5,6个参数是HoughLines的rho精度，theta精度和长度阈值

void Registration::CntPoint(Mat& srcImage, Mat& dstImage, vector<vector<Point>>& pts, const double rho_h, const double theta_h, const double th_h, const double srn, const double stn)
{
	//【1】载入原始图和Mat变量定义   
	//Mat srcImage = imread("remove.png");  //工程目录下应该有一张名为remove.png的素材图
	Mat midImage;//临时变量和目标图的定义
	//vector<vector<Point>> pts(1);//存放交点

	pts[0].clear();

	//【2】进行边缘检测和转化为灰度图
	Canny(srcImage, midImage, 50, 200, 3);//进行一此canny边缘检测
	cvtColor(midImage, dstImage, COLOR_GRAY2BGR);//转化边缘检测后的图为灰度图
	//存储Canny边缘检测结果的中间图
	MkdirAndImwrite("cannyEdge.png", midImage);

	//【3】进行霍夫线变换
	vector<Vec2f> lines;//定义一个矢量结构lines用于存放得到的线段矢量集合
	HoughLines(midImage, lines, rho_h, theta_h, th_h, srn, stn);

	//【4】依次在图中绘制出每条线段,并且两两求交点
	for (size_t i = 0; i < lines.size(); i++)
	{
		float rho = lines[i][0], theta = lines[i][1];
		Point pt1, pt2;
		double a = cos(theta), b = sin(theta);
		double x0 = a * rho, y0 = b * rho;
		pt1.x = cvRound(x0 + 10000 * (-b));
		pt1.y = cvRound(y0 + 10000 * (a));
		pt2.x = cvRound(x0 - 10000 * (-b));
		pt2.y = cvRound(y0 - 10000 * (a));
		//画线
		line(dstImage, pt1, pt2, Scalar(0, 255, 255), 3, LINE_AA);
		//两两求交点
		for (size_t j = i + 1; j < lines.size(); ++j)
		{
			double rho2 = lines[j][0], theta2 = lines[j][1];
			double c = cos(theta2), d = sin(theta2), x, y;
			SolvEqu(a, b, -rho, c, d, -rho2, x, y);
			if ((0 <= x && x <= srcImage.cols && 0 <= y && y <= srcImage.rows) && !((x > (srcImage.cols / 4) && x < (srcImage.cols / 6 * 5)) || (y > (srcImage.rows / 3) && y < (srcImage.rows / 3 * 2))))
				pts[0].push_back(Point(x, y));
		}
	}

}


void Registration::IntoPoly(Mat& dstImage, vector<vector<Point>>& pts)
{
	//将交点排序并连成多边形并填充
	ClockwiseSortPoints(pts[0]);
	dstImage = Mat::zeros(Size(dstImage.size()), CV_8UC3);
	fillPoly(dstImage, pts, Scalar(255, 255, 255));
	MkdirAndImwrite("srcPloy.png", dstImage);

}

void Registration::IntoPoly(Mat& dstImage, vector<Point>& pts)
{
	vector<vector<Point>> ptss(1); ptss[0] = pts;
	IntoPoly(dstImage, ptss);
}








//以下为配准过程
//以下为配准过程
//以下为配准过程

//从前到后分别为变换矩阵，原图，二值图，结果图，中间图1,2，参照图，flagM表示是否直接用传进来的变换矩阵为1用,succeed表示是否失败过，为1表示失败过要变更参数

bool Registration::Registrating1Pic(Mat& M, const String FnSrc, const String fnBinImg, const String FnFi, const bool flagM, const bool failed, const String FnMid1, const String FnMid2, const String fnRefImg)
{
	//用于记录失败的图片
	ofstream failedImg; failedImg.open("failedImg.txt", ofstream::app);
	//IMG1:二值图，refimg：参考二值图，src：原图，dst：结果图
	Mat binImg = imread(fnBinImg), refImg = imread(fnRefImg), src = imread(FnSrc), dst(src.size(), CV_8UC3);
	//没有二值图(png)返回
	if (binImg.empty()) {
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
		RemoveSmall(binImg, midImg);
		try {
			//获得多边形顶点1（重复3次减少误差默认failed ? CntPoint(midImg, Img1, pts, 15, CV_PI / 240, 800) : CntPoint(midImg, Img1, pts);
			failed ? CntPoint(midImg, binImg, pts, 12, CV_PI / 180, src.cols * 1000 / 4096) : CntPoint(midImg, binImg, pts, 1, CV_PI / 360, src.cols * 450 / 4096);
			MkdirAndImwrite("srcLines0.png", binImg);//存储霍夫变换直线识别情况
			std::cout << pts[0] << std::endl;
			//由顶点填充多边形1
			IntoPoly(binImg, pts);
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
		//CntPoint(binImg, midImg, pts, 7, CV_PI / 180, 700);
		CntPoint(binImg, midImg, pts, 1, CV_PI / 360, src.cols * 450 / 4096);
		if (!Has4Points(pts[0], src.cols / 4, src.rows / 4))
			CntPoint(binImg, midImg, pts, 1, CV_PI / 360, src.cols * 350 / 4096);
		if (!Has4Points(pts[0], src.cols / 4, src.rows / 4))
			CntPoint(binImg, midImg, pts, 1, CV_PI / 360, src.cols * 250 / 4096);
		if (!Has4Points(pts[0], src.cols / 4, src.rows / 4))
			CntPoint(binImg, midImg, pts, 7, CV_PI / 180, src.cols * 700 / 4096);
		std::cout << pts[0] << std::endl;
		MkdirAndImwrite(FnMid1, midImg);//中间图1
		if (pts[0].size() < 4 || pts[0].size() > 100) {
			std::cout << "识别不到4个顶点" << std::endl;
			Mat temp = midImg;
			MkdirAndImwrite(FnFi, temp);
			return false;
		}

		//3.1获取基准图顶点，存入01.png
		CntPoint(refImg, midImg, rectPts);
		std::cout << rectPts[0] << std::endl;
		MkdirAndImwrite(FnMid2, midImg);//中间图2

		//顶点排序
		ClockwiseSortPoints(pts[0]); ClockwiseSortPoints(rectPts[0]);
		//删除多余顶点(仅仅将不多余的放入2f)
		for (int i = 1, j = 1; i < pts[0].size(); ++i) {
			//不为多余顶点
			if (abs(pts[0][i - 1].x - pts[0][i].x) > (src.cols / 4) || abs(pts[0][i - 1].y - pts[0][i].y) > (src.rows / 4))
				pts2F[j++] = pts[0][i];
		}
		pts2F[0] = pts[0][0];
		ClockwiseSortPoints(pts2F, 4);
		for (int i = 0; i < 4; ++i) {
			rectPts2F[i] = rectPts[0][i];
			std::cout << pts2F[i] << std::endl;
			//如果有两个点太接近或不足4个点则失败
			if ((pts2F[i] == Point2f(0, 0)) || ((i > 0) && (abs(pts2F[i - 1].x - pts2F[i].x) < (src.cols / 4) && abs(pts2F[i - 1].y - pts2F[i].y) < (src.rows / 4)))) {
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
	//测试用，如果失败过在图片上标记
	//if (failed) putText(maskedDst, "2222", Point(1600, 900), FONT_HERSHEY_TRIPLEX, 10, Scalar(0, 0, 255));
	MkdirAndImwrite(FnFi, maskedDst);//最终结果
	failedImg.close();



	return true;
}





void Registration::RenameRoot(const String srcPattern) {
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

void Registration::on_mouse(int event, int x, int y, int flags, void* ustc) //event鼠标事件代号，x,y鼠标坐标，flags拖拽和键盘操作的代号
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


bool Registration::Registrating1PicSemi(Mat& M, const String FnSrc, const String fnBinImg, const String FnFi, const bool flagM, const bool succeed, const String FnMid1, const String FnMid2, const String fnRefImg)
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
			if (abs(pts[0][i - 1].x - pts[0][i].x) > (src.cols / 4) || abs(pts[0][i - 1].y - pts[0][i].y) > (src.rows / 4))
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
	MkdirAndImwrite(FnFi, maskedDst);//最终结果



	return true;
}




void Registration::GetFilePath(vector<String>& srcNames, const string& srcPattern, const char useFailedImg) {
	if (useFailedImg == 'y') {
		ifstream failedImg("failedImg.txt");
		string line;
		while (getline(failedImg, line)) {
			srcNames.push_back(line);
		}
	}
	else {
		RenameRoot(srcPattern);
		glob(srcPattern, srcNames, true);
	}
}

///
/// 以下为传统图像处理边缘分割
/// 


bool Registration::GetRectVertices1Pic(const String FnSrc, const String fnBinImg, Point2f pts2F[4]) {
	Mat srcImg = imread(FnSrc);
	Mat binImg(srcImg.size(), CV_8UC1), edge, blurImg;
	//转换为灰度图
	cvtColor(srcImg, srcImg, COLOR_BGR2GRAY);
	//滤波
	medianBlur(srcImg, blurImg, 21);
	//blur(srcImg, blurImg, Size(3, 3));
	MkdirAndImwrite(fnBinImg + "Blur.png", blurImg);
    ////二值化
    //threshold(blurImg, blurImg, 100, 255, THRESH_BINARY);
    //MkdirAndImwrite(fnBinImg + "BinBlur.png", blurImg);
	//边缘检测
	Canny(blurImg, edge, 5, 12, 3);
	//保存Canny边缘图像
	MkdirAndImwrite(fnBinImg + "CannyEdge.png", edge);
	binImg = Scalar::all(0);
	//提取轮廓
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	//找到并存储轮廓
    findContours(edge, contours, hierarchy, RETR_TREE, CHAIN_APPROX_TC89_KCOS);
	//画出所有轮廓
	for (int index = 0; index < contours.size(); index++) {
		drawContours(edge, contours, index, Scalar(255, 255, 255), 1, 8/*, hierarchy*/);
	}
	//保存画出的边缘图像
	MkdirAndImwrite(fnBinImg + "Contours.png", edge);
	//提取面积最大的轮廓，下标为maxArea，polyContours存储外接多边形轮廓
	edge = Scalar::all(0);
	vector<vector<Point>> polyContours(contours.size());
	int maxArea = 0;
	for (int index = 0; index < contours.size(); index++) {
		if (contourArea(contours[index]) > contourArea(contours[maxArea]))
			maxArea = index;
		//最小外接多边形
		approxPolyDP(contours[index], polyContours[index], 30, true);
		//画出所有多边形轮廓
		drawContours(edge, polyContours, index, Scalar(255, 255, 255), 1, 8);
	}
	//保存画出的多边形边缘图像
	MkdirAndImwrite(fnBinImg + "PolyContours.png", edge);

	//画出矩形
	Mat polyPic = Mat::zeros(srcImg.size(), CV_8UC3);
    //drawContours(polyPic, contours, maxArea, Scalar(0, 0, 255/*rand() & 255, rand() & 255, rand() & 255*/), 2);
    drawContours(polyPic, polyContours, maxArea, Scalar(0, 0, 255/*rand() & 255, rand() & 255, rand() & 255*/), 2);
	//保存画出的矩形图像
	MkdirAndImwrite(fnBinImg + "Rect.png", polyPic);
	//寻找凸包，hull储存凸包点
	vector<Point> hull;
	convexHull(polyContours[maxArea], hull, true);  //检测该轮廓的凸包,顺时针排列

	//填充矩形
	IntoPoly(binImg, hull);
	MkdirAndImwrite(fnBinImg, binImg);
	//检测是否有4个点
	if (!Has4Points(hull, srcImg.cols / 4, srcImg.rows / 4) || pts2F == nullptr) return false;

	//删除多余顶点(仅仅将不多余的放入2f)
	for (int i = 1, j = 1; i < hull.size(); ++i) {
		//不为多余顶点
		if (abs(hull[i - 1].x - hull[i].x) > (srcImg.cols / 4) || abs(hull[i - 1].y - hull[i].y) > (srcImg.rows / 4))
			pts2F[j++] = hull[i];
	}

}




