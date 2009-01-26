/*
============================================================================
 Name        : BitmapFactory.h
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : CBitmapFactory declaration
============================================================================
*/

#ifndef BITMAPFACTORY_H
#define BITMAPFACTORY_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#ifdef __SERIES60_3X__
	#include <e32cmn.h>
#endif
#include <FBS.H>
// CLASS DECLARATION
class CMainEngine;
class CFbsBitmap;
class CGraphic;
/**
*  CBitmapFactory
* 
*/
enum TDownLoadIconIndex
{
	EDownLoadRed,
	EDownLoadYellow,
	EDownLoadGreen,
	EDownLoadDownLoading,
	EDownLoadStop,
	EDownLoadError,
	EDownLoadWaiting
};
enum TMusicPlayIconIndex
{
	EPlayOneIcon,
	EPlayTwoIcon,
	EStopOneIcon,
	EStopTwoIcon
};
enum TMusicSelectIndex
{
	ELeftOneIcon,
	ELeftTwoIcon,
	ERightOneIcon,
	ERightTwoIcon
};
enum TMusicTimeIconIndex
{
	EMusicTimeRate,
	EMusicTimeBackground
};
enum TMusicBugleIconIndex
{
	EMusicBugleStart,
	EMusicBugleStop
};
enum TLogoIconIndex
{
	ELogoIconOne,
	ELogoIconTwo,
	ELogoIconThree,
	ELogoIconFour
};
enum TFileTypeIndex
{
	EFileTypeEBook,
	EFileTypeOther,
	EFileTypeAppOrGame,
	EFileTypeVideo,
	EFileTypeTheme,
	EFileTypeImage,
	EFileTypeMusic,
	EFileTypeFile,
	EFileTypeLink
};
enum TGradeStarIndex
{
	EGradeStarOne,
	EGradeStarHalf,
	EGradeStarZero
};
enum TMainIconIndex
{
	EMainIconOne,
	EMainIconTwo,
	EMainIconThree,
	EMainIconFour,
	EMainIconFive,
	EMainIconSix,
	EMainIconSevern,
	EMainIconEight,
	EMainIconNine,
	EMainIconTen
};
enum TWeatherIndex
{
	EWeatherRain,
	EWeatherWind,
	EWeatherSnow,
	EWeatherCloudy,
	EWeatherFine,
	EWeatherFog,
	EWeatherCloudySky
};
enum TInputIndex
{
	EInputNumber,
	EInputAbc,
	EInputabc,
	EInputABC,
	EInputChinese
};
enum TDialogIndex
{
	EDialogSuccess,
	EDialogWait,
	EDialogAngry,
	EDialogFail,
	EDialogSmile,
	EDialogQuestion
};
enum TOperationIndex
{
	EOperationUpArraw,
	EOperationDownArraw,
	EOperationLeftArraw,
	EOperationRightArraw,
	EOperationRadio,
	EOperationRadioSelected,
	EOperationCheckBox,
	EOperationCheckBoxSelected
};
enum TArrowIndex
{
	EArrawLeft,
	EArrawRight
};
class CBitmapFactory : public CBase
{
public: // Constructors and destructor

	~CBitmapFactory();
	static CBitmapFactory* NewL(CMainEngine& aMainEngine);
	static CBitmapFactory* NewLC(CMainEngine& aMainEngine);

private:

	CBitmapFactory(CMainEngine& aMainEngine);
	void ConstructL();

public:
	CFbsBitmap* GetBitmap(TInt aID);
	CFbsBitmap* GetBitmapByWidth(TInt aID);
	
	//��������
	void DrawGradeBmpByID(CGraphic& gc,TPoint aPoint,TInt aIndex) const;
	TSize GetGradeBmpSize(TInt aIndex=0) const;
	TInt GetGradeBmpCount();
	void ClearGradeBmp();

	//������Ź�ͼ��
	void DrawMainIconBmpByID(CGraphic& gc,TPoint aPoint,TInt aIndex) const;
	TSize GetMainIconBmpSize(TInt aIndex=0) const;
	TInt GetMainIconBmpCount();
	void ClearMainIconBmp();

	//����ͼ��
	void DrawWeatherIconBmpByID(CGraphic& gc,TPoint aPoint,TInt aIndex) const;
	TSize GetWeatherIconBmpSize(TInt aIndex=0) const;
	TInt GetWeatherIconBmpCount();
	void ClearWeatherIconBmp();

	//���뷨ͼ��
	void DrawInputIconBmpByID(CGraphic& gc,TPoint aPoint,TInt aIndex) const;
	TSize GetInputIconBmpSize(TInt aIndex=0) const;
	TInt GetInputIconBmpCount();
	void ClearInputIconBmp();
	const CFbsBitmap*	GetInputIconBmp(TInt aIndex) const;
	const CFbsBitmap*	GetInputIconBmpMask(TInt aIndex) const;

	//����ͼ��
	void DrawOperationIconBmpByID(CGraphic& gc,TPoint aPoint,TInt aIndex) const;
	TSize GetOperationIconBmpSize(TInt aIndex=0) const;
	TInt GetOperationIconBmpCount();
	void ClearOperationIcon();

	//�ļ����ͼ��
	void DrawFileTypeIconBmpByID(CGraphic& gc,TPoint aPoint,TInt aIndex) const;
	TSize GetFileTypeIconBmpSize(TInt aIndex=0) const;
	TInt GetFileTypeIconBmpCount();
	void ClearFileTypeIcon();
	const CFbsBitmap*	GetFileTypeIcon(TInt aIndex) const;
	const CFbsBitmap*	GetFileTypeIconMask(TInt aIndex) const;

	//��½����4��ͼƬ
	void DrawLogoIconBmpByID(CGraphic& gc,TPoint aPoint,TInt aIndex) const;
	TSize GetLogoIconBmpSize(TInt aIndex=0) const;
	TInt GetLogoIconBmpCount();
	void ClearLogoIcon();

	//�ȴ�ͼ��
	void DrawDialogIconBmpByID(CGraphic& gc,TPoint aPoint,TInt aIndex) const;
	TSize GetDialogIconBmpSize(TInt aIndex=0) const;
	TInt GetDialogIconBmpCount();
	void ClearDialogIcon();

	//���ֲ���
	void DrawMusicPlayIconBmpByID(CGraphic& gc,TPoint aPoint,TInt aIndex) const;
	TSize GetMusicPlayIconBmpSize(TInt aIndex=0) const;
	TInt GetMusicPlayIconBmpCount();
	void ClearMusicPlayIcon();

	//���ֽ���
	void DrawMusicTimeIconBmpByID(CGraphic& gc,TPoint aPoint,TInt aIndex) const;
	TSize GetMusicTimeIconBmpSize(TInt aIndex=0) const;
	TInt GetMusicTimeIconBmpCount();
	void ClearMusicTimeIcon();

	const CFbsBitmap*	GetMusicTimeIcon(TInt aIndex) const;
	const CFbsBitmap*	GetMusicTimeIconMask(TInt aIndex) const;

	//����ѡ��
	void DrawMusicSelectIconBmpByID(CGraphic& gc,TPoint aPoint,TInt aIndex) const;
	TSize GetMusicSelectIconBmpSize(TInt aIndex=0) const;
	TInt GetMusicSelectIconBmpCount();
	void ClearMusicSelectIcon();

	//��������
	void DrawMusicBugleIconBmpByID(CGraphic& gc,TPoint aPoint,TInt aIndex) const;
	TSize GetMusicBugleIconBmpSize(TInt aIndex=0) const;
	TInt GetMusicBugleIconBmpCount();
	void ClearMusicBugleIcon();

	//���Ҽ�ͷ
	void DrawArrowIconBmpByID(CGraphic& gc,TPoint aPoint,TInt aIndex) const;
	TSize GetArrowIconBmpSize(TInt aIndex=0) const;
	TInt GetArrowIconIconBmpCount();
	void ClearArrowIcon();

	//����ͼ��
	void DrawDownLoadIconBmpByID(CGraphic& gc,TPoint aPoint,TInt aIndex) const;
	TSize GetDownLoadIconBmpSize(TInt aIndex=0) const;
	TInt GetDownLoadIconBmpCount();
	void ClearDownLoadIcon();
	const CFbsBitmap*	GetDownLoadIcon(TInt aIndex) const;
	const CFbsBitmap*	GetDownLoadIconMask(TInt aIndex) const;
public:
	void InitGradeBmp();
	void InitMainIcon();
	void InitWeatherIcon();
	void InitInputIcon();
	void InitOperationIcon();
	void InitFileTypeIcon();
	void InitLogoIcon();
	void InitDialogIcon();
	void InitMusicPlayIcon();
	void InitMusicBugleIcon();
	void InitMusicTimeIcon();
	void InitMusicSelectIcon(); 
	void InitDownLoadIcon(); 
	void InitArrowIcon();

private:
	CMainEngine& iMainEngine;

	RPointerArray<CFbsBitmap>	iGradeIconArray;
	RPointerArray<CFbsBitmap>	iGradeIconMaskArray;

	RPointerArray<CFbsBitmap>	iMainIconArray;
	RPointerArray<CFbsBitmap>	iMainIconMaskArray;

	RPointerArray<CFbsBitmap>	iWeatherIconArray;
	RPointerArray<CFbsBitmap>	iWeatherIconMaskArray;

	RPointerArray<CFbsBitmap>	iInputIconArray;
	RPointerArray<CFbsBitmap>	iInputIconMaskArray;

	RPointerArray<CFbsBitmap>	iOperationIconArray;
	RPointerArray<CFbsBitmap>	iOperationIconMaskArray;

	RPointerArray<CFbsBitmap>	iFileTypeIconArray;
	RPointerArray<CFbsBitmap>	iFileTypeIconMaskArray;

	RPointerArray<CFbsBitmap>	iLogoIconArray;
	RPointerArray<CFbsBitmap>	iLogoIconMaskArray;

	RPointerArray<CFbsBitmap>	iDialogIconArray;
	RPointerArray<CFbsBitmap>	iDialogIconMaskArray;

	RPointerArray<CFbsBitmap>	iMusicPlayIconArray;
	RPointerArray<CFbsBitmap>	iMusicPlayIconMaskArray;

	RPointerArray<CFbsBitmap>	iMusicSelectIconArray;
	RPointerArray<CFbsBitmap>	iMusicSelectIconMaskArray;

	RPointerArray<CFbsBitmap>	iMusicBugleIconArray;
	RPointerArray<CFbsBitmap>	iMusicBugleIconMaskArray;

	RPointerArray<CFbsBitmap>	iMusicTimeIconArray;
	RPointerArray<CFbsBitmap>	iMusicTimeIconMaskArray;

	RPointerArray<CFbsBitmap>	iDownLoadIconArray;
	RPointerArray<CFbsBitmap>	iDownLoadIconMaskArray;

	RPointerArray<CFbsBitmap>	iArrowIconArray;
	RPointerArray<CFbsBitmap>	iArrowIconMaskArray;
};

#endif // BITMAPFACTORY_H
