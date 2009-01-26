/*
============================================================================
 Name        : PageController.cpp
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CPageController implementation
============================================================================
*/

#include "PageController.h"
#include "PageControlObserver.h"
#include "MainEngine.h"

enum TAddType
{
	EAddBefore,
	EAddAfter
};

CPageController::CPageController(CMainEngine& aMainEngine,MPageControlObserver& aObserver) : iMainEngine(aMainEngine)
																						   , iObserver(aObserver)
{
}


CPageController::~CPageController()
{
}

CPageController* CPageController::NewLC(CMainEngine& aMainEngine,MPageControlObserver& aObserver)
{
	CPageController* self = new (ELeave)CPageController(aMainEngine,aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CPageController* CPageController::NewL(CMainEngine& aMainEngine,MPageControlObserver& aObserver)
{
	CPageController* self=CPageController::NewLC(aMainEngine,aObserver);
	CleanupStack::Pop(); // self;
	return self;
}

void CPageController::ConstructL()
{

}
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void CPageController::SetMaxPageNum(TInt aNum)
{
	iMaxPageNum = aNum;
}

void CPageController::GetPrePage()
{
	if(iCurPageIndex > 0)
	{
		if(iCurArrayIndex == 0)		//左边已没有项,重新拉取
		{
			iObserver.GetNewArray(iCurPageIndex - 1);
			iNewArrayAddType = EAddBefore;
		}
		else
		{
			--iCurPageIndex;
			--iCurArrayIndex;
			iCurArray = iArrayArray[iCurArrayIndex];
			iObserver.Update();
		}
	}
}

void CPageController::GetNextPage()
{
	if(iCurPageIndex < iTotalPageNum - 1)
	{
		if(iCurArrayIndex == iArrayArray.Count() - 1)	//当前项指项队列的最后一项
		{
			iObserver.GetNewArray(iCurPageIndex + 1);
			iNewArrayAddType = EAddAfter;
		}
		else
		{
			++iCurPageIndex;
			++iCurArrayIndex;
			iCurArray = iArrayArray[iCurArrayIndex];
			iObserver.Update();
		}
	}
}
//以下三个函数的实现依赖于需求
void CPageController::GetFirstPage()
{
	//较难控制
}

void CPageController::GetLastPage()
{
	//较难控制
}

void CPageController::Goto(TInt aPageIndex)
{
	//较难控制
}

void CPageController::AddArray(RNewTypeArray* aThreadArray)
{
	iCurArray = aThreadArray;		//新页设置为当前页
	if(iNewArrayAddType == EAddAfter)
	{
		++iCurPageIndex;
		iArrayArray.Append(aThreadArray);
		if(iArrayArray.Count() > iMaxPageNum)
		{
			TInt removeIndex = 0;
			RNewTypeArray* array = iArrayArray[removeIndex];
			array->ResetAndDestroy();
			delete array;
			iArrayArray.Remove(removeIndex);
		}
		iCurArrayIndex = iArrayArray.Count() - 1;
	}
	else//
	{
		--iCurPageIndex;
		iArrayArray.Insert(aThreadArray,0);
		if(iArrayArray.Count() > iMaxPageNum)
		{
			TInt removeIndex = iMaxPageNum;
			RNewTypeArray* postArray = iArrayArray[removeIndex];
			postArray->ResetAndDestroy();
			delete postArray;
			iArrayArray.Remove(removeIndex);
		}
		iCurArrayIndex = 0;
	}
}
