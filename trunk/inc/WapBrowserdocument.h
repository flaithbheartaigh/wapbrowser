/*
============================================================================
 Name        : WapBrowserDocument.h
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : Application document class (model)
============================================================================
*/
#ifndef __WAPBROWSERDOCUMENT_H__
#define __WAPBROWSERDOCUMENT_H__

#include <akndoc.h>

class CWapBrowserAppUi;
class CEikApplication;


class CWapBrowserDocument : public CAknDocument
    {
    public: 
        static CWapBrowserDocument* NewL( CEikApplication& aApp );
        static CWapBrowserDocument* NewLC( CEikApplication& aApp );
        virtual ~CWapBrowserDocument();
    public: 
        CEikAppUi* CreateAppUiL();
    private: 
        void ConstructL();
        CWapBrowserDocument( CEikApplication& aApp );
    };
#endif 

