/*
============================================================================
 Name        : WapBrowserApplication.h
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : Main application class
============================================================================
*/
#ifndef __WAPBROWSERAPPLICATION_H__
#define __WAPBROWSERAPPLICATION_H__

#include <aknapp.h>


class CWapBrowserApplication : public CAknApplication
    {
    public: 
        TUid AppDllUid() const;

    protected: 
        CApaDocument* CreateDocumentL();
    };


#endif 

