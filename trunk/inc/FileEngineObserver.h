/*
============================================================================
 Name        : FileEngineObserver.h
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : FileEngineObserver.h - header file
============================================================================
*/

#ifndef FILEENGINEOBSERVER_H
#define FILEENGINEOBSERVER_H

enum TUpdateReason
{
	ECopyCompleted,
	ECreateFolderCompleted,
	ERenameCompleted,
	EDeleteCompleted,
	ESaveAsCompleted,
};

class MFileEngineEvent
{
public:
	virtual void Update(TInt aReason) = 0;
};

#endif // FILEENGINEOBSERVER_H

// End of File
