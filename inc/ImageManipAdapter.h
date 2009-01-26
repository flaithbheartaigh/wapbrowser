#ifndef IMAGEMANIPADAPTER_H
#define IMAGEMANIPADAPTER_H

// System Includes
#include <e32base.h>
#include <MdaImageConverter.h>	// MMdaImageUtilObserver
#include <gdi.h>				// TDisplayMode

// User includes
#include "ImageModel.h" // MImageModel


//FORWARD DECLARATION
class CMdaImageFileToBitmapUtility;
class CFbsBitmap;
class MMultimediaController;

/**
 *
 * @class	CImageManipAdapter ImageManipAdapter.h
 * @brief	This is an Adapter class (see a design patterns book) which
 * acts as a wrapper around the functionality of the Media Server. The example
 * illustrates some of the core concepts required in understanding
 * image conversion/manipulation on Series 60 v1.x.
 *
 * Copyright (c) EMCC Software Ltd 2003
 * @version	1.0
 *
 */
class CImageManipAdapter : public CBase, public MMdaImageUtilObserver, public MImageModel //this is from Media Server
	{
public:	// Construction and destruction.
	static CImageManipAdapter* NewL(MMultimediaController& aMultimediaController, TDisplayMode aDeviceDisplayMode);
	static CImageManipAdapter* NewLC( MMultimediaController& aMultimediaController,TDisplayMode aDeviceDisplayMode);
	virtual ~CImageManipAdapter();

public:	// Member functions
	void OpenImageByPath(const TDesC& aPath);
	// Rest image
	void ResetImageL();
	void CancelDecode();
public:	// From MImageModel
	CFbsBitmap& Image() const;


protected:	// From MMdaImageUtilObserver
	// Observer functions to inform client code that the current
	// asynchronous step within the manipulation process has completed
	void MiuoConvertComplete(TInt aError);
	void MiuoCreateComplete(TInt aError);
	void MiuoOpenComplete(TInt aError);

private:
	// enumeration to keep track of current manipulation
	enum TImageManipState
		{
		EDecode = 0,
		EDoNothing
		};

private:	// Construction
	void ConstructL();
	CImageManipAdapter(MMultimediaController& aMultimediaController,TDisplayMode aDeviceDisplayMode);

private:	// Member Functions.
	// Image converstion functions
	void DecodeOpenL(const TDesC& aPath);
	void DecodeConvertL();

private: // Member data
	// From Media Server
	CMdaImageFileToBitmapUtility* iFileToBitmap;
	CFbsBitmap* iImage;

	MMultimediaController& iMultimediaController;

	TImageManipState iManipulationState;

	TDisplayMode iDeviceDisplayMode; // used to store device display mode
	};

#endif
