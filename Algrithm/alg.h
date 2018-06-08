#ifndef ALD_H
#define ALD_H

#include "opencv2/highgui/highgui.hpp"  
#include "opencv2/imgproc/imgproc.hpp"  
#include <iostream> 
#include<algorithm>
#include "windows.h"  
#include"DataDefine.h"
using namespace std;
using namespace cv;

const Scalar RED = Scalar(0, 0, 255);
const Scalar PINK = Scalar(230, 130, 255);
const Scalar BLUE = Scalar(255, 0, 0);
const Scalar LIGHTBLUE = Scalar(255, 255, 160);
const Scalar GREEN = Scalar(0, 255, 0);
const int BGD_KEY = CV_EVENT_FLAG_CTRLKEY;  //Ctrl键  
const int FGD_KEY = CV_EVENT_FLAG_SHIFTKEY; //Shift键

static void getBinMask(const Mat& comMask, Mat& binMask);
class GCApplication
{
public:
	enum { NOT_SET = 0, IN_PROCESS = 1, SET = 2 };
	static const int radius = 2;
	static const int thickness = -1;
	GCApplication() {}
	~GCApplication() {}
	void reset();
	void setImageAndWinName(const Mat& _image, const string& _winName);
	void showImage() const;
	void mouseClick(int event, int x, int y, int flags, void* param);
	int nextIter();
	int getIterCount() const { return iterCount; }
	Mat mask;
	Rect rect;
	const Mat* image;
	void ShapDetect(Mat &res);
private:
/**********************前期预处理函数*****************************/
	void setRectInMask();
	void setLblsInMask(int flags, Point p, bool isPr);
	void GrayNorm(Mat& img);
	void MatToVector(Mat& img);
/**********************前期预处理函数变量*****************************/
	const string* winName;
	Mat bgdModel, fgdModel;
	Mat gray_img;
	Mat grab_img;//分割出来后的卡片图像;
	int MaxVaule;
	int MinValue;

/*******************鼠标截取前景图像***********************/
	uchar rectState, lblsState, prLblsState;
	bool isInitialized;	
	vector<Point> fgdPxls, bgdPxls, prFgdPxls, prBgdPxls;
	int iterCount;
	int nr;
	int nc;
	vector<MYPOINT>upPoint;//上边界点
	vector<MYPOINT>downPoint;//下边界点
	vector<MYPOINT>leftPoint;//左边界点
	vector<MYPOINT>rightPoint;//右边界点
};

#endif // !ALD_H

