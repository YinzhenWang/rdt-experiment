// StopWait.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "Global.h"
#include "RdtSender.h"
#include "RdtReceiver.h"
#include "StopWaitRdtSender.h"
#include "StopWaitRdtReceiver.h"
#include "GBNRdtReceiver.h"
#include "GBNRdtSender.h"
#include "SRRdtReceiver.h"
#include "SRRdtSender.h"
#include "TCPRdtReceiver.h"
#include "TCPRdtSender.h"

int main(int argc, char* argv[])
{
	int ifres = 0; //是否输出到result文件

	RdtSender* ps = NULL;
	RdtReceiver* pr = NULL;
	FILE* newOut = NULL;

	printf("Choose one:\n");
	printf("1:GBN\t2:SR\t3:TCP\n");
	int mode= 0;
	scanf_s("%d", &mode);
	while ( mode < 1 || mode > 3 ) 
	scanf_s("%d", &mode);
	switch (mode)
	{
	case 1:
		ps = new GBNRdtSender();
		pr = new GBNRdtReceiver();
		cout << "GBN" << endl;
		if (!ifres)
			freopen_s(&newOut, "result.txt", "w", stdout);
		break;

	case 2:
		ps = new SRRdtSender();
		pr = new SRRdtReceiver();
		cout << "SR" << endl;
		if (!ifres)
			freopen_s(&newOut, "result.txt", "w", stdout);
		break;

	case 3:
		ps = new TCPRdtSender();
		pr = new TCPRdtReceiver();
		cout << "TCP" << endl;
		if (!ifres)
			freopen_s(&newOut, "result.txt", "w", stdout);

		break;
	default:
		break;
	}



//	pns->setRunMode(0);  //VERBOS模式
	pns->setRunMode(1);  //安静模式
	pns->init();
	pns->setRtdSender(ps);
	pns->setRtdReceiver(pr);
	pns->setInputFile("input.txt");
	pns->setOutputFile("output.txt");

	pns->start();

	delete ps;
	delete pr;
	delete pUtils;									//指向唯一的工具类实例，只在main函数结束前delete
	delete pns;										//指向唯一的模拟网络环境类实例，只在main函数结束前delete
	if(newOut!=NULL)
		fclose(newOut);
	/*必须要进行判断，因为你不能嗯关一个NULL的文件指针指向的文件*/
	return 0;
}

