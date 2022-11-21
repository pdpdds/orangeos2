#include "Queue.h"

Queue::Queue()
	{Root=0;
	}
Queue::~Queue()
	{
	QueueDataContainer * D,*tmp;
	D=Root;
	while( D )
		{tmp=D;
		D=D->Next;
		delete tmp;
		}
	}

inline void * Queue::Get()
	{if ( IsEmpty() )
		return 0;
	else
		{
		void * x;
		QueueDataContainer * tmp=Root;
		x=Root->ActualData;
		Root=Root->Next;
		delete tmp;
		return x;
		}
	}
inline void * Queue::Top()
	{return IsEmpty()?0:Root->ActualData;
	}
void Queue::Put(void * Value)
	{QueueDataContainer * New,* Last;

	New=new (QueueDataContainer);
	New->Next=0;
	New->ActualData=Value;
	if ( Root == 0 )
		Root=New;
	else
		{Last=Root;
		while( Last->Next )
			Last=Last->Next;
		Last->Next=New;
		}
	}
int Queue::ItemCount()
	{int i=0;
	QueueDataContainer * D;
	D=Root;
	while( D )
		{D=D->Next;
		i++;
		}
	return i;
	}
inline bool Queue::IsEmpty()
	{return Root==0?true:false;
	}
