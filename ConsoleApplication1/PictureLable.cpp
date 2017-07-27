// pic_label.cpp : �������̨Ӧ�ó������ڵ㡣
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

//ȫ�ֱ���
bool is_drawing = false;
//���յı�־����
vector<CvRect> biaozhu_boxs;
//����ʱ��¼����ʱ��
CvRect drawing_box;
IplImage *img, *img1;


static void help();
static void onMouse(int event, int x, int y, int, void*);
void getFiles(string path, vector<string>& files);

//1.����Ŀ�꡾�ɶ����
//2.��n��������һ֡�����浱ǰ��Ŀ�����굽txt�ı����ɶ����
//3.��c�������ǰ֡�����ѱ궨���������з����ʱ�򡿻�����һ֡����������
//�ļ������ʽ��
//֡���Ŀ���ž������Ͻ�����������½�����
//int _tmain(int argc, _TCHAR* argv[])
int main()
{
	CvFont font;
	CvScalar scalar;
	char text[10];

	// ��ʼ������
	double hScale = 1;
	double vScale = 1;
	int lineWidth = 3;// �൱��д�ֵ�����
	scalar = CV_RGB(255, 0, 0);
	cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX | CV_FONT_ITALIC, hScale, vScale, 0, lineWidth);//��ʼ�����壬׼��д��ͼƬ�ϵ� 

	int frame_counter = 0;
	int obj_id = 0;

	////��ȡ��Ƶ
	//CvCapture *capture = cvCreateFileCapture("a.avi");
	////��ȡһ֡
	//img = cvQueryFrame(capture);
	//img1 = cvCreateImage(cvSize(img->width, img->height), IPL_DEPTH_8U, 3);
	////���浽ͼ������
	//cvCopy(img, img1);

	//��ȡ����ͼ���ļ���
	string imgPath = "..\\Data\\origin";  //ԭʼѵ��ͼƬ·��
	string lablePath="..\\Data\\labled\\";   //��ע�ù�Դ��ѵ��ͼƬ
	string lightFile="..\\Data";   //ѵ��ͼƬ��Դ��Ϣ�ļ�·��

	//������Ϣ��ʾ
	help();
	//�˵�ѡ��
	cout<<"--------------��ѡ��ģʽ-------------"<<endl;
	cout<<"-----ģʽ1��˳���עȫ��ѵ��ͼƬ-----"<<endl;
	cout<<"-----ģʽ2����עָ��ѵ��ͼƬ---------"<<endl;
	int model;
	cin>>model;

	//����ı��ļ�
	//fstream outfile(lightFile+"\\LightLable.txt");  //fstreamĬ�Ͽɶ��ļ����ж�д

	//ģʽ1��˳���עȫ��ѵ��ͼƬ
	if(model==1)
	{
		vector<string> files;  //ÿ��ѵ��ͼƬ��·��+����
		getFiles(imgPath, files);  //filesΪ���ص��ļ������ɵ��ַ���������  
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
			//����cvLoadImage()�ĵ�1 ��������ͼ���ļ���·��.
			//��2 �������Ƕ�ȡͼ��ķ�ʽ:-1 ��ʾ����ͼ�������������ȡ,1 ��ʾǿ�Ʋ�ɫ��,0 ��ʾǿ�ƻ�ֵ��.
			if (img == NULL)
			{
				cout << "�޷���ȡͼ�� " << files[0] << endl;

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
		//���浽ͼ������img1
		cvCopy(img, img1);

		//����ı��ļ�
		ofstream lableFile(lightFile + "\\LightLable.txt");   //ֻ��д�ļ�������ļ������ڣ��򴴽��ļ�  

		//��ʾԭʼͼƬ
		cvShowImage("Image", img);

		//���ص�
		cvSetMouseCallback("Image", onMouse, 0);

		while (1)
		{
			//�ȴ�����
			int c = cvWaitKey(0);
			//esc���˳�
			if ((c & 255) == 27)
			{
				cout << "Exiting ...\n";
				break;
			}

			switch ((char)c)
			{
				//��ȡ��һ֡
			case 'n':
				//��¼��ǰ֡�����б�ע��Ϣ
				for (vector<CvRect>::iterator it = biaozhu_boxs.begin();it != biaozhu_boxs.end();++it)
				{
					cvRectangle(img1, cvPoint((*it).x, (*it).y), cvPoint((*it).x + (*it).width, (*it).y + (*it).height), CV_RGB(0, 255, 0));
					//�γ��ı���¼�Ķ�������
					_itoa(obj_id, text, 10);  //char*_itoa(intvalue,char*string,intradix);����ת��Ϊ�ַ��� int--->char*;��3������Ϊʮ����
					//��ͼƬ������ַ�����¼�ǵ�ǰ֡�ĵڼ�����
					cvPutText(img1, text, cvPoint((*it).x, (*it).y), &font, CV_RGB(255, 0, 0));
					//img---ͼƬָ�룻text---��Ҫ��ӡ��ͼƬ�ϵ��ַ��������ݣ�origin---�ַ�����ͼƬ�ϴ�ӡ��ԭ�㣻font---�����������Եı�����color---�������ɫ��


					//�������Ǽ�¼�ı�
					string strOutFilePath = files[frame_counter];  //ͼƬ��
					int iPos = strOutFilePath.find_last_of("/");  //int find_last_of(char c):�����ַ��������һ�����ֵ�c����ƥ�䣬�򷵻�ƥ��λ�ã����򷵻�-1.
					if (iPos == -1)
					{
						iPos = strOutFilePath.find_last_of("\\");
					}

					//��txt�ļ���д��ͼƬ������Դ�����ţ���Դ���ο����Ͻ����ꡢ����
					lableFile << files[frame_counter].substr(iPos + 1) << " " << obj_id << " " << (*it).x << " "
						<< (*it).y << " " << (*it).width << " "
						<< (*it).height << endl;

					//�����ע�ù�Դ��ѵ��ͼƬ
					string labledName;
					labledName.assign(files[frame_counter].substr(iPos + 1));  //ѵ��ͼƬ������
					cvSaveImage((lablePath+labledName).c_str(), img1);

					obj_id++;
				}

				//read the next frame
				++frame_counter;
				//��ȡ��һ֡
				while (1)
				{
					if (frame_counter >= iFileSize)
					{
						goto EndProgram;
					}
					img = cvLoadImage((files[frame_counter]).c_str(), -1);
					//����cvLoadImage()�ĵ�1 ��������ͼ���ļ���·��.
					//��2 �������Ƕ�ȡͼ��ķ�ʽ:-1 ��ʾ����ͼ�������������ȡ,1 ��ʾǿ�Ʋ�ɫ��,0 ��ʾǿ�ƻ�ֵ��.
					if (img == NULL)
					{
						cout << "�޷���ȡͼ�� " << files[frame_counter] << endl;

						frame_counter++;
					}
					else
					{
						break;
					}
				}
				//���浽ͼ������
				cvReleaseImage(&img1);
				img1 = cvCreateImage(cvSize(img->width, img->height), img->depth, img->nChannels);
				//������һ֡ͼ�񵽲�������
				cvCopy(img, img1);

				//ÿ֡�еı�ע��������0
				obj_id = 0;
				//�����ע���б�
				biaozhu_boxs.clear();
				break;

			case 'c':
				//�����ע���б�
				biaozhu_boxs.clear();
				//���¿�����ǰ֡ͼ�񵽲�������
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
	else if(model==2)  //ģʽ2����עָ��ѵ��ͼƬ
	{
		//step1. ��txt�е����ݴ洢��multimap�У����ļ���Ϊkey��valueΪ�����Ϣ
		multimap<string, string> modify;
		modify.clear();
		string line;  //txtÿ������
		string picName, picInfo;  //ͼƬ���֣������Ϣ
		fstream originData;
		originData.open(lightFile+"\\LightLable.txt");
		while(!originData.eof())
		{
			getline(originData, line, '\n');  //��ȡһ�У��Ի��з�����������line��
			if (line.size() < 3)
			{
				continue;
			}
			int spacePos;  //��һ���ո��λ��
			spacePos=line.find(' ');  // s.find(s1):����s�е�һ�γ���s1��λ�ã������أ�����0��
			picName=line.substr(0,spacePos);  //s.substring(int beginIndex,int endIndex)��ָ���� beginIndex ����ʼ��ֱ������ endIndex - 1 �����ַ�
			picInfo=line.substr(spacePos+1);     //s.substr(pos):��ȡs�дӴ�pos��ʼ������0����ĩβ�������ַ����Ӵ�
			modify.insert(pair<string, string>(picName, picInfo));
		}
		originData.close();

		//step2. ����Ҫ�޸ĵ�ͼƬ����
		//string modifyName,modifyInfo;  //Ҫ�޸ĵ�ѵ��ͼƬ���ֺͿ����Ϣ
		//cout<<"����Ҫ�޸ĵ�ѵ��ͼƬ�����֣�";
		//cin>>modifyName;

		////step2. ����Ҫ�޸ĵ�ͼƬ����
		string modifyName,modifyInfo;  //Ҫ�޸ĵ�ѵ��ͼƬ���ֺͿ����Ϣ
		cout<<"����Ҫ�޸ĵ�ѵ��ͼƬ�����֣�";
		cin >> modifyName;

		img = cvLoadImage((imgPath+"\\"+modifyName).c_str(), -1);  //imgPath=..\\Data\\origin
		while (img == NULL)
		{
			cout << "�޷���ȡͼ�� " << endl;
			cout<<"��������Ҫ�޸ĵ�ѵ��ͼƬ�����֣�";
			cin >> modifyName;
			img = cvLoadImage((imgPath+"\\"+modifyName).c_str(), -1);  //imgPath=..\\Data\\origin
		}
		img1 = cvCreateImage(cvSize(img->width, img->height), img->depth, img->nChannels);	
		cvCopy(img, img1);  //���浽ͼ������img1

		//��ʾԭʼͼƬ
		cvShowImage("Image", img);

		//���ص�
		cvSetMouseCallback("Image", onMouse, 0);

		//step3. ����ԭʼѵ��ͼƬ����ʼ����ע������clear����Ŀ�
		//step4. �޸�multimap�ж�Ӧ����Ϣ�������µı�עͼƬ
		//step5. �Ƿ�����޸�ͼƬ������ǣ���������񣬽�multimapд��txt
		bool finish=false;
		cout<<"----����ģʽ2���������£�----"<<endl;
		cout<<"----����y��������ע��һ��ͼƬ----"<<endl;
		cout<<"----����c�������ǰͼƬ�ı�ע----"<<endl;
		cout<<"----����f���˳�����----"<<endl;

		while (finish==false)
		{
			//cout<<"�������������";
			//cout<<"----��ע�������밴y����հ�c----"<<endl;
			////�ȴ�����
			//int c = cvWaitKey(0);
			////esc���˳����������µ�txt
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
			cout<<"�������������";
			//cin >> c;
			int c = cvWaitKey(0);

			switch ((char)c)
				//switch(c)
			{
				//��ȡ��һ֡
			case 'y':
				{
					//�޸�multimap�ж�Ӧ����Ϣ
					//��ɾ��
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

					//��¼��ǰ֡�����б�ע��Ϣ
					for (vector<CvRect>::iterator it = biaozhu_boxs.begin();it != biaozhu_boxs.end();++it)
					{
						cvRectangle(img1, cvPoint((*it).x, (*it).y), cvPoint((*it).x + (*it).width, (*it).y + (*it).height), CV_RGB(0, 255, 0));
						//�γ��ı���¼�Ķ�������
						_itoa(obj_id, text, 10);  //char*_itoa(intvalue,char*string,intradix);����ת��Ϊ�ַ��� int--->char*;��3������Ϊʮ����
						//��ͼƬ������ַ�����¼�ǵ�ǰ֡�ĵڼ�����
						cvPutText(img1, text, cvPoint((*it).x, (*it).y), &font, CV_RGB(255, 0, 0));
						//img---ͼƬָ�룻text---��Ҫ��ӡ��ͼƬ�ϵ��ַ��������ݣ�origin---�ַ�����ͼƬ�ϴ�ӡ��ԭ�㣻font---�����������Եı�����color---�������ɫ��

						//�����					
						stringstream stream;
						stream << obj_id << " ";  //����
						stream << (*it).x << " ";  //����x
						stream<<(*it).y << " ";  //����y
						stream<<(*it).width << " ";  //��
						stream<<(*it).height;  //��
						modifyInfo.assign(stream.str());

						modify.insert(pair<string, string>(modifyName, modifyInfo));

						obj_id++;
					}
					//�����µı�ע�ù�Դ��ѵ��ͼƬ
					cvSaveImage((lablePath+modifyName).c_str(), img1);

					//ÿ֡�еı�ע��������0
					obj_id = 0;
					//�����ע���б�
					biaozhu_boxs.clear();

					//step2. ����Ҫ�޸ĵ�ͼƬ����
					cout<<"����Ҫ�޸ĵ�ѵ��ͼƬ�����֣�";
					cin >> modifyName;

					img = cvLoadImage((imgPath+"\\"+modifyName).c_str(), -1);  //imgPath=..\\Data\\origin
					while (img == NULL)
					{
						cout << "�޷���ȡͼ�� " << endl;
						cout<<"��������Ҫ�޸ĵ�ѵ��ͼƬ�����֣�";
						cin >> modifyName;
						img = cvLoadImage((imgPath+"\\"+modifyName).c_str(), -1);  //imgPath=..\\Data\\origin
					}
					img1 = cvCreateImage(cvSize(img->width, img->height), img->depth, img->nChannels);	
					cvCopy(img, img1);  //���浽ͼ������img1

					////��ʾԭʼͼƬ
					//cvShowImage("Image", img);

					////���ص�
					//cvSetMouseCallback("Image", onMouse, 0);

					//cout<<"---�Ƿ������ע����y����,����f�˳�---"<<endl;
					//cin>>c;
				}
				break;

			case 'c':
				//�����ע���б�
				biaozhu_boxs.clear();
				//���¿�����ǰ֡ͼ�񵽲�������
				cvCopy(img, img1);
				//cvShowImage("Image", img1);
				break;

			case'f':  //f���˳����������µ�txt
				cout << "Exiting ...\n";

				//�޸�multimap�ж�Ӧ����Ϣ
									//��ɾ��
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

				//��¼��ǰ֡�����б�ע��Ϣ
				for (vector<CvRect>::iterator it = biaozhu_boxs.begin();it != biaozhu_boxs.end();++it)
				{
					cvRectangle(img1, cvPoint((*it).x, (*it).y), cvPoint((*it).x + (*it).width, (*it).y + (*it).height), CV_RGB(0, 255, 0));
					//�γ��ı���¼�Ķ�������
					_itoa(obj_id, text, 10);  //char*_itoa(intvalue,char*string,intradix);����ת��Ϊ�ַ��� int--->char*;��3������Ϊʮ����
					//��ͼƬ������ַ�����¼�ǵ�ǰ֡�ĵڼ�����
					cvPutText(img1, text, cvPoint((*it).x, (*it).y), &font, CV_RGB(255, 0, 0));
					//img---ͼƬָ�룻text---��Ҫ��ӡ��ͼƬ�ϵ��ַ��������ݣ�origin---�ַ�����ͼƬ�ϴ�ӡ��ԭ�㣻font---�����������Եı�����color---�������ɫ��
					
					//�����
					stringstream stream;
					stream << obj_id << " ";  //����
					stream << (*it).x << " ";  //����x
					stream<<(*it).y << " ";  //����y
					stream<<(*it).width << " ";  //��
					stream<<(*it).height;  //��
					modifyInfo.assign(stream.str());

					modify.insert(pair<string, string>(modifyName, modifyInfo));

					obj_id++;
				}
									//�����µı�ע�ù�Դ��ѵ��ͼƬ
					cvSaveImage((lablePath+modifyName).c_str(), img1);

				//ÿ֡�еı�ע��������0
				obj_id = 0;
				//�����ע���б�
				biaozhu_boxs.clear();

				//ɾ��ԭtxt�ļ��������µ�txt�ļ�
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

			//////�ȴ�����
			////cvWaitKey(0);

			//cout<<"�������������";
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
		//�������
	case CV_EVENT_LBUTTONDOWN:
		//the left up point of the rect
		is_drawing = true;
		drawing_box.x = x;
		drawing_box.y = y;
		break;

		//����ƶ�
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

		//�������
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
	//�ļ����  
	long long hFile = 0;
	//�ļ���Ϣ  
	struct _finddata_t fileinfo;  //struct _finddata_t �������洢�ļ�������Ϣ�Ľṹ��
	string p;

	//long _findfirst(const char *, struct _finddata_t *);
	//��һ������Ϊ�ļ�����������"*.*"�����������ļ���Ҳ������"*.cpp"������.cpp�ļ���
	//�ڶ���������_finddata_t�ṹ��ָ�롣�����ҳɹ��������ļ��������ʧ�ܣ�����-1��
	if ((hFile = _findfirst(p.assign(inPath).append("\\*.*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			//�����Ŀ¼,����֮  
			//�������,�����б�  
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