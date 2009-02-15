/*
============================================================================
 Name        : WapBrowser.pan
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : Application panic codes
============================================================================
*/
#ifndef __WAPBROWSER_PAN__
#define __WAPBROWSER_PAN__
/** WapBrowser application panic codes */
enum TWapBrowserPanics
    {
    EWapBrowserUi = 1
    // add further panics here
    };
/*
inline void Panic(TWapBrowserPanics aReason)
    {
    _LIT(applicationName,"WapBrowser");
    User::Panic(applicationName, aReason);
    }
*/

#endif // __WAPBROWSER_PAN__
