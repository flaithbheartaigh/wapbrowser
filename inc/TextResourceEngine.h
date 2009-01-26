/*
============================================================================
 Name        : TextResourceEngine.h
 Author      : ����
 Version     : 2.10
 Copyright   : Soarsky
 Description : ��ȡ������Դ���������н�����⡢��Ŀ��������Ϣ�Ͱ����ȣ�
			   ������Դ�ļ����ڽ���ͬ������ʱ������Դ�ļ����и��²�����
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
	//��ȡ������Դ�б�
	inline CArrayFixFlat<TTEXTRES>* GetTextRes()
	{
		return iResText;
	}

	const TDesC& GetDesById(TInt aId);
private:

	CTextResourceEngine(CMainControlEngine& aMainControlEngine);
	void ConstructL();

	//��ȡ��Դ�ļ�
	TInt ReadTextRes();
private:
	CArrayFixFlat<TTEXTRES>* iResText;

	CMainControlEngine& iMainControlEngine;
};

#endif // TEXTRESOURCEENGINE_H
