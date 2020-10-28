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
		cout << "---------���շ��������ڿ�ʼ��ӡ---------" << endl;
		cout << "��ǰbase:" << this->base << endl;
		list <Packet> ::iterator it4window;
		int i = 0;
		for (it4window = this->window.begin(); it4window != this->window.end(); it4window++, i++)
		{
			cout << "��������" << i;
			pUtils->printPacket("����", *it4window);
		}//�õ���������
		while (i != this->windowSize)
		{
			cout << "��������" << i << "Ϊ��" << endl;
			i++;
		}
		cout << "---------���շ��������ڴ�ӡ����---------" << endl;
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
		/*ģ�ļ�����ֻ��С��0ʱ�ż���ģ��������0 ���ӣ�*/
			return seqnum - this->base;
		else return seqnum - this->base + 8;
	}




	void SRRdtReceiver::receive(const Packet& packet) {
		//���У����Ƿ���ȷ
		int checkSum = pUtils->calculateCheckSum(packet);

		//���У�����ȷ�����Ƿ����յ�������ȷ�ϱ���
		if (checkSum == packet.checksum ) {
			pUtils->printPacket("���շ���ȷ�յ����ͷ��ı���", packet);

			Packet AckPkt;
			AckPkt.acknum = packet.seqnum; //ȷ����ŵ����յ��ı������
			for (int i = 0; i < Configuration::PAYLOAD_SIZE; i++) {
				AckPkt.payload[i] = '.'; //��ֹ����
			}
			AckPkt.checksum = pUtils->calculateCheckSum(AckPkt);
			pUtils->printPacket("���շ�����ȷ�ϱ���", AckPkt);
			pns->sendToNetworkLayer(SENDER, AckPkt);	//����ģ�����绷����sendToNetworkLayer��ͨ������㷢��ȷ�ϱ��ĵ��Է�

			if (IncludeBase(packet.seqnum))	//���seqnum�ڴ����У����ж��Ƿ���Ҫ������߽���
			{
				cout << "hey" << IncludeBase(packet.seqnum) << endl;
				if (!this->window.size())
					this->window.push_front(packet);
				else {
					list <Packet> ::iterator it4window;
					for (it4window = this->window.begin(); it4window != this->window.end(); it4window++) {
						if (Order(packet.seqnum) == Order(it4window->seqnum)) {
							cout << "���յ���" << endl;
							return;
						}
						else if (Order(packet.seqnum) < Order(it4window->seqnum) ) { 
							cout << "�������" << endl;
							this->window.insert(it4window, packet);
							break;
						}
					}
					if(it4window==this->window.end())
						this->window.push_back(packet); //���ֱ��push�����λ��
				}
				this->print();
				if (packet.seqnum == this->base)	//��������base��ͬ����packet�����л����packet���򽻸�Ӧ�ò�
				{
					//ȡ��Message�����ϵݽ���Ӧ�ò�
					Message msg;
					for (; this->window.size()!=0 && this->window.front().seqnum == this->base; )
					{
						memcpy(msg.data, this->window.front().payload, sizeof(this->window.front().payload));
						pns->delivertoAppLayer(RECEIVER, msg);
						this->window.pop_front();
						this->base = (this->base + 1) % 8;
						cout << "���շ��ɹ����ϲ��ϴ�һ������" << endl;
						this->print();
					}
				}
			}
		}
		else {
			if (checkSum != packet.checksum) {
				pUtils->printPacket("���շ�û����ȷ�յ����ͷ��ı���,����У�����", packet);
			}
			else {
				pUtils->printPacket("���շ�û����ȷ�յ����ͷ��ı���,������Ų���", packet);
			}
		}
	}
#endif