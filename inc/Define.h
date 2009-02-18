/*
============================================================================
 Name        : Define.h
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : Define.h - header file
============================================================================
*/

#ifndef DEFINE_H
#define DEFINE_H

#include <string.h>
#include <e32std.h>
#include <e32base.h>
#include <coecntrl.h>
#include <coemain.h>

#include <avkon.hrh>
#include <aknnotewrappers.h>
#include <stringloader.h>
#include <WapBrowser.rsg>
#include <f32file.h>
#include <s32file.h>

#include <aknappui.h>

#ifdef __SERIES60_3X__
	#include <favouritessession.h> 
#endif
#include <favouritesitem.h>
#include <FavouritesDb.h>



#define KDefaultFont CCoeEnv::Static()->NormalFont()

#define KKeyCodeUp			EKeyUpArrow
#define KKeyCodeDown		EKeyDownArrow
#define KKeyCodeLeft		EKeyLeftArrow
#define KKeyCodeRight		EKeyRightArrow

#define KLeftSoftKey		EKeyDevice0
#define KRightSoftKey		EKeyDevice1
#define KOKKey				EKeyDevice3

#define KKeyCodeHome		'1'
#define KKeyCodeEnd			'4'
#define KKeyCodePageUp		'2'
#define KKeyCodePageDown	'5'


#define KScrollBackgroundColor TRgb(68,80,103)
#define KScrollSpaceColor TRgb(16,188,255)


#define SCREEN_WIDTH (176)
#define SCREEN_HEIGHT (208)

class MWebClientObserver
{
public:
	virtual void ClientEvent(const TDesC& aEventDescription) = 0;

public:
	virtual void ClientHeaderReceived(const TDesC& aHeaderData) = 0;
	virtual void ClientBodyReceived(const TDesC8& aBodyData,const TBool& isLast) = 0;
};

class MPageBuilder		
{
public:
	virtual void AddBr() = 0;
	virtual void AddPicture(const char* aName,const char* alt,const char* aLink = NULL) = 0;
	virtual void AddText(const char* aText,const char* aLink = NULL) = 0;
	virtual void SetRootLink(const char* aLink) = 0;
};

namespace Utils
{
/*
	HBufC* CreateHBufCFromCharL(const char* str)
	{
		HBufC* buf = HBufC::NewL(strlen(str) + 1);
		buf->Des().Copy(TPtrC8((const TUint8*)str));
		return buf;
	}

	HBufC* CreateHBufCFromCharLC(const char* str)
	{
		HBufC* buf = HBufC::NewLC(strlen(str) + 1);
		buf->Des().Copy(TPtrC8((const TUint8*)str));
		return buf;
	}
*/
#undef ASSERT
#ifdef _DEBUG
	inline void ASSERT(TBool aExpression)
	{
		if(!aExpression)
		{
			TBool stop = ETrue;		//用于调试，请在此加断点
		}
	}

	inline void ASSERT(const void* p)
	{
		ASSERT(NULL != p);
	}
#else

	inline void Assert(const char* filename,int line)
	{
		TPtrC8 ptr8((const TUint8*)filename);
		TFileName buf;
		buf.Zero();
		buf.Copy(ptr8);
		//UtilityTools::WriteLogsL(buf);
		User::Panic(buf,line);
	}
#define ASSERT(a)	if(a == 0)	\
	{	\
	Assert(__FILE__,__LINE__);	\
	}

#endif

	inline void TRACE(const TDesC8& aDes8)
	{
		HBufC* buf = HBufC::NewLC(100);
		buf->Des().Copy(aDes8.Mid(0,aDes8.Length() > 100 ? 100:aDes8.Length()));
		//buf->Des().Insert(0,_L("\n"));
		RDebug::Print(*buf);
		CleanupStack::PopAndDestroy();
	}
}

using namespace Utils;

class CCommand
{
public:
	virtual void Execute() = 0;
};
/*

class CGetCommand : public CCommand
{
public:
	CGetCommand(CWapBrowserAppUi& aWapBrowserAppUi)
		: iWapBrowserAppUi(aWapBrowserAppUi)
	{
	}
	~CGetCommand()
	{
		delete iUri;
	};

	void SetUri(const TDesC8& aUri)
	{
		iUri = aUri.Alloc();
	}

	virtual void Execute()
	{
		iWapBrowserAppUi.IssueHTTPGetL(*iUri);
	}
	// 	void SetProvider(CWapBrowserAppUi& aWapBrowserAppUi)
	// 	{
	// 	}
private:
	CWapBrowserAppUi& iWapBrowserAppUi;
	HBufC8* iUri;
};
*/

#endif // DEFINE_H

