#include "stdafx.h"
#ifndef NONE_SR_RDT_SENDER_MODE	
	#include "SRRdtReceiver.h"
	#include "Global.h"

	int SRRdtReceiver::windowSize = 4;


	SRRdtReceiver::SRRdtReceiver() :base(0)
	{
	}

	SRRdtReceiver::~SRRdtReceiver()
	{
	}



	void SRRdtReceiver::print()
	{
		cout << "---------接收方滑动窗口开始打印---------" << endl;
		cout << "当前base:" << this->base << endl;
		list <Packet> ::iterator it4window;
		int i = 0;
		for (it4window = this->window.begin(); it4window != this->window.end(); it4window++, i++)
		{
			cout << "滑动窗口" << i;
			pUtils->printPacket("报文", *it4window);
		}//用迭代器遍历
		while (i != this->windowSize)
		{
			cout << "滑动窗口" << i << "为空" << endl;
			i++;
		}
		cout << "---------接收方滑动窗口打印结束---------" << endl;
		return;
	}

	bool SRRdtReceiver::IncludeBase(int seqnum) {
		for (int i = 0; i < this->windowSize; i++)
			if ( seqnum == (this->base + i) % 8 ) {
				return true;
			}
		return false;
	}

	int SRRdtReceiver::Order(int seqnum) {
		if (seqnum - this->base >=0)
		/*模的减法，只有小于0时才加上模数，等于0 不加！*/
			return seqnum - this->base;
		else return seqnum - this->base + 8;
	}




	void SRRdtReceiver::receive(const Packet& packet) {
		//检查校验和是否正确
		int checkSum = pUtils->calculateCheckSum(packet);

		//如果校验和正确不管是否已收到都发送确认报文
		if (checkSum == packet.checksum ) {
			pUtils->printPacket("接收方正确收到发送方的报文", packet);

			Packet AckPkt;
			AckPkt.acknum = packet.seqnum; //确认序号等于收到的报文序号
			for (int i = 0; i < Configuration::PAYLOAD_SIZE; i++) {
				AckPkt.payload[i] = '.'; //防止乱码
			}
			AckPkt.checksum = pUtils->calculateCheckSum(AckPkt);
			pUtils->printPacket("接收方发送确认报文", AckPkt);
			pns->sendToNetworkLayer(SENDER, AckPkt);	//调用模拟网络环境的sendToNetworkLayer，通过网络层发送确认报文到对方

			if (IncludeBase(packet.seqnum))	//如果seqnum在窗口中，就判断是否需要缓存或者交送
			{
				cout << "hey" << IncludeBase(packet.seqnum) << endl;
				if (!this->window.size())
					this->window.push_front(packet);
				else {
					list <Packet> ::iterator it4window;
					for (it4window = this->window.begin(); it4window != this->window.end(); it4window++) {
						if (Order(packet.seqnum) == Order(it4window->seqnum)) {
							cout << "已收到过" << endl;
							return;
						}
						else if (Order(packet.seqnum) < Order(it4window->seqnum) ) { 
							cout << "按序插入" << endl;
							this->window.insert(it4window, packet);
							break;
						}
					}
					if(it4window==this->window.end())
						this->window.push_back(packet); //最后直接push到最后位置
				}
				this->print();
				if (packet.seqnum == this->base)	//如果序号与base相同，则将packet与所有缓存的packet按序交付应用层
				{
					//取出Message，向上递交给应用层
					Message msg;
					for (; this->window.size()!=0 && this->window.front().seqnum == this->base; )
					{
						memcpy(msg.data, this->window.front().payload, sizeof(this->window.front().payload));
						pns->delivertoAppLayer(RECEIVER, msg);
						this->window.pop_front();
						this->base = (this->base + 1) % 8;
						cout << "接收方成功向上层上传一个报文" << endl;
						this->print();
					}
				}
			}
		}
		else {
			if (checkSum != packet.checksum) {
				pUtils->printPacket("接收方没有正确收到发送方的报文,数据校验错误", packet);
			}
			else {
				pUtils->printPacket("接收方没有正确收到发送方的报文,报文序号不对", packet);
			}
		}
	}
#endif