/*
============================================================================
 Name        : CoCoView.h
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : Application view
============================================================================
*/

#ifndef __COCOAPPVIEW_H__
#define __COCOAPPVIEW_H__

// INCLUDES
#include <coecntrl.h>
#include <aknindicatorcontainer.h> 

#include "NotifyTimer.h"

class CWindow;
class CMainWindow;
class CMainEngine;

struct S_uid: public TUid
{
        S_uid(int i)
        { 
                iUid = i;
        }
};
enum TAknEditingState
	{
	// No input mode
	EStateNone,
	// Predictive upper case input mode
	ET9Upper,
	// Predictive lower case input mode
	ET9Lower,
	// Predictive shifted input mode
	ET9Shifted,
	// Numeric input mode
	ENumeric,
	// Multitap upper case input mode
	EMultitapUpper,
	// Multitap lower case input mode
	EMultitapLower,
	// Multitap shifted input mode
	EMultitapShifted,
	// Predictive Arabic input mode
    ET9Arabic,
	// Predictive Hebrew input mode
    ET9Hebrew,
	// Multitap Arabic input mode
    EMultitapArabic,
	// Multitap Hebrew input mode
    EMultitapHebrew,
	// Arabic-Indic numeric input mode
    EArabicIndicNumeric,
    // Multitap Thai input mode
    EMultitapThai,
    // Predictive Thai input mode
    ET9Thai,
    // QWERTY shifted input mode
    EQwertyShift,
    // Predictive Farsi and Urdu input mode
    ET9FarsiAndUrdu,
    // Multitap Farsi and Urdu input mode
    EMultitapFarsiAndUrdu,

	//
	// Chinese states
	//
	// Pinyin input mode
	EIndicatorStatePinyin =20,
	// Zhuyin inout mode
	EIndicatorStateZhuyin,
	// Stroke input mode
	EIndicatorStateStroke,
	// Zhuyin find input mode
	EIndicatorStateZhuyinFind,
	// Stroke find input mode
    EIndicatorStateStrokeFind,
	// Stroke trad input mode
    EIndicatorStateStrokeTrad,
	// Cang Jie input mode
    EIndicatorStateCangJie,

    //
    // Japanese states
	//
	// Kanji and Hiragana input mode
    EIndicatorStateHiraganaKanji = 40,
    // Hiragana input mode
    EIndicatorStateHiragana,
    // Half-width Katakana input mode
    EIndicatorStateHalfKatakana,
    // Full-width Katakana input mode
    EIndicatorStateFullKatakana,
    // Full-width LatinUpper input mode
    EIndicatorStateFullLatinUpper,
    // Full-width LatinLower input mode
    EIndicatorStateFullLatinLower,
    // Full-width LatinText input mode
    EIndicatorStateFullLatinText,
    // Numeric input mode
    EIndicatorStateFullNumeric,
    // japanese predictive mode
    EIndicatorStateJapanesePredictive,

    //
    // Vietnamese
    //
    // Predictive Vietnamese upper case input mode
    ET9UpperVietnamese = 60,
    // Predictive Vietnamese lower case input mode
    ET9LowerVietnamese,
    // Predictive Vietnamese shifted input mode
    ET9ShiftedVietnamese,
    // Multitap Vietnamese upper case input mode
    EMultitapUpperVietnamese,
    // Multitap Vietnamese lower case input mode
    EMultitapLowerVietnamese,
    // Multitap Vietnamese shifted input mode
    EMultitapShiftedVietnamese,

    //
    // Hindi
    //
    // Multitap Hindi input mode
    EIndicatorStateMultitapHindi= 80,
    // Predictive Hindi input mode
    EIndicatorStatePredictiveHindi,
    // Devangari Indic numeric input mode
    EDevanagariIndicNumeric
	};

class MAknEditingStateIndicator
{
public:
        virtual void SetState(TAknEditingState aState) = 0; 
        virtual CAknIndicatorContainer *IndicatorContainer() = 0;
};

class CCoCoAppView : public CCoeControl , public MTimerNotifier
    {
    public: // New methods
        static CCoCoAppView* NewL( const TRect& aRect );
        static CCoCoAppView* NewLC( const TRect& aRect );
        virtual ~CCoCoAppView();

    private: // Constructors
        void ConstructL(const TRect& aRect);
        CCoCoAppView();

    public:  // Functions from base classes
        void Draw( const TRect& aRect ) const;

        virtual void SizeChanged();

		TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);

		TInt CountComponentControls() const;

		CCoeControl* ComponentControl(TInt aIndex) const;
	
		inline CMainEngine& GetMainEnging(){ return *iMainEngine; };

		RWindow& GetWindow()
		{
			return Window();
		}
	public://From MTimerNotifier
		virtual TBool	DoPeriodTask();

	public:
		void	RequestDraw();

		void	SetCurWindow(CWindow* aWindow);

		CWindow*	CurWindow() const;

		TInt	GetEditorInputMethod();

		void	SetClippingRect(const TRect& aRect);

		void	SetClippingRegion(const TRegion& aRegion);

		void	CancelClipping();

	private:
		void	ApplicationStartL();		//初始化程序
		void	ApplicationEnd();			//清理与析构

		void	DrawWindow() const;

	private:
		CMainEngine*	iMainEngine;
		CMainWindow*	iMainWindow;
		CWindow*		iCurWindow;

		CNotifyTimer*	iTimer;

		TBool			iSizeChanged;
		TBool			iIsDraw;	

		TInt			iDrawCount;
    };

#endif // __COCOAPPVIEW_H__

// End of File
