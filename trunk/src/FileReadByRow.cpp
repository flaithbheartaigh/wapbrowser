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
	//����FF FEǰ׺
	const TDesC& des = ptr.Mid(1);


	//˳���ȡ�����ļ��е����ݣ��������в�֣��õ�ʱ���ڴ���
	TInt textPos = 0;
	while(textPos < des.Length())
	{	
		const TDesC& tmpDes = des.Mid(textPos);
		TInt charNum = tmpDes.Find(_L("\r\n"));//�س�����

		if(charNum == KErrNotFound)
		{
			iItem->AppendL(tmpDes);
			break;//˵���ļ��������˳�

		}
		else
		{
			if (charNum!=0)//��ֹ�ļ����кܶ�س����е����
			{
				iItem->AppendL(des.Mid(textPos,charNum));
			}


		}
		textPos += charNum;
		textPos += 2;//�س����У�

	}	
	CleanupStack::PopAndDestroy();		//fileContent
}
const TDesC & CFileReadByRow::GetTextById(TInt aId)
{

	return (*iItem)[aId];
}
