#ifndef P2P_INTERFACE_H

#define P2P_INTERFACE_H

#include <time.h>

enum p2p_error_code
{
	P2P_ERR_UNKNOW = -1,
		
	P2P_OK = 0,
	P2P_ERR_OFFLINE = 1	
};

typedef struct _p2p_find_playback_file_t
{
    short  file_type;
    short  alarm_level;
    int  channel;
    int  file_size;
    int  is_finished;
    char create_date[32];
    char create_time[32];
    char finish_time[32];
    char file_path[96];
} p2p_find_playback_file_t;

typedef struct 
{
	/* ���16λ */
	char physical_id[20];	
	/* ���16��utf-8�ַ�  */
	char device_name[50];	
	unsigned char  device_type;
	unsigned char  channel_id;
	/* md5 32λ */
	char local_pwd[34];	
	/* ʹ�ܿ��� 1������0 �ر�*/
	int  use_on;
	char lan_ip[32];
    /* ����������ʱ�䣬 ��λ��*/
    int  buzzer_last; 
    /* �Ƿ����ñ����� */
    int  buzzer_trigger;

    /* �����豸������ģʽ�´�������������ֵ��һ����λ���ֵ��
     * Ĭ��ֵΪ-1����ʱ��ʾ����ģʽ�¶�������
     * ������0x1��ʾģʽ0�´�����������
     * 0x2��ʾģʽ1�´���������, 
     * 0x4��ʾģʽ2�´���������,��������;
     * ģʽ��ָ�û�ģʽ*/
    int  mode_buzzer;

    /* С�¿ص��¶� */
    int  temperature; 
    /* ͨ��׵Ŀ��أ�С�¿�ʹ�ã�0��ʾ�أ�1��ʾ����2��ʾ�Զ� */
    int  vent_switch;
}sub_device_info; 

/* ���ڱ���ӵı����豸����Ϣ */
typedef struct 
{
	/* ���16λ */
	char physical_id[32];
    /* �豸���ͣ� �Ŵ� 9*/
	int  dev_type;
}adding_device_info;

struct p2p_pt_preset_info_t
{
	char trigger_id[16]; // �����豸��ID
	char preset_name[48]; // Ԥ��λ������: Ψһ
	int  preset_on;		// Ԥ��λ�Ŀ���  0: ��  1�� ��	
};
typedef struct 
{
	int 				schedule_id;
	int 				flag; 			// 0: ��schedule Ϊ�����豸���� ; 1: ���Ƹ澯����
	int 				repeat_day; 	//��λ���ֶ�, bit0 ��ʾ�����죬�Դ�����
	int 				off_at;		//�رյ���ʼʱ��
	int			 		on_at;		//�رյĽ���ʱ��˵�������off_at ��on_at ��Ϊ0, ���ʾȫ��ر�
}p2p_schedule_t;

typedef struct _p2p_md_region_t {
    float x;      //���������
    float y;      //���������
    float width;  //�����
    float height; //�߱���
#ifdef _BUILD_FOR_NVR_
    int   channel;  //ͨ����
    char  physical_id[16];  //ͨ���Ŷ�Ӧ���豸ID
#endif
} p2p_md_region_t;

typedef struct _p2p_timezonelist_t {
    char timezone[40];
    char desc_zh[200];
    char desc_en[48];
    char offset[8];
} p2p_timezonelist_t;

typedef struct _p2p_thermostatinfo_t {
    unsigned int air_switch;    // �յ����أ�0Ϊ�أ�1Ϊ��
    unsigned int work_mode;     // ����ģʽ��0Ϊ���䣬1Ϊ���ȣ�2Ϊ��ʪ��3Ϊͨ��
    unsigned int cool_temp;     // ����ģʽ�µ��¶�����
    unsigned int heat_temp;     // ����ģʽ�µ��¶�����
    unsigned int dry_temp;      // ��ʪģʽ�µ��¶�����
    unsigned int ven_temp;      // ͨ��ģʽ�µ��¶�����
} p2p_thermostatinfo_t;

typedef struct _p2p_thermostat_stat_t{
    char physical_id[16];// �����豸���Լ���ID�����²������ٴ�һ����
    int temperature;// �¶�����
    int vent_switch;// ͨ�������
    int current_temp;// ��ǰ�¶�״̬
    //��ʾͨ���״̬��0Ϊ�أ�1Ϊ��
    //��physical_idΪС�¿�����ͨ��׿�������Ϊ�Զ�ʱ����Ҫ���ֵ���������ֵ
    int vent_status; 
} p2p_thermostat_stat_t;

typedef struct _p2p_watering_ctrl_t {
    int op_type;   // 1 ��ʼ��ˮ�� 2 ֹͣ��ˮ 
    char physical_id[32];   //��ˮ�豸��ID
    int hole_id;    //��ˮ�ڵ���ţ���0��ʼ
    int time;   //��ˮʱ������λ����
} p2p_watering_ctrl_t;

typedef struct _p2p_push_condition_t {
    char delta[16];  // 10-20 ��λ�ٷֱȣ��澯ʱ����仯��Χ
    char push_time[32]; // 07:00-20:00, 24Сʱ��
    int  alarm_interval;    //������Ч�澯������ʱ��������λ��
    int  after_nightview;   //ҹ���л������ƶ������Ч����λ��
    char width_height_div[16];  // 10-20 ��λ�ǰٷֱȣ���߱仯��Χ
    char speed[16];             // 10-20 ��λ�ǰٷֱȣ�Ŀ���ƶ��ٶȷ�Χ
    int  position_x;        // ������򣬵�λ�ٷֱ�
    int  position_y;
    int  position_w;
    int  position_h;
    int  push_type;         // ���ͷ�ʽ�� 1 ͼƬ��2 ��Ƶ��3 ����
    int  picture_count;     // ͼƬ������ push_type Ϊ1ʱ����
    int  video_last;        // �澯��Ƶʱ������λ�룬 push_type Ϊ 2ʱ����
    int  video_bitrate;     // ��Ƶ���ʼ��� 1-5
    int  change_bitrate_interval;   //�л����ʼ��, ��λ��
    int  if_push;           // �Ƿ�Ҫ����
} p2p_push_condition_t;

typedef struct
{
	/* 
	�豸����		
	0-��ͨ����ͷ
	1-NVR
	2-DVR
	3-��������
	4-�����¿�
	5-���ܿ���
	6-�����ŵ�
	7-Pivot
	8-��������
	9-�Ŵ�(�����豸)
	10-PIR(�����豸)
	11-�̸�(�����豸)
	12-����(�����豸)
	13-������ť(�����豸)
	14-��԰��
    15-�м��豸
    16-�����м��豸
    17-���ⱨ����(�����豸)
    18-ң����(�����豸)
    19-���¿�
    20-С�¿�(�����豸)
    21-ͨ���(�����豸)
    22-����PT
    23-����PT
	24-ˮλ������(�����豸)
	25-����豸(�����豸)
	26-ȫ�����
	27-���۵���
	28-�����豸

	*/
	unsigned int device_type;
	/* �豸���� DEV_TYPE_INFO */
	unsigned int device_capacity;
	/* 	
	�����豸������0����֧�֣�1��֧�֣�
	bit0: �Ƿ�֧���豸������澯schedule�����ù��ܣ�Ŀǰ�豸���ֶ�Ҫbit1֧�֣��ſ������ã�
	bit1: �豸�Ƿ�֧��sync�ӿ�
	bit2: �豸�Ƿ�֧���쳣�������
	bit3: ��ʾspoeһ�϶��װ�NVR����Ҫ�������ipc
	bit4: �Ƿ�֧�ָ澯��¼��
	bit5: �Ƿ�֧����ͨ��¼�񣬼�֧��������Э����ƴ洢¼��
	bit6: �豸�Ƿ�֧��ʱ����������.
	8-15λ����ͬZSPЭ��CMD_DECIVE_TYPE ��ȡ�豸�ͺ�����DeviceType�ֶ�
	bit16: �Ƿ�֧���豸������
	bit17: �Ƿ�֧��sd������(��ѯ�͸�ʽ��)��
	bit18: �Ƿ�֧�����ж����ú͸澯�����б�����,�����ͼ������
	bit19���Ƿ�֧��ҹ������
	bit20���Ƿ�֧��ͼ��ת
	bit21���Ƿ�֧��SD��Զ�̻ط�
	bit22���Ƿ�֧��360�����
	bit23���Ƿ�֧�ֱ����豸
	bit24���Ƿ�֧��h265
	bit25���Ƿ�֧��Ԥ��λ����
	bit26: �Ƿ�֧��bluetooth speak
	bit27: �Ƿ�֧��ȫ˫���Խ�
	bit28���Ƿ�֧��ҹ��
	bit29: �Ƿ�֧��SD����ʽ��
	bit30���Ƿ�֧��zink
	*/
	unsigned int device_extend_capacity;

	/*  �����豸������չ
	bit0: �Ƿ�֧���豸���ѹ���
	bit1: �Ƿ�֧���������
    bit2: �Ƿ�֧�ֹ�������
    bit3: �Ƿ�֧�ֵ������
    bit4: �Ƿ�֧����������
    bit5: �Ƿ�֧�ַǾ���������Ԥ��λ
    bit6: �Ƿ�֧�ַ������·��ĸ澯����
    bit7: �Ƿ�֧����������
    bit8: �Ƿ�֧���Զ���ʾwifi�б�

	*/
	unsigned long long device_supply_capacity;

	/* ��������ͨ���� */
	unsigned int device_alarm_in_num;
	/* ��Ƶͨ���� */
	unsigned int device_video_num;
		
	/* �豸�Ƿ�֧����������, 1�ǣ�0�� */
	unsigned int use_wired_network;

	/* �豸��ID */
	const char *device_id;
	/* ����Ŀ¼ eg:/config */
	const char *config_dir;
	
	const char *uboot_version;
	
	const char *kernel_version;
	
	const char *rootfs_version;
	
	const char *app_version;
	
	const char *device_name;
	/* �������� wlan0, ra0, eth0 ... */
	const char *network_interface;

	/* ������(����)�ķֱ���, ��ʽ 1280*720 */
	const char *high_resolution;

	/* ������(����)�ķֱ���, ��ʽ 1280*720 */
	const char *secondary_resolution;
	
	/* ������(����)�ķֱ���, ��ʽ 1280*720 */
	const char *low_resolution;
	
	/* ���������ļ���ȫ·�� */
	const char *voice_message_path[5];

    /* ���ܵ�key */
    const char *aes_key;
	/*
	���ÿͻ��˽����豸�����¼��Ļص�����, 
	���ͻ���ͨ��Զ�̴��������¼�ʱ���ô˻ص�,
	����Ƶ���̴߳������벻Ҫ�ڻص�������!!!
	*/
	void (*answer_call_callback)();

	/* 	
		�˺������������ϵı����豸�б��ȫ������ͬ��������
		���� int num, �б��б����豸����
    	ע��: �� num=0ʱ����˼������б�
	*/

	int (*set_sub_device_list_callback)(int, const sub_device_info *);


	/* 
		�˺������������ϵ�Ԥ�õ��б��ȫ������ͬ��������
		���� int num, �б���Ԥ�õ����
		ע��: �� num=0ʱ����˼������б� 
	*/
	void (*set_preset_list_callback)(int , const p2p_pt_preset_info_t *);

	/*
		�������Իص�����, APPѡ��һ�����ԣ����豸��ʱ����
		��������Ƶ�ļ�·��
	*/
	void (*play_audio_file_callback)(const char*);
	
	/*
		��ѯ���ػط��б�ص�����, ֧��Զ�̱��ػطŵ��豸�븳ֵ
		@date: ��ѯ������, ��ʽ '2014-07-16'
		@channel: ��ѯ��ͨ��
		@list: ��ѯ���Ļط��б�, �ڲ���malloc�������ݣ����ú��ͷ�
		@num: ��ѯ����list���¼�����
		return 0 on success
		if success and num is not 0, user must free info
	*/
	int (*find_playback_list_callback)(const char* date, int channel, 
			p2p_find_playback_file_t** list ,int* num);

	/* �µ�Уʱ��ʽ,���÷��������ص�ʱ���Уʱ */
	void (*timing_callback)(time_t timestamp);
	
	/* 
		ˢ���豸��״̬�ص�����,��app����ˢ�·���ӿ�ʱ�����ô˻ص�, ��ʱ��pivot�����ϱ���ʪ��;
		����ֵ0�����ٴγ��Եĺ�������������ֵ������0ʱ���ڲ���ȴ����صĺ��������ٴε��ô˻ص�����
	*/
	int (*on_refresh_device_callback)();

	/*
		�豸���ߺ����ô˻ص�������@status ����Ŀǰ����
	*/
	void (*on_device_online_callback)(int status);

	/*
		ʹ�豸����ap ģʽ�Ļص�����
		@mode  1����ʾ������APģʽ; 2:��ʾ������ӱ����豸�ļ���ģʽ
		@device_count Ҫ���ӵ��豸����
	*/
	void (*on_ap_mode_callback)(int mode, int device_count);

    /*
     * ����schedule�ص�
     * @num schedule������
     */
    void (*on_set_schedule)(p2p_schedule_t *sch, int num);

    /*
     * ���Ա�����
     * @op_type 0ֹͣ��1����
     */
    void (*on_test_buzzer)(int op_type);

    /*
     * �����������û��߻�ȡ
     * @op_type 1 Ϊ���ã� 2Ϊ��ȡ
     */
    void (*on_md_region_ctrl)(int op_type, p2p_md_region_t* reg);

    /*
     * �¿����ûص�����
     */
    void (*on_thermostat_set)(p2p_thermostatinfo_t * info);

    /*
     * ����豸����
     */
    void (*on_watering_ctrl)(p2p_watering_ctrl_t * wc);

    /*
     * ����״̬��ѯ
     * @curtain_status ����״̬ 0:��ֹ״̬�� 1�����ڴ򿪣�2�����ڹر�
     * @screen_status  ��ɴ״̬ 0:��ֹ״̬�� 1�����ڴ򿪣�2�����ڹر�
     */
    void (*on_curtain_get_state)(int *curtain_status, int *screen_status);

    /*
     * ��������
     * @op_type 0:ֹͣ�˶���1���򿪣�2���ر�
     * @is_curtain 1:������ 2����ɴ
     */
    void (*on_curtain_ctrl)(int op_type, int is_curtain);

    /*
     * Ԥ�õ�����
     * @op_type 0: ��ӣ� 1���޸�
     * @physical_id �����豸ID
     * @preset_name Ԥ�õ�����
     * @new_name �µ�Ԥ�õ�����(�޸ĵ�ʱ�����Ч�����ʱΪ��)
     * @image_url, ��p2p_upload_pt_preset ���ص� image_url, �ص��������ڲ������free�ͷ�
     * ����ֵ 0 �ɹ�������ʧ��
     *
     */
    int  (*on_preset_ctrl)(int op_type, const char* physical_id, const char* preset_name, const char* new_name, char** image_url);

    /*
     * ���ñ������Ͳ���
     * @pc_count p2p_push_condition_t �ĸ���
     * */
    void (*on_set_push_condition)(p2p_push_condition_t* pc, int pc_count);

}p2p_init_info_t;

/* 
�ж��豸�Ƿ�����
*/
bool p2p_is_online();


struct p2p_report_value_t
{
    int  channel; // �豸��ص�ͨ���ţ� ��Ƶ����Ƶ��IO��
    int  type;   // 1���¶� 2��ʪ�� 
    char value[128];
};

struct p2p_report_battery_t
{
	char physical_id[32]; // ��Ӧ�������豸ID, ���Ŵ�ID
	int  device_type;	  // �豸���ͣ��Ŵ���9
	//int  battery_level;	  // �������� 0�����磬1�ǳ��㣬2�ǲ��� 
	float  battery_voltage;	//�����ѹֵ����battery_level��ѡһ
};

/*
�ϱ�ʵʱ�仯�Ĳ�������������
@v, �ϱ��Ĳ���
@num, v�ĸ���
return 0 �ɹ�
*/
int p2p_report_value(p2p_report_value_t * v, int num);


/*
�ϱ�ָ���豸�ĵ�������������
@v, �ϱ��Ĳ���
@num, v�ĸ���
return 0 �ɹ�
*/
int p2p_report_battery(p2p_report_battery_t * v, int num);
/*
 * �첽�ϴ���ص������᳢��3��
 */
int p2p_report_battery_a(p2p_report_battery_t * v, int num);


int p2p_upload_sub_device_list(int num, const sub_device_info *sd_list);

/*
	�����ӻ����޸ĵ�Ԥ�õ���Ϣͬ����������
	@flag  		// 0: ��� ��1���޸�
	@trigger_id  // �����豸��ID
	@preset_name  // Ԥ��λ������: Ψһ
	@preset_on 	// Ԥ��λ�Ŀ���  0: ��  1�� ��
	@image_name  // Ԥ�õ��ץͼ	
	@original_name	//�޸�Ԥ�õ�����ʱ���ṩԭʼ����
	@image_url	// ���ص�ץͼurl������ free !!!
	return p2p_error_code
*/
int p2p_upload_pt_preset(int flag, const char* trigger_id,
	const char* preset_name, int  preset_on, 
	const char* image_name, const char* original_name,
	char** image_url);


int p2p_init(p2p_init_info_t* info, unsigned int info_len);

/*
	���شӷ�������ȡ��utcʱ���������Уʱ 
	return -1 ���豸û�����ӵ�������ʱ������ʧ��
*/
int p2p_get_server_timestamp();

/*
 *  ���ط� 0 ��ʾ�����ڿ���Ƶ
 *
 * */
int p2p_if_somebody_watching_video();

/*
 * �ϱ�������ӵı����豸��Ϣ 
 * ����0��ʾ�ɹ�����������ֵ��Ҫ���µ��ô˺���
 * @num �Ǳ����豸�ĸ���    
 */
int p2p_report_adding_devices(adding_device_info *ad, int num);


int p2p_sync_value(const char* name, const char* value);

/*	
	�ж��Ƿ���������
	����1�ǣ�0��
*/
int p2p_if_upgrading();

typedef struct
{
	char server_ip[32];
	int server_port;
	char auth_code[64];
}sleep_server_info;

/*
	��ȡ���߷�������Ϣ
*/
int p2p_get_sleep_server_info(sleep_server_info *info);

/*
	�������ߣ��첽���á�ͨ��bool p2p_is_online() ���ж��Ƿ�ɹ���
*/
int p2p_request_offline();

/* 
 * ��ȡʱ���б�
 * @list �����б�Ŀռ䣬������numָ��
 * @num  ����ʱΪ�ɱ����ʱ�����������Ϊ��ȡ����ʱ������
 * return 0 �ɹ��� -1 Ϊ�������-2Ϊ������б����̫С, 1Ϊ�豸����
 * */
int p2p_get_timezonelist(p2p_timezonelist_t *list, int *num);

/*
 * �ϱ��¿ص�״̬
 * return 0 �ɹ���1Ϊ�豸����
 */
int p2p_report_thermostat_stat(p2p_thermostat_stat_t * st);

/*
 * �ϱ���ȿڵ�״̬
 * @physical_id ����豸��ID
 * @hole_id, ��ȿ���ţ� ��0��ʼ
 * return 0 �ɹ���1Ϊ�豸����, -1 Ϊ�������
 * */
int p2p_report_waterholeinfo(const char* physical_id, int hole_id, int watering);

#endif /* end of include guard: P2P_INTERFACE_H */
