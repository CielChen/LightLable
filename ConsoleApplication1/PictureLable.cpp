// pic_label.cpp : 定义控制台应用程序的入口点。
//

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <io.h>
#include <Windows.h>
using namespace std;

//全局变量
bool is_drawing = false;
//最终的标志盒子
vector<CvRect> biaozhu_boxs;
//绘制时记录的临时框
CvRect drawing_box;
IplImage *img, *img1;


static void help();
static void onMouse(int event, int x, int y, int, void*);
void getFiles(string path, vector<string>& files);

//1.鼠标框定目标【可多个】
//2.按n，进入下一帧，保存当前框定目标坐标到txt文本【可多个】
//3.按c，清除当前帧所有已标定区域【人总有犯错的时候】或者上一帧遗留的区域
//文件保存格式：
//帧编号目标编号矩形左上角坐标矩形右下角坐标
//int _tmain(int argc, _TCHAR* argv[])
int main()
{
	CvFont font;
	CvScalar scalar;
	char text[10];

	// 初始化字体
	double hScale = 1;
	double vScale = 1;
	int lineWidth = 3;// 相当于写字的线条
	scalar = CV_RGB(255, 0, 0);
	cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX | CV_FONT_ITALIC, hScale, vScale, 0, lineWidth);//初始化字体，准备写到图片上的 

	int frame_counter = 0;
	int obj_id = 0;

	////读取视频
	//CvCapture *capture = cvCreateFileCapture("a.avi");
	////获取一帧
	//img = cvQueryFrame(capture);
	//img1 = cvCreateImage(cvSize(img->width, img->height), IPL_DEPTH_8U, 3);
	////保存到图像区域
	//cvCopy(img, img1);

	//读取所有图像文件名
	string imgPath = "..\\Data\\origin";  //原始训练图片路径
	string lablePath="..\\Data\\labled\\";   //标注好光源的训练图片
	string lightFile="..\\Data";   //训练图片光源信息文件路径

	//帮助信息提示
	help();
	//菜单选择
	cout<<"--------------请选择模式-------------"<<endl;
	cout<<"-----模式1：顺序标注全部训练图片-----"<<endl;
	cout<<"-----模式2：标注指定训练图片---------"<<endl;
	int model;
	cin>>model;

	//输出文本文件
	//fstream outfile(lightFile+"\\LightLable.txt");  //fstream默认可对文件进行读写

	//模式1：顺序标注全部训练图片
	if(model==1)
	{
		vector<string> files;  //每张训练图片的路径+名字
		getFiles(imgPath, files);  //files为返回的文件名构成的字符串向量组  
		int iFileSize = files.size();
		if (iFileSize <= 0)
		{
			cout << "No Valid Texture." << endl;
			cout << "Press any key quit..." << endl;
			char cTmp;
			cin >> cTmp;
			return 0;
		}
		while (1)
		{
			img = cvLoadImage((files[frame_counter]).c_str(), -1);
			//函数cvLoadImage()的第1 个参数是图像文件的路径.
			//第2 个参数是读取图像的方式:-1 表示按照图像本身的类型来读取,1 表示强制彩色化,0 表示强制灰值化.
			if (img == NULL)
			{
				cout << "无法读取图像 " << files[0] << endl;

				frame_counter++;
				if (frame_counter >= iFileSize)
				{
					cout << "Done!" << endl;
					cout << "Press any key quit..." << endl;
					char cTmp;
					cin >> cTmp;
					return 0;
				}
			}
			else
			{
				break;
			}
		}
		img1 = cvCreateImage(cvSize(img->width, img->height), img->depth, img->nChannels);
		//保存到图像区域img1
		cvCopy(img, img1);

		//输出文本文件
		ofstream lableFile(lightFile + "\\LightLable.txt");   //只能写文件，如果文件不存在，则创建文件  

		//显示原始图片
		cvShowImage("Image", img);

		//鼠标回调
		cvSetMouseCallback("Image", onMouse, 0);

		while (1)
		{
			//等待输入
			int c = cvWaitKey(0);
			//esc键退出
			if ((c & 255) == 27)
			{
				cout << "Exiting ...\n";
				break;
			}

			switch ((char)c)
			{
				//读取下一帧
			case 'n':
				//记录当前帧的所有标注信息
				for (vector<CvRect>::iterator it = biaozhu_boxs.begin();it != biaozhu_boxs.end();++it)
				{
					cvRectangle(img1, cvPoint((*it).x, (*it).y), cvPoint((*it).x + (*it).width, (*it).y + (*it).height), CV_RGB(0, 255, 0));
					//形成文本记录的对象名称
					_itoa(obj_id, text, 10);  //char*_itoa(intvalue,char*string,intradix);整形转换为字符串 int--->char*;第3个参数为十进制
					//在图片中输出字符，记录是当前帧的第几个框
					cvPutText(img1, text, cvPoint((*it).x, (*it).y), &font, CV_RGB(255, 0, 0));
					//img---图片指针；text---需要打印到图片上的字符串的内容；origin---字符串在图片上打印的原点；font---描述字体属性的变量；color---字体的颜色；


					//输出到标记记录文本
					string strOutFilePath = files[frame_counter];  //图片名
					int iPos = strOutFilePath.find_last_of("/");  //int find_last_of(char c):查找字符串中最后一个出现的c。有匹配，则返回匹配位置；否则返回-1.
					if (iPos == -1)
					{
						iPos = strOutFilePath.find_last_of("\\");
					}

					//向txt文件中写入图片名，光源索引号，光源矩形框左上角坐标、宽、高
					lableFile << files[frame_counter].substr(iPos + 1) << " " << obj_id << " " << (*it).x << " "
						<< (*it).y << " " << (*it).width << " "
						<< (*it).height << endl;

					//保存标注好光源的训练图片
					string labledName;
					labledName.assign(files[frame_counter].substr(iPos + 1));  //训练图片的名字
					cvSaveImage((lablePath+labledName).c_str(), img1);

					obj_id++;
				}

				//read the next frame
				++frame_counter;
				//获取下一帧
				while (1)
				{
					if (frame_counter >= iFileSize)
					{
						goto EndProgram;
					}
					img = cvLoadImage((files[frame_counter]).c_str(), -1);
					//函数cvLoadImage()的第1 个参数是图像文件的路径.
					//第2 个参数是读取图像的方式:-1 表示按照图像本身的类型来读取,1 表示强制彩色化,0 表示强制灰值化.
					if (img == NULL)
					{
						cout << "无法读取图像 " << files[frame_counter] << endl;

						frame_counter++;
					}
					else
					{
						break;
					}
				}
				//保存到图像区域
				cvReleaseImage(&img1);
				img1 = cvCreateImage(cvSize(img->width, img->height), img->depth, img->nChannels);
				//拷贝下一帧图像到操作区域
				cvCopy(img, img1);

				//每帧中的标注框索引清0
				obj_id = 0;
				//清除标注框列表
				biaozhu_boxs.clear();
				break;

			case 'c':
				//清除标注框列表
				biaozhu_boxs.clear();
				//重新拷贝当前帧图像到操作区域
				cvCopy(img, img1);
			}
			cvShowImage("Image", img1);
		}

EndProgram:
		cvNamedWindow("Image", 0);
		cvReleaseImage(&img);
		cvReleaseImage(&img1);
		cvDestroyWindow("Image");

		cout << "Done!" << endl;
		cout << "Press any key quit..." << endl;
		char cTmp;
		cin >> cTmp;

		lableFile.close();
		return 0;
	}
	else if(model==2)  //模式2：标注指定训练图片
	{
		//step1. 将txt中的内容存储到multimap中，以文件名为key，value为框的信息
		multimap<string, string> modify;
		modify.clear();
		string line;  //txt每行内容
		string picName, picInfo;  //图片名字，框的信息
		fstream originData;
		originData.open(lightFile+"\\LightLable.txt");
		while(!originData.eof())
		{
			getline(originData, line, '\n');  //读取一行，以换行符结束，存入line中
			if (line.size() < 3)
			{
				continue;
			}
			int spacePos;  //第一个空格的位置
			spacePos=line.find(' ');  // s.find(s1):查找s中第一次出现s1的位置，并返回（包括0）
			picName=line.substr(0,spacePos);  //s.substring(int beginIndex,int endIndex)从指定的 beginIndex 处开始，直到索引 endIndex - 1 处的字符
			picInfo=line.substr(spacePos+1);     //s.substr(pos):截取s中从从pos开始（包括0）到末尾的所有字符的子串
			modify.insert(pair<string, string>(picName, picInfo));
		}
		originData.close();

		//step2. 输入要修改的图片名字
		//string modifyName,modifyInfo;  //要修改的训练图片名字和框的信息
		//cout<<"输入要修改的训练图片的名字：";
		//cin>>modifyName;

		////step2. 输入要修改的图片名字
		string modifyName,modifyInfo;  //要修改的训练图片名字和框的信息
		cout<<"输入要修改的训练图片的名字：";
		cin >> modifyName;

		img = cvLoadImage((imgPath+"\\"+modifyName).c_str(), -1);  //imgPath=..\\Data\\origin
		while (img == NULL)
		{
			cout << "无法读取图像 " << endl;
			cout<<"重新输入要修改的训练图片的名字：";
			cin >> modifyName;
			img = cvLoadImage((imgPath+"\\"+modifyName).c_str(), -1);  //imgPath=..\\Data\\origin
		}
		img1 = cvCreateImage(cvSize(img->width, img->height), img->depth, img->nChannels);	
		cvCopy(img, img1);  //保存到图像区域img1

		//显示原始图片
		cvShowImage("Image", img);

		//鼠标回调
		cvSetMouseCallback("Image", onMouse, 0);

		//step3. 调出原始训练图片，开始画框。注：可以clear画错的框
		//step4. 修改multimap中对应的信息，保存新的标注图片
		//step5. 是否继续修改图片：如果是，则继续；否，将multimap写入txt
		bool finish=false;
		cout<<"----进入模式2，操作如下：----"<<endl;
		cout<<"----按键y：继续标注下一张图片----"<<endl;
		cout<<"----按键c：清除当前图片的标注----"<<endl;
		cout<<"----按键f：退出程序----"<<endl;

		while (finish==false)
		{
			//cout<<"输入操作按键：";
			//cout<<"----标注结束后请按y，清空按c----"<<endl;
			////等待输入
			//int c = cvWaitKey(0);
			////esc键退出，并保存新的txt
			//if ((c & 255) == 27)
			//{
			//	cout << "Exiting ...\n";
			//	originData.open(lightFile+"\\LightLable.txt");
			//	for(multimap<string,string>::iterator itr = modify.begin(); itr != modify.end(); itr++)
			//	{
			//		originData << itr->first << itr->second <<  endl;
			//	}
			//	originData.close();
			//	break;
			//}			
			//cin >> c;

			//char c;
			cout<<"输入操作按键：";
			//cin >> c;
			int c = cvWaitKey(0);

			switch ((char)c)
				//switch(c)
			{
				//读取下一帧
			case 'y':
				{
					//修改multimap中对应的信息
					//先删除
					for(multimap<string,string>::iterator itr=modify.begin();itr!=modify.end(); )
					{
						if(itr->first==modifyName)
						{
							itr=modify.erase(itr);
						}
						else
						{
							++itr;
						}
					}

					//记录当前帧的所有标注信息
					for (vector<CvRect>::iterator it = biaozhu_boxs.begin();it != biaozhu_boxs.end();++it)
					{
						cvRectangle(img1, cvPoint((*it).x, (*it).y), cvPoint((*it).x + (*it).width, (*it).y + (*it).height), CV_RGB(0, 255, 0));
						//形成文本记录的对象名称
						_itoa(obj_id, text, 10);  //char*_itoa(intvalue,char*string,intradix);整形转换为字符串 int--->char*;第3个参数为十进制
						//在图片中输出字符，记录是当前帧的第几个框
						cvPutText(img1, text, cvPoint((*it).x, (*it).y), &font, CV_RGB(255, 0, 0));
						//img---图片指针；text---需要打印到图片上的字符串的内容；origin---字符串在图片上打印的原点；font---描述字体属性的变量；color---字体的颜色；

						//再添加					
						stringstream stream;
						stream << obj_id << " ";  //索引
						stream << (*it).x << " ";  //坐标x
						stream<<(*it).y << " ";  //坐标y
						stream<<(*it).width << " ";  //宽
						stream<<(*it).height;  //高
						modifyInfo.assign(stream.str());

						modify.insert(pair<string, string>(modifyName, modifyInfo));

						obj_id++;
					}
					//保存新的标注好光源的训练图片
					cvSaveImage((lablePath+modifyName).c_str(), img1);

					//每帧中的标注框索引清0
					obj_id = 0;
					//清除标注框列表
					biaozhu_boxs.clear();

					//step2. 输入要修改的图片名字
					cout<<"输入要修改的训练图片的名字：";
					cin >> modifyName;

					img = cvLoadImage((imgPath+"\\"+modifyName).c_str(), -1);  //imgPath=..\\Data\\origin
					while (img == NULL)
					{
						cout << "无法读取图像 " << endl;
						cout<<"重新输入要修改的训练图片的名字：";
						cin >> modifyName;
						img = cvLoadImage((imgPath+"\\"+modifyName).c_str(), -1);  //imgPath=..\\Data\\origin
					}
					img1 = cvCreateImage(cvSize(img->width, img->height), img->depth, img->nChannels);	
					cvCopy(img, img1);  //保存到图像区域img1

					////显示原始图片
					//cvShowImage("Image", img);

					////鼠标回调
					//cvSetMouseCallback("Image", onMouse, 0);

					//cout<<"---是否继续标注？按y继续,否则按f退出---"<<endl;
					//cin>>c;
				}
				break;

			case 'c':
				//清除标注框列表
				biaozhu_boxs.clear();
				//重新拷贝当前帧图像到操作区域
				cvCopy(img, img1);
				//cvShowImage("Image", img1);
				break;

			case'f':  //f键退出，并保存新的txt
				cout << "Exiting ...\n";

				//修改multimap中对应的信息
									//先删除
					for(multimap<string,string>::iterator itr=modify.begin();itr!=modify.end(); )
					{
						if(itr->first==modifyName)
						{
							itr=modify.erase(itr);
						}
						else
						{
							++itr;
						}
					}

				//记录当前帧的所有标注信息
				for (vector<CvRect>::iterator it = biaozhu_boxs.begin();it != biaozhu_boxs.end();++it)
				{
					cvRectangle(img1, cvPoint((*it).x, (*it).y), cvPoint((*it).x + (*it).width, (*it).y + (*it).height), CV_RGB(0, 255, 0));
					//形成文本记录的对象名称
					_itoa(obj_id, text, 10);  //char*_itoa(intvalue,char*string,intradix);整形转换为字符串 int--->char*;第3个参数为十进制
					//在图片中输出字符，记录是当前帧的第几个框
					cvPutText(img1, text, cvPoint((*it).x, (*it).y), &font, CV_RGB(255, 0, 0));
					//img---图片指针；text---需要打印到图片上的字符串的内容；origin---字符串在图片上打印的原点；font---描述字体属性的变量；color---字体的颜色；
					
					//再添加
					stringstream stream;
					stream << obj_id << " ";  //索引
					stream << (*it).x << " ";  //坐标x
					stream<<(*it).y << " ";  //坐标y
					stream<<(*it).width << " ";  //宽
					stream<<(*it).height;  //高
					modifyInfo.assign(stream.str());

					modify.insert(pair<string, string>(modifyName, modifyInfo));

					obj_id++;
				}
									//保存新的标注好光源的训练图片
					cvSaveImage((lablePath+modifyName).c_str(), img1);

				//每帧中的标注框索引清0
				obj_id = 0;
				//清除标注框列表
				biaozhu_boxs.clear();

				//删除原txt文件，创建新的txt文件
				//DeleteFile((lightFile+"\\LightLable.txt").c_str());
				ofstream newData(lightFile+"\\newLightLable.txt");
				for(multimap<string,string>::iterator itr = modify.begin(); itr != modify.end(); itr++)
				{
					newData << itr->first << " " << itr->second <<  endl;
				}
				newData.close();
				finish=true;
			}

			cvShowImage("Image", img1);

			//////等待输入
			////cvWaitKey(0);

			//cout<<"输入操作按键：";
			//   cin >> c;
		}

	}
}

static void help()
{
	cout << "This program designed for labeling image \n"
		<< "Coded by Chen Dali on 7/25/2017\n" << endl;

	cout << "Use the mouse to draw rectangle on the image for labeling.\n" << endl;

	cout << "Hot keys: \n"
		"\tESC - quit the program\n"
		"\tn - next image\n"
		"\tc - clear all the labels\n"
		<< endl;
}

static void onMouse(int event, int x, int y, int, void*)
{
	switch (event)
	{
		//左键按下
	case CV_EVENT_LBUTTONDOWN:
		//the left up point of the rect
		is_drawing = true;
		drawing_box.x = x;
		drawing_box.y = y;
		break;

		//鼠标移动
	case CV_EVENT_MOUSEMOVE:
		//adjust the rect (use color blue for moving)
		if (is_drawing) {
			drawing_box.width = x - drawing_box.x;
			drawing_box.height = y - drawing_box.y;
			cvCopy(img, img1);
			for (vector<CvRect>::iterator it = biaozhu_boxs.begin();it != biaozhu_boxs.end();++it)
			{
				cvRectangle(img1, cvPoint((*it).x, (*it).y), cvPoint((*it).x + (*it).width, (*it).y + (*it).height), CV_RGB(0, 255, 0));
			}
			cvRectangle(img1, cvPoint(drawing_box.x, drawing_box.y), cvPoint(drawing_box.x + drawing_box.width, drawing_box.y + drawing_box.height), CV_RGB(255, 0, 0));
		}
		break;

		//左键弹起
	case CV_EVENT_LBUTTONUP:
		//finish drawing the rect (use color green for finish)
		if (is_drawing) {
			drawing_box.width = x - drawing_box.x;
			drawing_box.height = y - drawing_box.y;
			cvCopy(img, img1);
			for (vector<CvRect>::iterator it = biaozhu_boxs.begin();
				it != biaozhu_boxs.end();++it) {
					cvRectangle(img1, cvPoint((*it).x, (*it).y), cvPoint((*it).x + (*it).width, (*it).y + (*it).height), CV_RGB(0, 255, 0));
			}
			cvRectangle(img1, cvPoint(drawing_box.x, drawing_box.y), cvPoint(drawing_box.x + drawing_box.width, drawing_box.y + drawing_box.height), CV_RGB(255, 0, 0));
			biaozhu_boxs.push_back(drawing_box);
		}
		is_drawing = false;
		break;
	}
	cvShowImage("Image", img1);
	return;
}

void getFiles(string inPath, vector<string>& inFiles)
{
	//文件句柄  
	long long hFile = 0;
	//文件信息  
	struct _finddata_t fileinfo;  //struct _finddata_t 是用来存储文件各种信息的结构体
	string p;

	//long _findfirst(const char *, struct _finddata_t *);
	//第一个参数为文件名，可以用"*.*"来查找所有文件，也可以用"*.cpp"来查找.cpp文件。
	//第二个参数是_finddata_t结构体指针。若查找成功，返回文件句柄，若失败，返回-1。
	if ((hFile = _findfirst(p.assign(inPath).append("\\*.*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			//如果是目录,迭代之  
			//如果不是,加入列表  
			if ((fileinfo.attrib & _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
					getFiles(p.assign(inPath).append("\\").append(fileinfo.name), inFiles);
			}
			else
			{
				inFiles.push_back(p.assign(inPath).append("\\").append(fileinfo.name));
				//names.push_back(fileinfo.name);
			}
		} while (_findnext(hFile, &fileinfo) == 0);

		_findclose(hFile);
	}
}