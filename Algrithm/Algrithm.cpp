// Algrithm.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include"alg.h"

GCApplication gcapp;

static void on_mouse(int event, int x, int y, int flags, void* param)
{
	gcapp.mouseClick(event, x, y, flags, param);
}
static void getBinMask2(const Mat& comMask, Mat& binMask)
{
	if (comMask.empty() || comMask.type() != CV_8UC1)
		CV_Error(CV_StsBadArg, "comMask is empty or has incorrect type (not CV_8UC1)");
	if (binMask.empty() || binMask.rows != comMask.rows || binMask.cols != comMask.cols)
		binMask.create(comMask.size(), CV_8UC1);
	binMask = comMask & 1;
}
int main()
{
	string str = "D:\\Datas\\CardImages\\彩色图\\正面\\7.bmp";
	string savepath = "D:\\Datas\\CardImages\\分割效果图\\model1\\背面\\output_8.bmp";
	string modelpath = "D:\\Datas\\CardImages\\分割效果图\\model1\\正面\\model.bmp";
	cv::Mat src = cv::imread(str.c_str(), 1);
	if (src.empty())
		return -1;
	gcapp.image = &src;
	gcapp.mask = imread(modelpath, 1);
	gcapp.rect.x = 24;
	gcapp.rect.y = 30;
	gcapp.rect.width = src.cols - 2 * 24;
	gcapp.rect.height = src.rows - 2 * 30;
	int iterCount = gcapp.getIterCount();
	int newIterCount = gcapp.nextIter();
	if (newIterCount > iterCount)
	{
		Mat res;
		Mat binMask;
		getBinMask2(gcapp.mask, binMask);
		src.copyTo(res, binMask);
		gcapp.ShapDetect(res);
	//	imwrite(savepath, res);	
	//	imshow("output_0", res);
	//	waitKey(0);

	}
/*	cvNamedWindow(str.c_str(), CV_WINDOW_AUTOSIZE);
	cvSetMouseCallback(str.c_str(), on_mouse, 0);
	gcapp.setImageAndWinName(src, str);
	gcapp.showImage();
	while (1)
	{
		int c = cvWaitKey(0);
		if (c != 32)
			return -1;
		else
		{
			if (src.channels() == 1)
				return -1;
			int iterCount = gcapp.getIterCount();
			int newIterCount = gcapp.nextIter();
			if (newIterCount > iterCount)
			{
				gcapp.showImage();
			}
		}
	}	*/
	printf("done!\n");
    return 0;
}

