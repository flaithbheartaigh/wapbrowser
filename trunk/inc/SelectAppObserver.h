/*
============================================================================
Name        : SelectAppObserver.h
Author      : ∏°…˙»Ù≤Ë
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
