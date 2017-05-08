
#ifndef 	__GUI_TYPES_H
#define 	__GUI_TYPES_H


#include "hi_type.h"
#include "common.h"
#include <linux/fb.h>

#define MENU_STRING_OFFSET		5

//����ÿ��ҳ�����ʾ��ҳ�����Ե�Լ��.
#define MAX_GUI_PAGES			64	// per session
#define MAX_GUI_PANELS			32	// per page
#define MAX_GUI_EVENTS 			32	// per object
#define MAX_GUI_BITMAPS 			32	// per page
#define MAX_GUI_COMMANDS 		720	// per session
#define MAX_GUI_MENUS			64	// per page
#define MAX_GUI_MENUITEMS		64	// per menu
#define MAX_GUI_STRINGS			128	// per page
#define MAX_GUI_LISTITEMS		64	// per list
#define MAX_GUI_EVENTBITMAPS	80	// per session
#define MAX_GUI_SELECTS			20	// per page
#define MAX_GUI_SELECTITEMS  	32
#define MAX_GUI_BMPBTN			32	// per page

#define MAX_GUI_BUTTONS 		64	// per page / menu item

#define GUI_INVALID_COMMAND_ID	 0

// object ID masks

#define MENU_IDMASK					0x00010000
#define STRING_IDMASK				0x00020000
#define BUTTON_IDMASK				0x00040000
#define PANEL_IDMASK				0x00080000

#define ENTER_COMMAND_MASK			0x00001000
#define TXTCHANGE_COMMAND_MASK  	0x00002000


// object id check-up macros
#define ISMENU(x) 			(((x) & MENU_IDMASK) && ((x) >> 16))
#define ISSTRING(x) 			(((x) & STRING_IDMASK) && ((x) >> 17))
#define ISBUTTON(x) 			(((x) & BUTTON_IDMASK) && ((x) >> 18))
#define ISPANEL(x)			(((x) & PANEL_IDMASK) && ((x) >> 19))


#define	CURSOR_COLOR				0x6000

#define	STRING_CONTROL_LEN		33

typedef enum 
{
	LINK_BUTTON = 0,
	LINK_STRING,
	LINK_PAGE,
	
} GuiLinkType;


/*һ�����������״̬:
	����,��ѡ��,������.*/
typedef enum 
{
	STATE_ENABLED = 0,
	STATE_SELECTED,	
	STATE_DISABLED	
	
} GuiObjectState;


/*�����ѡ����*/
typedef enum 
{
	SELECT_LEFT = 0,
	SELECT_UP,
	SELECT_RIGHT,
	SELECT_DOWN	
	
}GuiObjectSelectDirection;


/*�ı���λ��*/
typedef enum 
{
	ALIGN_LEFT = 0,
	ALIGN_RIGHT,
	ALIGN_CENTER,
	ALIGN_UP,
	ALIGN_DOWN,
	ALIGN_DOWN_NOTE,
}GuiObjectTextAlignment;


/*�ַ�������*/
typedef enum 
{
	STRING_UNKNOW 		= 0,
	STRING_PASSWORD 		= 1,
	STRING_NUM 			= 2,
	STRING_DATE 			= 3,
	STRING_IP 				= 4,
	STRING_MAC 			= 5,
	STRING_CHAR 			= 6,
	STRING_CHOOSE 			= 7,
	STRING_BUTTON 			= 8,
	STRING_COMBOX 		= 9,
	STRING_COMBOX_INDEX	= 10,
	STRING_EDIT 			= 11,
	STRING_STATIC 			= 12,
	STRING_MENU 			= 13,
	STRING_MENU_EXPAND	= 14,
	STRING_TIME 			= 15,
	STRING_TIMER 			= 16,
	STRING_CHECK 			= 17,
	STRING_LIST 			= 18,
	STRING_LIST_COLUMN 	= 19,
	STRING_IPBOX 			= 20,
	STRING_MACBOX 		= 21,
	STRING_PROCESS 		= 22,
	STRING_BUTTON_NOTE	= 23,
	STRING_PLAYER			= 24,
	STRING_OUTLINE_FILLBOX, // ֻ�д����Ͳſ��Ի��߿�����
	
}GuiObjectStringType;


typedef enum
{
	DISABLE_VISIBLE = 0,
	ENABLE_VISIBLE,
	
}OBJVISIBLE_STATE;


#define MAX_OSD_STATUS			32	

typedef enum
{

	OSD_NULLMODE					= 0,	 	// ��ģʽ
	OSD_LIVEMODE 					= 1, 	// 1  ��ֱͨ����ģʽ
	OSD_PLAYBACKMODE 				= 2,		// 2 �ڻطŻ�����
	OSD_INTERFACEMODE			= 3, 	// 3 �ڲ���������
	OSD_LOGINMODE					= 4,		// 4  �����Ѿ���¼״̬
	OSD_PLAYCURFILE				= 5,  	// 5  ���ŵ�ǰ�ļ�
	OSD_ADJUST_LCS				= 6, 	// 6  ����ģ����
	OSD_DATETIME_MODE			= 7,		// 7 ˢ��ʱ����Ϣ
	OSD_FORMAT_HDD_MODE			= 8,		// ��ʽ��Ӳ��
	OSD_MDSET_MODE				= 9,
	OSD_PTZ_MODE					= 10,
	OSD_TESTALARM_MODE			= 11,
	OSD_COLOR_MODE				= 12,	//��ɫ����ģʽ
	OSD_SOFTKEY					= 13,	//����̴�
	OSD_REBOOT_MODE				= 14,	//����ϵͳ��
	OSD_SHUTDOWN_MODE			= 15,	//�ر�ϵͳ��
	OSD_EXPAND_MENU				= 16,	//��չ�˵�
	OSD_PTZ_IR_INPUT				= 17,	//������̨����
	OSD_3G_MODE 					= 18,	// 3G����ҳ��
	OSD_SYSTEM_MENU				= 19,	// ���˵�
}OSDMODETYPE;


typedef enum
{
	/*Ӣ���ַ�8X16����*/
	ENGLISH_8_16 = 0,

	/*Ӣ���ַ�10 X 20����*/
	ENGLISH_10_20,

	/*Ӣ���ַ�12 X 24����*/
	ENGLISH_12_24,

	/*Ӣ���ַ�15 X 30����*/
	ENGLISH_15_30,

	/*�����ַ�16 X 16����*/
	CHINESE_16_16,

	/*�����ַ�20 X 20����*/
	CHINESE_20_20,

	/*�����ַ�24 X 24����*/
	CHINESE_24_24,

	/*�����ַ�30 X 30����*/
	CHINESE_30_30
	
}FontType;

typedef enum
{
	STYLE_NUMBER = 0,
	STYLE_CHAR,
	STYLE_TIME,
	STYLE_DATE_YEAR,
	STYLE_DATE_MONTH,
	STYLE_DATE_DAY,
	STYLE_MAC,
}INPUT_STYLE;

typedef enum
{
	NOTE_ALARM 	= 0,
	NOTE_HOME_ALARM ,
	NOTE_DISALARM ,
	NOTE_CLEAN_ALARM,
	NOTE_SYS_DEF,
	NOTE_DEL_USER,
	NOTE_STORAGE_SET,
	NOTE_TVMODE_SET,
	NOTE_INPUT_ERROR,
	NOTE_ENCODE_RECODE,
	NOTE_ENCODE_RESTART,
	NOTE_CENTER_OPT,
	NOTE_HDD_FORMAT,
	NOTE_HDD_WAIT,
	NOTE_BACKUP_FILE,
	NOTE_RESTART,
	NOTE_SHUTDOWN,
	NOTE_NETSET_LISTEN,
	NOTE_PLEAR_ERROPT,
	NOTE_3GSET_REBOOT,
	NOTE_ZONESET_DEFAULT,
	NOTE_EAVS,
	NOTE_LOGIN_ERR,
	NOTE_CMSFAIL,
	NOTE_VERIFYUSERFAIL,
}NoteBoxType;

typedef enum
{
	IDNULL = 0,
	IDYES,
	IDYESNO,
}NoteBoxMode;

#ifndef    RELEASE_APP
#define	PICTURE_DIR			"/mnt/icon/"
#define   FONTFILE_DIR			"/mnt/fontfile/"
#define   PICTURE_NOTE			"/mnt/dvr_bmp/info/"

#else 
#define	PICTURE_DIR			"/app/icon/"
#define   FONTFILE_DIR			"/app/fontfile/"
#define   PICTURE_NOTE			"/app/dvr_bmp/info/"
#endif 

/*OSD page definition*/
typedef struct 
{	
	char	*			m_filename;	// bitmap file
	unsigned short 	m_x;		// for now full size at (0,0)
	unsigned short		m_y;
	unsigned short		m_width;
	unsigned short		m_height;
	int				m_isDisplayed;
	unsigned int		m_backgroundcolor;		/*0x00000000-0x00ffffff  */
	int 				m_fbnumber;
	
} PageObject;


/* Text Button definition*/
typedef struct  
{
	char	*			m_filename;		// NULL for text buttons, otherwise if selectedFile = NULL contains 3 states (enabled, focused, disabled)
	char	*			m_selectedFile;	// selected view, if NULL, file is 3 state
	unsigned short 	m_x;
	unsigned short 	m_y;
	unsigned short 	m_width;
	unsigned short 	m_height;
	GuiObjectState 	m_state;
	char *			m_link;
	char *			m_target;
	char	*			m_text;
	FontType 			m_fonttype;	
	int   				m_hasfocus;
	int				m_nbType;				// buttom type 
	int 				m_fbnumber;
	int				m_backgroundcolor;		/*ignored if bitmapped0x00000000-0x00ffffff  */
	int 				m_foregroudcolor;
	int				m_outlinecolor;			/* ignored if bitmapped  */
	int				m_selectedoutlinecolor;		/* ѡ�к�����ͼ*/
	int				m_disablecolor;			/*���ɲ����İ�ťɫ*/
	int				m_activecolor;
} ButtonObject;


/* String object definition*/
typedef  struct
{
	unsigned short 		m_x;
	unsigned short 		m_y;
	unsigned short 		m_width;
	unsigned short 		m_height;
	char *				m_text;
	FontType 				m_fonttype;
	GuiObjectStringType	m_type;
	int					m_hasfocus;
	unsigned int 			m_foregroundcolor;
	unsigned int 			m_backgroundcolor;
	unsigned int 			m_outlinecolor;
	unsigned int			m_validgroudcolor;
	GuiObjectTextAlignment 	m_textalign;
	unsigned int 			m_selectioncolor;
	unsigned char 			m_maxlength;
	short 				m_selcharindex;
	int					m_fbnumber;
	unsigned int			m_noempty;
	unsigned short		m_value;
} StringObject;


typedef struct  
{
	int 		m_fbNumber;
	int		m_x;
	int		m_y;
	int		m_width;
	int		m_height;
	char 		m_filename[128];
	
} BitmapObject;


typedef struct
{
	int 		m_id;
	char* 	name;
	char* 	m_event;			/* used if part of event bitmap list  */
	BitmapObject object;
	
} guiBitmapType;


/* MenuItem object definition*/
typedef struct  
{
	char* 			m_text;
	unsigned char 		m_index;
	char*			m_picfilename;
	char*			m_selpicfilename;
	int				m_hasfocus;
	int 				m_fbnumber;
	unsigned short 	m_x;
	unsigned short 	m_y;
	unsigned short 	m_width;
	unsigned short 	m_height;
	
} MenuItemObject;


typedef struct 
{
	unsigned short 	m_x;
	unsigned short 	m_y;
	unsigned short 	m_width;
	unsigned short 	m_height;
	unsigned int 		m_foregroundcolor;
	unsigned int 		m_backgroundcolor;
	char *			m_parent;
	unsigned int 		m_selectioncolor;
	char	*			m_filename;		// NULL for text buttons, otherwise if selectedFile = NULL contains 3 states (enabled, focused, disabled)
	FontType 			m_fonttype;
	int 				m_isDisplayed;
	int 				m_fbnumber;
	
}MenuObject;


typedef struct
{
	char 			m_LogInPageName[32];
	char 			m_SysMenuPageName[32];
	char 			m_MainMenuPageName[32];
	char 			m_RecordQueryPageName[32];
	char			m_SysInfoPageName[32];
	char			m_SysSettingPageName[32];
	char			m_AdvacedOptionPageName[32];
	char			m_FileManagePageName[32];
	char			m_SaftyPageName[32];
	char			m_HZinputPageName[32];
	char			m_AsciiInputPageName[32];
	char			m_DigitInputPageName[32];
	char			m_logofile[64];

	int 			m_LogInPageId;
	int 			m_SysMenuPageId;
	int 			m_MainMenuPageId;
	int 			m_RecordQueryPageId;
	int			m_SysInfoPageId;
	int			m_SysSettingPageId;
	int			m_AdvacedOptionPageId;
	int			m_FileManagePageId;
	int			m_SaftyPageId;
	int			m_HZinputPageId;
	int			m_AsciiInputPageId;
	int			m_DigitInputPageId;
	
	unsigned 	short 	m_backgroudcolor;
	
} guiPlayerParams;


typedef struct 
{
	char *			m_name;
	unsigned int 		m_id;
	ButtonObject 		m_object;
	char* 			m_keyleft;
	char* 			m_keyup;
	char* 			m_keyright;
	char* 			m_keydown;
	char* 			m_onclick;
	char* 			m_onselect;
	char*			m_clickcmdname;
	unsigned int		m_nCommandId;
	unsigned short 	m_nbEvents;
	unsigned short		m_nVisible;
	
}GuiButton;

typedef struct 
{
	char *			m_name;
	MenuItemObject 	m_object;
	unsigned short 	m_nbEvents;
	char *			m_keydown;
	char *			m_keyup;
	char *			m_onclick;
	char *			m_target;	
	char	 *			m_clickcmdname;
	unsigned int		m_nCommandId;	
	int				m_nVisible;
	
}GuiMenuItem;


typedef struct 
{
	char *			m_name;
	unsigned int 		m_id;
	GuiMenuItem *		m_menuItem[MAX_GUI_MENUITEMS];
	MenuObject 		m_object;
	unsigned short 	m_nbMenuItems;
	unsigned char 		m_selectedMenuitemIndex;
	unsigned short		m_nVisible;

}GuiMenu;


typedef struct 
{
	char* 			m_name;
	unsigned int 		m_id;
	StringObject 		m_object;
	char* 			m_keyleft;
	char* 			m_keyup;
	char* 			m_keyright;
	char* 			m_keydown;
	char	*			m_ontextchange;
	char*			m_onclick;
	char*			m_clickcmdname;	
	char*			m_symbolinput;
	char*			m_target;
	char*			m_onselect;
	char*			m_delclick;
	unsigned int		m_nCommandId;	
	unsigned int		m_InputCmdId;
	unsigned short 	m_nbEvents;
	unsigned short		m_nVisible;
	
	
}GuiString;


typedef struct 
{
	char* 			m_name;
	unsigned int 		m_id;
	PageObject 		m_object;
	char *			m_selectObj;
	char *			m_parent;
	char	*			m_handleload;
	char *			m_onclose;
	GuiButton* 		m_buttons[MAX_GUI_BUTTONS];
	GuiMenu* 		m_menus[MAX_GUI_MENUS];
	GuiString* 		m_strings[MAX_GUI_STRINGS];

	unsigned short 	m_nbMenus;
	unsigned short 	m_nbButtons;
	unsigned short 	m_nbStrings;
	unsigned short	m_nVisible;
	unsigned int		m_nfocusObjId;
	
}GuiPage;

typedef struct
{
	char*		 m_name;
	unsigned int  	 m_id;

} GuiCommandType;


typedef struct
{
	int		 m_id;
	int		 m_linkId;
	int		 m_validLink;
	int		  m_spring;

}typeCommandInfo;


/*�߶���*/
typedef struct 
{
	int	m_fbNumber;	
	int	m_x;
	int	m_y;
	int	m_width;
	int	m_height;
	int	m_color;
	
} LineObject ;


/*����*/
typedef struct  
{
	int	m_fbNumber;
	int	m_x;
	int	m_y;
	int	m_height;
	int	m_width;
	int	m_color;
	int	m_outlinenum; 	/*���ߺ��*/
	
} OutLineObject;


/*������*/
typedef struct  
{
	int  m_fbNumber;	
	int	m_x;
	int	m_y;
	int	m_height;
	int	m_width;
	int	m_color;
	int   m_transpanrent;
	
} FilledObject;


//��ɫ�����������Ҫ��COLOR_BITS����һ��
#if defined(SUPPORT_COLOR_ARGB8888)
	#define color_t unsigned long
#else
	#define color_t unsigned short
#endif

#define BLUE   	0x001f
#define RED    	0x7C00
#define GREEN  	0x03e0


/*********************************************************
	͸����ɫ��ʹ�������ɫ���Խ�����
	FRAMEBUFFER����������Ϳ��Կ���ֱͨ����
 *********************************************************/
#define HI3511_TRANSPARENT_COLOR		0x5de8


typedef enum _COLOR_BITS_
{
	COLOR_BITS_16 = 16,
	COLOR_BITS_32 = 32
	
}COLOR_BITS;		//ɫ��λ��


typedef enum _OSD_LAY_WHITCH_
{
	OSD_LAY_0 = 0,
	OSD_LAY_1,
	OSD_LAY_ALL
	
}OSD_LAY_NUM;


// framebuffer�豸����
#define DEVNAME_LEN		20


typedef struct _SURFACE_S
{
	HI_U32	x;
	HI_U32	y;
	HI_U32	w;
	HI_U32	h;     
	unsigned char isSync;	//�Ƿ����ʱ����Ķ��������Դ�
	
}SURFACE_S;


typedef enum 
{
	FB_DOUBLE_BUFFER,
	FB_SINGLE_BUFFER
	
}FB_BUFFER_TYPE;


typedef struct _osdlayerinfo_type_{
	//osd����Ϣ�ṹ��
	
	/*FRAMEBUFFERӳ���ַ*/
	void *mapped_mem;		
	
	/*������ʾӳ���ַ*/
	void *DisplayMem;			

	/*��ʾ�������ַ*/
	HI_U8 *DisPlayPhyAddr;	

	/*��ʱ����Ҫ���ڿ���˸*/
	void *TmpMem;		

	/*��ʱ��Ӧ�������ַ*/	
	HI_U8 *TmpPhyAddr;	

	//�����ڴ��ַ������ǵ������ʹ��DislayMem,����ʹ��tmpMem;	
	void *ProxyMem;	

	/*ӳ�䳤��*/
       unsigned long mapped_memlen;

	/*FRAMEBUFFER���ļ�������*/
	int FrameBufferFd;		

	/*�豸�ļ�*/
	char file[DEVNAME_LEN + 1];	

	/*�̶���Ϣ*/
       struct fb_fix_screeninfo finfo;	

	/*�ɱ���Ϣ*/
       struct fb_var_screeninfo vinfo;
	   
}osdlayerinfo_t;

#endif 

