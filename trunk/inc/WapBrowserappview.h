/*
============================================================================
 Name	   : WapBrowserView.h
 Author	 : 浮生若茶
 Version	:
 Copyright   : Your copyright notice
 Description : Application view
============================================================================
*/
#ifndef __WAPBROWSERAPPVIEW_H__
#define __WAPBROWSERAPPVIEW_H__

class CPageBuilder;
class CPage;

class CWapBrowserAppView : public CCoeControl
{
public: // New methods
	static CWapBrowserAppView* NewL( const TRect& aRect );
	static CWapBrowserAppView* NewLC( const TRect& aRect );
	virtual ~CWapBrowserAppView();

private: // Constructors
	void ConstructL(const TRect& aRect);
	CWapBrowserAppView();

public:  // Functions from base classes
	void Draw( const TRect& aRect ) const;
	virtual void SizeChanged();
	virtual TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);

public:
	void ShowPage(CPageBuilder* aPageBuilder);
	void ShowPage(CPage* aPage);

private:
	CPage* iPage;
	CPageBuilder* iPageBuilder;
	int startYPos;
};
#endif // __WAPBROWSERAPPVIEW_H__
// End of File