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
private://删去waitingState
	int nextSeqnum;			// 下一个发送序号 
	static int windowSize;
	list <SRPacket> window;	//windowSize大小的list
	int base;

public:

	bool getWaitingState();
	bool send(const Message& message);						//发送应用层下来的Message，由NetworkServiceSimulator调用,如果发送方成功地将Message发送到网络层，返回true;如果因为发送方处于等待正确确认状态而拒绝发送Message，则返回false
	void receive(const Packet& ackPkt);						//接受确认Ack，将被NetworkServiceSimulator调用	
	void timeoutHandler(int seqNum);					//Timeout handler，将被NetworkServiceSimulator调用
	void print();
	bool inList(int seqnum);		//规范函数，判断在不在已发送list中

public:
	SRRdtSender();
	virtual ~SRRdtSender();
};

#endif