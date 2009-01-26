/*
============================================================================
Name        : AudioPlayObserverObserver.h
Author      : ∏°…˙»Ù≤Ë
Version     :
Copyright   : Your copyright notice
Description : PopUpMenuObserver.h - header file
============================================================================
*/

class MAudioPlayNoticeObserver
{
public:
	virtual void AudioUpdate(const TDesC& aVol,const TDesC& aCurrentTime,const TDesC& aTotalTime,TInt aPercent) = 0;

	virtual void AudioSelected(const TInt aSelectIndex) = 0;
};