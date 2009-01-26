/*
============================================================================
 Name        : FileReadByRow.cpp
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : CFileReadByRow implementation
============================================================================
*/
#include <f32file.h>
#include <coemain.h>
#include "FileReadByRow.h"
#include "UtilityTools.h"

CFileReadByRow::CFileReadByRow()
{
	// No implementation required
}


CFileReadByRow::~CFileReadByRow()
{
	if (iItem!=NULL)
	{
		delete iItem;
	}
	
}

CFileReadByRow* CFileReadByRow::NewLC()
{
	CFileReadByRow* self = new (ELeave)CFileReadByRow();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CFileReadByRow* CFileReadByRow::NewL()
{
	CFileReadByRow* self=CFileReadByRow::NewLC();
	CleanupStack::Pop(); // self;
	return self;
}

void CFileReadByRow::ConstructL()
{
	if (!iItem)
	{
		delete iItem;
		iItem=NULL;
	}
	
	iItem = new (ELeave) CDesCArrayFlat(10);
}

void CFileReadByRow::LoadItem(const TDesC& aFileName)
{

	int i=0;
	RFs& fs = CCoeEnv::Static()->FsSession();
	HBufC8* fileContent = UtilityTools::GetFileContentL(fs,aFileName);
	CleanupStack::PushL(fileContent);

	

	TPtrC ptr((const TUint16*)(fileContent->Ptr()),fileContent->Length()/2);
	//处理FF FE前缀
	const TDesC& des = ptr.Mid(1);


	//顺序读取配置文件中的数据，并不进行拆分，用的时候在处理
	TInt textPos = 0;
	while(textPos < des.Length())
	{	
		const TDesC& tmpDes = des.Mid(textPos);
		TInt charNum = tmpDes.Find(_L("\r\n"));//回车换行

		if(charNum == KErrNotFound)
		{
			iItem->AppendL(tmpDes);
			break;//说明文件结束，退出

		}
		else
		{
			if (charNum!=0)//防止文件中有很多回车换行的情况
			{
				iItem->AppendL(des.Mid(textPos,charNum));
			}


		}
		textPos += charNum;
		textPos += 2;//回车换行，

	}	
	CleanupStack::PopAndDestroy();		//fileContent
}
const TDesC & CFileReadByRow::GetTextById(TInt aId)
{

	return (*iItem)[aId];
}
