/*
============================================================================
 Name        : MainControlEngine.h
 Author      : ����
 Version     : 2.10
 Copyright   : Soarsky
 Description : ����������ƣ��Է����������ʵ�����͵��ã����з�������ͳһ������������ʵ�������ͷţ�
			   ģ������ݴ��ݣ�����������ģ��֮�䴫������ʱ��ͨ������������г��ش��ݣ�
			   ������־��¼�����㿪���е��ԣ�����������д��
			   �������滹�������ñ���������ö��������
			   �������滹����һЩ���÷��������������ж��Ƿ�Ϊ���ֵȣ�
============================================================================
*/

#ifndef MAINCONTROLENGINE_H
#define MAINCONTROLENGINE_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <f32file.h>
//Ԥ����
//#include <CoCoPreDefine.h>
#include "PreDefine.h"
#include "UtilityTools.h"

class CTextResourceEngine;  //������Դ����
class CImageResourceEngine; //ͼƬ��Դ����
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

public: //�������úͻ�ȡ
	//�����ֻ�IMEI��
	void SetIMEI(const TDesC& aIMEI);
	//��ȡ�ֻ�IMEI��
	inline const TDesC& GetIMEI()
	{

		#ifdef __WINS__
			iIMEI.Zero();
			iIMEI.Append(_L("358361002809113"));
		#endif

		return iIMEI;
	}

	//��������״̬
	inline void SetLoginTypeL(TInt aType)
	{
		iLoginType=aType;
	}
	//��ȡ����״̬�Ƿ���Ҫ����
	inline TInt GetConState() const
	{
		return (TInt)iConState;
	}
	//��������״̬�Ƿ���Ҫ����
	inline void SetConState(TConState aConState)
	{
		iConState = aConState;
	}

	//��ȡ����װ·��
	inline const TDesC& GetAppPath() const
	{
		return iAppPath;
	}

	//��ȡ����װ�̷�
	inline const TDesC& GetAppDrive() const
	{
		return iDrive;
	}
	//��ȡ��Դ·��
	inline const TDesC& GetResPath() const
	{
		return iResPath;
	}
	
	//��ȡ�ļ�����Ŀ¼
	inline const TDesC& GetDownLoadDir()
	{
		return iDownLoadDir;
	}

	//��������߳���
	inline TInt GetDownLoadMaxThreadNum()
	{
		return iDownLoadMaxThreadNum;
	}

	//������ʷ��¼��
	inline TInt GetDownLoadMaxHistoryNum()
	{
		return iDownLoadMaxHistoryNum;
	}

	//��ȡ��¼��ʽ
	inline TInt GetLoginType() const
	{
		return iLoginType;
	}

	//��ȡ��Ϣ������
	inline TInt GetContentInfoMaxNum() const
	{
		return iContentInfoMaxNum;
	}

	//��ȡ��Ϣ�б���
	inline TInt GetContentListMaxNum() const
	{
		return iContentListMaxNum;
	}

	inline TInt GetIAPValue() const
	{
		return iIapValue;
	}

	//��ȡ���·�������
	inline TInt64 GetMonthTraffic_Send() const
	{
		return iMonthTraffic_Send;
	}

	//��ȡ���½�������
	inline TInt64 GetMonthTraffic_Rev() const
	{
		return iMonthTraffic_Rev;
	}

	//��ȡ���η�������
	inline TInt64 GetCurrentTraffic_Rev() const
	{
		return iCurrentTraffic_Rev;
	}

	//��ȡ���ν�������
	inline TInt64 GetCurrentTraffic_Send() const
	{
		return iCurrentTraffic_Send;
	}

	//��ȡ�Ƿ���������ʾ��
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
public: //���������ȡ

	//��ȡ������Դ����
	inline const CTextResourceEngine* GetTextResEngine() const
	{
		return iTextResEngine;
	}
	//��ȡͼƬ��Դ����
	inline const CImageResourceEngine* GetImageResEngine() const
	{
		return iImageResEngine;
	}
	//��ȡͼƬ��Դ����
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
	//���ó���װ�̷�
	void SetAppDrive();
	//������Դ����λ��
	void SetResPath();

	//��ʼ��

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
protected: //���ݳ�Ա����
	
	TBuf<20> iIMEI; //�ֻ�IMEI��
	TFileName iDrive; //����װ�̷�

	TFileName	iAppPath; //����װ·��

	TFileName iResPath;//��Դ·��

	TConState	iConState;//ѡ��cmnet cmwap����

	TBuf<255> iDownLoadDir;

	TInt iLoginType;//��¼��ʽ:0-����;1-����
	TInt iDownLoadMaxThreadNum;
	TInt iDownLoadMaxHistoryNum;
	TInt iContentInfoMaxNum;
	TInt iContentListMaxNum;
	TInt iIapValue;

	TBool iDownLoadVoice;

	//����ͳ��
	TBuf<20> iCurrentMonth;				//��ǰ�·�

	TInt64 iMonthTraffic_Rev;				//���½�������
	TInt64 iMonthTraffic_Send;			//���·�������

	TInt64 iCurrentTraffic_Rev;			//���ν�������
	TInt64 iCurrentTraffic_Send;			//���η�������

	void SetNetTrafficInfo();
	//дdebug_log��Ա����
	RFile iFile;
	RFs iRfs;

protected: //���������Ա����

	CFileActiveEngine* iFileActiverEngine;

	CImageResourceEngine* iImageResEngine; //������Դ����
	CGraphicsEngine *iGraphicsEngine; //��ͼ����

protected:
	CScreenLayout*		iScreenLayout;
	CTextResourceEngine* iTextResEngine; //������Դ����
};

#endif // MAINCONTROLENGINE_H
