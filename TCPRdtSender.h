#ifndef TCP_RDT_SENDER_H
#define TCP_RDT_SENDER_H
#include "RdtSender.h"

class TCPRdtSender :public RdtSender
{
private://删去waitingState，GBN判断发送方状态不靠它而是靠windowSize
	int nextSeqnum;			// 下一个发送序号 
	static int windowSize;
	queue <Packet> window;	//windowSize大小的queue
	int base;						//记录发送窗口的当前序号
	int AckAccount;         //记录当前收到的ack

public:

	bool getWaitingState();
	bool send(const Message& message);						//发送应用层下来的Message，由NetworkServiceSimulator调用,如果发送方成功地将Message发送到网络层，返回true;如果因为发送方处于等待正确确认状态而拒绝发送Message，则返回false
	void receive(const Packet& ackPkt);						//接受确认Ack，将被NetworkServiceSimulator调用	
	void timeoutHandler(int seqNum);					//Timeout handler，将被NetworkServiceSimulator调用
	void print();

public:
	TCPRdtSender();
	virtual ~TCPRdtSender();
};

#endif