/*
============================================================================
 Name        : DirBrowser.cpp
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CDirBrowser implementation
============================================================================
*/

#include "DirBrowser.h"

CDirBrowser::CDirBrowser(CMainEngine& aMainEngine,TInt aSelType)
{
	// No implementation required
}


CDirBrowser::~CDirBrowser()
{
	delete iSaveAsBmp;
	iSaveAsBmp = NULL;
	return TRUE;
}

CDirBrowser* CDirBrowser::NewLC(CMainEngine& aMainEngine,const TDesC& aTitleText,TInt aSelType)
{
	CDirBrowser* self = new (ELeave)CDirBrowser();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CDirBrowser* CDirBrowser::NewL(CMainEngine& aMainEngine,const TDesC& aTitleText,TInt aSelType)
{
	CDirBrowser* self=CDirBrowser::NewLC();
	CleanupStack::Pop(); // self;
	return self;
}

void CDirBrowser::ConstructL(const TDesC& aTitleText)
{
	iTitleText = aTitleText.AllocL();

	InitBitmaps();

	CArrayFixFlat<TPOPUPTEXT>* pRes = GetEngine()->GetRes();

	if(m_nSelType==0)
	{
		SetLeftKeyText(pRes->At(ETurkeyTextRes_Save));
	}

	if(m_nSelType==1)
	{	
		SetLeftKeyText(pRes->At(ETurkeyTextRes_Ok));
	}

	SetRightKeyText(pRes->At(ETurkeyTextRes_Back));
}
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void CDirBrowser::Draw(CGraphic& aGraphic) const
{
	if (NULL == m_pPathArray)
		return TRUE;
	CBaseBrowserState::ReDraw();
	CGraphics* gc = GetGc();
	CBitmapContext* bmpGc = gc->GetBmpGc();

	// 	TPoint point(0,0);
	// 	point.SetXY(9,55);

	bmpGc->BitBlt(iStartPoint,iSaveAsBmp);

	/*
	//gc->BitBltMasked(point , m_pBmpFileBarArray, 0);	
	gc->SetPenColor(TRgb(KRgbBlack));
	gc->SetPenStyle(CGraphicsContext::ESolidPen);
	gc->SetFont(GetEngine()->GetFont());
	gc->BitBltMasked(point,GetEngine()->GetBmpSaveAs());
	*/

	gc->SetPenColor(TRgb(KRgbBlack));
	gc->SetPenStyle(CGraphicsContext::ESolidPen);
	gc->SetFont(GetEngine()->GetFont());

	TPoint titlePos;
	TPoint listPos;
	iScreenLayout->GetSaveAsPos(titlePos,listPos);
	if(m_nTitleText.Length() > 0)
	{
		gc->DrawText(iStartPoint + titlePos, m_nTitleText);			
	}

	CBmpPictureArray* pListIcon = GetEngine()->GetListIcon();

	gc->SetPenStyle(CGraphicsContext::ENullPen);

	listPos += iStartPoint;
	for (int i = 0; i < SAVEASMAXLINE && (m_nFirstIndex+i < m_pPathArray->Count() ); i++)
	{
		gc->SetBrushStyle(CGraphicsContext::ENullBrush);
		gc->SetPenColor(TRgb(KRgbBlack));

		if (m_nSelIndex == i)
		{			
			TRgb rgb(153,190,47);
			gc->SetBrushColor(rgb);
			//gc->SetPenColor(TRgb(KRgbBlack));
			gc->SetPenStyle(CGraphicsContext::ESolidPen);
			gc->SetBrushStyle(CGraphicsContext::ESolidBrush);			
			//TRect rect(TPoint(13, iSaveAsBmp * i + 90), TSize(152,20));
			//gc->SetClippingRect(rect);
			TRect rect(listPos,TSize(iBmpSize.iWidth,iLineHeight));
			gc->DrawRect(rect);
			gc->SetBrushStyle(CGraphicsContext::ENullBrush);
			//gc->SetPenColor(TRgb(KRgbBlack));
		}		
		//gc->SetClippingRect(TRect(TPoint(9,55),TSize(163, 120)));
		//TPOPUPTEXT nBuf = ;

		gc->BitBltMasked(listPos,pListIcon,7);
		TSize iconSize = pListIcon->IconSize(7);
		TPoint textPoint = listPos;
		textPoint.iX += iconSize.iWidth;
		//textPoint.iY += 
		gc->DrawText(textPoint, m_pPathArray->At(m_nFirstIndex+i));		
		listPos.iY += iLineHeight;	
		//gc->SetClippingRect(TRect(TSize(SCREEN_WIDTH, SCREEN_HEIGHT)));
	}
	gc->SetPenStyle(CGraphicsContext::ESolidPen);

	if(NULL != GetChild())
		GetChild()->ReDraw();
	if(iDialog)
	{
		CBitmapContext* bmpGc = gc->GetBmpGc();
		iDialog->Draw(*bmpGc);
	}

}

TBool CDirBrowser::KeyEventL(TInt aKeyCode)
{
	if(NULL != GetChild())
	{
		GetChild()->NewEvent(nEventType1, 0, pEventBuf, 0);	
		return ETrue;
	}

	if (nEventType1 != ObserverEvent_Key)
		return FALSE;

	TTurkeyKeyEvent* pEvent = (TTurkeyKeyEvent*)pEventBuf;	
	if(iDialog)
	{
		return iDialog->KeyEventL(pEvent->nKeyCode );
	}
	switch(pEvent->nScanCode) 
	{				
	case ScanCode_Up://选择目录
		if (--m_nSelIndex < 0)
		{
			m_nSelIndex = 0;
			if (m_nFirstIndex > 0)
				--m_nFirstIndex;
		}
		break;
	case ScanCode_Down://选择目录
		if (++m_nSelIndex >= m_pPathArray->Count()) 
		{
			m_nSelIndex --;
		}
		else if (m_nSelIndex >= SAVEASMAXLINE)
		{
			m_nSelIndex = SAVEASMAXLINE - 1;
			if (m_nFirstIndex + SAVEASMAXLINE < m_pPathArray->Count())
				m_nFirstIndex ++;
		}
		break;
	case ScanCode_Right://进入子目录
	case ScanCode_Enter:
		ReadSubDir();
		m_nFirstIndex = m_nSelIndex = 0;			
		break;
	case ScanCode_Left://返回上级目录
		ReadParentsDir();
		m_nFirstIndex = m_nSelIndex = 0;
		break;
		//ScanCode_Enter
	case ScanCode_F1:
		{
			if(m_nSelType==0)
			{
				//另存为
				TInt i= SaveFile(0);

				if(i==0)
				{

					ShowDialog(GetEngine()->GetRes()->At(ETurkeyTextRes_NewSearch_SaveFile),CDialog::EDialogWaiting);

					//保存成功
					/*CBrowserDialog* pState = new(ELeave)CBrowserDialog(this, 
					GetEngine(), 
					1, 
					GetEngine()->GetRes()->At(ETurkeyTextRes_SaveSucess));
					pState->Activate();
					pState->SetParent(this);
					SetChild(pState);*/
				}
				else if(i==1)
				{
					//文件不存在
					CBrowserDialog* pState = new(ELeave)CBrowserDialog(this, 
						GetEngine(), 
						0, 
						GetEngine()->GetRes()->At(ETurkeyTextRes_SaveAs_NoSourceFile));
					pState->Activate();
					pState->SetParent(this);
					SetChild(pState);
				}
				else if(i==2)
				{
					//存储空间不够
					CBrowserDialog* pState = new(ELeave)CBrowserDialog(this, 
						GetEngine(), 
						0, 
						GetEngine()->GetRes()->At(ETurkeyTextRes_SaveAs_NoSpace));
					pState->Activate();
					pState->SetParent(this);
					SetChild(pState);
				}
				else if(i==3)
				{
					//有同名文件存在
					TBuf<100> nInfo;
					nInfo.Zero();
					nInfo.Append (GetEngine()->GetRes()->At(ETurkeyTextRes_File));

					//获取文件名称
					TBuf<50> nFileName;
					TParse parse;

					parse.Set(m_pFilePath,NULL,NULL);
					nFileName.Copy(parse.NameAndExt());

					nInfo.Append (nFileName);
					nInfo.Append (GetEngine()->GetRes()->At(ETurkeyTextRes_SaveAs_FileExist));

					CBrowserDialog* pState = new(ELeave)CBrowserDialog(this, 
						GetEngine(), 
						2, 
						nInfo);
					pState->Activate();
					pState->SetParent(this);
					SetChild(pState);
				}
			}

			if(m_nSelType==1)
			{
				//更改保存目录
				CBrowserDialog* pState = new(ELeave)CBrowserDialog(this, 
					GetEngine(), 
					2, 
					GetEngine()->GetRes()->At(ETurkeyTextRes_Setting_DownLoadDir_IsOK));
				pState->Activate();
				pState->SetParent(this);
				SetChild(pState);
			}
			return TRUE;
		}
		break;
	case ScanCode_F2://返回上级菜单
		//	GetEngine()->SetCurState(GetParent());
		{
			GetParent()->SetChild(NULL);
			return TRUE;	
		}
		break;
	}
	return EFalse;
}

TBool CDirBrowser::HandleCommandL(TInt aCommand)
{
	return EFalse;
}

void CDirBrowser::SizeChanged(const TRect& aScreenRect)
{

}
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void CDirBrowser::SetPath(const TDesC& aFilePath)
{
	m_pFilePath.Append(aFilePath);
	delete m_pPathArray;
	m_pPathArray = new (ELeave) CArrayFixFlat<TFileName>(10);
	m_pPathArray->AppendL(_L("C:"));
	//a b c d e 0 1 2 3 4 5
	if (RFs::IsValidDrive(4)) 
	{
		m_pPathArray->AppendL(_L("E:"));
	}
}

TInt CDirBrowser::SaveFile(TInt IsOverwrite)
{	
	//返回值：1-文件不存在；2-存储空间不足
	TInt aFileLen=0;

	TFileName szNewFileName;
	TInt nFileNum=0, nFileExNum=0;	

	TFileName szSrcPathname;
	szSrcPathname.Append(m_pFilePath);

	::GetFileName1(szSrcPathname, nFileNum, nFileExNum);
	szNewFileName = szSrcPathname.Right(nFileNum);

	TFileName szNewFileNamePath;
	szNewFileNamePath.Append(m_pCurPath);
	szNewFileNamePath.Append(m_pPathArray->At(m_nFirstIndex + m_nSelIndex));
	szNewFileNamePath.Append(_L("\\"));
	szNewFileNamePath.Append(szNewFileName);

	if(iNewFileName.Length()>0 )
	{
		rfs.Delete(iNewFileName);
	}

	iNewFileName.Zero ();
	iNewFileName.Copy (szNewFileNamePath);

	//判断是否已有此文件
	if(IsOverwrite==0)
	{
		RFile tFile;
		TInt ret =0;

		ret = tFile.Open(rfs, szNewFileNamePath, EFileWrite|EFileShareAny);
		if (KErrNone == ret)
		{
			tFile.Close();
			return 3;
		}
		else
			tFile.Close();
	}

	RFile oFile;
	TInt rv =0;

	rv = oFile.Open(rfs, szSrcPathname, EFileWrite|EFileShareAny);
	if (KErrNone != rv)
	{
		return 1;
	}
	oFile.Size(aFileLen);

	aFileLen=aFileLen/1024;

	TInt64 nDriverSize=::GetFreeSpace (szNewFileNamePath.Mid(0,1));

	if((TInt64)aFileLen>nDriverSize)
	{
		//存储空间不足
		oFile.Close();
		return	2;
	}
	oFile.Close();

	iCancel=0;

	if(iFileEngine)
	{
		iFileEngine->Cancel();
		delete iFileEngine;
		iFileEngine=NULL;
	}	

	iFileEngine=CFileEngine::NewLC ();
	CleanupStack::Pop();

	iFileEngine->CopyFile(szSrcPathname,szNewFileNamePath,this);

	return 0;
}

void CDirBrowser::ReadSubDir()
{
	TFileName path;
	path.Append(m_pCurPath);
	path.Append(m_pPathArray->At(m_nFirstIndex + m_nSelIndex));
	path.Append(_L("\\"));
	RFs rfs;
	rfs.Connect();
	CDir*	pDir;
	rfs.GetDir(path, KEntryAttNormal | KEntryAttDir, ESortByName, pDir);	
	TBool isDir = EFalse;
	int i=0; 
	for (; pDir && (i<pDir->Count()); i++)
	{
		TEntry oName = (*pDir)[i];
		if (oName.IsDir())
		{
			if (!isDir)
			{
				m_pCurPath.Append(m_pPathArray->At(m_nFirstIndex + m_nSelIndex));
				m_pCurPath.Append(_L("\\"));
				delete m_pPathArray;
				m_pPathArray = new (ELeave) CArrayFixFlat<TFileName>(10);
				isDir = ETrue;
			}
			TFileName szPath;
			szPath.Append(oName.iName);
			m_pPathArray->AppendL(szPath);			
		}
	}
	delete pDir;
	rfs.Close();
}



void CDirBrowser::ReadParentsDir()
{
	if (m_pCurPath.Length() <= 3)
	{
		if (m_pPathArray)
			delete m_pPathArray;
		m_pPathArray = new (ELeave) CArrayFixFlat<TFileName>(10);
		m_pCurPath.Delete(0, m_pCurPath.Length());
		m_pPathArray->AppendL(_L("C:"));
		//a b c d e 0 1 2 3 4 5
		if (RFs::IsValidDrive(4)) 
		{
			m_pPathArray->AppendL(_L("E:"));
		}
		return;
	}
	TFileName szPath;
	szPath = m_pCurPath.Left(m_pCurPath.Length() - 1);	
	TInt nFileNum=0, nFileExNum=0;	
	::GetFileName1(szPath, nFileNum, nFileExNum);
	TInt length = szPath.Length() - nFileNum;
	TFileName path;
	path = szPath.Left(length);
	RFs rfs;
	rfs.Connect();
	CDir*	pDir;
	rfs.GetDir(path, KEntryAttNormal | KEntryAttDir, ESortByName, pDir);	
	TBool isDir = EFalse;
	int i=0; 
	for (; pDir && (i<pDir->Count()); i++)
	{
		TEntry oName = (*pDir)[i];
		if (oName.IsDir())
		{
			if (!isDir)
			{
				m_pCurPath.Delete(0, m_pCurPath.Length());
				m_pCurPath.Append(path);
				delete m_pPathArray;
				m_pPathArray = new (ELeave) CArrayFixFlat<TFileName>(10);
				isDir = ETrue;
			}
			TFileName szPath1;
			szPath1.Append(oName.iName);
			m_pPathArray->AppendL(szPath1);			
		}
	}
	delete pDir;
	rfs.Close();
}
void CDirBrowser::DialogConfirm()
{
	/*delete iDialog;
	iDialog = NULL;*/
}

void CDirBrowser::DialogCancel()
{	
	if(iDialog)
	{
		delete iDialog;
		iDialog = NULL;
	}

	iCancel=1;

	iFileEngine->Cancel();
}

void CDirBrowser::ShowDialog(const TDesC& aDes,TInt aType)
{
	//= CDialog::EDialogTip
	delete iDialog;
	iDialog = NULL;

	iDialog = CDialog::NewL(aDes,aType,this,GetEngine());
	iDialog->SetEngine(GetEngine());
}

void CDirBrowser::HandleComplete(TInt aStatus,TInt aType)
{
	if(iDialog)
	{
		delete iDialog;
		iDialog = NULL;
	}

	if(aStatus==KErrNone && aType==0)
	{
		CBrowserDialog* pState = new(ELeave)CBrowserDialog(this, 
			GetEngine(), 
			1, 
			GetEngine()->GetRes()->At(ETurkeyTextRes_SaveSucess));
		pState->Activate();
		pState->SetParent(this);
		SetChild(pState);

		iCancel=0;
	}
}

void CDirBrowser::InitBitmaps()
{
	iSaveAsBmp = GetEngine()->CreateBmpL(EMbmSoarskyresSaveas);

	TInt multiple = iScreenLayout->Multiple();
	TInt divisor = iScreenLayout->Divisor();

	iBmpSize = iSaveAsBmp->SizeInPixels();
	iBmpSize.iWidth = (iBmpSize.iWidth*multiple)/divisor;
	iBmpSize.iHeight = (iBmpSize.iHeight*multiple)/divisor;

	AknIconUtils::SetSize(iSaveAsBmp,iBmpSize);

	//上下左右居中
	TRect clientRect = iScreenLayout->GetClientRect();

	iStartPoint = clientRect.iTl;
	iStartPoint.iX += (clientRect.Width() - iBmpSize.iWidth)/2;
	iStartPoint.iY += (clientRect.Height() - iBmpSize.iHeight)/2;

	iLineHeight = iScreenLayout->LineHeight();
}
