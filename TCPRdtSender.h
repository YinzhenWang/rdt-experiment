#ifndef TCP_RDT_SENDER_H
#define TCP_RDT_SENDER_H
#include "RdtSender.h"

class TCPRdtSender :public RdtSender
{
private://ɾȥwaitingState��GBN�жϷ��ͷ�״̬���������ǿ�windowSize
	int nextSeqnum;			// ��һ��������� 
	static int windowSize;
	queue <Packet> window;	//windowSize��С��queue
	int base;						//��¼���ʹ��ڵĵ�ǰ���
	int AckAccount;         //��¼��ǰ�յ���ack

public:

	bool getWaitingState();
	bool send(const Message& message);						//����Ӧ�ò�������Message����NetworkServiceSimulator����,������ͷ��ɹ��ؽ�Message���͵�����㣬����true;�����Ϊ���ͷ����ڵȴ���ȷȷ��״̬���ܾ�����Message���򷵻�false
	void receive(const Packet& ackPkt);						//����ȷ��Ack������NetworkServiceSimulator����	
	void timeoutHandler(int seqNum);					//Timeout handler������NetworkServiceSimulator����
	void print();

public:
	TCPRdtSender();
	virtual ~TCPRdtSender();
};

#endif