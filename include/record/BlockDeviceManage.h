
#ifndef __BLOCK_DEVICE_MANAGE_H__

#define __BLOCK_DEVICE_MANAGE_H__

#include "common.h"

typedef enum
{
	BLK_DEV_ID_HDD00 = 0,			//�̶��Ĵ洢�豸��HDD,SD,TF��
	BLK_DEV_ID_HDD01,
	BLK_DEV_ID_HDD02,
	BLK_DEV_ID_HDD03,
	BLK_DEV_ID_UDISK0,				//�ɲ�εĴ洢�豸��u�̣��ݲ�֧��
	BLK_DEV_ID_UDISK1,
	BLK_DEV_ID_MAX_NUM			//���֧�ֿ��豸������
}BLK_DEV_ID;

typedef enum
{
	BLK_DEV_STATUS_NOT_EXIST = 0,	 //�ÿ��豸������
	BLK_DEV_STATUS_NOT_MOUNTE,	 //�ÿ��豸��ʽ����ɵ�δ����	
	BLK_DEV_STATUS_MOUNTED,		 //�ÿ��豸�Ѿ�����	
	BLK_DEV_STATUS_FORMATING	 //�ÿ��豸���ڸ�ʽ��	
}BLK_DEV_STATUS;

typedef struct
{
	BLK_DEV_STATUS enBlkDevStatus;	//���豸״̬
	char 	strDeviceNode[32];		//�豸�ڵ�,��ʽ:/dev/sda1
	char		strMountPoint[32];		//���ص㣬��ʽ:/hdd00/p01
	unsigned long u32TotalCapacityMB;	//������,	��λ:MB
	unsigned long u32FreeSpaceMB;	//ʣ��ռ�,��λ:MB
}BLK_DEV_INFO;


class BlockDevManage
{

public:
	static BlockDevManage * m_pInstance;
	int					s32BlkDevNum;							//���ֵĿ��豸����	
	BLK_DEV_INFO		stBlkDevInfo[BLK_DEV_ID_MAX_NUM];		//���豸��Ϣ
	int ProbeSDCard();
	
public:	
	BlockDevManage();	
	~BlockDevManage();
	static BlockDevManage * GetInstance();	
	int GetBlockDeviceNum();	
	int Fat32Format(BLK_DEV_ID enBlkDevId);	
	int MountBlockDevice(BLK_DEV_ID enBlkDevId);
	int UmountBlockDevice(BLK_DEV_ID enBlkDevId);
	int GetBlockDeviceInfo(BLK_DEV_ID enBlkDevId, BLK_DEV_INFO *pstBlkDevInfo);	
};

#endif 


