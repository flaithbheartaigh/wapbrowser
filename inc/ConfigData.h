/*
============================================================================
 Name        : ConfigData.h
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CConfigData declaration
============================================================================
*/

#ifndef CONFIGDATA_H
#define CONFIGDATA_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include "PreDefine.h"
//#include "UtilityTools.h"
#include "Define.h"


class CConfigData
{
public:
	CConfigData()
		: mobile_url(NULL)
		, mobile_pre_str(NULL)
		, mobile_len(0)
		, service_url(NULL)
	{

	}

	~CConfigData()
	{
		delete mobile_url;
		delete mobile_pre_str;
		delete service_url;
	}
	/*
	CConfigData(HBufC8* aBuf)
	{
	ASSERT(aBuf);

	}
	*/
	//void Parse(const HBufC8* aBuf)
	void Parse(const TDesC8& aDes)
	{
		int firstPos = 0;
		int lastPos = 0;

		TPtrC8 ptr;
		ptr.Set(aDes);

		firstPos = ptr.Find(_L8("="));
		lastPos = ptr.Find(_L8("\n"));	
		TPtrC8 mobile_url = ptr.Mid(firstPos + 1,lastPos - firstPos - 2);
		TRACE(mobile_url);
		ASSERT(this->mobile_url);
		this->mobile_url = mobile_url.AllocL();


		ptr.Set(ptr.Mid(lastPos + 1));
		firstPos = ptr.Find(_L8("="));
		lastPos = ptr.Find(_L8("\n"));		
		TPtrC8 mobile_pre_str = ptr.Mid(firstPos + 1,lastPos - firstPos - 2);
		TRACE(mobile_pre_str);
		ASSERT(this->mobile_pre_str);
		this->mobile_pre_str = mobile_pre_str.AllocL();

		ptr.Set(ptr.Mid(lastPos + 1));
		firstPos = ptr.Find(_L8("="));
		lastPos = ptr.Find(_L8("\n"));		
		TPtrC8 mobile_len = ptr.Mid(firstPos + 1,lastPos - firstPos - 2);
		TRACE(mobile_len);
		this->mobile_len = mobile_len.AllocL();

		ptr.Set(ptr.Mid(lastPos + 1));
		firstPos = ptr.Find(_L8("="));
		//lastPos = ptr.Find(_L8("0X0A"));	
		TPtrC8 service_url = ptr.Mid(firstPos + 1,ptr.Length() - firstPos - 3);
		TRACE(service_url);
		ASSERT(this->service_url);
		this->service_url = service_url.AllocL();
	}

	const TDesC8& MobileUrl() const
	{
		if(mobile_url)
		{
			return *mobile_url;
		}
		else
		{
			return KNullDesC8;
		}
	}

	const TDesC8& MobilePreStr() const
	{
		if(mobile_pre_str)
		{
			return *mobile_pre_str;
		}
		else
		{
			return KNullDesC8;
		}
	}

	const TDesC8& ServiceUrl() const
	{
		if(service_url)
		{
			return *service_url;
		}
		else
		{
			return KNullDesC8;
		}
	}

	const TDesC8&  MobileLen() const
	{
		if(mobile_len)
		{
			return *mobile_len;
		}
		else
		{
			return KNullDesC8;
		}
	}

	int MobileLenInInt() const
	{
		int len;
		if(mobile_len)
		{
			TLex8 lex(*mobile_len);
			lex.Val((TUint&)len);
		}
		return len;
	}

private:
	HBufC8* mobile_url;
	HBufC8* mobile_pre_str;
	HBufC8* mobile_len;
	//int mobile_len;
	HBufC8* service_url;
};


#endif // CONFIGDATA_H
