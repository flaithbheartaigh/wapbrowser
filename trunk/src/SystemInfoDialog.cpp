/*
============================================================================
 Name        : SystemInfoDialog.cpp
 Author      : 卞涛
 Version     : 2.10
 Copyright   : Soarsky
 Description : 显示公告信息
============================================================================
*/

#include "SystemInfoDialog.h"
#include "MainEngine.h"

#include "MControlObserver.h"

#include "ScreenLayout.h"
#include "TypeDefine.h"
#include "cocobmp.h"

#include "utf.h"
#include "Graphic.h"

CSystemInfoDialog::CSystemInfoDialog(MInfoObserver& aObserver,CMainEngine& aMainEngine)
:CControl(EInfoDialog)
,iObserver(aObserver)
,iMainEngine(aMainEngine)
,iScreenLayout(aMainEngine.ScreenLayout())
,iFlag(0)
,iPage(0)
,iSFlag(0)
,iShowType(0)
,iUrlIndex(-1)
,iSelIndex(-1)
{
	// No implementation required
	iTextData=new (ELeave)CArrayFixFlat<TCONTENTTEXT>(10); //每行显示文字信息内容
	iAIndexData=new (ELeave)CArrayFixFlat<TURLTEXT>(10); //对于超链接的索引，存放起始位置，标题长度和链接URL

	SetDrawEventObserver(&aMainEngine);
	SetCleanUpObserver(&aMainEngine);
	iHasButton = ETrue;
}


CSystemInfoDialog::~CSystemInfoDialog()
{
	delete iTextData;
	delete iAIndexData;
}

CSystemInfoDialog* CSystemInfoDialog::NewLC(MInfoObserver& aObserver,CMainEngine& aMainEngine)
{
	CSystemInfoDialog* self = new (ELeave)CSystemInfoDialog(aObserver, aMainEngine);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CSystemInfoDialog* CSystemInfoDialog::NewL(MInfoObserver& aObserver,CMainEngine& aMainEngine)
{
	CSystemInfoDialog* self=CSystemInfoDialog::NewLC(aObserver, aMainEngine);
	CleanupStack::Pop(); // self;
	return self;
}

void CSystemInfoDialog::ConstructL()
{
	iFontHeight=iScreenLayout.FontHeight();
	iLineHeight=iFontHeight+iFontHeight/2;

	iMaxLine=iScreenLayout.LineNum();
	iMaxLine++;

	iLRect=iScreenLayout.GetClientRect();
	iBRect=iScreenLayout.GetClientRect();

	iSSzie=iScreenLayout.ScreenSize();

	/*TPoint point=iLRect.iTl-TPoint(0,iLRect.Height());
	TRect rect(point, TSize(iLRect.Width(),iLRect.Height()*2+iBRect.Height()));
	iScrollBar = iControlFactory->CreateScrollBar(rect);
	iScrollBar->SetMaxLinePerPage(iMaxLine);
	iScrollBar->SetTotalLineNum(iTextData->Count());
	iScrollBar->Layout();	*/

	iFlag=0;
	iSFlag=0;
	iPage=0;
	iUrlIndex=-1;
	iSelIndex=-1;
}

void CSystemInfoDialog::SetInitValue()
{
	TBuf<100> sText;
	TBuf<100> tempSText;
	TBuf<10> textIndex;
	TBuf<10> sTLength;
	TBuf<10> sTTLength;
	TBuf<10> sUrlIndex;
	
	TInt i_s=0;
	if(iTextData)
	{
		for(TInt i=0;i<iMaxLine;i++)
		{
			if((i+iPage*iMaxLine)>iTextData->Count()-1)
			{
				break;
			}

			sText.Zero();
			sText.Append(iTextData->At(i+iPage*iMaxLine));

			if(sText.Find(_L("|"))!=KErrNotFound)
			{
				i_s++;

				TInt i_i=sText.Find(_L("|"));

				tempSText.Zero();
				textIndex.Zero();
				sTLength.Zero();
				sTTLength.Zero();
				sUrlIndex.Zero();
				
				tempSText.Append(sText.Mid(0,i_i));//显示的一行字符
				sText.Delete(0,i_i+1);
				i_i=sText.Find(_L("|"));
				textIndex.Append(sText.Mid(0,i_i));//字符串中超链接标题起始位置
				sText.Delete(0,i_i+1);
				i_i=sText.Find(_L("|"));
				sTLength.Append(sText.Mid(0,i_i));//超链接标题分割长度
				sText.Delete(0,i_i+1);
				i_i=sText.Find(_L("|"));
				sTTLength.Append(sText.Mid(0,i_i));//超链接标题总长度
				sText.Delete(0,i_i+1);
				sUrlIndex.Append(sText);//对应url索引


				//字符串中超链接标题起始位置
				TLex temp1(textIndex);
				TInt b1;
				TInt iVal1=temp1.Val( b1 );

				//超链接标题分割长度
				TLex temp2(sTLength);
				TInt b2;
				TInt iVal2=temp2.Val( b2 );

				//超链接标题总长度
				TLex temp3(sTTLength);
				TInt b3;
				TInt iVal3=temp3.Val( b3 );

				//对应url索引
				TLex temp4(sUrlIndex);
				TInt b4;
				TInt iVal4=temp4.Val( b4 );
				
				//设置被选中的索引
				if(iFlag==0)
				{
					iFlag=1;
					iSFlag=1;
					iUrlIndex=b4;
					iSelIndex=i+iPage*iMaxLine;
				}
				if(b3>b2)
				{
					i++;
				}

				if(iSFlag==i_s)
				{	
					iUrlIndex=b4;
					iSelIndex=i+iPage*iMaxLine;
				}
			}	
		}
		iFlag=i_s;
	}	
}
void CSystemInfoDialog::DrawInfo(CGraphic& gc) const
{
	TPoint point=iLRect.iTl;
	TRect rect=iMainEngine.ScreenLayout().GetClientRect();

	gc.SetPenColor(TRgb(255,255,255));
	gc.SetBrushColor(TRgb(64,61,90));
	gc.SetPenStyle(CBitmapContext::ESolidPen);
	gc.SetBrushStyle(CBitmapContext::ESolidBrush);

	gc.DrawRoundRect(rect);

	TInt i_rww=iScreenLayout.Font()->MeasureText(iTitle);
	if(i_rww>(iScreenLayout.WhiteBackgroundRect().Width()-20))
	{
		TBuf<50> temp_nameBufText;
		temp_nameBufText.Zero();
		TInt i_rw=0;


		gc.DrawText(point+TPoint(iLineHeight/3,iLineHeight/6), temp_nameBufText);
	}
	else
		gc.DrawText(point+TPoint(iLineHeight/3,iLineHeight/6), iTitle);

	gc.SetPenColor(TRgb(KRgbBlack));
	gc.DrawLine(point+TPoint(iLineHeight/3,iLineHeight),point+TPoint(iLRect.Width()-iLineHeight*2/3,iLineHeight));
	gc.SetPenColor(KTextColor);

	TBuf<100> sText;
	TBuf<100> tempSText;
	TBuf<10> textIndex;
	TBuf<10> sTLength;
	TBuf<10> sTTLength;
	TBuf<10> sUrlIndex;
	
	TInt i_s=0;
	if(iTextData)
	{
		for(TInt i=0;i<iMaxLine;i++)
		{
			if((i+iPage*iMaxLine)>iTextData->Count()-1)
			{
				break;
			}

			sText.Zero();
			sText.Append(iTextData->At(i+iPage*iMaxLine));

			//gc.DrawText(TPoint(4, iLineHeight *i +40)+point, iTextData->At(i+iPage*iMaxLine));

			if(sText.Find(_L("|"))==KErrNotFound)
			{
				gc.DrawText(TPoint(iLineHeight/4, iLineHeight *i +iLineHeight)+point, iTextData->At(i+iPage*iMaxLine));
			}
			else
			{
				i_s++;

				TInt i_i=sText.Find(_L("|"));

				tempSText.Zero();
				textIndex.Zero();
				sTLength.Zero();
				sTTLength.Zero();
				sUrlIndex.Zero();
				
				tempSText.Append(sText.Mid(0,i_i));//显示的一行字符
				sText.Delete(0,i_i+1);
				i_i=sText.Find(_L("|"));
				textIndex.Append(sText.Mid(0,i_i));//字符串中超链接标题起始位置
				sText.Delete(0,i_i+1);
				i_i=sText.Find(_L("|"));
				sTLength.Append(sText.Mid(0,i_i));//超链接标题分割长度
				sText.Delete(0,i_i+1);
				i_i=sText.Find(_L("|"));
				sTTLength.Append(sText.Mid(0,i_i));//超链接标题总长度
				sText.Delete(0,i_i+1);
				sUrlIndex.Append(sText);//对应url索引


				//字符串中超链接标题起始位置
				TLex temp1(textIndex);
				TInt b1;
				TInt iVal1=temp1.Val( b1 );

				//超链接标题分割长度
				TLex temp2(sTLength);
				TInt b2;
				TInt iVal2=temp2.Val( b2 );

				//超链接标题总长度
				TLex temp3(sTTLength);
				TInt b3;
				TInt iVal3=temp3.Val( b3 );

				//对应url索引
				TLex temp4(sUrlIndex);
				TInt b4;
				TInt iVal4=temp4.Val( b4 );
				
				//设置被选中的索引
				/*if(iFlag==0)
				{
					iFlag=1;
					iSFlag=1;
					iUrlIndex=b4;
					iSelIndex=i+iPage*iMaxLine;
				}*/


				if(b3>b2)
				{
					//如果超链接标题不在一行里面显示
					if(b1>0)
					{
						TInt i_tw=iScreenLayout.Font()->MeasureText(tempSText.Mid(0,b1));
						gc.DrawText(TPoint(4, iLineHeight *i +iLineHeight)+point, tempSText.Mid(0,b1));
						if(iSFlag==i_s)
						{	
							/*iUrlIndex=b4;
							iSelIndex=i+iPage*iMaxLine;*/
							gc.DrawText_Red(TPoint(iLineHeight/4, iLineHeight *i +iLineHeight)+point+TPoint(i_tw,0), tempSText.Mid(b1,b2));
						}
						else
							gc.DrawText_Blue(TPoint(iLineHeight/4, iLineHeight *i +iLineHeight)+point+TPoint(i_tw,0), tempSText.Mid(b1,b2));
					}
					else
					{
						if(iSFlag==i_s)
						{	
							/*iUrlIndex=b4;
							iSelIndex=i+iPage*iMaxLine;*/
							gc.DrawText_Red(TPoint(iLineHeight/4, iLineHeight *i +iLineHeight)+point, tempSText.Mid(0,b2));
						}
						else
							gc.DrawText_Blue(TPoint(iLineHeight/4, iLineHeight *i +iLineHeight)+point, tempSText.Mid(0,b2));
					}

					sText.Zero();
					i++;
					sText.Append(iTextData->At(i+iPage*iMaxLine));
					if(iSFlag==i_s)
					{	
						/*iUrlIndex=b4;
						iSelIndex=i+iPage*iMaxLine;*/
						gc.DrawText_Red(TPoint(iLineHeight/4, iLineHeight *i +iLineHeight)+point, sText.Mid(0,b3-b2));
					}
					else
						gc.DrawText_Blue(TPoint(iLineHeight/4, iLineHeight *i +iLineHeight)+point, sText.Mid(0,b3-b2));
			
					TInt ii_tw=iScreenLayout.Font()->MeasureText(sText.Mid(0,b3-b2));
					gc.DrawText(TPoint(iLineHeight/4, iLineHeight *i +iLineHeight)+point+TPoint(ii_tw,0), sText.Mid(b3-b2,sText.Length()-(b3-b2)));
					
				}
				else
				{
					//如果超链接标题在一行里面显示
					if(b1>0)
					{
						TInt i_tw=iScreenLayout.Font()->MeasureText(tempSText.Mid(0,b1));
						gc.DrawText(TPoint(iLineHeight/4, iLineHeight *i +iLineHeight)+point, tempSText.Mid(0,b1));
						if(iSFlag==i_s)
						{	
							/*iUrlIndex=b4;
							iSelIndex=i+iPage*iMaxLine;*/
							gc.DrawText_Red(TPoint(iLineHeight/4, iLineHeight *i +iLineHeight)+point+TPoint(i_tw,0), tempSText.Mid(b1,b2));
						}
						else
							gc.DrawText_Blue(TPoint(iLineHeight/4, iLineHeight *i +iLineHeight)+point+TPoint(i_tw,0), tempSText.Mid(b1,b2));
						
						if((b1+b2)<tempSText.Length())
						{
							TInt ii_tw=iScreenLayout.Font()->MeasureText(tempSText.Mid(b1,b2));
							gc.DrawText(TPoint(iLineHeight/4, iLineHeight *i +iLineHeight)+point+TPoint(i_tw,0)+TPoint(ii_tw,0), tempSText.Mid(b1+b2,tempSText.Length()-b1-b2));
						}
					}
					else
					{
						if(iSFlag==i_s)
						{	
							/*iUrlIndex=b4;
							iSelIndex=i+iPage*iMaxLine;*/
							gc.DrawText_Red(TPoint(iLineHeight/4, iLineHeight *i +iLineHeight)+point, tempSText.Mid(0,b2));
						}
						else
							gc.DrawText_Blue(TPoint(iLineHeight/4, iLineHeight *i +iLineHeight)+point, tempSText.Mid(0,b2));

						TInt ii_tw=iScreenLayout.Font()->MeasureText(tempSText.Mid(0,b2));
						gc.DrawText(TPoint(iLineHeight/4, iLineHeight *i +iLineHeight)+point+TPoint(ii_tw,0), tempSText.Mid(b2,tempSText.Length()-b2));
					}
				}

			}	
			
		}
		//iFlag=i_s;
	}	
	gc.SetBrushStyle(CGraphicsContext::ENullBrush);
	gc.SetPenColor(KRgbWhite);
}
//////////////////////////////////////////////////////////////////////////
//From CControl
//////////////////////////////////////////////////////////////////////////
void CSystemInfoDialog::Draw(CGraphic& gc)const
{
	DrawInfo(gc);	
}

TBool CSystemInfoDialog::KeyEventL(TInt aKeyCode)
{
	TBool keyResult = ETrue;
	switch(aKeyCode)
	{
	case KKeyCodeUp:
		{
			if(iTextData)
			{
				if(iSFlag==0||iSFlag==1)
				{
					if(iPage>0)
					{
						iSFlag=0;
						iFlag=0;
						iPage--;
						//iScrollBar->PageUp();
						this->SetInitValue();
					}
				}
				else
				{
					iSFlag--;
					//iScrollBar->Up();
					this->SetInitValue();
				}
			}
			break;
		}
	case KKeyCodeDown:
		{
			if(iTextData)
			{
				if(iSFlag>=iFlag)
				{
					if(iTextData->Count()%iMaxLine==0)
					{
						if(iPage<(iTextData->Count()/iMaxLine-1))
						{
							iSFlag=0;
							iFlag=0;
							iPage++;
							//iScrollBar->PageDown();
							this->SetInitValue();
						}
					}
					else
					{
						if(iPage<iTextData->Count()/iMaxLine)
						{
							iSFlag=0;
							iFlag=0;
							iPage++;
							//iScrollBar->PageDown();
							this->SetInitValue();
						}
					}
				}
				else
				{
					iSFlag++;
					//iScrollBar->Down();
					this->SetInitValue();
				}
			}
			break;
		}
	case KKeyCodeRight:
		{
			if(iTextData)
			{
				if(iTextData->Count()%iMaxLine==0)
				{
					if(iPage<(iTextData->Count()/iMaxLine-1))
					{
						iSFlag=0;
						iFlag=0;
						iPage++;
						//iScrollBar->PageDown();
						this->SetInitValue();
					}
				}
				else
				{
					if(iPage<iTextData->Count()/iMaxLine)
					{
						iSFlag=0;
						iFlag=0;
						iPage++;
						//iScrollBar->PageDown();
						this->SetInitValue();
					}
				}
			}
			break;
		}
	case KKeyCodeLeft:
		{	
			if(iTextData)
			{
				if(iPage>0)
				{
					iSFlag=0;
					iFlag=0;
					iPage--;
					//iScrollBar->PageUp();
					this->SetInitValue();
				}
			}
		}	
		break;
	case KOKKey:
		{
			if(iUrlIndex>-1)
			{
				if(iAIndexData)
				{
					TBuf<200> uBuf;
					uBuf.Append(_L("4 "));
					uBuf.Append(iAIndexData->At(iUrlIndex));
					iMainEngine.LaunchWapBrowswerL(uBuf);			
				}
			}
		}
		break;
	case KLeftSoftKey:				
		{
			MInfoObserver& aObserver=iObserver;
			CleanupSelf();
			aObserver.InfoResponseEvent(0);
		}
		break;
	case KRightSoftKey:
		{
			MInfoObserver& aObserver=iObserver;
			CleanupSelf();
			aObserver.InfoResponseEvent(1);
		}
		break;
	default:
		keyResult = EFalse;
		break;
	}
	return ETrue;
}

TBool CSystemInfoDialog::HandleCommandL(TInt /*aCommand*/)
{
	return EFalse;
}

void CSystemInfoDialog::SizeChanged(const TRect& /*aScreenRect*/)
{

}
const TDesC& CSystemInfoDialog::LeftButton() const
{
	if (iShowType==0)
	{
		return iMainEngine.GetDesById(ETurkeyTextRes_NextStep);
	} 
	else
	{
		return _L("");
	}
}

const TDesC& CSystemInfoDialog::RightButton() const
{
	if (iShowType==0)
	{
		return iMainEngine.GetDesById(ETurkeyTextRes_Cancel);
	}
	else
	{
		return iMainEngine.GetDesById(ETurkeyTextRes_Back);
	}
}
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void CSystemInfoDialog::SetShowInfo(const TDesC& aTitle, const TDesC& aText,TInt aType)
{
	iTitle.Zero();
	iTitle.Append(aTitle);

	ParseString(aText);

	iShowType=aType;

	if (iShowType==0)
	{
		SaveInfoToFile(aText,aTitle);
	}

	iMainEngine.RequestDraw();
}

void CSystemInfoDialog::ParseString(const TDesC& aDes)
{
	TBuf<2> tempBuf;//信息的每个字符
	TBuf<100> showText;//每行字符
	TInt length;//需要跳过的字符串长度
	
	TBuf<200> aIndex;//超链接处理时的索引
	TBuf<50> tempTitle; //超链接标题临时变量
	TBuf<150> tempURl; //超链接URL临时变量
	TInt tIndex;//超链接对应开始那一行
	TInt tLength;//超链接显示换行的字符长度

	showText.Append(_L("      "));

	for(TInt i=0;i<aDes.Length();i++)
	{
		length=0;
		tempBuf.Zero();
		tempBuf.Append(aDes.Mid(i,1));
		if(tempBuf.Compare(_L("<"))==0)
		{
			if((i+6)<aDes.Length())
			{
				if(aDes.Mid(i,5).Find(_L("<br/>"))==0)
				{
					length=4;
					if(aIndex.Length()>0)
					{
						showText.Append(aIndex);
						aIndex.Zero();
					}
					iTextData->AppendL(showText);
					showText.Zero();
					showText.Append(_L("      "));
					
				}
				else if(aDes.Mid(i,5).Find(_L("<a>"))==0)
				{
					if(aDes.Mid(i,aDes.Length()-1-i).Find(_L("</a>"))!=KErrNotFound)
					{

						aIndex.Zero();
						tempTitle.Zero();
						tempURl.Zero();
						tIndex=0;
						tLength=0;

						TInt m_nT=aDes.Mid(i,aDes.Length()-1-i).Find(_L("</a>"));

						TInt m_nTT=aDes.Mid(i+3,m_nT-3).Find(_L("|"));
						
						length=m_nT+3;

						tempTitle.Append(aDes.Mid(i+3,m_nTT));
						tempURl.Append(aDes.Mid(i+3+m_nTT+1,m_nT-3-m_nTT-1));

						TInt m_nW=0;

						tIndex=showText.Length();

						for(TInt ii=0;ii<tempTitle.Length();ii++)
						{
							showText.Append(tempTitle.Mid(ii,1));
							m_nW=iScreenLayout.Font()->MeasureText(showText);

							if(m_nW>(iScreenLayout.WhiteBackgroundRect().Width()-20))
							{
								showText.Delete(showText.Length()-1,1);

								tLength=ii;

								if(ii!=0)
								{
									aIndex.Zero();
									aIndex.Append(_L("|"));
									aIndex.AppendNum(tIndex);
									aIndex.Append(_L("|"));
									aIndex.AppendNum(tLength);
									aIndex.Append(_L("|"));
									aIndex.AppendNum(m_nTT);
									aIndex.Append(_L("|"));
									aIndex.AppendNum(iAIndexData->Count());

									iAIndexData->AppendL(tempURl);

									showText.Append(aIndex);
								}
								else
									tIndex=0;

								iTextData->AppendL(showText);
								showText.Zero();
								showText.Append(tempTitle.Mid(ii,1));

								if(ii!=0)
								{
									aIndex.Zero();
									aIndex.Append(_L("a"));
								}
							}
						}

						if(aIndex.Compare(_L("a"))==0)
						{
							aIndex.Zero();
						}
						else
						{
							aIndex.Zero();
							aIndex.Append(_L("|"));
							aIndex.AppendNum(tIndex);
							aIndex.Append(_L("|"));
							aIndex.AppendNum(m_nTT);
							aIndex.Append(_L("|"));
							aIndex.AppendNum(m_nTT);
							aIndex.Append(_L("|"));
							aIndex.AppendNum(iAIndexData->Count());
							iAIndexData->AppendL(tempURl);
						}
					}
				}
				i=i+length;
			}
		}
		else
		{
			showText.Append(tempBuf);
			TInt m_nWW=iScreenLayout.Font()->MeasureText(showText);
			if(m_nWW>(iScreenLayout.WhiteBackgroundRect().Width()-20))
			{
				showText.Delete(showText.Length()-1,1);
				if(aIndex.Length()>0)
				{
					showText.Append(aIndex);
					aIndex.Zero();
				}
				iTextData->AppendL(showText);
				showText.Zero();
				showText.Append(tempBuf);
			}
			else
			{

				if(i==(aDes.Length()-1))
				{
					if(aIndex.Length()>0)
					{
						showText.Append(aIndex);
						aIndex.Zero();
					}
					iTextData->AppendL(showText);
				}
			}
		}
		
	}

	this->SetInitValue();
}

void CSystemInfoDialog::SaveInfoToFile(const TDesC& aInfo,const TDesC& aTitle)
{
	//20070529张晖增加：保存公告信息到文件
	TFileName fn;
	fn.Append(iMainEngine.GetAppPath());
	fn.Append(_L("SysInfo\\"));

	//以时间作为文件名
	TBuf<64> nFileName;
	TTime	oTime;
	oTime.HomeTime();
	oTime.FormatL(nFileName, _L("%D%M%Y%2%3%H%T%S"));

	nFileName.Append(aTitle);

	fn.Append(nFileName);

	RFs rfs;
	rfs.Connect();

	RFile oFile;
	if (KErrNone != oFile.Replace(rfs, fn, EFileWrite|EFileShareAny))
	{
		rfs.Close();
		return;
	}

	TBuf8<1024>	szContent;
	
	if(aInfo.Length ()>0)
	{
		CnvUtfConverter::ConvertFromUnicodeToUtf8(szContent,aInfo);

		oFile.Write(szContent);
	}

	oFile.Close();
	rfs.Close();
}
