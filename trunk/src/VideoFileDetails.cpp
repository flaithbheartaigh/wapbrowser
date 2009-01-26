
#include    "VideoFileDetails.h"


CVideoFileDetails::CVideoFileDetails()
{
}

CVideoFileDetails::~CVideoFileDetails()
{
    delete iTitle;
    delete iFileName;
    delete iUrl;
    delete iFormat;
    delete iCopyright;
}
