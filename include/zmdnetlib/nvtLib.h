/*********************************************************************
 * Copyright (C), Zmodo Technology Corp., Ltd.
 *********************************************************************
 * File Name    : nvtLib.h
 * Description  : Interfaces for nvt library.
 * Author       : hulz <danielhu@zmodo.cn>
 * Create Date  : 2012-10-18
 ********************************************************************/

#ifndef __NVTLIB_H__
#define __NVTLIB_H__


#if defined (__cplusplus) || defined (_cplusplus)
extern "C" {
#endif

#define MAX_URI_SZ          256
#define MAX_UUID_SZ         64
#define MAX_IPV4_ADDR_SZ    16
#define MAX_MAC_ADDR_SZ     18


#define RTSP_PORT           10554   /* RTSP port. */
#define HTTP_PORT           10080   /* Web service port for NVT. */
#define BACKLOG             10      /* The maximum length to which the queue
                                     * of pending connections for sockfd may grow. */

int zmd_nvt_init_nvt_lib(int (*ctrl_dev)(int type, void *buf, int buf_sz),
                         int (*set_param)(int type, void *buf, int buf_sz),
                         int (*get_param)(int type, void *buf, int *buf_sz));
void zmd_nvt_deinit_nvt_lib(void);


/* NVT control types. */
enum nvt_ctrl_type
 {
    NVT_CTRL_REBOOT_SYSTEM  = 1,
    NVT_CTRL_UPGRADE_SYSTEM = 2,
    NVT_CTRL_SNAP_IMAGE = 3,
    NVT_CTRL_RE_IFRERAME = 4,
};

/* NVT control structures. */
typedef struct upgrade_info_s 
{
    char file_name[128];    /* Upgrade file name. */
    unsigned char *file_buf;         /* Buffer pointer to memory storing upgrade file. */
    unsigned int file_sz;   /* Size of the said buffer. */
} upgrade_info_t;


/* NVT parameter types. */
enum nvt_param_type
{
    NVT_PARAM_IMG_CFG       = 1,
    NVT_PARAM_NET_STATUS    = 2,
    NVT_PARAM_IPC_TYPE      = 3,
    NVT_PARAM_GET_NETWORK  	= 4,
    NVT_PARAM_SET_NETWORK	= 5,
    NVT_PARAM_GET_DEV		= 6,
    NVT_PARAM_SET_DEV		= 7,
    NVT_PARAM_GET_USER		= 8,
    NVT_PARAM_SET_USER		= 9,
};

/* NVT parameter structures. */
typedef struct img_cfg_s  /* NVT_PARAM_IMG_CFG */
{            
    int contrast;
    int brightness;
    int saturation;
    int hue;
} img_cfg_t;

typedef struct nvt_net_cfg_s  /*network interface param*/
{  
 	char					ipaddr[32];			//eth0 ipaddr
	char					mDNSIp[32]; 		// �� DNS
	unsigned char			dhcp;				// DHCP����1:��0:��
}nvt_net_cfg_t;

typedef struct nvt_dev_cfg_s
{
	int discv_mode; 			//0:�ɱ�����1:���ɱ�����
	int serialId;				// �������к�
	char firmware_version[16];	//����汾��
	char hardware_id[16];		//Ӳ���汾��
}nvt_dev_cfg_t;

typedef struct User_s
{
	char Username[16]; 		//�û���
	char Password[16]; 		//����
	int  UserLevel;			//0:����Ȩ�� 1:��ͨȨ��
}User;

typedef struct nvt_dev_user_s
{
	User				UserSet[16];    //����û���16
	unsigned int		changeinfo;		// ����ָʾ: λ0-15��Ӧ16���û�
	unsigned int		UserNum;		//�û�����
}nvt_dev_user;

int onvif_init_net_param(nvt_net_cfg_t *pParam);

#if defined (__cplusplus) || defined (_cplusplus)
}
#endif

#endif /* __NVTLIB_H__ */
