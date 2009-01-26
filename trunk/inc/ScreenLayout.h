/*
============================================================================
 Name        : ScreenLayout.h
 Author      : ��������
 Version     :
 Copyright   : Your copyright notice
 Description : CScreenLayout declaration

 ����		 : ������沼��
============================================================================
*/

#ifndef SCREENLAYOUT_H
#define SCREENLAYOUT_H

// INCLUDES
#include "PreDefine.h"

class CMainEngine;

class CScreenLayout : public CBase
{
public: // Constructors and destructor
	~CScreenLayout();
	static CScreenLayout* NewL(CMainEngine* aEngine);
	static CScreenLayout* NewLC(CMainEngine* aEngine);

private:
	CScreenLayout(CMainEngine* aEngine);
	void ConstructL();

public:

public:
	void	SetFont(const CFont* aDefaultFont);
	const CFont* Font()const;

	void	SetClientRect(const TRect& aRect);	//���ö���״̬����ײ�������֮��Ŀͻ�����

	void	SetTitleBarRect(const TRect& aRect);

	const TRect&	TitleBarRect() const;

	TRect	GetStatusPaneRect()const;	//����״̬������

	TRect	GetFullClientRect()const;	//����״̬����ײ�������֮��������ͻ�����

	TRect	GetClientRect()const;		//����״̬����ײ�������֮��Ŀͻ�����

	TRect	GetControlPaneRect()const;	//�ײ�����������

	TPoint	GetNaviPanePoint()const;	//δʹ��
	TSize	GetNaviPaneSize()const;		//δʹ��
	TRect	GetNaviPaneRect()const;		//�����������������

	TPoint	GetListBoxPoint()const;		//δʹ��
	TSize	GetListBoxSize()const;		//δʹ��
	TRect	GetListBoxRect()const;		//δʹ��

	TRect	GetDialogRect()const;		//δʹ��

	TRect	GetDialogIconRect()const;	//δʹ��

	TRect	GetScrollbarRect()const;	//δʹ��

	TRect	LeftButtonRect()const;			//δʹ��

	TRect	RightButtonRect()const;			//δʹ��		

	TRect	MultiTextRect()const;			//δʹ��
	
	TRect	WhiteBackgroundRect()const;		//͸���׵׵ľ���,�б�����������һ����ʾ�����������

	TRect	InfoRect()const;				//��Ϣ����

	TSize	PopMenuItemSize()const;			

	TInt	TextLineHeight()const;			//�����м��
				
	TInt	LineHeight()const;				//�б��о�

	TInt	LineNum()const;					//�б���һ������ʾ������

	TInt	FontHeight()const;							//Ĭ������ĸ߶�
	TInt	ScrollBarWidth()const;						//���������

	TInt	IconNumPerLine()const;						
	TInt	IconLineNum()const;

	//��Ϊ��ʹ�ø�����,�������������ӱ�ʾ����
	TInt	Multiple()const;							//��������ʱ�ĳ���
	TInt	Divisor()const;								//��������ʱ�ĳ���
	//TSize	ScaleFixToNewScreen(CFbsBitmap* aBitmap);

	TInt	InnerMargin() const;								//��ҳ�߾�

	TInt	OuterMargin() const;
	//TInt	AttributeOffset()const;
	void	GetAttributeOffset(TInt& aOffset1,TInt& aOffset2)const;

	void	GetLoadingTextPos(TInt& aFirstTextPos,TInt& aSecondTextPost)const;

	void	GetOfferRewardEditorHeight(TInt& aHeight1,TInt& aHeight2)const;

	void	GetSaveAsPos(TPoint& aTitlePos,TPoint& aListPos)const;

	TSize	ScaleByDefaultRatio(CFbsBitmap* aBitmap)const;			//��Ĭ�ϱ�������

	//��ȡͼ�������������
	//�÷���������Draw������ʹ��
	//����ͼƬ���������Ŵ����С
	CFbsBitmap*	GetIconL(TInt aId,TInt aMaxXIndex = 1,TInt aMaxYIndex = 1,TInt aXIndex = 0,TInt aYIndex = 0)const;

	CFbsBitmap*	GetIconL(const TDesC& aFileName,TInt aId,TInt aMaxXIndex = 1,TInt aMaxYIndex = 1,TInt aXIndex = 0,TInt aYIndex = 0)const;

/*
	CFbsBitmap*	GetIconL(const TDesC& aFileName,TInt aId,const TRect& aSrcRect)const;	//��ȡͼ�������������
																						//�÷���������Draw������ʹ��
																						//����ͼƬ���������Ŵ����С
*/

public:
	static	TSize	ScreenSize();									//��Ļ�ߴ�
	static	TSize	ScaleFitToScreenWidth(CFbsBitmap* aBitmap);		//����Ļ���Ϊ׼��������
	static	TSize	ScaleFitToScreenHeight(CFbsBitmap* aBitmap);	//����Ļ�߶�Ϊ׼��������
	static	TSize	ScaleWithShortSide(CFbsBitmap* aBitmap);		//�Զ̱�Ϊ��׼��������
	//static	TSize	Scale(CFbsBitmap* aBitmap,TInt)

private:
	void	InitData();
	void	InitByScreenSize();			//������Ļ�ߴ��趨	
	void	InitByFont();				//��������ߴ��趨

	//void	InitDialogRect();			//��ʼ���Ի���ľ���

private:
	CMainEngine*	iEngine;
	const	CFont*	iDefaultFont;

	TRect	iStatusPaneRect;
	TRect	iFullClientRect;
	TRect	iClientRect;
	TRect	iControlPaneRect;

	TRect	iTitleBarRect;

	TRect	iNaviPaneRect;
	TRect	iListBoxRect;
	TRect	iDialogRect;
	TRect	iDialogIconRect;

	TRect	iScrollbarRect;

	TRect	iLeftButtonRect;
	TRect	iRightButtonRect;

	TRect	iMultiTextRect;

	TRect	iWhiteBackgroundRect;

	TRect	iInfoRect;

	TSize	iPopMenuItemSize;

	TInt	iFontHeight;

	TInt	iTextLineHeight;

	TInt	iListBoxLineHeight;

	TInt	iLineNum;

	//////////////////////////////////////////////////////////////////////////
	
	TInt	iScrollBarWidth;

	TInt	iIconNumPerLine;
	TInt	iIconLineNum;
	TInt	iInnerMargin;
	TInt	iOuterMargin;

	TInt	iMultiple;			//��176*208��ĻΪ��׼�ı���
	TInt	iDivisor;			//��176*208��ĻΪ��׼�ĳ���

	TInt	iLoadingTextPos_1;	//�������������һ���ļ���ʾ�Ĵ�ֱλ��
	TInt	iLoadingTextPos_2;	//�������������һ���ļ���ʾ�Ĵ�ֱλ��

	TInt	iAttributeOffset1;
	TInt	iAttributeOffset2;

	//TInt	iSaveAsTitlePos;
	TPoint	iSaveAsTitlePos;	//���Ϊ,ѡ�����Ի����б���Ļ���λ��
	TPoint	iSaveAsListPos;		//���Ϊ,ѡ�����Ի������б�Ļ���λ��

	//TInt	iOfferRewardEditorLineHeight;
	TInt	iOfferRewardEditorHeight1;
	TInt	iOfferRewardEditorHeight2;
};

#endif // SCREENLAYOUT_H
