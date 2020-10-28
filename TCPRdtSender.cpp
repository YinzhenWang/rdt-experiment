#include "stdafx.h"
#ifndef NONE_TCP_RDT_SENDER_MODE
	#include "Global.h"
	#include "TCPRdtSender.h"



	int TCPRdtSender::windowSize = 4;//实验书建议


	TCPRdtSender::TCPRdtSender() :nextSeqnum(0),base(0),AckAccount(0)
	{
	}

	TCPRdtSender::~TCPRdtSender()
	{
	}

	void TCPRdtSender::print()
	{
		cout << "---------滑动窗口开始打印---------" << endl;
		int i;
		for (i = 0; i < this->window.size(); i++)
		{
			Packet temp = this->window.front();
			cout << "滑动窗口" << i;
			pUtils->printPacket("报文", temp);
			this->window.pop();
			this->window.push(temp);
		}//queue不能用迭代器遍历，只能用temp去pop和push来遍历
		while (i != this->windowSize)
		{
			cout << "滑动窗口" << i << "为空" << endl;
			i++;
		}
		cout << "---------滑动窗口打印结束---------" << endl;
		return;
	}


	//实验书提示不能简单判断大小，因为伪代码中的base不取余，这里通过比较窗口大小情况实现
	bool TCPRdtSender::getWaitingState() {
		if (this->window.size() == this->windowSize)
			return true;
		else
			return false;
	}


	bool TCPRdtSender::send(const Message& message) {
		//如果窗口已满，必须等到ACK才能发送，则返回false
		if (this->window.size() == this->windowSize) {
			return false;
		}
		else {
			Packet temp4insert;//内容很小没必要动态申请内存，也方便生命周期结束自动析构
			temp4insert.acknum = -1;//忽略该字段
			temp4insert.seqnum = this->nextSeqnum;
			temp4insert.checksum = 0;
			memcpy(temp4insert.payload, message.data, sizeof(message.data));
			temp4insert.checksum = pUtils->calculateCheckSum(temp4insert);
			this->window.push(temp4insert);
			pUtils->printPacket("发送方发送报文", this->window.back());

			this->print();
			if (this->base == this->nextSeqnum)
				pns->startTimer(SENDER, Configuration::TIME_OUT, this->base); //启动发送方定时器

			this->nextSeqnum = (this->nextSeqnum + 1) % 8;

			pns->sendToNetworkLayer(RECEIVER, temp4insert);	//调用模拟网络环境的sendToNetworkLayer，通过网络层发送到对方


			return true;
		}
	}

	void TCPRdtSender::receive(const Packet& ackPkt) {
		if (this->window.size()) {
			//检查校验和是否正确
			int checkSum = pUtils->calculateCheckSum(ackPkt);

			//如果校验和正确,并且确认序号=发送方已发送并等待确认的数据包序号
			if (checkSum == ackPkt.checksum) {
				pUtils->printPacket("发送方正确收到确认", ackPkt);
				if (this->base != ackPkt.acknum)
				{
					this->AckAccount = 0;	//窗口移动，将冗余ACK数归零
					pns->stopTimer(SENDER, this->base);		//关闭定时器
					this->base = ackPkt.acknum; //TCP的ACK与GBN不同，本身就是下一个期待值，修订任务书？
					if (this->window.front().seqnum != this->base) {
						while (this->window.size() != 0 && this->window.front().seqnum != this->base)
							this->window.pop();//出队到队首序号为base，即下一个需要的包
						this->print();
					}
					if (this->window.size())
						pns->startTimer(SENDER, Configuration::TIME_OUT, this->base);//如果窗口中还有待确认的报文，就重新启动发送方定时器
				}
				else
				{
					this->AckAccount++;
					cout << "发送方已经收到" << AckAccount << "个冗余ACK" << endl << endl;

					if (this->AckAccount == 3)	//三次冗余，第四次重发
					{
						cout << "重发窗口内所有的报文" << endl << endl;
						int i;
						for (i = 0; i < this->window.size(); i++)
						{
							Packet temp = this->window.front();
							cout << "滑动窗口" << i;
							pUtils->printPacket("报文", temp);
							this->window.pop();
							this->window.push(temp);
						}//queue不能用迭代器遍历，只能用temp去pop和push来遍历

						pns->stopTimer(SENDER, this->base);										//首先关闭定时器
						pns->startTimer(SENDER, Configuration::TIME_OUT, this->base);			//重新启动发送方定时器

						for (i = 0; i < this->window.size(); i++)
						{
							Packet temp = this->window.front();
							pns->sendToNetworkLayer(RECEIVER, temp);
							this->window.pop();
							this->window.push(temp);
						}//queue不能用迭代器遍历，只能用temp去pop和push来遍历

						this->AckAccount = 0;	//全部重传后，将冗余ACK的数量归0
					}
				}

			}
			else
				pUtils->printPacket("发送方没有正确收到确认", ackPkt);
		}
		
	}

	void TCPRdtSender::timeoutHandler(int seqNum) {
		//唯一一个定时器,无需考虑seqNum,且仅重传最早发送且没被确认的报文
		cout << "发送方定时器时间到，重发窗口内的base报文" << endl;
		pUtils->printPacket("报文：", this->window.front());

		pns->stopTimer(SENDER, seqNum);										//首先关闭定时器
		pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum);			//重新启动发送方定时器
	
		pns->sendToNetworkLayer(RECEIVER, this->window.front());			//重新发送窗口中的base对应数据包

	}
#endif
