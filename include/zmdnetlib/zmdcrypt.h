

#ifndef _JIANGHM_ZMD_CRYPT_HEADER_3298743987432
#define _JIANGHM_ZMD_CRYPT_HEADER_3298743987432

//+---------------------------------------------------------------------------
//
//  File:   	zmdcrypt.h
//
//  Author:		TableJiang
//
//  Contents:   
//
//  Notes:		zmd���ܽ���
//
//  Version:	1.00
//  			
//  Date:		2013-2-1
//
//  History:		
// 			 Tablejiang 2013-2-1		�����ļ�
//
//---------------------------------------------------------------------------


#define CRYPT_KEY		"zmodo19820816"

#ifdef __cplusplus
extern "C" {
#endif


//========================================================
// ���ܱ��뺯��
// src :    ��Ҫ���ܱ�����ִ�
// key :    ��Կ�ִ�
// detail:  ���ܱ��뺯�������ܽ��һ����16�ֽڵ��ڴ�.�п��ܻ���\0
// ���Բ�Ҫ��strcpy������ʹ��memcpy��
bool ZmdEnCrypt( char* src , char* key ) ;

//========================================================
// ���ܺ���
// crypt : ���ܴ����ض���16�ֽڵ�
// key :	��Կ�ִ���
bool ZmdDeCrypt( char* crypt , char* key ) ;



#ifdef __cplusplus
}
#endif

#endif


