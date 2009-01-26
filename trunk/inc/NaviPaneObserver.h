/*
============================================================================
 Name        : NaviPaneObserver.h
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : NaviPaneObserver.h - header file
============================================================================
*/

#ifndef NAVIPANEOBSERVER_H
#define NAVIPANEOBSERVER_H


class MNaviPaneObserver
{
public:
	virtual void NaviPaneIndexChanged(TInt aNewIndex) = 0;
};

#endif // NAVIPANEOBSERVER_H

// End of File
