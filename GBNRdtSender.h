#ifndef GBN_RDT_SENDER_H
#define GBN_RDT_SENDER_H
#include "RdtSender.h"

class GBNRdtSender :public RdtSender
{
private://ɾȥwaitingState
	int nextSeqnum;			// ��һ��������� 
	static int windowSize;	
	queue <Packet> window;	//windowSize��С��queue
	int base;				//���ڵ�һ�����ݰ����

public:

	bool getWaitingState();
	bool send(const Message& message);						//����Ӧ�ò�������Message����NetworkServiceSimulator����,������ͷ��ɹ��ؽ�Message���͵�����㣬����true;�����Ϊ���ͷ����ڵȴ���ȷȷ��״̬���ܾ�����Message���򷵻�false
	void receive(const Packet& ackPkt);						//����ȷ��Ack������NetworkServiceSimulator����	
	void timeoutHandler(int seqNum);					//Timeout handler������NetworkServiceSimulator����
	void print();

public:
	GBNRdtSender();
	virtual ~GBNRdtSender();
};

#endif