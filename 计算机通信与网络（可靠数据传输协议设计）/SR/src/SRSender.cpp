#include "Global.h"
#include "SRSender.h"
SRSender::SRSender():base(0),nextseqnum(0),waitingState(false),num_pac_win(0)
{
}
SRSender::~SRSender()
{
}
bool SRSender::getWaitingState()
{
    return waitingState;
}
bool SRSender::send(const struct Message & message)
{
    if(num_pac_win < len/2)//�п�λ
    {
        if(num_pac_win == 0)
            for(int i = 0; i<len/2;i++)
                this->cache[i] = 0;//��ʼ��cache

        this->waitingState = false;

        this->win[this->num_pac_win].acknum = -1;
        this->win[this->num_pac_win].seqnum = this->nextseqnum;//�����0~7
        this->win[this->num_pac_win].checksum = 0;
        memcpy(this->win[this->num_pac_win].payload, message.data, sizeof(message.data));//��Ӧ�ò��ȡ����
        this->win[this->num_pac_win].checksum = pUtils->calculateCheckSum(this->win[this->num_pac_win]);//��������

        pUtils->printPacket("���ͷ����ͱ���",this->win[this->num_pac_win]);
        pns->startTimer(SENDER,Configuration::TIME_OUT, this->win[this->num_pac_win].seqnum);//ÿ�����һ����ʱ��
        pns->sendToNetworkLayer(RECEIVER,this->win[this->num_pac_win]);//�����������

        this->num_pac_win ++;//�����ڱ���������һ
        this->nextseqnum = (this->nextseqnum + 1) % len;

        if(this->num_pac_win == len / 2)
            this->waitingState = true;//����

        return true;
    }

    else//û��λ���� false
    {
        this->waitingState = true;
        return false;
    }
}

void SRSender::receive(const struct Packet & ackPkt)
{
        int sign = 0;
        int check_sum = pUtils->calculateCheckSum(ackPkt);//�������

        if(check_sum == ackPkt.checksum)//��ȷ����
        {
            int check = 0;
            for(int i = 0; i < num_pac_win ; i++)//�������ʹ��ڣ���û��ack��Ӧ�����
                if(ackPkt.acknum == win[i].seqnum )
                {
                    cache[i] = 1;
                    sign = i;
                    check = 1;
                }//�������ж�Ӧ��seqnum��cache�ж�Ӧ�Ĵ����±�ָʾ�Ĳ�����1

                if(check == 1)
                {
                    if(cache[0] == 1)//���ack���Ϊ�±�
                    {
                        pUtils->printPacket("�յ�base��ACK",ackPkt);
                        int valid = 0;
                        pns->stopTimer(SENDER,this->win[0].seqnum);//��ͣʱ��
                        for(int i = 0; i < len/2; i++)//�жϴ�base��ʼ�ж��ٸ��Ѿ�ack�˵�
                        {
                            if(cache[i] == 0)
                                break;
                            else valid++;
                        }
                        if(valid != this->num_pac_win)
                        {
                            for(int i = 0; i < this->num_pac_win - valid ; i++)
                            {
                                this->win[i] = this->win[i + valid];
                                this->cache[i] = this->cache[i + valid];
                                printf("The current windows's %d number is %d\n",i,win[i].seqnum);
                            }
                            for (int j = this->num_pac_win - valid; j < len/2; j++)
                                cache[j] = 0;
                        }
                        else{
                            for (int i = 0; i < len / 2; i++)
                                cache[i] = 0;
                        }
                        this->num_pac_win = this->num_pac_win - valid;
                        this->base = (this->base + valid) % len;//��Ϊack����base������base��ǰ�ƶ���
                        this->waitingState = false;
                    }
                    else
                    {
                        pns->stopTimer(SENDER,this->win[sign].seqnum);//�յ�����BASE��ACK��ֹֻͣʱ��
                        pUtils->printPacket("�յ������ڷ�base��ACK",ackPkt);

                    }
                }
                else
                {
                    pns->stopTimer(SENDER,ackPkt.acknum);
                    pUtils->printPacket("�յ�����֮ǰ���ĵ�ACK",ackPkt);
                }
        }else
            pUtils->printPacket("�յ�ACK��У��ͳ���",ackPkt);
}


void SRSender::timeoutHandler(int seqNum)
{
    int i = 0;
    while(this->win[i].seqnum != seqNum)//ȷ�����ʹ��ڵĵڼ�����ʱ
        i++;
    pns->stopTimer(SENDER,seqNum);//���ӹر�
    pUtils->printPacket("���ͷ���ʱ����ʱ�����·��ͱ��Ķ�",this->win[i]);
    pns->sendToNetworkLayer(RECEIVER, this->win[i]);//���·���
    pns->startTimer(SENDER, Configuration::TIME_OUT,seqNum);//��������
}