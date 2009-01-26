/*
============================================================================
 Name        : MainControlEngine.h
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

#ifndef MAINCONTROLENGINE_H
#define MAINCONTROLENGINE_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <f32file.h>
//预定义
//#include <CoCoPreDefine.h>
#include "PreDefine.h"
#include "UtilityTools.h"

class CTextResourceEngine;  //文字资源引擎
class CImageResourceEngine; //图片资源引擎
class CGraphicsEngine;
class CScreenLayout;
class CDBEngine;

class CFileActiveEngine;

/**
*  CMainControlEngine
* 
*/
class CMainControlEngine : public CBase
{
public: // Constructors and destructor
	virtual ~CMainControlEngine();
	static CMainControlEngine* NewL();
	static CMainControlEngine* NewLC();
protected:
	CMainControlEngine();
	void ConstructL();

public: //数据设置和获取
	//设置手机IMEI码
	void SetIMEI(const TDesC& aIMEI);
	//获取手机IMEI码
	inline const TDesC& GetIMEI()
	{

		#ifdef __WINS__
			iIMEI.Zero();
			iIMEI.Append(_L("358361002809113"));
		#endif

		return iIMEI;
	}

	//设置联网状态
	inline void SetLoginTypeL(TInt aType)
	{
		iLoginType=aType;
	}
	//获取联网状态是否需要代理
	inline TInt GetConState() const
	{
		return (TInt)iConState;
	}
	//设置联网状态是否需要代理
	inline void SetConState(TConState aConState)
	{
		iConState = aConState;
	}

	//获取程序安装路径
	inline const TDesC& GetAppPath() const
	{
		return iAppPath;
	}

	//获取程序安装盘符
	inline const TDesC& GetAppDrive() const
	{
		return iDrive;
	}
	//获取资源路径
	inline const TDesC& GetResPath() const
	{
		return iResPath;
	}
	
	//获取文件下载目录
	inline const TDesC& GetDownLoadDir()
	{
		return iDownLoadDir;
	}

	//下载最大线程数
	inline TInt GetDownLoadMaxThreadNum()
	{
		return iDownLoadMaxThreadNum;
	}

	//下载历史记录数
	inline TInt GetDownLoadMaxHistoryNum()
	{
		return iDownLoadMaxHistoryNum;
	}

	//获取登录方式
	inline TInt GetLoginType() const
	{
		return iLoginType;
	}

	//获取信息缓存数
	inline TInt GetContentInfoMaxNum() const
	{
		return iContentInfoMaxNum;
	}

	//获取信息列表数
	inline TInt GetContentListMaxNum() const
	{
		return iContentListMaxNum;
	}

	inline TInt GetIAPValue() const
	{
		return iIapValue;
	}

	//获取当月发送流量
	inline TInt64 GetMonthTraffic_Send() const
	{
		return iMonthTraffic_Send;
	}

	//获取当月接收流量
	inline TInt64 GetMonthTraffic_Rev() const
	{
		return iMonthTraffic_Rev;
	}

	//获取本次发送流量
	inline TInt64 GetCurrentTraffic_Rev() const
	{
		return iCurrentTraffic_Rev;
	}

	//获取本次接收流量
	inline TInt64 GetCurrentTraffic_Send() const
	{
		return iCurrentTraffic_Send;
	}

	//获取是否有下载提示音
	inline TBool HaveDownLoadVoice() const
	{
		return iDownLoadVoice;
	}

	TInt SetDownLoadMaxThreadNum(TInt aValue);
	TInt SetDownLoadMaxHistoryNum(TInt aValue);
	TInt SetContentInfoMaxNum(TInt aValue);
	TInt SetContentListMaxNum(TInt aValue);
	TInt SetLoginType(TInt aLoginType);
	TInt SetDownLoadDir(TDesC& aDownLoadDir);
	TInt SetIapNum(TInt aIapNum);
	TInt SetDownLoadVoice(TBool aValue);

	TInt GetSavedLoginType();
	TInt GetSavedIapNum();

	void SetCurrentTraffice_Rev(TInt64 aValue);
	void SetCurrentTraffice_Send(TInt64 aValue);

	void WriteLog8(TRefByValue<const TDesC8> aFmt, ...);
	void WriteLog16(TRefByValue<const TDesC> aFmt, ...);

	void LaunchWapBrowswerL(const TDesC& aAddr );

	TInt SaveNetTrafficInfo();

	void CreateAllDownLoadDir(const TDesC& aPath);
public: //其它引擎获取

	//获取文字资源引擎
	inline const CTextResourceEngine* GetTextResEngine() const
	{
		return iTextResEngine;
	}
	//获取图片资源引擎
	inline const CImageResourceEngine* GetImageResEngine() const
	{
		return iImageResEngine;
	}
	//获取图片资源引擎
	inline const CGraphicsEngine* GetGraphicsEngine() const
	{
		return iGraphicsEngine;
	}
	const CScreenLayout*	GetScreenLayout() const
	{
		ASSERT(iScreenLayout);
		return iScreenLayout;
	}

	
	inline CFileActiveEngine*	GetFileActiveEngine() 
	{
		return iFileActiverEngine;
	}

	void	GetVersionInfo();

public:
	const CArrayFixFlat<TTEXTRES>&	GetTextRes() const;

public:
	CDBEngine&	DBEngine();

private:
	CDBEngine*			iDBEngine;

private:
	//设置程序安装盘符
	void SetAppDrive();
	//设置资源所在位置
	void SetResPath();

	//初始化

	void InitTextResourceEngine();
	void InitImageResourceEngine();
	void InitGraphicsEngine();

	void InitScreenLayout();

	void InitFileActiveEngine();

	void InitDBEngine();
	void GetSystemParm();

	void GetDownLoadDirFromFile();
	void GetIapNumFromFile();
	void GetNetTrafficInfo();

	void CreateAllSoftDir(const TDesC& aPath);

	void CreateDir(const TDesC& aPath, const TDesC& aDirName);
protected: //数据成员变量
	
	TBuf<20> iIMEI; //手机IMEI码
	TFileName iDrive; //程序安装盘符

	TFileName	iAppPath; //程序安装路径

	TFileName iResPath;//资源路径

	TConState	iConState;//选择cmnet cmwap代理

	TBuf<255> iDownLoadDir;

	TInt iLoginType;//登录方式:0-离线;1-在线
	TInt iDownLoadMaxThreadNum;
	TInt iDownLoadMaxHistoryNum;
	TInt iContentInfoMaxNum;
	TInt iContentListMaxNum;
	TInt iIapValue;

	TBool iDownLoadVoice;

	//流量统计
	TBuf<20> iCurrentMonth;				//当前月份

	TInt64 iMonthTraffic_Rev;				//本月接收流量
	TInt64 iMonthTraffic_Send;			//本月发送流量

	TInt64 iCurrentTraffic_Rev;			//本次接收流量
	TInt64 iCurrentTraffic_Send;			//本次发送流量

	void SetNetTrafficInfo();
	//写debug_log成员变量
	RFile iFile;
	RFs iRfs;

protected: //分类引擎成员变量

	CFileActiveEngine* iFileActiverEngine;

	CImageResourceEngine* iImageResEngine; //文字资源引擎
	CGraphicsEngine *iGraphicsEngine; //绘图引擎

protected:
	CScreenLayout*		iScreenLayout;
	CTextResourceEngine* iTextResEngine; //文字资源引擎
};

#endif // MAINCONTROLENGINE_H
