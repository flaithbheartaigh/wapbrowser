/*
============================================================================
 Name        : RichControl.h
 Author      : 浮生若茶
 Version     :
 Copyright   : Your copyright notice
 Description : CRichControl declaration
============================================================================
*/

#ifndef RICHCONTROL_H
#define RICHCONTROL_H

#include "Control.h"

class CWindow;
class CMainEngine;

class CRichControl : public CControl
{
public: // Constructors and destructor
	//CRichControl(CWindow* aParent,CMainEngine& aMainEngine);
	CRichControl::CRichControl(CWindow* aParent,CMainEngine& aMainEngine):CControl(aParent),aMainEngine(aMainEngine){};
	virtual ~CRichControl();

private:
	CMainWindow& aMainEngine;
};

#endif // RICHCONTROL_H
