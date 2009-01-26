/*
============================================================================
 Name        : MainControlEngine.cpp
 Author      : 卞涛
 Version     : 2.10
 Copyright   : Soarsky
 Description : 分类引擎控制：对分类引擎进行实例化和调用，所有分类引擎统一在主控引擎中实例化和释放；
			   模块间数据传递：当两个独立模块之间传递数据时，通过主控引擎进行承载传递；
			   调试日志记录：方便开发中调试，包括断言重写；
			   主控引擎还包括公用变量声明和枚举声明；
			   主控引擎还包括一些公用方法函数，例如判断是否为数字等；
============================================================================
*/

#include "MainControlEngine.h"
#include <aknutils.h>
#include <bautils.h>

#ifndef __SERIES60_3X__
	#include <plpvariant.h>
#endif

#include "ImageResourceEngine.h"
#include "TextResourceEngine.h"
#include "GraphicsEngine.h"
#include "DBEngine.h"
#include "UtilityTools.h"
#include "CoCoPreDefine.h"
#include "PreDefine.h"

#include "FileActiveEngine.h"
#include <COMMDB.H> 		//Communications database 
#include <CDBPREFTABLE.H>	//Connection Preferences table
#include <APGTASK.H>
#include <APGCLI.H>
#include "StaticCommonTools.h"
#include "ScreenLayout.h"


_LIT(KDOWNLOADTHREADSET,"DownLoadThreadSet");
_LIT(KDOWNLOADHISTORYSET,"DownLoadHistorySet");
_LIT(KLOGINTYPE,"LoginType");
_LIT(KCONTENTINFOMAXNUM,"ContentInfoMaxNum");
_LIT(KCONTENTLISTMAXNUM,"ContentListMaxNum");

_LIT(KDOWNLOADDIR,"DownLoadDir");
_LIT(KIAPNUM,"IapNum");

_LIT(KTRAFFICINFO,"TrafficInfo");
_LIT(KDOWNLOADVOICE,"DownLoadVoice");

CMainControlEngine::CMainControlEngine()
:iTextResEngine(NULL)
,iImageResEngine(NULL)
,iGraphicsEngine(NULL)
,iConState(ECMWAP)
,iIapValue(-1)
{
	// No implementation required

}


CMainControlEngine::~CMainControlEngine()
{
	iFile.Close();
	iRfs.Close();
	DELETE(iTextResEngine);
	DELETE(iImageResEngine);
	DELETE(iGraphicsEngine);
	DELETE(iFileActiverEngine);
	delete iScreenLayout;

	if(iDBEngine)
	{
		iDBEngine->Close ();
		delete iDBEngine;
		iDBEngine = NULL;
	}
}

CMainControlEngine* CMainControlEngine::NewLC()
{
	CMainControlEngine* self = new (ELeave)CMainControlEngine();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CMainControlEngine* CMainControlEngine::NewL()
{
	CMainControlEngine* self=CMainControlEngine::NewLC();
	CleanupStack::Pop(); // self;
	return self;
}

void CMainControlEngine::ConstructL()
{
	//实例化rfs和rfile
	iRfs.Connect();
	if (KErrNone != iFile.Replace(iRfs, _L("c:\\log.txt"), EFileWrite|EFileShareAny))
	{
		iRfs.Close();
	}

	#ifndef __SERIES60_3X__
		WriteLog16(_L("setimei_start"));
		this->SetIMEI(_L(""));
		WriteLog16(_L("setimei_end"));
	#endif
	//设置程序安装路径
	SetAppDrive();
	iAppPath.Zero();
	iAppPath.Append(iDrive);
	iAppPath.Append(KAPPPATH);
	//User::InfoPrint(iAppPath);

	this->InitTextResourceEngine();
	this->InitImageResourceEngine();
	this->InitFileActiveEngine();

	InitDBEngine();

	GetSystemParm();

	GetDownLoadDirFromFile();

	GetIapNumFromFile();

	GetNetTrafficInfo();

	CreateAllDownLoadDir(iDownLoadDir);

	CreateAllSoftDir(iAppPath);
}

void CMainControlEngine::SetIMEI(const TDesC& aIMEI)
{
	UtilityTools::WriteLogsL(_L("CMainControlEngine::SetIMEI"));

	#ifdef __WINS__
		iIMEI.Zero();
		iIMEI.Append(_L("358361002809113"));
	#else
		#ifdef __SERIES60_3X__
			iIMEI.Zero();
			iIMEI.Append(aIMEI);
		#else
			WriteLog16(_L("SetImei"));
			TPlpVariantMachineId machineId;
			PlpVariant::GetMachineIdL(machineId);
			iIMEI.Copy(machineId);
			WriteLog16(iIMEI);
		#endif
	#endif
	UtilityTools::WriteLogsL(_L("IMEI:"));
	UtilityTools::WriteLogsL(iIMEI);
}
//////////////////////////////////////////////////////////////////////////
const CArrayFixFlat<TTEXTRES>& CMainControlEngine::GetTextRes() const
{
	CArrayFixFlat<TTEXTRES>* textArray = iTextResEngine->GetTextRes();
	ASSERT(textArray);
	return *textArray;
}
//////////////////////////////////////////////////////////////////////////
void CMainControlEngine::SetAppDrive()
{
#ifdef __WINS__
	iDrive.Append(_L("C:"));
#else
	TFileName fileName;
	::CompleteWithAppPath(fileName);
	TParse parse;
	parse.Set(fileName,NULL,NULL);
	iDrive.Zero();
	iDrive.Append(parse.Drive());
#endif
}
void CMainControlEngine::SetResPath()
{
	TFileName fileName;
	::CompleteWithAppPath(fileName);
	TParse parse;
	parse.Set(fileName,NULL,NULL);
	iResPath.Zero();
	iResPath.Append(parse.Drive());
	iResPath.Append(KRESOURCEPATH);
}
void CMainControlEngine::InitTextResourceEngine()
{
	iTextResEngine=CTextResourceEngine::NewL(*this);
}
void CMainControlEngine::InitImageResourceEngine()
{
	//iImageResEngine=CImageResourceEngine::NewL(*this);
}
void CMainControlEngine::InitGraphicsEngine()
{
	iGraphicsEngine=CGraphicsEngine::NewL(*this);
}

void CMainControlEngine::InitScreenLayout()
{
	//iScreenLayout = CScreenLayout::NewL();
}

void CMainControlEngine::WriteLog8(TRefByValue<const TDesC8> aFmt, ...)
{
#ifdef __WRITE_LOG__
	//UtilityTools::WriteLogsL(_L("WriteLog8"));
	TBuf8<400> tBuf;
	VA_LIST	list;
	VA_START(list,aFmt);
	tBuf.Zero();
	tBuf.AppendFormatList(aFmt,list);
	VA_END(list);

	TBuf<64>	time16;
	TBuf8<64>	time8;
	TTime	tTime;
	tTime.HomeTime();
	tTime.FormatL(time16, _L("%D%M%Y%2/%3 %H:%T:%S "));
	time8.Copy(time16);

	iFile.Write(time8);
	iFile.Write(tBuf);
	iFile.Write(_L8("\x0a\x0d"));
	//UtilityTools::WriteLogsL(_L("WriteLog8 End"));
#endif
}

void CMainControlEngine::WriteLog16(TRefByValue<const TDesC> aFmt, ...)
{
#ifdef __WRITE_LOG__
	//UtilityTools::WriteLogsL(_L("WriteLog16"));
	TBuf<400> tBuf;
	VA_LIST	list;
	VA_START(list,aFmt);
	tBuf.Zero();
	tBuf.AppendFormatList(aFmt,list);
	VA_END(list);

	HBufC8* buf=CnvUtfConverter::ConvertFromUnicodeToUtf8L(tBuf);
	ASSERT(buf);
	CleanupStack::PushL(buf);

	TBuf<64>	time16;
	TBuf8<64>	time8;
	TTime	tTime;
	tTime.HomeTime();
	tTime.FormatL(time16, _L("%D%M%Y%2/%3 %H:%T:%S "));
	time8.Copy(time16);

	iFile.Write(time8);
	iFile.Write(*buf);
	iFile.Write(_L8("\x0a\x0d"));
	CleanupStack::PopAndDestroy(1);
	//UtilityTools::WriteLogsL(_L("WriteLog16 End"));
#endif
}

//从文件中获取保存路径
void CMainControlEngine::GetDownLoadDirFromFile()
{
	//首先从数据库中获取
	TBuf<255> nParmValue;

	iDBEngine->GetSystemParmWithSQL(KDOWNLOADDIR,nParmValue); 

	if(nParmValue.Length ()==0)
	{
		_LIT(KLogFileName,"3GestDownLoadDir.3G");

		TFileName fn;
		fn.Append(GetAppPath());
		fn.Append(KLogFileName);

		RFs rfs;
		rfs.Connect();

		RFile oFile;
		if (KErrNone != oFile.Open(rfs, fn, EFileWrite|EFileShareAny))
		{
			iDownLoadDir.Zero ();
			iDownLoadDir.Append (GetAppDrive());
			iDownLoadDir.Append (_L("\\Data\\CoCo\\"));
		}
		else
		{
			TBuf<2048>	szContent;
			TBuf8<4096>	szContent1;

			szContent.Zero ();
			szContent1.Zero ();

			oFile.Read(szContent1);
			
			CnvUtfConverter::ConvertToUnicodeFromUtf8(szContent,szContent1);

			if(szContent.Length ()>0)
			{
				iDownLoadDir=szContent;
			}
			else
			{
				iDownLoadDir.Zero ();
				iDownLoadDir.Append (GetAppDrive());
				iDownLoadDir.Append (_L("\\Data\\CoCo\\"));
			}
		}
	
		oFile.Close();
		rfs.Close();

		//保存到数据库中
		iDBEngine->SetSystemParmWithSQL(KDOWNLOADDIR,iDownLoadDir);
	}
	else
	{
		iDownLoadDir.Zero ();
		iDownLoadDir.Append(nParmValue);
	}
}

//=====================================================================================
//==========20070910张晖增加===========================================================
void CMainControlEngine::InitDBEngine()
{
	//初始化数据库
	_LIT(KDBNAME,"soarsky.db");

	TFileName fn;
	fn.Copy (GetAppPath());
	fn.Append (KDBNAME);

	iDBEngine = CDBEngine::NewL();

	TInt nRet;
	nRet=iDBEngine->OpenDb (fn);

	if(nRet==KErrNotFound)
	{
		nRet=iDBEngine->CreateDb(fn);
	}

	if(nRet!=KErrNone)
		return ;
}

CDBEngine& CMainControlEngine::DBEngine()
{
	ASSERT(iDBEngine);
	return *iDBEngine;
}

void CMainControlEngine::InitFileActiveEngine()
{
	iFileActiverEngine=new ( ELeave )CFileActiveEngine;
	iFileActiverEngine->ConstructL();
}

void CMainControlEngine::GetIapNumFromFile()
{
	
	TBuf<255> nParmValue;

	iDBEngine->GetSystemParmWithSQL(KIAPNUM,nParmValue); 

	if(nParmValue.Length ()==0)
	{
		// open the IAP communications database 
		CCommsDatabase* commDB = CCommsDatabase::NewL(EDatabaseTypeIAP);
		CleanupStack::PushL(commDB);

		// initialize a view 
		CCommsDbConnectionPrefTableView* commDBView = commDB->OpenConnectionPrefTableInRankOrderLC(ECommDbConnectionDirectionUnknown);
		//CCommsDbConnectionPrefTableView* commDBView = commDB->OpenConnectionPrefTableInRankOrderLC(ECommDbConnectionDirectionOutgoing);

		// go to the first record 
		TInt err=commDBView->GotoFirstRecord();

		if(err==KErrNotFound)
		{
			iIapValue=-1;
		}
		else
		{
			// Declare a prefTableView Object.
			CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref pref;
			// read the connection preferences 
			commDBView->ReadConnectionPreferenceL(pref);
			TUint32 iap= pref.iBearer.iIapId; 
			iIapValue=((TInt)iap);

			//保存到数据库中
			SetIapNum(iIapValue);
		}
		// pop and destroy the IAP View 
		CleanupStack::PopAndDestroy(commDBView);

		// pop and destroy the database object
		CleanupStack::PopAndDestroy(commDB);
	}
	else
	{
		TLex temp(nParmValue);
		temp.Val( iIapValue);
	}
}

void CMainControlEngine::GetVersionInfo()
{
/*
	TBuf<20> VersionInfo;
	TInt i=0,j=0;

	VersionInfo.Zero();
	VersionInfo.Append (GetRes_Help()->At(0));

	i=VersionInfo.Find (_L("R"));

	//获取版本信息中的V值
	m_nVersion.Zero ();
	m_nVersion.Append (VersionInfo.Mid (1,i-1));

	//获取版本信息中的R值
	j=VersionInfo.Find (_L("A"));
	m_nRVersion.Zero ();
	m_nRVersion.Append (VersionInfo.Mid (i+1,j-i-1));

	//获取版本信息中的A值
	i=VersionInfo.Find (_L("P"));
	m_nAVersion.Zero ();
	m_nAVersion.Append (VersionInfo.Mid (j+1,i-j-1));

	//获取版本信息中的P值
	m_nPVersion.Zero ();
	m_nPVersion.Append (VersionInfo.Mid (i+1));
*/

}

const TUid KPhoneUidWmlBrowser = { 0x10008D39 };

void CMainControlEngine::LaunchWapBrowswerL(const TDesC& aAddr )
{
RApaLsSession iApaLsSession;
TUid id( KPhoneUidWmlBrowser );
TApaTaskList taskList( CEikonEnv::Static()->WsSession() );
TApaTask task = taskList.FindApp( id );

if ( task.Exists() )
	{
	HBufC8* param8 = HBufC8::NewLC( aAddr.Length() );
	param8->Des().Append( aAddr );
	task.SendMessage( TUid::Uid( 0 ), *param8 ); // UID is not used
	CleanupStack::PopAndDestroy( param8 );
	}
else
	{
	if ( !iApaLsSession.Handle() )
		{
		User::LeaveIfError( iApaLsSession.Connect() );
		}

	TThreadId thread;
	User::LeaveIfError( iApaLsSession.StartDocument( aAddr, KPhoneUidWmlBrowser, thread ) );
	}
}

void CMainControlEngine::GetSystemParm()
{
	//多线程设置
	TBuf<255> nParmValue;

	iDBEngine->GetSystemParmWithSQL(KDOWNLOADTHREADSET,nParmValue); 

	if(nParmValue.Length ()==0)
	{
		iDownLoadMaxThreadNum=3;
	}
	else if(nParmValue.Compare(_L("1"))==0)
	{
		iDownLoadMaxThreadNum=1;
	}
	else if(nParmValue.Compare(_L("3"))==0)
	{
		iDownLoadMaxThreadNum=3;
	}
	else 
	{
		iDownLoadMaxThreadNum=5;
	}

	//下载历史记录设置
	iDBEngine->GetSystemParmWithSQL(KDOWNLOADHISTORYSET,nParmValue); 

	if(nParmValue.Length ()==0)
	{
		iDownLoadMaxHistoryNum=100;
	}
	else if(nParmValue.Compare(_L("50"))==0)
	{
		iDownLoadMaxHistoryNum=50;
	}
	else if(nParmValue.Compare(_L("100"))==0)
	{
		iDownLoadMaxHistoryNum=100;
	}
	else 
	{
		iDownLoadMaxHistoryNum=200;
	}

	//登录方式设置
	iLoginType=GetSavedLoginType();

	//信息详情缓存设置
	iDBEngine->GetSystemParmWithSQL(KCONTENTINFOMAXNUM,nParmValue); 

	if(nParmValue.Length ()==0)
	{
		iContentInfoMaxNum=50;
	}
	else if(nParmValue.Compare(_L("20"))==0)
	{
		iContentInfoMaxNum=20;
	}
	else if(nParmValue.Compare(_L("50"))==0)
	{
		iContentInfoMaxNum=50;
	}
	else if(nParmValue.Compare(_L("100"))==0)
	{
		iContentInfoMaxNum=100;
	}
	else 
	{
		iContentInfoMaxNum=50;
	}

	//内容列表缓存设置
	iDBEngine->GetSystemParmWithSQL(KCONTENTLISTMAXNUM,nParmValue); 

	if(nParmValue.Length ()==0)
	{
		iContentListMaxNum=50;
	}
	else if(nParmValue.Compare(_L("20"))==0)
	{
		iContentListMaxNum=20;
	}
	else if(nParmValue.Compare(_L("50"))==0)
	{
		iContentListMaxNum=50;
	}
	else if(nParmValue.Compare(_L("100"))==0)
	{
		iContentListMaxNum=100;
	}
	else 
	{
		iContentListMaxNum=50;
	}

	//下载提示音设置
	iDBEngine->GetSystemParmWithSQL(KDOWNLOADVOICE,nParmValue); 

	if(nParmValue.Length ()==0)
	{
		//默认情况为有下载提示音
		iDownLoadVoice=true;
	}
	else if(nParmValue.Compare(_L("0"))==0)
	{
		iDownLoadVoice=false;
	}
	else 
	{
		iDownLoadVoice=true;
	}
}

//====================参数设置=============================
TInt CMainControlEngine::SetDownLoadMaxThreadNum(TInt aValue)
{
	TBuf<10> nInfo;

	nInfo.Zero();
	nInfo.AppendNum(aValue);
	
	TInt nRet=iDBEngine->SetSystemParmWithSQL(KDOWNLOADTHREADSET,nInfo);

	if (nRet==KErrNone)
	{
		iDownLoadMaxThreadNum=aValue;
	}

	return nRet;
}

TInt CMainControlEngine::SetDownLoadMaxHistoryNum(TInt aValue)
{
	TBuf<10> nInfo;

	nInfo.Zero();
	nInfo.AppendNum(aValue);

	TInt nRet=iDBEngine->SetSystemParmWithSQL(KDOWNLOADHISTORYSET,nInfo);

	if (nRet==KErrNone)
	{
		iDownLoadMaxHistoryNum=aValue;
	}

	return nRet;
}

TInt CMainControlEngine::SetLoginType(TInt aLoginType)
{
	//登录方式只有在下次登录时才发生作用
	TBuf<10> nInfo;

	nInfo.Zero();
	nInfo.AppendNum(aLoginType);

	return iDBEngine->SetSystemParmWithSQL(KLOGINTYPE,nInfo);
}

TInt CMainControlEngine::SetContentInfoMaxNum(TInt aValue)
{
	TBuf<10> nInfo;

	nInfo.Zero();
	nInfo.AppendNum(aValue);

	TInt nRet=iDBEngine->SetSystemParmWithSQL(KCONTENTINFOMAXNUM,nInfo);

	if (nRet==KErrNone)
	{
		iContentInfoMaxNum=aValue;
	}
	
	return nRet;
}

TInt CMainControlEngine::SetContentListMaxNum(TInt aValue)
{
	TBuf<10> nInfo;

	nInfo.Zero();
	nInfo.AppendNum(aValue);

	TInt nRet=iDBEngine->SetSystemParmWithSQL(KCONTENTLISTMAXNUM,nInfo);

	if (nRet==KErrNone)
	{
		iContentListMaxNum=aValue;
	}
	
	return nRet;
}

TInt CMainControlEngine::SetDownLoadDir(TDesC& aDownLoadDir)
{
	if (aDownLoadDir.Length()==0)
	{
		return KErrGeneral;
	} 
	else
	{
		TInt nRet=iDBEngine->SetSystemParmWithSQL(KDOWNLOADDIR,aDownLoadDir);

		if (nRet==KErrNone)
		{
			iDownLoadDir.Zero();
			iDownLoadDir.Append(aDownLoadDir);
		} 
		
		return nRet;
	}
}

TInt CMainControlEngine::SetIapNum(TInt aIapNum)
{
	//联网方式只有下次启动才发生作用
	TBuf<10> nInfo;

	nInfo.Zero();
	nInfo.AppendNum(aIapNum);

	return iDBEngine->SetSystemParmWithSQL(KIAPNUM,nInfo);
}

TInt CMainControlEngine::SetDownLoadVoice(TBool aValue)
{
	TBuf<10> nInfo;

	nInfo.Zero();

	if (aValue)
	{
		nInfo.Append(_L("1"));
	} 
	else
	{
		nInfo.Append(_L("0"));
	}
	
	TInt nRet=iDBEngine->SetSystemParmWithSQL(KDOWNLOADVOICE,nInfo);

	if (nRet==KErrNone)
	{
		iDownLoadVoice=aValue;
	}

	return nRet;
}

void CMainControlEngine::GetNetTrafficInfo()
{
	iMonthTraffic_Rev=0;
	iMonthTraffic_Send=0;

	iCurrentTraffic_Rev=0;
	iCurrentTraffic_Send=0;

	TBuf<64>	szTime;
	TTime	oTime;
	oTime.HomeTime();
	oTime.FormatL(szTime, _L("%D%M%Y%2%3%H%T%S"));

	iCurrentMonth.Copy (szTime.Mid(0,2));

	//获取流量信息
	TBuf<255> nParmValue;

	iDBEngine->GetSystemParmWithSQL(KTRAFFICINFO,nParmValue); 

	if(nParmValue.Length ()>0)
	{
		TBuf<100> nInfo;
		TInt i=0;

		i=nParmValue.Find (_L("*#"));

		if(i>0)
		{
			nInfo.Zero ();
			nInfo=nParmValue.Mid(0,i);

			nParmValue.Delete (0,i+2);

			if(UtilityTools::CheckIsNumber(nInfo)==1)
			{
				if(iCurrentMonth.Compare(nInfo)!=0)
					return;
				else
				{
					i=nParmValue.Find (_L("*#"));

					if(i>0)
					{
						nInfo.Zero ();
						nInfo=nParmValue.Mid(0,i);
						nParmValue.Delete (0,i+2);

						if(UtilityTools::CheckIsNumber(nInfo)==1)
						{
							TLex temp(nInfo);
							temp.Val( iMonthTraffic_Rev);

							if(UtilityTools::CheckIsNumber(nParmValue)==1)
							{
								TLex temp(nParmValue);
								temp.Val( iMonthTraffic_Send);
							}
						}
					}
				}
			}
		}
	}
}

TInt CMainControlEngine::SaveNetTrafficInfo()
{
	TBuf<100>	nContent;

	nContent.Zero ();

	nContent.Append (iCurrentMonth);
	nContent.Append (_L("*#"));

	nContent.AppendNum(iMonthTraffic_Rev);
	nContent.Append (_L("*#"));

	nContent.AppendNum(iMonthTraffic_Send);

	return iDBEngine->SetSystemParmWithSQL(KTRAFFICINFO,nContent);	
}

void CMainControlEngine::SetCurrentTraffice_Rev(TInt64 aValue)
{
	iCurrentTraffic_Rev=iCurrentTraffic_Rev+aValue;

	iMonthTraffic_Rev=iMonthTraffic_Rev+aValue;
}

void CMainControlEngine::SetCurrentTraffice_Send(TInt64 aValue)
{
	iCurrentTraffic_Send=iCurrentTraffic_Send+aValue;

	iMonthTraffic_Send=iMonthTraffic_Send+aValue;
}

void CMainControlEngine::CreateAllDownLoadDir(const TDesC& aPath)
{
	//创建所有的下载目录
	TFileName downloadDir;
	downloadDir.Append (GetAppDrive());
	downloadDir.Append (_L("\\Data\\"));
	RFs& fs = CCoeEnv::Static()->FsSession();
	if(!BaflUtils::PathExists(fs,downloadDir))
	{
		TInt err = fs.MkDir(downloadDir);
		User::LeaveIfError(err);
	}
	downloadDir.Append(_L("CoCo\\"));
	if(!BaflUtils::PathExists(fs,downloadDir))
	{
		TInt err = fs.MkDir(downloadDir);
		User::LeaveIfError(err);
	}
	CreateDir(aPath, KIMAGES1);
	CreateDir(aPath, KMUSIC1);
	CreateDir(aPath, KAPPS1);
	CreateDir(aPath, KVIDEO1);
	CreateDir(aPath, KBOOKS1);
	CreateDir(aPath, KTOPS1);
	CreateDir(aPath, KOTHER1);
}

void CMainControlEngine::CreateAllSoftDir(const TDesC& aPath)
{
	//创建除下载外的其他目录
	CreateDir(aPath, KSYSINFO);
}

void CMainControlEngine::CreateDir(const TDesC& aPath, const TDesC& aDirName)
{
	if (NULL == aPath.Length()) 
		return;

	TFileName fileName;
	fileName.Append(aPath);

	if(fileName.Right(1).Compare(_L("\\"))!=0)
		fileName.Append(_L("\\"));

	fileName.Append(aDirName);

	if(fileName.Right(1).Compare(_L("\\"))!=0)
		fileName.Append(_L("\\"));

	::MakeDir(fileName);
	/*RFs		oRFs;
	User::LeaveIfError(oRFs.Connect());	

	CDir*	pDir;
	oRFs.GetDir(aPath, KEntryAttNormal|KEntryAttDir, ESortByName, pDir);
	for (TInt n = 0; pDir && (n < pDir->Count()); n ++)
	{
		TEntry	oFile = (*pDir)[n];
		if (oFile.IsDir())
		{
			TFileName oName = oFile.iName;
			oName.LowerCase();
			TInt n = oName.Compare(aDirName);
			if (!n) 
			{
				DELETE(pDir);
				oRFs.Close();
				return;		
			}
		}		
	}

	TFileName nDirPathAndName;
	nDirPathAndName.Append(aPath);
	nDirPathAndName.Append(aDirName);
	nDirPathAndName.Append(_L("\\"));
	oRFs.MkDirAll(nDirPathAndName);

	DELETE(pDir);

	oRFs.Close();*/
}

TInt CMainControlEngine::GetSavedLoginType()
{
	TBuf<255> nParmValue;
	TInt nRet;

	iDBEngine->GetSystemParmWithSQL(KLOGINTYPE,nParmValue); 

	if(nParmValue.Length ()==0)
	{
		nRet=1;
	}
	else if(nParmValue.Compare(_L("0"))==0)
	{
		nRet=0;
	}
	else 
	{
		nRet=1;
	}

	return nRet;
}

TInt CMainControlEngine::GetSavedIapNum()
{
	TBuf<255> nParmValue;
	TInt nRet;

	iDBEngine->GetSystemParmWithSQL(KIAPNUM,nParmValue); 

	TLex temp(nParmValue);
	temp.Val( nRet);

	return nRet;
}