/*
============================================================================
 Name        : CoCo.pan
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : Application panic codes
============================================================================
*/

#ifndef __COCO_PAN__
#define __COCO_PAN__

/** CoCo application panic codes */
enum TCoCoPanics
    {
    ECoCoUi = 1
    // add further panics here
    };

inline void Panic(TCoCoPanics aReason)
    {
    _LIT(applicationName,"CoCo");
    User::Panic(applicationName, aReason);
    }

#endif // __COCO_PAN__
