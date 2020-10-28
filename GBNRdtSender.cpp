#include "stdafx.h"
#ifndef NONE_GBN_RDT_SENDER_MODE
	#include "Global.h"
	#include "GBNRdtSender.h"


	int GBNRdtSender::windowSize = 4;//ʵ���齨��

	GBNRdtSender::GBNRdtSender() :nextSeqnum(0),base(0)		
	{
	}

	GBNRdtSender::~GBNRdtSender()
	{
	}

	void GBNRdtSender::print()
	{
		cout << "---------�������ڿ�ʼ��ӡ---------" << endl;
		int i;
		for (i = 0; i<this->window.size(); i++)
		{
			Packet temp = this->window.front();
			cout << "��������" << i;
			pUtils->printPacket("����", temp);
			this->window.pop();
			this->window.push(temp);
		}//queue�����õ�����������ֻ����tempȥpop��push������
		while (i != this->windowSize)
		{
			cout << "��������" << i << "Ϊ��" << endl;
			i++;
		}
		cout << "---------�������ڴ�ӡ����---------" << endl;
		return;
	}


	//ʵ������ʾ���ܼ��жϴ�С����Ϊα�����е�base��ȡ�࣬����ͨ���Ƚϴ��ڴ�С���ʵ��
	bool GBNRdtSender::getWaitingState() {
		if (this->window.size() == this->windowSize)
			return true;
		else
			return false;
	}
	

	bool GBNRdtSender::send(const Message& message) {
		//�����������������ȵ�ACK���ܷ��ͣ��򷵻�false
		if (this->window.size() == this->windowSize) {
			return false;
		}
		else {
			Packet temp4insert;//���ݺ�Сû��Ҫ��̬�����ڴ棬Ҳ�����������ڽ����Զ�����
			temp4insert.acknum = -1;//���Ը��ֶ�
			temp4insert.seqnum = this->nextSeqnum;
			temp4insert.checksum = 0;
			memcpy(temp4insert.payload, message.data, sizeof(message.data));
			temp4insert.checksum = pUtils->calculateCheckSum(temp4insert);
			this->window.push(temp4insert);
			pUtils->printPacket("���ͷ����ͱ���", this->window.back());
			
			this->print();
			if (this->base==this->nextSeqnum)
				pns->startTimer(SENDER, Configuration::TIME_OUT, this->base); //�������ͷ���ʱ��

			this->nextSeqnum = (this->nextSeqnum + 1) % 8; 
			
			pns->sendToNetworkLayer(RECEIVER, temp4insert);	//����ģ�����绷����sendToNetworkLayer��ͨ������㷢�͵��Է�
			
	
			return true;
		}
	}

	void GBNRdtSender::receive(const Packet& ackPkt) {
		if (this->window.size()) {
			//���У����Ƿ���ȷ
			int checkSum = pUtils->calculateCheckSum(ackPkt);

			//���У�����ȷ,����ȷ�����=���ͷ��ѷ��Ͳ��ȴ�ȷ�ϵ����ݰ����
			if (checkSum == ackPkt.checksum) {
				pUtils->printPacket("���ͷ���ȷ�յ�ȷ��", ackPkt);
				pns->stopTimer(SENDER, this->base);		//�رն�ʱ��
				this->base = (ackPkt.acknum + 1) % 8;
				if (this->window.front().seqnum != this->base) {
					while (this->window.size() != 0 && this->window.front().seqnum != this->base)
						this->window.pop();//���ӵ��������Ϊbase������һ����Ҫ��ack
					this->print();
				}
				if (this->window.size())
					pns->startTimer(SENDER, Configuration::TIME_OUT, this->base);//��������л��д�ȷ�ϵı��ģ��������������ͷ���ʱ��
			}
			else
				pUtils->printPacket("���ͷ�û����ȷ�յ�ȷ��", ackPkt);
		}
	}
		

	void GBNRdtSender::timeoutHandler(int seqNum) {
		//Ψһһ����ʱ��,���迼��seqNum
		cout << "���ͷ���ʱ��ʱ�䵽���ط��������б���" << endl;
		int i;
		for (i = 0; i < this->window.size(); i++)
		{
			Packet temp = this->window.front();
			cout << "��������" << i;
			pUtils->printPacket("����", temp);
			this->window.pop();
			this->window.push(temp);
		}//queue�����õ�����������ֻ����tempȥpop��push������

		pns->stopTimer(SENDER, seqNum);										//���ȹرն�ʱ��
		pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum);			//�����������ͷ���ʱ��
	
		for (i = 0; i < this->window.size(); i++)
		{
			Packet temp = this->window.front();
			pns->sendToNetworkLayer(RECEIVER, temp);
			this->window.pop();
			this->window.push(temp);
		}//queue�����õ�����������ֻ����tempȥpop��push������

	}
#endif
