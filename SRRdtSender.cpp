#include "stdafx.h"
#ifndef NONE_SR_RDT_SENDER_MODE
	#include "Global.h"
	#include "SRRdtSender.h"


	int SRRdtSender::windowSize = 4;//实验书建议

	SRRdtSender::SRRdtSender() :nextSeqnum(0), base(0)
	{
	}

	SRRdtSender::~SRRdtSender()
	{
	}

	void SRRdtSender::print()
	{
		cout << "---------发送方滑动窗口开始打印---------" << endl;
		list <SRPacket> ::iterator it4window;
		int i = 0;
		for (it4window = this->window.begin(); it4window!= this->window.end(); it4window++,i++)
		{
			cout << "滑动窗口" << i;
			cout <<"报文状态:" << it4window->status;	//打应ack状态
			pUtils->printPacket("报文", it4window->Pac);
		}//用迭代器遍历
		while (i != this->windowSize)
		{
			cout << "滑动窗口" << i << "为空" << endl;
			i++;
		}
		cout << "---------发送方滑动窗口打印结束---------" << endl;
		return;
	}

	bool SRRdtSender::inList(int seqnum) {
		if (this->window.size()) {
			list <SRPacket> ::iterator it4window;
			for (it4window = this->window.begin(); it4window != this->window.end(); it4window++) {
				if (it4window->Pac.seqnum == seqnum && it4window->status == false) {
					it4window->status = true;//在这里改变status节省判断后再遍历的成本

					return true;
				}
			}
		}

		return false;
	}

	bool SRRdtSender::getWaitingState() {

		if (this->window.size() == this->windowSize)
			return true;
		else
			return false;
	}



	bool SRRdtSender::send(const Message& message) {
		if (this->window.size() == this->windowSize) {
			return false;
		}
		else {			
			SRPacket temp4insert = SRPacket();
			temp4insert.Pac.acknum = -1;
			temp4insert.Pac.seqnum = this->nextSeqnum;
			temp4insert.Pac.checksum = 0;
			memcpy(temp4insert.Pac.payload, message.data, sizeof(message.data));
			temp4insert.Pac.checksum = pUtils->calculateCheckSum(temp4insert.Pac);
			this->window.push_back(temp4insert);
			pUtils->printPacket("发送方发送报文", this->window.back().Pac);
			this->print();
			
			pns->startTimer(SENDER, Configuration::TIME_OUT, this->window.back().Pac.seqnum);	///启动发送方定时器

			this->nextSeqnum = (this->nextSeqnum + 1) % 8;
			pns->sendToNetworkLayer(RECEIVER, this->window.back().Pac);	//调用模拟网络环境的sendToNetworkLayer，通过网络层发送到对方

			return true;
		}
	}

	void SRRdtSender::receive(const Packet& ackPkt) {
		//检查校验和是否正确
		if (this->window.size()) {
			int checkSum = pUtils->calculateCheckSum(ackPkt);

			//如果校验和正确,并且确认序号=发送方已发送并等待确认的数据包序号
			if (checkSum == ackPkt.checksum) {
				pUtils->printPacket("发送方正确收到确认", ackPkt);
				if (inList(ackPkt.acknum))//判断是否在list中
				{
					pns->stopTimer(SENDER, ackPkt.acknum);	//关闭定时器
					while (this->window.size() != 0 && this->window.front().status == true)//链表头结点ture则pop
					{
						this->window.pop_front();
						this->base = (this->base + 1) % 8;
					}
					this->print();	//由于inList只有在收到的确认报文序号在发送窗口中并且不是冗余确认时才返回true，因此只要打印，要么窗口滑动，要么有窗口中报文得到确认并停止计时
				}
				//由于每个已发送报文都有单独的计时器，因此不用再像GBN一样判断是否重启计时器
				else 
					cout << "发送方收到重复确认" << endl;
				
			}
		}
		
	}

	void SRRdtSender::timeoutHandler(int seqNum) {
		//定时器不唯一，需要考虑seqNum
		cout << "发送方序号" << seqNum << "的报文定时器时间到，重发该报文" << endl;
		list <SRPacket> ::iterator it4window;
		for (it4window = this->window.begin(); it4window != this->window.end(); it4window++) {
			if (it4window->Pac.seqnum == seqNum) {
				pUtils->printPacket("报文", it4window->Pac);
				pns->stopTimer(SENDER, seqNum);										//首先关闭定时器
				pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum);			//重新启动发送方定时器
				pns->sendToNetworkLayer(RECEIVER, it4window->Pac);
				break;
			}
		}
	}
#endif
