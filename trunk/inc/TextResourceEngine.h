/*
============================================================================
 Name        : TextResourceEngine.h
 Author      : 卞涛
 Version     : 2.10
 Copyright   : Soarsky
 Description : 获取文字资源，例如所有界面标题、栏目、错误信息和帮助等；
			   更新资源文件，在进行同步更新时，对资源文件进行更新操作；
============================================================================
*/

#ifndef TEXTRESOURCEENGINE_H
#define TEXTRESOURCEENGINE_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <CoCoPreDefine.h>
// CLASS DECLARATION
class CMainControlEngine;

/**
*  CTextResourceEngine
* 
*/
class CTextResourceEngine : public CBase
{
public: // Constructors and destructor

	~CTextResourceEngine();
	static CTextResourceEngine* NewL(CMainControlEngine& aMainControlEngine);
	static CTextResourceEngine* NewLC(CMainControlEngine& aMainControlEngine);
public:
	//获取文字资源列表
	inline CArrayFixFlat<TTEXTRES>* GetTextRes()
	{
		return iResText;
	}

	const TDesC& GetDesById(TInt aId);
private:

	CTextResourceEngine(CMainControlEngine& aMainControlEngine);
	void ConstructL();

	//读取资源文件
	TInt ReadTextRes();
private:
	CArrayFixFlat<TTEXTRES>* iResText;

	CMainControlEngine& iMainControlEngine;
};

#endif // TEXTRESOURCEENGINE_H
