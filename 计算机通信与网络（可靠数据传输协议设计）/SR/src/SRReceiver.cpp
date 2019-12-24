#include "Global.h"
#include "SRReceiver.h"
SRReceiver::SRReceiver():base(0),num_rcv(0)
{
    Ack.acknum = -1;
    Ack.checksum = 0;
    Ack.seqnum = -1;
    for(int i = 0; i < Configuration::PAYLOAD_SIZE; i++)
        Ack.payload[i] = '.';
    Ack.checksum = pUtils->calculateCheckSum(Ack);
}
SRReceiver::~SRReceiver()
{
}
void SRReceiver::receive(const struct Packet & packet)
{
    if(this->num_rcv == 0) {//�����շ�Ϊ������cache������
        for (int i = 0; i < len / 2; i++)
            cache[i] = 0;
    }
    int checksum = pUtils->calculateCheckSum(packet);
    if(checksum == packet.checksum )//�������
    {
        int check = 0;
        for(int i = 0; i< len/2;i++)//�ж�����Ƿ��ڴ����ڲ�
        {
            if(packet.seqnum == (base + i)% len)
            {
                check = 1;
                break;
            }
        }

        if(check == 1)//���ڽ��շ��Ĵ�������
        {
            for(int i = 0; i < len/2 ;i++)//�������ܴ��ڣ�������һ����ŵ�packet�յ������ж��Ƿ��Ѿ�����
                if(packet.seqnum == (this->base +i)%len) //���ҵ��˶�Ӧ�����
                {
                    cache[i] = 1;//���Ϊ�Ѿ�����
                    RcvWin[i] = packet;//������������ܴ���
                    pUtils->printPacket("���շ���ȷ�յ����ͷ��ı���",this->RcvWin[i]);
                    this->Ack.acknum = RcvWin[0].seqnum;
                    this->Ack.checksum = pUtils->calculateCheckSum(Ack);
                    pns->sendToNetworkLayer(SENDER,this->Ack);
                    pUtils->printPacket("���շ�������ȷ����",this->RcvWin[i]);
                }

            if(cache[0] == 1)//���յ��ı�����BASE
            {
                pUtils->printPacket("���շ���ȷ�յ�BASE�ı���",this->RcvWin[0]);
                int valid = 0;
                for(int i = 0; this->cache[i]!=0 ; i++)//�жϴ�base��ʼ�ж��ٸ��Ѿ�ack�˵�
                    valid++;
                for(int i = 0;i < valid; i++)//���ӷ��ͷ����������Ͻ������ҷ���ACK
                {
                    Message msg;
                    memcpy(msg.data, this->RcvWin[i].payload, sizeof(this->RcvWin[i].payload));
                    pns->delivertoAppLayer(RECEIVER,msg);
                }
                if(valid != this->num_rcv)//��������
                {
                    for(int i = 0; i < this->num_rcv - valid ; i++)
                    {
                        this->RcvWin[i] = this->RcvWin[i + valid];

                    }

                }
                this->num_rcv = this->num_rcv - valid;//�����ڱ��ļ���valid��
                this->base = (this->base + valid) % len;//��Ϊ��������base������base��ǰ�ƶ���
                for(int i = 0; i < len/2 ; i++)
                    cache[i] = 0;//��cache����0


            }
        }
        else //���յ���ACK��[base-len/2,base-1]֮�䣬��ʹ�Ѿ����͹�ACK�����ɷ���
        {
            pUtils->printPacket("WARNING:���շ��յ����ͷ��ķǴ����ڵı���",packet);
            this->Ack.acknum = packet.seqnum;
            this->Ack.checksum = pUtils->calculateCheckSum(Ack);
            pns->sendToNetworkLayer(SENDER,this->Ack);//����ACK
        }
    }

    else
    {
        pUtils->printPacket("ERROR:���շ�δ��ȷ�յ�����:����ʹ���",packet);
    }
}