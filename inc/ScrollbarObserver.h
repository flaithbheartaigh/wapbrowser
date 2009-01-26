/*
============================================================================
 Name        : ScrollbarObserver.h
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : ScrollbarObserver.h - header file
============================================================================
*/

#ifndef SCROLLBAROBSERVER_H
#define SCROLLBAROBSERVER_H

class MScrollbarObserver
{
public:
	virtual void ScrollBarIndexChanged(TInt aNewIndex) = 0;
};

#endif // SCROLLBAROBSERVER_H

// End of File
