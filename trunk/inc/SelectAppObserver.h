/*
============================================================================
Name        : SelectAppObserver.h
Author      : ��������
Version     :
Copyright   : Your copyright notice
Description : FileBrowserObserver.h - header file
============================================================================
*/

#ifndef SELECTAPPOBSERVER_H
#define SELECTAPPOBSERVER_H


class MSelectAppObserver
{
public:
	virtual void	DoSelectEvent(const TDesC& aAppName,TInt aCommand) = 0;
};

#endif 

// End of File
