#include"stdafx.h"
#include"alg.h"
#include<math.h>

static void getBinMask(const Mat& comMask, Mat& binMask)
{
	if (comMask.empty() || comMask.type() != CV_8UC1)
		CV_Error(CV_StsBadArg, "comMask is empty or has incorrect type (not CV_8UC1)");
	if (binMask.empty() || binMask.rows != comMask.rows || binMask.cols != comMask.cols)
		binMask.create(comMask.size(), CV_8UC1);
	binMask = comMask & 1;
}
void GCApplication::reset()
{
	if (!mask.empty())
		mask.setTo(Scalar::all(GC_BGD));
	bgdPxls.clear(); fgdPxls.clear();
	prBgdPxls.clear();  prFgdPxls.clear();

	isInitialized = false;
	rectState = NOT_SET;    //NOT_SET == 0  
	lblsState = NOT_SET;
	prLblsState = NOT_SET;
	iterCount = 0;
}

/*����ĳ�Ա������ֵ����*/
void GCApplication::setImageAndWinName(const Mat& _image, const string& _winName)
{
	if (_image.empty() || _winName.empty())
		return;
	image = &_image;
	winName = &_winName;
	mask.create(image->size(), CV_8UC1);
	reset();
}

void GCApplication::showImage() const
{
	if (image->empty() || winName->empty())
		return;

	Mat res;
	Mat binMask;
	if (!isInitialized)
		image->copyTo(res);
	else
	{
		getBinMask(mask, binMask);
		image->copyTo(res, binMask);  //�������λ��0����1�����ƣ�ֻ������ǰ���йص�ͼ�񣬱���˵���ܵ�ǰ�������ܵı��� 
		//��ӱ���model�Ĵ���;
/*		Rect ret;
		ret.x = 90;
		ret.y = 121;
		ret.width = 429;
		ret.height = 277;
		Mat idt = res(ret);
		imwrite("D:\\Datas\\CardImages\\model.bmp", idt);*/
	}

	vector<Point>::const_iterator it;
	/*����4������ǽ�ѡ�е�4�����ò�ͬ����ɫ��ʾ����*/
	for (it = bgdPxls.begin(); it != bgdPxls.end(); ++it)  //���������Կ�����һ��ָ��  
		circle(res, *it, radius, BLUE, thickness);
	for (it = fgdPxls.begin(); it != fgdPxls.end(); ++it)  //ȷ����ǰ���ú�ɫ��ʾ  
		circle(res, *it, radius, RED, thickness);
	for (it = prBgdPxls.begin(); it != prBgdPxls.end(); ++it)
		circle(res, *it, radius, LIGHTBLUE, thickness);
	for (it = prFgdPxls.begin(); it != prFgdPxls.end(); ++it)
		circle(res, *it, radius, PINK, thickness);

	/*������*/
	if (rectState == IN_PROCESS || rectState == SET)
	{
		rectangle(res, Point(rect.x, rect.y), Point(rect.x + rect.width, rect.y + rect.height), GREEN, 2);
		printf("x: %d,y: %d,width: %d,heigth: %d\n", rect.x, rect.y, rect.width, rect.height);
	}

	imshow(*winName, res);
}
void GCApplication::setRectInMask()
{
	assert(!mask.empty());
	mask.setTo(GC_BGD);   //GC_BGD == 0  
	rect.x = max(0, rect.x);
	rect.y = max(0, rect.y);
	rect.width = min(rect.width, image->cols - rect.x);
	rect.height = min(rect.height, image->rows - rect.y);
	(mask(rect)).setTo(Scalar(GC_PR_FGD));    //GC_PR_FGD == 3�������ڲ�,Ϊ���ܵ�ǰ����  
}

void GCApplication::setLblsInMask(int flags, Point p, bool isPr)
{
	vector<Point> *bpxls, *fpxls;
	uchar bvalue, fvalue;
	if (!isPr) //ȷ���ĵ�  
	{
		bpxls = &bgdPxls;
		fpxls = &fgdPxls;
		bvalue = GC_BGD;    //0  
		fvalue = GC_FGD;    //1  
	}
	else    //���ʵ�  
	{
		bpxls = &prBgdPxls;
		fpxls = &prFgdPxls;
		bvalue = GC_PR_BGD; //2  
		fvalue = GC_PR_FGD; //3  
	}
	if (flags & BGD_KEY)
	{
		bpxls->push_back(p);
		circle(mask, p, radius, bvalue, thickness);   //�õ㴦Ϊ2  
	}
	if (flags & FGD_KEY)
	{
		fpxls->push_back(p);
		circle(mask, p, radius, fvalue, thickness);   //�õ㴦Ϊ3  
	}
}

/*�����Ӧ����������flagsΪCV_EVENT_FLAG�����*/
void GCApplication::mouseClick(int event, int x, int y, int flags, void*)
{
	// TODO add bad args check  
	switch (event)
	{
	case CV_EVENT_LBUTTONDOWN: // set rect or GC_BGD(GC_FGD) labels  
	{
		bool isb = (flags & BGD_KEY) != 0,
			isf = (flags & FGD_KEY) != 0;
		if (rectState == NOT_SET && !isb && !isf)//ֻ���������ʱ  
		{
			rectState = IN_PROCESS; //��ʾ���ڻ�����  
			rect = Rect(x, y, 1, 1);
		}
		if ((isb || isf) && rectState == SET) //������alt������shift�����һ����˾��Σ���ʾ���ڻ�ǰ��������  
			lblsState = IN_PROCESS;
	}
	break;
	case CV_EVENT_RBUTTONDOWN: // set GC_PR_BGD(GC_PR_FGD) labels  
	{
		bool isb = (flags & BGD_KEY) != 0,
			isf = (flags & FGD_KEY) != 0;
		if ((isb || isf) && rectState == SET) //���ڻ����ܵ�ǰ��������  
			prLblsState = IN_PROCESS;
	}
	break;
	case CV_EVENT_LBUTTONUP:
		if (rectState == IN_PROCESS)
		{
			rect = Rect(Point(rect.x, rect.y), Point(x, y));   //���ν���  
			rectState = SET;
			setRectInMask();
			assert(bgdPxls.empty() && fgdPxls.empty() && prBgdPxls.empty() && prFgdPxls.empty());
			showImage();
		}
		if (lblsState == IN_PROCESS)   //�ѻ���ǰ�󾰵�  
		{
			setLblsInMask(flags, Point(x, y), false);    //����ǰ����  
			lblsState = SET;
			showImage();
		}
		break;
	case CV_EVENT_RBUTTONUP:
		if (prLblsState == IN_PROCESS)
		{
			setLblsInMask(flags, Point(x, y), true); //����������  
			prLblsState = SET;
			showImage();
		}
		break;
	case CV_EVENT_MOUSEMOVE:
		if (rectState == IN_PROCESS)
		{
			rect = Rect(Point(rect.x, rect.y), Point(x, y));
			assert(bgdPxls.empty() && fgdPxls.empty() && prBgdPxls.empty() && prFgdPxls.empty());
			showImage();    //���ϵ���ʾͼƬ  
		}
		else if (lblsState == IN_PROCESS)
		{
			setLblsInMask(flags, Point(x, y), false);
			showImage();
		}
		else if (prLblsState == IN_PROCESS)
		{
			setLblsInMask(flags, Point(x, y), true);
			showImage();
		}
		break;
	}
}

int GCApplication::nextIter()//Ŀ������ָ�;
{
	if (isInitialized)
		//ʹ��grab�㷨����һ�ε���������2Ϊmask��������maskλ�ǣ������ڲ�������Щ�����Ǳ��������Ѿ�ȷ���Ǳ���������еĵ㣬��maskͬʱҲΪ���  
		//������Ƿָ���ǰ��ͼ��  
		grabCut(*image, mask, rect, bgdModel, fgdModel, 1);
	else
	{
	/*	if (rectState != SET)
			return iterCount;

		if (lblsState == SET || prLblsState == SET)
				grabCut(*image, mask, rect, bgdModel, fgdModel, 1, GC_INIT_WITH_MASK);
			
		else
				grabCut(*image, mask, rect, bgdModel, fgdModel, 1, GC_INIT_WITH_RECT);*/
		grabCut(*image, mask, rect, bgdModel, fgdModel, 1, GC_INIT_WITH_RECT);
	//	isInitialized = true;
	}
	iterCount++;

//	bgdPxls.clear(); fgdPxls.clear();
//	prBgdPxls.clear(); prFgdPxls.clear();

	return iterCount;
}

void GCApplication::ShapDetect(Mat &res)
{
	
	Mat gray;
	Mat threshImg;
	Mat show;
	cvtColor(res, gray, CV_BGR2GRAY);
	resize(gray, gray_img, Size(gray.cols / 2, gray.rows / 2), 0, 0, INTER_LINEAR);
//	resize(*image, show, Size(image->cols / 2, image->rows / 2), 0, 0, INTER_LINEAR);
	nr = gray_img.rows;
	nc = gray_img.cols;
	threshold(gray_img, threshImg, 0, 255, CV_THRESH_OTSU);
	vector<vector<Point> >contours;
	vector<Vec4i>hierachy;
	float angle = 0.0;
	findContours(threshImg, contours, hierachy, RETR_EXTERNAL, CHAIN_APPROX_NONE, Point());

	RotatedRect Rect_out= minAreaRect(contours[0]);	
	Size si = Rect_out.size;
	if (Rect_out.size.width <= Rect_out.size.height)
	{
		angle = Rect_out.angle + 90;
		float temp = 0.0;
		temp = si.height;
		si.height = si.width;
		si.width = temp;
	}
	else
		angle = Rect_out.angle;
	Mat rotmat = getRotationMatrix2D(Rect_out.center, angle, 1);
	Mat deal_img;
	warpAffine(gray_img, deal_img, rotmat, show.size(), CV_INTER_CUBIC);
	getRectSubPix(deal_img, si, Rect_out.center, grab_img);
}

void GCApplication::MatToVector(Mat& img)
{
	int nCols = img.cols;
	int nRows = img.rows;
	vector<uchar>tempcol(nCols);
	vector<uchar>tempMaxrow(nRows);
	vector<uchar>tempMinrow(nRows);
	pixel *imgMatrix = new pixel[nRows];
	for (int i = 0; i < nRows; i++)
		imgMatrix[i] = img.ptr<uchar>(i);
	for (int i = 0; i < nRows; i++)
	{
		tempcol.clear();
		for (int j = 0; j < nCols; j++)
		{
			tempcol.push_back(imgMatrix[i][j]);
		}
		auto maxP = max_element(tempcol.begin(), tempcol.end());
		auto minP = min_element(tempcol.begin(), tempcol.end());
		tempMaxrow.push_back(*maxP);
		tempMinrow.push_back(*minP);
	}
	if ((tempMaxrow.size() > 0) && (tempMinrow.size()>0))
	{
		auto maxP = max_element(tempMaxrow.begin(), tempMaxrow.end());
		auto minP = min_element(tempMinrow.begin(), tempMinrow.end());
		MaxVaule = (int)(*maxP);
		MinValue = (int)(*minP);
		delete[]imgMatrix;
	}
	else
	{
		delete[]imgMatrix;
		return;
	}
}

void GCApplication::GrayNorm(Mat& img)
{
	if ((MaxVaule == 0) && (MinValue == 0))
	{
		printf("Error!\n");
		return;
	}
	else
	{

	}
}