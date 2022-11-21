/*
 * Stack.h
 *
 *  Created on: 2012. 2. 23.
 *      Author: darkx
 */

#ifndef STACK_H_
#define STACK_H_

typedef struct tag_StackDataContainer
{
	tag_StackDataContainer*  Back;
	void* ActualData;

}StackDataContainer;

class Stack
{
public:
	Stack();
	~Stack();

	inline void * Pop();
	inline void * Top();
	inline void Push(void * Value);
	int ItemCount();
	inline bool IsEmpty();

private:
	StackDataContainer* Root;
};


#endif /* STACK_H_ */
