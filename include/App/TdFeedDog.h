#ifndef _TD_FEED_DOG_H_
#define _TD_FEED_DOG_H_

#include "FeedDog.h"

//�̹߳�
class CTrdDog	
{
public:
	CTrdDog();
	~CTrdDog();
	bool RegisterDog();
	void DeregisterDog();
	bool EatFood();
private:
	CFeedDog	*m_pFdDog;
public:
	int m_trdId;
};


#endif//_TD_FEED_DOG_H_

