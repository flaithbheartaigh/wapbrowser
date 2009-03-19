/*
============================================================================
 Name        : PhoneNumParser.h
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CPhoneNumParser declaration
============================================================================
*/

#ifndef PHONENUMPARSER_H
#define PHONENUMPARSER_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>


class CPhoneNumParser
{
public:
	CPhoneNumParser()
		: iPhoneNum(NULL)
	{
	}

	~CPhoneNumParser()
	{
		delete iPhoneNum;
	}

public:
	void Parse(const TDesC8& aData)
	{
		ASSERT(NULL == iPhoneNum);
// 		TPtrC8 ptr;
// 		ptr.Set(*aData);

		TPtrC8 findStr(_L8("mobileID="));

		int pos = aData.Find(findStr);
		pos += findStr.Length();
		TPtrC8 phoneNum = aData.Mid(pos,11);
		iPhoneNum = HBufC8::NewL(phoneNum.Length());
		iPhoneNum->Des().Append(phoneNum);
		//iPhoneNum->Des().Copy(phoneNum,phoneNum.Length());
		//iPhoneNum = phoneNum.AllocL();
	}

	const TDesC8& PhoneNum()  const
	{
		if(iPhoneNum)
		{
			return *iPhoneNum;
		}
		return KNullDesC8;
	}

private:
	HBufC8* iPhoneNum;
};

#endif // PHONENUMPARSER_H
