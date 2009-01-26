/*
============================================================================
 Name        : PageController.h
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CPageController declaration
 说明		 : 该类给有分页的界面提供分页控制
============================================================================
*/

#ifndef PAGECONTROLLER_H
#define PAGECONTROLLER_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

class CMainEngine;
class MPageControlObserver;

class TNewType;

//template <class TNewType>
class CPageController : public CBase
{
public:
	typedef RPointerArray<TNewType>	RNewTypeArray;

public: // Constructors and destructor
	~CPageController();
	static CPageController* NewL(CMainEngine& aMainEngine,MPageControlObserver& aObserver);
	static CPageController* NewLC(CMainEngine& aMainEngine,MPageControlObserver& aObserver);

private:
	CPageController(CMainEngine& aMainEngine,MPageControlObserver& aObserver);
	void ConstructL();

public:
	void	SetMaxPageNum(TInt aNum);
	void	GetPrePage();
	void	GetNextPage();
	void	GetFirstPage();
	void	GetLastPage();
	void	Goto(TInt aPageIndex);
	void	AddArray(RNewTypeArray* aThreadArray);

private:
	CMainEngine&	iMainEngine;

	MPageControlObserver&	iObserver;
	
	RPointerArray<RNewTypeArray>	iArrayArray;
	RNewTypeArray*	iCurArray;

	TInt			iCurPageIndex;
	TInt			iTotalPageNum;
	TInt			iCurArrayIndex;
	TInt			iNewArrayAddType;
	TInt			iMaxPageNum;

};

#endif // PAGECONTROLLER_H
