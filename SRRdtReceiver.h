#ifndef SR_RDT_RECEIVER_H			//��֤ͷ�ļ���������ͬ��ֻ������һ�Σ���������ͬʱʹ��
#define SR_RDT_RECEIVER_H			
#include "RdtReceiver.h"

class SRRdtReceiver : public RdtReceiver
{
private:
	static int windowSize;          //���շ����ڴ�С
	list <Packet> window;	   //windowSize��С��list
	int base;						//��¼���մ��ڵĵ�ǰ���

public:
	SRRdtReceiver();
	virtual ~SRRdtReceiver();
	bool IncludeBase(int seqnum);
	int Order(int seqnum);	//��base����ģ�����õ�������order
	void print(void);	//��ӡ��������

public:

	void receive(const Packet& packet);	//���ձ��ģ�����NetworkService����
};

#endif