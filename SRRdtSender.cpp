#include "stdafx.h"
#ifndef NONE_SR_RDT_SENDER_MODE
	#include "Global.h"
	#include "SRRdtSender.h"


	int SRRdtSender::windowSize = 4;//ʵ���齨��

	SRRdtSender::SRRdtSender() :nextSeqnum(0), base(0)
	{
	}

	SRRdtSender::~SRRdtSender()
	{
	}

	void SRRdtSender::print()
	{
		cout << "---------���ͷ��������ڿ�ʼ��ӡ---------" << endl;
		list <SRPacket> ::iterator it4window;
		int i = 0;
		for (it4window = this->window.begin(); it4window!= this->window.end(); it4window++,i++)
		{
			cout << "��������" << i;
			cout <<"����״̬:" << it4window->status;	//��Ӧack״̬
			pUtils->printPacket("����", it4window->Pac);
		}//�õ���������
		while (i != this->windowSize)
		{
			cout << "��������" << i << "Ϊ��" << endl;
			i++;
		}
		cout << "---------���ͷ��������ڴ�ӡ����---------" << endl;
		return;
	}

	bool SRRdtSender::inList(int seqnum) {
		if (this->window.size()) {
			list <SRPacket> ::iterator it4window;
			for (it4window = this->window.begin(); it4window != this->window.end(); it4window++) {
				if (it4window->Pac.seqnum == seqnum && it4window->status == false) {
					it4window->status = true;//������ı�status��ʡ�жϺ��ٱ����ĳɱ�

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
			pUtils->printPacket("���ͷ����ͱ���", this->window.back().Pac);
			this->print();
			
			pns->startTimer(SENDER, Configuration::TIME_OUT, this->window.back().Pac.seqnum);	///�������ͷ���ʱ��

			this->nextSeqnum = (this->nextSeqnum + 1) % 8;
			pns->sendToNetworkLayer(RECEIVER, this->window.back().Pac);	//����ģ�����绷����sendToNetworkLayer��ͨ������㷢�͵��Է�

			return true;
		}
	}

	void SRRdtSender::receive(const Packet& ackPkt) {
		//���У����Ƿ���ȷ
		if (this->window.size()) {
			int checkSum = pUtils->calculateCheckSum(ackPkt);

			//���У�����ȷ,����ȷ�����=���ͷ��ѷ��Ͳ��ȴ�ȷ�ϵ����ݰ����
			if (checkSum == ackPkt.checksum) {
				pUtils->printPacket("���ͷ���ȷ�յ�ȷ��", ackPkt);
				if (inList(ackPkt.acknum))//�ж��Ƿ���list��
				{
					pns->stopTimer(SENDER, ackPkt.acknum);	//�رն�ʱ��
					while (this->window.size() != 0 && this->window.front().status == true)//����ͷ���ture��pop
					{
						this->window.pop_front();
						this->base = (this->base + 1) % 8;
					}
					this->print();	//����inListֻ�����յ���ȷ�ϱ�������ڷ��ʹ����в��Ҳ�������ȷ��ʱ�ŷ���true�����ֻҪ��ӡ��Ҫô���ڻ�����Ҫô�д����б��ĵõ�ȷ�ϲ�ֹͣ��ʱ
				}
				//����ÿ���ѷ��ͱ��Ķ��е����ļ�ʱ������˲�������GBNһ���ж��Ƿ�������ʱ��
				else 
					cout << "���ͷ��յ��ظ�ȷ��" << endl;
				
			}
		}
		
	}

	void SRRdtSender::timeoutHandler(int seqNum) {
		//��ʱ����Ψһ����Ҫ����seqNum
		cout << "���ͷ����" << seqNum << "�ı��Ķ�ʱ��ʱ�䵽���ط��ñ���" << endl;
		list <SRPacket> ::iterator it4window;
		for (it4window = this->window.begin(); it4window != this->window.end(); it4window++) {
			if (it4window->Pac.seqnum == seqNum) {
				pUtils->printPacket("����", it4window->Pac);
				pns->stopTimer(SENDER, seqNum);										//���ȹرն�ʱ��
				pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum);			//�����������ͷ���ʱ��
				pns->sendToNetworkLayer(RECEIVER, it4window->Pac);
				break;
			}
		}
	}
#endif
