

#include <e32base.h>

class CVideoEngine;


class CVideoFileDetails : public CBase
    {
    public: 

        CVideoFileDetails();
        virtual ~CVideoFileDetails();

    private:   

        HBufC* iTitle;
        HBufC* iFileName;
        HBufC* iUrl;
        HBufC* iFormat;
        HBufC* iCopyright;
        TInt   iResolutionWidth;
        TInt   iResolutionHeight;
        TInt   iBitrate;
        TInt   iSize;
        TTime  iTime;
        TInt64 iDurationInSeconds;
        TBool  iAudioTrack;
        TBool  iVideoTrack;
        
    public: 
		friend class CVideoPlayEngine;
		friend class CVideoFileDetailsDialog;
    };
