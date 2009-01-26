#ifndef MULTIMEDIACONTROLLER_H
#define MULTIMEDIACONTROLLER_H

/**
*
* @class	MMultimediaController MultimediaController.h
* @brief	The MMultiMediaController class offers a mixin for the
* application controller to redraw the application view
*
* Copyright (c) EMCC Software Ltd 2003
* @version	1.0
*
*/
class MMultimediaController
	{
	public:
		virtual void RedrawView(TInt aType) = 0;
	};

#endif	// MULTIMEDIACONTROLLER_H

// End of File.
