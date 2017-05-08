/*
* Copyright(C), 2011-2012 // ��Ȩ����
* File name: //CATsha204.h
* Author:     // л���� LukeXie
* Version:   // V1.0
* Date: // 2012-11-20
* Description: // ��ATSHA204����IC�Ĺ��ܽ��з�װ������ʹ��ͷ�ļ�
*/






#ifndef __ATSHA204_H__ 
#define __ATSHA204_H__
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "sha256.h"

#include "GpioApi.h"


#define CONFIG_ZONE 0X00
#define OPT_ZONE 0X01
#define DATA_ZONE 0X02
#define BYTE4 0x04
#define BYTE32 0x20
#define WRITE 0xfe
#define READ 0x01



//��дATSHA204ʱ���صĴ�������
typedef enum 
{
    SUCCESS=0x00,                  //����ִ�гɹ�
	MAC_DO_NOT_MATCH=0x01,         // MAC��ƥ��
	PARSE_ERROR=0x03,              //��������
	EXEC_ERROR=0x0f,               //ִ�д���
	WAKE_ERROR=0x11,               //���Ѻ󣬵��ڴ�֮ǰ�ĵ�һ������
	CRC_OR_OTHER_ERROR=0xff        //CRC��ͨ�Ŵ���
}ErrorCodeTable;



//ATSHA204���ֵ�ַ
typedef enum 
{
	SHA204_I2C_PACKET_FUNCTION_RESET,  //!< Reset device.
	SHA204_I2C_PACKET_FUNCTION_SLEEP,  //!< Put device into Sleep mode.
	SHA204_I2C_PACKET_FUNCTION_IDLE,   //!< Put device into Idle mode.
	SHA204_I2C_PACKET_FUNCTION_NORMAL  //!< Write / evaluate data that follow this word address byte.
}I2cWordAddr;



//ATSHA204�����
typedef enum  
{
    PAUSE_CMD=0x01,       //Selectively put just one chip on a shared bus into the idle state
    READ_CMD=0x02,        //Read four bytes from the chip, with or without authentication and encryption
    MAC_CMD=0x08,         //Calculate response from key and other internal data using SHA-256
    HMAC_CMD=0x11,        //Calculate response from key and other internal data using HMAC/SHA-256 
    WRITE_CMD=0x12,       //Write 4 or 32 bytes to the chip, with or without authentication and encryption
    GENDIG_CMD=0x15,      //Generate a data protection digest from a random or input seed and a key
    NONCE_CMD=0x16,       //Generate a 32-byte random number and an internally stored nonce
    LOCK_CMD=0x17,        //Prevent further modifications to a zone of the chip
    TEMP_SENSE_CMD=0x18,  //Return current reading from the optional temperature sensor
    RENDOM_CMD=0x1b,      //Generate a random number
    DERIVE_KEY_CMD=0x1c,  //Derive a target key value from the target or parent key
    UPDATA_EXTRA_CMD=0x20,//Update bytes 84 or 85 within the configuration zone after the configuration zone is locked
    CHECK_MAC_CMD=0x28,   //Verify a MAC calculated on another Atmel CryptoAuthentication device
    DEV_REV_CMD=0x30      //Return device revision information  
}ATsha204Cmd;

#define SC_CLK		0x0506			/*gpio5_6  SCL*/
#define SC_SDA		0x0507			/* gpio5_7  SDA*/

#define 		GPIO_OUTPUT		1
#define 		GPIO_INPUT			0  

       


void SetSclDir(char dir);
void SetSclDat(char dat);



void SetSdaDir(char dir);
void SetSdaDat(char dat);


unsigned char GetSdaDat(void);

void Delay_us(unsigned long us)	;


void Delay_ms(unsigned long ms);


void IoInit(void);

int CheckATsha204();




//ATSHA204�Ĳ�����
class CATsha204
{
private:
	unsigned char TagAddr;	
	unsigned char *pSlot[16];
	unsigned char ID[9];

public:
	/*
	* Function:     // ��Ĺ��캯��
	* Called By:    // 
	* Input:        // ��
	* Output:       // ��
	* Return:       // ��
	* Others:       // ��
	*/
	CATsha204();



	/*
	* Function:     // �����������
	* Called By:    // 
	* Input:        // ��
	* Output:       // ��
	* Return:       // ��
	* Others:       // ��
	*/
	~CATsha204();




	/*
	* Function:     // Write�����
	* Called By:    // 
	* Input:        // zone��Ҫд�ĸ���ȡֵΪCONFIG_ZONE,OPT_ZONE,DATA_ZONE  (0,1,2)
					// byteSize Ҫд����ֽ�����ȡֵΪ 4��32  BYTE4 , BYTE32
					// addr Ҫд�����ʼ��ַ
					// buf Ҫд������� 4��32�ֽ� ������byteSize
	* Output:       // ��
	* Return:       // 1�ɹ���0ʧ��
	* Others:       // ��
	*/
	unsigned char Write(unsigned char zone,unsigned char byteSize,unsigned short addr,unsigned char *buf);






	/*
	* Function:     // read�����
	* Called By:    // 
	* Input:        // zoneҪ���ĸ���ȡֵΪCONFIG_ZONE,OPT_ZONE,DATA_ZONE  (0,1,2)
					// byteSize Ҫ�������ֽ�����ȡֵΪ 4��32  BYTE4 , BYTE32
					// addr Ҫ��������ʼ��ַ
	* Output:       // RecBuf ���������ݣ�����Ϊ3+byteSize RecBuf[0]Ϊ������������ֽ�ΪCRC16���м�Ϊ��������
	* Return:       // 1�ɹ���0ʧ��
	* Others:       // ��
	*/
	unsigned char read(unsigned char zone,unsigned char byteSize,unsigned short addr,unsigned char *RecBuf);





	/*
	* Function:     // ������֤
	* Called By:    // 
	* Input:        // slotNum ѡ��ڼ������������֤��0-15�����룬
					// RandomNum 32�ֽڵ������
	* Output:       // recBuf ��ŷ��صĽ������35�ֽ�
	* Return:       // 0 ʧ�ܣ�1�ɹ�
	* Others:       // recBuf[0]Ϊ���������ܳ��ȣ���������ֽ�ΪУ����CRC16���м��32����Ϊ������			
	*/
	unsigned char Mac(unsigned char slotNum,unsigned char *RandomNum,unsigned char *recBuf);




	
	/*
	* Function:     // �����������󣬿��Ը��������ֽڵ����ݣ�ֵΪ0x00 ���ܱ�����
	* Called By:    // 
	* Input:        // Mode  If zero, update config byte 84.    If one, update config byte 85.
					// newVol Ҫ���µ�����
	* Output:       // ��
	* Return:       // ��״̬��
	* Others:       // 
	*/
	unsigned char UpdateExtra(unsigned char modle,unsigned char newVol);



	


	/*
	* Function:     // ����32�ֽڵ������
	* Called By:    // 
	* Input:        // ��
	* Output:       // recRandomNum ���ڽ��������
	* Return:       // 1�ɹ���0ʧ��
	* Others:       // 
	*/
	unsigned char GenerateRandomNum(unsigned char *recRandomNum);




	/*
	* Function:     // �����ϴ�ֵ(���üĴ���Selector)������оƬ���������״̬
	* Called By:    // 
	* Input:        // ��
	* Output:       // recRandomNum ���ڽ��������
	* Return:       // 1�ɹ���0ʧ��
	* Others:       // 
	*/
	unsigned char Pause(unsigned char Selector);







	/*
	* Function:     //���������һ��32�ֽڵ�������������GenDig��MAC��HMAC����
	* Called By:    // 
	* Input:        // InRandomNumLen ������������ȣ�ȡֵΪ20��32
					// InRandomNum 20��32�ֽ�����������õ�ʱ����
	* Output:       // outRandomNum ���ڽ��������
	* Return:       // 1�ɹ���0ʧ��
	* Others:       // 
	Mode
	0: Combine new random number with NumIn, store in TempKey. Automatically update EEPROM seed only if necessary prior to random number generation. Recommended for highest security.
	1: Combine new random number with NumIn, store in TempKey. Generate random number using existing EEPROM seed, do NOT update EEPROM seed.
	2: Invalid
	3: Operate in pass-through mode and write TempKey with NumIn.
	*/
	unsigned char Nonce(unsigned char Mode,unsigned char InRandomNumLen,unsigned char *InRandomNum,unsigned char *outRandomNum);





	/*
	* Function:     // ���ĸ�����
	* Called By:    // 
	* Input:        // zone  ����
	* Output:       // ��
	* Return:       // ��״̬��
	* Others:       
	zone��
	Bit 0: Zero for config zone, 1 for data and OTP zones
	Bits 1-6: Must be zero
	Bit 7: If one, the check of the zone CRC is ignored and the zone is locked, regardless of the state 
			of the memory. Atmel does not recommend using this mode.
	*/
	unsigned char Lock(unsigned char zone);




	/*
	* Function:     // ������֤
	* Called By:    // 
	* Input:        // slotNum ѡ��ڼ������������֤��0-15�����룬
					// RandomNum 32�ֽڵ������  
					//modle
	* Output:       // recBuf ��ŷ��صĽ��
	* Return:       // 0 ʧ�ܣ�1�ɹ�
	* Others:       // 
	*/
	unsigned char Hmac(unsigned char modle,unsigned char slotNum,unsigned char *recBuf);







	/*
	* Function:     // GenDig
	* Called By:    // 
	* Input:        // slotNum ѡ��ڼ������������֤��0-15�����룬
					// Zone 
	* Output:       // ��
	* Return:       // ��״̬��
	* Others:       // 
	*/
	unsigned char GenDig(unsigned char Zone,unsigned char slotNum);




	/*
	* Function:     // ��ȡ�汾��4�ֽ� 
	* Called By:    // 
	* Input:        // ��
	* Output:       // recBuf �汾�Ž���
	* Return:       // 1�ɹ� 0ʧ��
	* Others:       // 
	*/
	unsigned char DevRev(unsigned char *recBuf);




	/*
	* Function:     // ��ȡ�汾��4�ֽ� 
	* Called By:    // 
	* Input:        // ��
	* Output:       // recBuf �汾�Ž���
	* Return:       // ��״̬��
	* Others:       // 
	*/
	unsigned char DeriveKey(unsigned char Random,unsigned char TargetKey);








	/*
	* Function:     // CheckMac 
	* Called By:    // 
	* Input:        // Mode
					// KeyID
					// ClientChal
					// ClientResp
					//
	* Output:       // recBuf �汾�Ž���
	* Return:       // ��״̬��
	* Others:       // OtherData
	*/
	unsigned char CheckMac(unsigned char Mode,unsigned char KeyID,unsigned char *ClientChal,unsigned char *ClientResp,unsigned char *OtherData);




	/*
	* Function: 	// ��ȡ������IC��ID��9BYTE
	* Called By:	// 
	* Input:		// ��
	* Output:		// IdBuf
	* Return:		// ��״̬��
	* Others:		// OtherData
	*/
	unsigned char GetID(unsigned char *IdBuf);





protected:
	/*
	* Function:     // I2C��ʼ��������
	* Called By:    // 
	* Input:        // ��
	* Output:       // ��
	* Return:       // ��
	* Others:       // ��
	*/
	void I2cStart(void);



	/*
	* Function:     // I2Cֹͣ��������
	* Called By:    // 
	* Input:        // ��
	* Output:       // ��
	* Return:       // ��
	* Others:       // ��
	*/
	void I2cStop(void);



	/*
	* Function:     // MCU��ATSHA204����Ӧ���ź�
	* Called By:    //  
	* Input:        // ��
	* Output:       // ��
	* Return:       // ��
	* Others:       // ���յ�һ�ֽڵ�ʱ��
	*/
	void SendAck(void);



	/*
	* Function:     // MCU��ATSHA204������Ӧ���ź�
	* Called By:    //  
	* Input:        // ��
	* Output:       // ��
	* Return:       // ��
	* Others:       // ���յ����һ�ֽڵ�ʱ��
	*/
	void SendNotAck(void);



	/*
	* Function:     // MCU�ȴ�ATSHA204оƬӦ��
	* Called By:    //  
	* Input:        // ��
	* Output:       // ��
	* Return:       // 0������Ӧ��1������Ӧ��
	* Others:       // ��
	*/
	unsigned char WaitAck(void);




	/*
	* Function:     // MCU��ATSHA204�����ֽ�
	* Called By:    //  
	* Input:        // ch  Ҫ���͵��ֽ�
	* Output:       // ��
	* Return:       // ��
	* Others:       // ��λ�ȷ�
	*/ 
	void I2cSendByte(unsigned char ch);


	/*
	* Function:     // MCU����ATSHA204���ֽ�
	* Called By:    //  
	* Input:        // ��
	* Output:       // ��
	* Return:       // ���ؽ��յ����ֽ�
	* Others:       // ��
	*/ 
	unsigned char I2cReceiveByte(void);



	/*
	* Function:     // MCU��ATSHA204д���ݰ���buf�а��������İ�Э��
	* Called By:    // 
	* Input:        // buf Ҫд������ݰ���lengthҪд������ݰ�����
	* Output:       // ��
	* Return:       // 0 ʧ�ܣ�1�ɹ�
	* Others:       // ��20��д����
	*/
	unsigned char WriteDataToATSHA(unsigned char *buf,unsigned char length);




	/*
	* Function:     // MCU��ATSHA204������
	* Called By:    // 
	* Input:        // CmdBuf Ҫд�������(�������)��CmdBufLengthҪд�������(������)���ȣ�RecBufLengthҪ���������ݳ���
	* Output:       // RecBuf��Ŷ���������
	* Return:       // 0 ʧ�ܣ�1�ɹ�
	* Others:       // ��20�ζ�����
	*/
	unsigned char ReadDataFromATSHA(unsigned char *CmdBuf,unsigned char CmdBufLength,unsigned char *RecBuf,unsigned char RecBufLength);





	/*
	* Function:     // CRC16���㺯��
	* Called By:    // 
	* Input:        // length data�ĳ���  data ���Ҫ���������
	* Output:       // crc ���ص�CRC16
	* Return:       // ��
	* Others:       // ��
	*/
	void sha204c_calculate_crc(unsigned char length, unsigned char *data, unsigned char *crc); 



};
#endif 



