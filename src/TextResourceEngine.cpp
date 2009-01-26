/*
============================================================================
 Name        : TextResourceEngine.cpp
 Author      : 卞涛
 Version     : 2.10
 Copyright   : Soarsky
 Description : 获取文字资源，例如所有界面标题、栏目、错误信息和帮助等；
			   更新资源文件，在进行同步更新时，对资源文件进行更新操作；
============================================================================
*/

#include "TextResourceEngine.h"
#include "MainControlEngine.h"
#include <S32FILE.H>
CTextResourceEngine::CTextResourceEngine(CMainControlEngine& aMainControlEngine)
:iMainControlEngine(aMainControlEngine)
,iResText(NULL)
{
	// No implementation required
}


CTextResourceEngine::~CTextResourceEngine()
{
	DELETE(iResText);
}

CTextResourceEngine* CTextResourceEngine::NewLC(CMainControlEngine& aMainControlEngine)
{
	CTextResourceEngine* self = new (ELeave)CTextResourceEngine(aMainControlEngine);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CTextResourceEngine* CTextResourceEngine::NewL(CMainControlEngine& aMainControlEngine)
{
	CTextResourceEngine* self=CTextResourceEngine::NewLC(aMainControlEngine);
	CleanupStack::Pop(); // self;
	return self;
}

void CTextResourceEngine::ConstructL()
{
	ReadTextRes();
}

const TDesC& CTextResourceEngine::GetDesById(TInt aDesId)
{
	ASSERT(iResText);
	ASSERT(aDesId >= 0 && aDesId < iResText->Count());
	const TDesC& des = iResText->At(aDesId);
	return des;
}

TInt CTextResourceEngine::ReadTextRes()
{
	TInt returnErr=1;

	if (NULL == iResText) 
		iResText = new (ELeave) CArrayFixFlat<TTEXTRES>(10);
	else
	{
		DELETE(iResText);
		iResText = new (ELeave) CArrayFixFlat<TTEXTRES>(10);
	}

	_LIT(KTEXTRESFILENAME, "turkeyres.txt");

	TFileName	szPathname;
#ifndef __WINS__
	szPathname.Append(iMainControlEngine.GetAppPath());
#else
	szPathname.Append(_L("C:\\system\\apps\\coco\\"));
#endif
	szPathname.Append(KTEXTRESFILENAME);

	//User::InfoPrint(szPathname);

	RFs		rfs;
	User::LeaveIfError(rfs.Connect());
	RFile	file;
	TInt	nFileSize;
	if (KErrNone != file.Open(rfs, szPathname, EFileRead))
	{
		ASSERT(EFalse);
		rfs.Close();
		returnErr=0;
	}
	else
	{
		file.Size(nFileSize);
		file.Close();
		if (nFileSize <= TInt(sizeof(TUint16)))
		{
			rfs.Close();
			returnErr=0;
		}
		else
		{
			TInt	nCharCount = (nFileSize - sizeof(TUint16)) / sizeof(TUint16);
			RFileReadStream	rfrs;
			rfrs.Open(rfs, szPathname, EFileRead); 
			rfrs.ReadUint16L();	//file flag
			TInt	nIndex;
			TTEXTRES szText;
			for (nIndex = 0; nIndex < nCharCount; nIndex ++)
			{
				TUint16	wWord = rfrs.ReadUint16L();		
				if (wWord == 0x0D)
					continue;					
				if (wWord == 0x0A)
				{
					iResText->AppendL(szText);
					szText.Delete(0, szText.Length());
					continue;
				}
				szText.Append(&wWord, 1);		
			}
			rfrs.Close();
			rfs.Close();
		}
	}
	return returnErr;
}
