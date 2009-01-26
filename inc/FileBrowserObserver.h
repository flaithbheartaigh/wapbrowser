/*
============================================================================
 Name        : FileBrowserObserver.h
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : FileBrowserObserver.h - header file
============================================================================
*/

#ifndef FILEBROWSEROBSERVER_H
#define FILEBROWSEROBSERVER_H


class MFileBrowserObserver
{
public:
	virtual void	DoFileEvent(const TDesC& aFileName,TInt aCommand) = 0;
};

#endif // FILEBROWSEROBSERVER_H

// End of File
