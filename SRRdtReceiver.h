#ifndef SR_RDT_RECEIVER_H			//保证头文件的内容相同项只被包含一次，可与上行同时使用
#define SR_RDT_RECEIVER_H			
#include "RdtReceiver.h"

class SRRdtReceiver : public RdtReceiver
{
private:
	static int windowSize;          //接收方窗口大小
	list <Packet> window;	   //windowSize大小的list
	int base;						//记录接收窗口的当前序号

public:
	SRRdtReceiver();
	virtual ~SRRdtReceiver();
	bool IncludeBase(int seqnum);
	int Order(int seqnum);	//和base进行模运算差得到真正的order
	void print(void);	//打印滑动窗口

public:

	void receive(const Packet& packet);	//接收报文，将被NetworkService调用
};

#endif