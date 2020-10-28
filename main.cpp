// StopWait.cpp : �������̨Ӧ�ó������ڵ㡣
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
	int ifres = 0; //�Ƿ������result�ļ�

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



//	pns->setRunMode(0);  //VERBOSģʽ
	pns->setRunMode(1);  //����ģʽ
	pns->init();
	pns->setRtdSender(ps);
	pns->setRtdReceiver(pr);
	pns->setInputFile("input.txt");
	pns->setOutputFile("output.txt");

	pns->start();

	delete ps;
	delete pr;
	delete pUtils;									//ָ��Ψһ�Ĺ�����ʵ����ֻ��main��������ǰdelete
	delete pns;										//ָ��Ψһ��ģ�����绷����ʵ����ֻ��main��������ǰdelete
	if(newOut!=NULL)
		fclose(newOut);
	/*����Ҫ�����жϣ���Ϊ�㲻���Ź�һ��NULL���ļ�ָ��ָ����ļ�*/
	return 0;
}

