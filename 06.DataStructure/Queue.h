/*
 * Queue
 *
 *  Created on: 2012. 2. 23.
 *      Author: darkx
 */

#ifndef QUEUE_h
#define QUEUE_h

typedef struct tag_QueueDataContainer
{
	tag_QueueDataContainer*  Next;
	void* ActualData;

}QueueDataContainer;

class Queue
{
public:
	Queue();
	~Queue();

	inline void * Get();

	inline void * Top();

	void Put(void * Value);

	int ItemCount();
	inline bool IsEmpty();

protected:

private:
	QueueDataContainer* Root;
};

#endif /* QUEUE_ */
