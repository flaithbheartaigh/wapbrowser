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


#define KDefaultFont CCoeEnv::Static()->NormalFont()

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
		UtilityTools::WriteLogsL(buf);
		User::Panic(buf,line);
	}
#define ASSERT(a)	if(a == 0)	\
	{	\
	Assert(__FILE__,__LINE__);	\
	}

#endif


}

using namespace Utils;

#endif // DEFINE_H

