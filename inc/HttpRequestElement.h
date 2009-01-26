/*
============================================================================
 Name        : HttpRequestElement.h
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : CHttpRequestElement declaration
============================================================================
*/

#ifndef HTTPREQUESTELEMENT_H
#define HTTPREQUESTELEMENT_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

// CLASS DECLARATION
class MOperationObserver;
/**
*  CHttpRequestElement
* 
*/
class CHttpRequestElement : public CBase
{
public: // Constructors and destructor

	/**
        * Destructor.
        */
	~CHttpRequestElement();

        /**
        * Two-phased constructor.
        */
	static CHttpRequestElement* NewL(MOperationObserver& aObserver);

        /**
        * Two-phased constructor.
        */
	static CHttpRequestElement* NewLC(MOperationObserver& aObserver);

	//inline void SetObserver(MOperationObserver& aObserver){iObserver=aObserver;}; //����Observer
	inline MOperationObserver* GetObserver(){ return &iObserver;};

	inline void SetRequestType(TInt aRequestType){iRequestType=aRequestType;};
	inline TInt GetRequestType(){ return iRequestType;};

	inline void SetHandleType(TInt aType){iType=aType;};
	inline TInt GetHandleType(){ return iType;};
	
	void SetContentType(const TDesC8& aContentType);
	const TDesC8& GetContentType();

	void SetUrl(const TDesC8& aUrl);
	const TDesC8& GetUrl();

	void SetSendData(const TDesC8& aSendData);
	const TDesC8& GetSendData();

	void SetReceiveData(const TDesC8& aReceiveData);
	const TDesC& GetReceiveData();

	void SetFileName(const TDesC& aFileName);
private:

	CHttpRequestElement(MOperationObserver& aObserver);
	void ConstructL();
	void GetClientBody(const TDesC8& aBody);
private:
	MOperationObserver& iObserver; //����Observer
	TInt iRequestType; //����ʽ(Get=1/Post=2)
	HBufC8* iContentType;//ContentType(���ϴ�����ʱ���͵Ĳ�һ��)
	HBufC8* iUrl;//��������url
	HBufC8* iSendData;//��������
	HBufC* iReceiveData; //���յ�������
	HBufC8* iReceiveData8; //���յ�������
	HBufC* iFileName;
	TInt iType;
};

#endif // HTTPREQUESTELEMENT_H
