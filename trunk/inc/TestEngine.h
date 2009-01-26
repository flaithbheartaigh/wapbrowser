/*
============================================================================
 Name        : TestEngine.h
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : 测试代码可以写在这里
============================================================================
*/

#ifndef TESTENGINE_H
#define TESTENGINE_H

#include "PreDefine.h"
#include "CacheObserver.h"

class CMainEngine;

class CCacheEngine;

class CTestEngine : public CBase
				  , public MCacheObserver
{
public: // Constructors and destructor

	/**
        * Destructor.
        */
	~CTestEngine();

        /**
        * Two-phased constructor.
        */
	static CTestEngine* NewL(CMainEngine& aMainEngine);

        /**
        * Two-phased constructor.
        */
	static CTestEngine* NewLC(CMainEngine& aMainEngine);

private:

	/**
        * Constructor for performing 1st stage construction
        */
	CTestEngine(CMainEngine& aMainEngine);

	/**
        * EPOC default constructor for performing 2nd stage construction
        */
	void ConstructL();

public://From MCacheObserver
	virtual void	CacheUpdate(const TDesC& aFileName);
	virtual	void	CacheFailed();

public:
	void	TestCacheEngine();

public:
	CMainEngine&	iMainEngine;

	CCacheEngine*	iCacheEngine;
};

#endif // TESTENGINE_H
