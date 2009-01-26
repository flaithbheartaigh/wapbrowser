/*
============================================================================
 Name        : HttpObserver.h
 Author      : 卞涛
 Version     : 2.10
 Copyright   : Soarsky
 Description : 方便需要继承类include
============================================================================
*/

#ifndef HTTPOBSERVER_H
#define HTTPOBSERVER_H

/*
* MClientObserver
* CHTTPEngine passes events and responses body data with this interface.
* An instance of this class must be provided for construction of CHTTPEngine.
*/
class MClientObserver
	{
	public:
		/*
		* ClientEvent()
		*
		* Called when event occurs in CHTTPEngine.
		*
		* Params:
		*		aEventDescription:	A event in textual format, e.g.
		*							"Transaction Successful"
		*
		* Returns:
		* 		-
		*
		*/
		virtual void ClientEvent(const TDesC& aEventDescription,TInt aIndex) = 0;

		/*
		* ClientBodyReceived()
		*
		* Called when a part of the HTTP body is received.
		*
		* Params:
		*		aBodyData:	Part of the body data received. (e.g. part of
		*					the received HTML page)
		*		
		* Returns:
		* 		-
		*
		*/
		virtual void ClientBodyReceived(const TDesC8& aBodyData,TInt aIndex) = 0;
	};
/*
* MOperationObserver
* 业务模块需要继承了
*/
class MOperationObserver
	{
	//aEventType表示状态
	//aEventData表示数据
	public:
		virtual void OperationEvent(TInt aEventType,const TDesC& aEventData,TInt aType=0) = 0;
	};
#endif // HTTPOBSERVER_H

// End of File
