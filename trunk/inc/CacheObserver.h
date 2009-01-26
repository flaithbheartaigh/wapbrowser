/*
============================================================================
 Name        : CacheObserver.h
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CacheObserver.h - header file
============================================================================
*/

#ifndef CACHEOBSERVER_H
#define CACHEOBSERVER_H


class MCacheObserver
{
public:
	virtual void	CacheUpdate(const TDesC& aFileName) = 0;
	virtual	void	CacheFailed() = 0;
};

#endif // CACHEOBSERVER_H

// End of File
