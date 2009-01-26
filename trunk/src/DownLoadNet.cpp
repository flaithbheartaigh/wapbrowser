// BrowserDownLoadState.cpp: implementation of the CDownLoadNet class.
//
//////////////////////////////////////////////////////////////////////
#include <COEMAIN.H>
#include "DownLoadNet.h"
#include "HTTPEngine.h"
#include "MainEngine.h"
#include "utf.h"

#include "StaticCommonTools.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define	KMAXTIMER (5000)

CDownLoadNet::CDownLoadNet(CMainEngine& aMainEngine)
:iMainEngine(aMainEngine),
iStatus(ENone),
iBeginPos(0),
iEndPos(0),
iTaskID(-1),
iThreadID(-1),
iContentType(0),
iBeyondTimeNum(0),
iReceivedData(NULL),
iReceivedTextData(NULL),
iDownLoadServer(NULL),
iStream(NULL)
{
	iHttpBinary=CHTTPEngine::NewL(*this);

	iHttpBinary->SetupConnectionL(iMainEngine.GetIAPValue());

	ifs.Connect();
}

CDownLoadNet::~CDownLoadNet()
{
	delete iStream;

	ifs.Close() ;
	iSaveFile.Close();

	if(iReceivedData)
		delete iReceivedData;

	if(iReceivedTextData)
		delete iReceivedTextData;
}

TInt CDownLoadNet::Activate()
{
	iStatus = ENone;
	iNewGetFile = 0;

	return TRUE;
}

TInt CDownLoadNet::CancelDownLoad(TInt aType)
{	
	//aType:0-��ֹ����
	//	    2-�ļ���ʧ����ֹ���أ�3-����URL������ֹ����
	iHttpBinary->CancelTransaction();

	if(iStream)
	{
		if(iReceivedData)
		{
			iStream->WriteL (*iReceivedData);
			iStream->CommitL ();

			delete iReceivedData;
			iReceivedData=NULL;
		}
		iStream->Close ();

		iSaveFile.Close();

		delete iStream; 
		iStream=NULL;
	}

	iSaveFileName.Zero ();
	iBeginPos=0;
	iEndPos=0;

	iBeyondTimeNum=0;
	if (iDownLoadServer)
	{
		if(aType==0 || aType==1)
			iDownLoadServer->DownLoadEvent(iTaskID,iThreadID,0);
		else
			iDownLoadServer->DownLoadEvent(iTaskID,iThreadID,aType);

		iTaskID=-1;
	}

	iStatus = ENone;
	return TRUE;	
}

void CDownLoadNet::GetFile()
{
	if (NULL == iStream)
	{
		HttpBinaryGet();
	}
}


void CDownLoadNet::HttpBinaryGet()
{
	iHttpBinary->CancelTransaction();

	iOldURL.Zero();
	iOldURL.Copy(iUrl);

	TBuf8<350> url8(0);

	url8.Copy(iUrl);

	TBuf8<100> nBeginPos;
	nBeginPos.Zero ();
	nBeginPos.Append (_L8("bytes="));
	nBeginPos.AppendNum (iBeginPos);
	nBeginPos.Append (_L8("-"));

	if(iEndPos>0)
	{
		nBeginPos.AppendNum (iEndPos);
	}

	if(iHandleType==0)
	{
		//��������
		iHttpBinary->IssueHTTPGetL(url8,nBeginPos);
	}
	else if(iHandleType==1)
	{
		//��ȡ�ļ���С
		iHttpBinary->IssueHTTPHeadL (url8);
	}
}

void CDownLoadNet::HttpBinaryGet(TDesC& aURL)
{
	iHttpBinary->CancelTransaction();

	TBuf8<350> url8(0);
	url8.Copy(aURL);

	TBuf8<100> nBeginPos;
	nBeginPos.Zero ();
	nBeginPos.Append (_L8("bytes="));
	nBeginPos.AppendNum (iBeginPos);
	nBeginPos.Append (_L8("-"));

	if(iEndPos>0)
	{
		nBeginPos.AppendNum (iEndPos);
	}

	if(iHandleType==0)
	{
		iHttpBinary->IssueHTTPGetL(url8,nBeginPos);
	}
	else if(iHandleType==1)
	{
		iHttpBinary->IssueHTTPHeadL (url8);
	}
}

void CDownLoadNet::ClientEvent(const TDesC& aEventDescription,TInt aIndex)
{
	UtilityTools::WriteLogsL(_L("CDownLoadNet::ClientEvent"));
	if (aEventDescription.Compare (_L("Connecting..."))==0)
	{
		iMainEngine.SetCurrentTraffice_Rev((TInt64)iHttpBinary->GetSendSize());
	}

	switch(iHttpBinary->GetNetState())
	{
	case THTTPEvent::EGotResponseHeaders:
		{
			if(!iStream)
			{	
				//20070601���������ж��Ƿ�Ϊ��������������
				TBuf<100> nContentType;
				nContentType.Zero ();
				nContentType.Append (aEventDescription);

				//iMainEngine.WriteLog16(nContentType);
				
				#ifdef __WINS__
				if(nContentType.Find(_L("application/vnd.oma.dd"))!=KErrNotFound || iHttpBinary->GetContentLength()<2048)
				#else
				if(nContentType.Find(_L("application/vnd.oma.dd"))!=KErrNotFound )
				#endif
				{
					//DD�����ļ�
					iContentType=2;
					//iMainEngine.WriteLog16(_L("nContentType=2"));
					return;
				}
				#ifdef __WINS__
				else if(nContentType.Find(_L("text/vnd.sun.j2me.app-descriptor"))!=KErrNotFound)
				#else
				if(nContentType.Find(_L("text/vnd.sun.j2me.app-descriptor"))!=KErrNotFound )
				#endif
				{
					//JAD�����ļ�
					iContentType=3;
					//iMainEngine.WriteLog16(_L("nContentType=3"));
					return;
				}
				else if(nContentType.Find(_L("Content-Type:text"))!=KErrNotFound && nContentType.Find (_L("Content-Type:text/plain"))==KErrNotFound)
				{
					iContentType=1;
					//iMainEngine.WriteLog16(_L("nContentType=1"));
					return;
				}
				else
				{
					//iMainEngine.WriteLog16(_L("nContentType=0"));
					iContentType=0;
				}

				if(iHandleType==1)
				{
					//����ǻ�ȡ�ļ���С�򷵻ؽ��
					if (iDownLoadServer && iTaskID!=-1 && iThreadID!=-1)
					{
						TInt nTotalSize = iHttpBinary->GetContentLength();
						iDownLoadServer->DownLoadReceived(iTaskID,iThreadID,nTotalSize,0);
					}

					return;
				}

				TInt err=iSaveFile.Open(ifs,iSaveFileName,EFileWrite|EFileShareAny);
				if (err==KErrNotFound) // file does not exist - create it
				{
					iSaveFile.Close();

					CancelDownLoad(5);

					return;
				}
				iStream=new(ELeave) RFileWriteStream(iSaveFile,iBeginPos);
			}
			break;
		}
	case THTTPEvent::EGotResponseBodyData:
		{
			break;
		}
	case -888: // sign : now is the last data
		{
			//iMainEngine.WriteLog16(_L("888-1"));
			::WriteLogsTemp(_L("ClientEvent-888-1"));
			if(iContentType==1)
			{	
				//�ƶ�����ҳ���ж�
				if (iReceivedTextData)
				{
					HBufC* newUrl = HBufC::NewLC(500);
					TInt gErr=iMainEngine.ParseMobiWapPage(iOldURL,*iReceivedTextData,newUrl);

					if(gErr==1)  //��������ҳ��
					{
						iOldURL.Zero();
						iOldURL.Append(*newUrl);

						HttpBinaryGet(iOldURL);

						CleanupStack::PopAndDestroy(newUrl);

						delete iReceivedTextData;
						iReceivedTextData=NULL;

						return;
					}
					else
					{
						//������
						TFileName nSaveFile;
						nSaveFile.Zero();
						nSaveFile.Append(_L("c:\\httperr.txt"));

						WriteFile(nSaveFile,*iReceivedTextData);

						CancelDownLoad(3);
					}
					
					CleanupStack::PopAndDestroy(newUrl);
					
					delete iReceivedTextData;
					iReceivedTextData=NULL;

					return;
				}
			}
			else if (iContentType==2)
			{
				//iMainEngine.WriteLog16(_L("888-2"));
				HandleDDFile(*iReceivedTextData);

				delete iReceivedTextData;
				iReceivedTextData=NULL;

				return;
			}
			else if (iContentType==3)
			{
				//iMainEngine.WriteLog16(_L("888-3"));
				HandleJADFile(*iReceivedTextData);

				delete iReceivedTextData;
				iReceivedTextData=NULL;

				return;
			}
			
			//iMainEngine.WriteLog16(_L("888-4"));
			if(iStream)
			{	
				if (iReceivedData)
				{
					//::WriteLogsL(_L("ClientEvent-X-1")); 
					iStream->WriteL(*iReceivedData);

					iStream->CommitL();
					//::WriteLogsL(_L("ClientEvent-X-2")); 
					delete iReceivedData;
					iReceivedData=NULL;
				}

				if (iReceivedTextData)
				{
					delete iReceivedTextData;
					iReceivedTextData=NULL;
				}

				iStream->Close();

				iSaveFile.Close();

				delete iStream;
				iStream=NULL;

				iStatus=EConnectOver;				

				if (iDownLoadServer)
				{
					iDownLoadServer->DownLoadEvent(iTaskID,iThreadID,1); 
					iTaskID=-1;
					iThreadID=-1;
				}
			}
			//iMainEngine.WriteLog16(_L("888-5"));
			break;
		}
	case -999:
		{
			//iMainEngine.WriteLog16(_L("999-1"));
			//20070708�������ӳ�ʱ�Զ�����
			::WriteLogsTemp(_L("ClientEvent-999-1"));
			
			iBeyondTimeNum++;

			iHttpBinary->CancelTransaction();
			//::WriteLogsL(_L("ClientEvent-999-2")); 
			if(iBeyondTimeNum<5)
			{
				//::WriteLogsL(_L("ClientEvent-999-3")); 
				::WriteLogsTemp(_L("ClientEvent-999-2"));
				if(iStream && iReceivedData)
				{
					iStream->WriteL (*iReceivedData);
					iStream->CommitL ();
				}
				//::WriteLogsL(_L("ClientEvent-999-4")); 
				if(iReceivedData)
				{
					delete iReceivedData;
					iReceivedData=NULL;
				}

				if (iReceivedTextData)
				{
					delete iReceivedTextData;
					iReceivedTextData=NULL;
				}

				//::WriteLogsL(_L("ClientEvent-999-5")); 
				HttpBinaryGet();
				//::WriteLogsL(_L("ClientEvent-999-6")); 
				::WriteLogsTemp(_L("ClientEvent-999-3"));
			}
			else
			{
				//����5������Ϊ�Ǵ������ӣ�
				//::WriteLogsL(_L("ClientEvent-999-7")); 
				//iMainEngine.WriteLog16(_L("999-2"));
				::WriteLogsTemp(_L("ClientEvent-999-4"));
				if (iReceivedTextData)
				{
					delete iReceivedTextData;
					iReceivedTextData=NULL;
				}

				CancelDownLoad(3);
				::WriteLogsTemp(_L("ClientEvent-999-5"));
				//::WriteLogsL(_L("ClientEvent-999-8")); 
				return;
			}
			//iMainEngine.WriteLog16(_L("999-3"));
		}
	}
	UtilityTools::WriteLogsL(_L("CDownLoadNet::ClientEvent End"));
}

void CDownLoadNet::ClientBodyReceived(const TDesC8& aBodyData,TInt aIndex)
{
	UtilityTools::WriteLogsL(_L("CDownLoadNet::ClientBodyReceived"));
	iMainEngine.SetCurrentTraffice_Rev((TInt64)aBodyData.Length());


	if(iContentType>=1)
	{
		//�ı���Ϣ
		HBufC* newData = HBufC::NewLC(aBodyData.Length());
		TPtr16 ptr = newData->Des();
		CnvUtfConverter::ConvertToUnicodeFromUtf8(ptr,aBodyData);

		if (iReceivedTextData)
		{
			iReceivedTextData = iReceivedTextData->ReAllocL(iReceivedTextData->Length() + aBodyData.Length());
			iReceivedTextData->Des() += newData->Des();
		}
		else
		{
			iReceivedTextData = HBufC::NewL(aBodyData.Length());
			iReceivedTextData->Des().Copy(*newData);
		}
		
		//tmp
		/*TFileName nSaveFile;
		nSaveFile.Zero();
		nSaveFile.Append(_L("c:\\httpbody.txt"));

		WriteFile(nSaveFile,*newData);*/

		CleanupStack::PopAndDestroy(newData);
	}
	else
	{
		//��������������Ϣ
		iBeyondTimeNum=0;

		if(iStream && iTaskID!=-1)
		{
			if (iReceivedData)
			{
				iReceivedData = iReceivedData->ReAllocL(iReceivedData->Length() + aBodyData.Length());
				HBufC8* newData = HBufC8::NewLC(aBodyData.Length());
				newData->Des().Copy(aBodyData);
				iReceivedData->Des() += newData->Des();
				CleanupStack::PopAndDestroy(newData);
			}
			// no data yet received.
			else
			{
				iReceivedData = HBufC8::NewL(aBodyData.Length());
				iReceivedData->Des().Copy(aBodyData);
			}

			if(iReceivedData->Length()> 60480)
			{	
				iStream->WriteL(*iReceivedData);

				iStream->CommitL();

				delete iReceivedData;
				iReceivedData=NULL;
			}

			//20070708��������
			iBeginPos=iBeginPos+aBodyData.Length();

			if (iDownLoadServer && iTaskID!=-1 && iThreadID!=-1)
			{
				TInt nReservedSize = aBodyData.Length();
				TInt nTotalSize = iHttpBinary->GetContentLength();

				iDownLoadServer->DownLoadReceived(iTaskID,iThreadID,nTotalSize,nReservedSize);
			}
		}
	}
	UtilityTools::WriteLogsL(_L("CDownLoadNet::ClientBodyReceived End"));
}

void CDownLoadNet::GetDownLoadFileSize(TInt aTaskID,TInt aThreadID,const TDesC& aUrl)
{
	//����HTTP HeadЭ���ȡ�ļ���С
	iHandleType=1;
	iBeyondTimeNum=0;

	iTaskID=aTaskID;
	iThreadID=aThreadID;

	iUrl.Delete(0, iUrl.Length());
	iUrl.Append(aUrl);

	HttpBinaryGet();
}

void CDownLoadNet::SetUrl(TInt aTaskID,TInt aThreadID,const TDesC& aUrl, const TDesC& aName,TInt aBeginPos,TInt aEndPos)
{
	if (iNewGetFile == -1)
		iNewGetFile = 1;

	iBeyondTimeNum=0;
	iHandleType=0;

	iBeginPos=aBeginPos;
	iEndPos=aEndPos;

	iTaskID=aTaskID;
	iThreadID=aThreadID;

	iSaveFileName.Delete(0, iSaveFileName.Length());
	iSaveFileName.Append(aName);

	iUrl.Delete(0, iUrl.Length());
	iUrl.Append(aUrl);

	GetFile();
}

void CDownLoadNet::HandleDDFile(TDesC& aContent)
{
	//����DD�ļ�
	TInt i=0;
	TInt j=0;

	TBuf<50> nType;
	TBuf<255> nFileName;
	TBuf<255> nDownLoadURL;
	TBuf<255> nDownLoadResponseURL;
	TInt nFileSize;

	//��ȡ�ļ�����
	i=aContent.Find(_L("<type>"));
	
	if (i==KErrNotFound)
	{
		CancelDownLoad(3);

		return;
	} 
	
	j=aContent.Find(_L("</type>"));
	
	nType.Zero();
	nType.Append(aContent.Mid(i+6,j-i-6));

	//��ȡ�ļ�����
	i=aContent.Find(_L("<name>"));

	if (i==KErrNotFound)
	{
		CancelDownLoad(3);

		return;
	} 

	j=aContent.Find(_L("</name>"));

	nFileName.Zero();
	nFileName.Append(aContent.Mid(i+6,j-i-6)); 

	//��ȡ�ļ���С
	i=aContent.Find(_L("<size>"));

	if (i==KErrNotFound)
	{
		CancelDownLoad(3);

		return;
	} 

	j=aContent.Find(_L("</size>"));

	TLex temp(aContent.Mid(i+6,j-i-6));
	temp.Val( nFileSize);

	//��ȡ�ļ�����URL
	i=aContent.Find(_L("<objectURI>"));

	if (i==KErrNotFound)
	{
		CancelDownLoad(3);

		return;
	} 

	j=aContent.Find(_L("</objectURI>"));

	nDownLoadURL=aContent.Mid(i+11,j-i-11);

	//��ȡ�ļ����سɹ�����URL
	i=aContent.Find(_L("<installNotifyURI>"));

	if (i==KErrNotFound)
	{
		CancelDownLoad(3);

		return;
	} 

	j=aContent.Find(_L("</installNotifyURI>"));

	nDownLoadResponseURL=aContent.Mid(i+18,j-i-18);

	//iMainEngine.WriteLog16(_L("HandleDDFile"));
	//iMainEngine.WriteLog16(nDownLoadURL);
	//iMainEngine.WriteLog16(nDownLoadResponseURL);

	iDownLoadServer->DownLoadReceived(iTaskID,iThreadID,nFileSize,nFileName,nDownLoadURL,nDownLoadResponseURL);

}
void CDownLoadNet::HandleJADFile(TDesC& aContent)
{
	//����JAD�ļ�
	TInt i=0;
	TInt j=0;

	TBuf<500> nInfo;
	TBuf<255> nFileName;
	TBuf<255> nDownLoadURL;
	TBuf<255> nDownLoadResponseURL;
	TInt nFileSize;

	nInfo.Zero();
	nInfo.Append(aContent);

	//��ȡ�ļ�����
	i=nInfo.Find(_L("MIDlet-Name:"));

	j=nInfo.Find(_L("\n"));

	if (i==KErrNotFound || j==KErrNotFound)
	{
		CancelDownLoad(3);

		return;
	} 

	nFileName.Zero();
	nFileName.Append(nInfo.Mid(i+12,j-i-12)); 

	nInfo.Delete(0,j+1);

	//��ȡ�ļ���С
	i=nInfo.Find(_L("MIDlet-Jar-Size:"));

	j=nInfo.Find(_L("\n"));

	if (i==KErrNotFound || j==KErrNotFound)
	{
		CancelDownLoad(3);

		return;
	} 

	TLex temp(nInfo.Mid(i+16,j-i-16));
	temp.Val( nFileSize);

	nInfo.Delete(0,j+1);

	//��ȡ�ļ�����URL
	i=nInfo.Find(_L("MIDlet-Jar-URL:"));

	j=nInfo.Find(_L("\n"));

	if (i==KErrNotFound || j==KErrNotFound)
	{
		CancelDownLoad(3);

		return;
	} 

	nDownLoadURL=nInfo.Mid(i+15,j-i-15);

	nInfo.Delete(0,j+1);

	//��ȡ�ļ����سɹ�����URL
	i=nInfo.Find(_L("MIDlet-Install-Notify:"));

	if (i==KErrNotFound)
	{
		CancelDownLoad(3);

		return;
	} 

	j=nInfo.Find(_L("\n"));

	if (j!=KErrNotFound)
	{
		nDownLoadResponseURL=nInfo.Mid(i+22,j-i-22);
	} 
	else
	{
		nDownLoadResponseURL=nInfo.Mid(i+22);
	}
	
	iDownLoadServer->DownLoadReceived(iTaskID,iThreadID,nFileSize,nFileName,nDownLoadURL,nDownLoadResponseURL);
}

void CDownLoadNet::SetHttpIndex(TInt aIndex)
{
	if (iHttpBinary)
	{
		iHttpBinary->SetThreadIndex(aIndex);
	}
}