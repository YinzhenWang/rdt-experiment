#ifndef SR_RDT_SENDER_H
#define SR_RDT_SENDER_H
#include "RdtSender.h"

struct SRPacket
{
	struct Packet Pac;
	bool status;

	SRPacket():Pac(){
		status = false;
	}
	~SRPacket() {}
};


class SRRdtSender :public RdtSender
{
private://ɾȥwaitingState
	int nextSeqnum;			// ��һ��������� 
	static int windowSize;
	list <SRPacket> window;	//windowSize��С��list
	int base;

public:

	bool getWaitingState();
	bool send(const Message& message);						//����Ӧ�ò�������Message����NetworkServiceSimulator����,������ͷ��ɹ��ؽ�Message���͵�����㣬����true;�����Ϊ���ͷ����ڵȴ���ȷȷ��״̬���ܾ�����Message���򷵻�false
	void receive(const Packet& ackPkt);						//����ȷ��Ack������NetworkServiceSimulator����	
	void timeoutHandler(int seqNum);					//Timeout handler������NetworkServiceSimulator����
	void print();
	bool inList(int seqnum);		//�淶�������ж��ڲ����ѷ���list��

public:
	SRRdtSender();
	virtual ~SRRdtSender();
};

#endif