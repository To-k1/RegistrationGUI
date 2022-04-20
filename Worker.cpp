#include "Worker.h"


//完成整个配准过程


bool Worker::handle_pause(Worker* worker) {
	if (worker->isPaused()) {
		emit worker->HandlePause();
        return true;
	}
    return false;
}


void Worker::send_process(const string processingName, const int processingNum, const int allNum, Worker* worker) {
	emit worker->SendProcess(QString::fromStdString(processingName), processingNum, allNum);
}

void Worker::Registrating(const string& srcPattern, const string& binPattern, const string& dstPattern, const char useSemiAuto, const char useFailedImg, const int startPos) {

	Registrator::Registrating(srcPattern, binPattern, dstPattern, useSemiAuto, useFailedImg, handle_pause, send_process, this, startPos);

}


//void Worker::Registrating(const string& srcPattern, const string& binPattern, const string& dstPattern, const char useSemiAuto, const char useFailedImg, const int startPos) {
//	//qDebug() << "12311111111111111111111111111111111111111111111";
//	vector<String> srcNames;//保存图名
//	//String srcPattern = "E:\\root\\";
//	//String srcPattern = "srcImg";
//	//char useSemiAuto = 'y';//如果为y则使用半自动配准
//	//std::cout << "是否使用半自动配准？[y/n]" << std::endl;
//	//cin >> useSemiAuto;
//	//char useFailedImg = 'y';//如果为y则使用failedImg.txt作为文件列表
//	//std::cout << "是否使用上次失败的文件列表？[y/n]" << std::endl;
//	//cin >> useFailedImg;
//
//	////得到源文件目录
//	//if (useFailedImg == 'y') {
//	//	ifstream failedImg("failedImg.txt");
//	//	string line;
//	//	while (getline(failedImg, line)) {
//	//		srcNames.push_back(line);
//	//	}
//	//}
//	//else {
//	//	renameRoot(srcPattern);
//	//	glob(srcPattern, srcNames, true);
//	//}
//	GetFilePath(srcNames, srcPattern, useFailedImg);
//
//
//	//仅自动配准且从头开始时用于清空失败的图片列表
//	if (useSemiAuto != 'y' || startPos == 0) {
//		ofstream failedImg("failedImg.txt");
//		failedImg.close();
//	}
//	String binName, dstName;
//	Mat M;//变换矩阵
//	fs::path fnPath;
//
//	for (int i = startPos; i < srcNames.size(); ++i) {
//		//若暂停了就退出，此时进度上显示的序号正好是下一个文件名的下标
//		if (this->isPaused()) {
//			//emit set_pause();
//			emit HandlePause();
//			return;
//		}
//
//		////任务进度
//		//string progress(std::to_string(i) + "/" + std::to_string(srcNames.size()));
//		////win->set_labelProgress_text(QString::fromStdString(progress));
//		//emit set_labelProgress_text(QString::fromStdString(progress));
//		////显示百分比进度
//		////win->set_progressBar_val(int(100*double(i+1)/srcNames.size()));
//		//emit set_progressBar_val(int(100 * double(i) / srcNames.size()));
//		////输出进度
//		////std::cout << srcNames[i] << std::endl; std::cout << binName << std::endl; std::cout << dstName << std::endl;
//		////win->set_labelProcessingName_text(QString::fromStdString(srcNames[i]));
//		//emit set_labelProcessingName_text(QString::fromStdString(srcNames[i]));
//		emit SendProcess(QString::fromStdString(srcNames[i]), i, srcNames.size());
//
//
//
//
//		bool flagM = true;
//		int startChar = srcPattern.size() + 1;//除去floderPattern外的第一个字符下标
//		String fn(srcNames[i], startChar, srcNames[i].size() - startChar);//从startChar开始,得到相对于srcNames[i]的路径
//		fnPath = fn.c_str();
//		//获得后缀如jpg,png
//		//String fnTail(srcNames[i], srcNames[i].size() - 4, 4);
//		String fnTail(fnPath.extension().string());
//		//去掉fn的后缀名如.jpg
//		fn = fnPath.parent_path().string() + "\\" + fnPath.stem().string();
//		//if (srcNames[i].size() < 23) continue;
//		if (fnTail != ".png" && fnTail != ".jpg") continue;
//		//用于分割后2个根的路径(短，如052_1.png
//		if (fn[fn.size() - 2] == '-' || fn[fn.size() - 2] == '_') {
//			binName = binPattern + "\\" + String(fn, 0, fn.size() - 2) + ".png";
//		}
//		//用于分割后2个根的路径(长，如001_Right_F0_01.png
//		else if ((fn[fn.size() - 3] == '-' || fn[fn.size() - 3] == '_')) {
//			if (fn[fn.size() - 5] != 'F' && fn[fn.size() - 5] != 'R') continue;
//			binName = binPattern + "\\" + String(fn, 0, fn.size() - 12) + ".png";
//		}
//		//一般原图路径如052.png
//		else {
//			binName = binPattern + "\\" + fn + ".png";
//			flagM = false;
//			std::cout << "原图" << std::endl;
//		}
//
//
//		//dstName = String("F:\\RootImg\\") + fn + ".png";
//		dstName = dstPattern + "\\" + fn + ".png";
//
//
//
//		if (fnTail != ".png" && fnTail != ".jpg") continue;
//
//		if (useSemiAuto != 'y') {
//			if (!Registrating1Pic(M, srcNames[i], binName, dstName, flagM))//失败
//				Registrating1Pic(M, srcNames[i], binName, dstName, flagM, true);
//		}
//		else {
//			if (!Registrating1PicSemi(M, srcNames[i], binName, dstName, flagM))//失败
//				Registrating1PicSemi(M, srcNames[i], binName, dstName, flagM, true);
//		}
//
//		pts[0].clear(), rectPts[0].clear();
//
//
//	}
//	//仅在全自动程序成功运行结束时把这次的失败列表备份
//	if (useSemiAuto != 'y')
//		CopyFile(L"failedImg.txt", L"failedImg.txt.bak", FALSE);
//	//// 先更新进度到完成再发送结束信号
//	//string progress(std::to_string(srcNames.size()) + "/" + std::to_string(srcNames.size()));
//	////win->set_labelProgress_text(QString::fromStdString(progress));
//	//emit set_labelProgress_text(QString::fromStdString(progress));
//	////显示百分比进度
//	////win->set_progressBar_val(int(100*double(i+1)/srcNames.size()));
//	//emit set_progressBar_val(100);
//	//emit resultReady();
//	emit SendProcess(QString::fromStdString(srcNames.back()), srcNames.size(), srcNames.size());
//
//}






