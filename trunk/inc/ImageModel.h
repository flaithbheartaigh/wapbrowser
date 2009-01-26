#ifndef IMAGEMODEL_H
#define IMAGEMODEL_H

// FORWARD DECLARATIONS
class CFbsBitmap;

/**
*
* @class	MImageModel ImageModel.h
* @brief	Allows the view to get the newly maniuplated
* image so redrawing is possible
*
* Copyright (c) EMCC Software Ltd 2003
* @version	1.0
*
*/
class MImageModel
	{
	public:
		virtual CFbsBitmap& Image() const = 0;
	};

#endif	// IMAGEMODEL_H

// End of File.