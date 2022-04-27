#include "Registrator.h"





bool Registrator::handle_pause(Registrator* worker) {
	if (worker->isPaused()) {
		emit worker->HandlePause();
		return true;
	}
	return false;
}


void Registrator::send_process(const string processingName, const int processingNum, const int allNum, Registrator* worker) {
	emit worker->SendProcess(QString::fromStdString(processingName), processingNum, allNum);
}

void Registrator::Registrating(const string& srcPattern, const string& binPattern, const string& dstPattern, const char useSemiAuto, const char useFailedImg, const int startPos) {

	Registration::Registrating(srcPattern, binPattern, dstPattern, useSemiAuto, useFailedImg, handle_pause, send_process, this, startPos);

}

void Registrator::GetRectVertices(const string& srcPattern, const string& binPattern, const int startPos) {
	Registration::GetRectVertices(srcPattern, binPattern, handle_pause, send_process, this, startPos);
}


