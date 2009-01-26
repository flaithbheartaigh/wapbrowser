/*
============================================================================
 Name        : WindowFactory.h
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : 界面工厂
 补充说明	 : 以后扩充以实现通过DLL动态扩充的情况
============================================================================
*/

#ifndef WINDOWFACTORY_H
#define WINDOWFACTORY_H

#include <e32std.h>
#include <e32base.h>

class CWindow;
class CMainEngine;

class CWindowFactory : public CBase
{
public: // Constructors and destructor
	~CWindowFactory();
	static CWindowFactory* NewL(CMainEngine& aMainEngine);

private:
	CWindowFactory(CMainEngine& aMainEngine);
	void ConstructL();

public:
	CWindow* CreateWindow(TInt aWindowType,CWindow* aParent) const;

	CWindow* CreateSearchLocalResultView(TInt aWindowType,CWindow* aParent,TInt aIndex,const TDesC& aTitle) const;
	CWindow* CreateSearchResultView(TInt aWindowType,CWindow* aParent,TInt aIndex,const TDesC8& aUrl,const TDesC& aTitle) const;
private:
	CMainEngine&	iMainEngine;
};

#endif // WINDOWFACTORY_H
