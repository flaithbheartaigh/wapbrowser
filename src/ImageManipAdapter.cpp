/**
 *
 * @brief Definition of CImageManipApplication
 *
 * Copyright (c) EMCC Software Ltd 2003
 * @version 1.0
 */

// INCLUDE FILES

// Class includes
#include "ImageManipAdapter.h"

// System includes
#include <aknutils.h>		// CompletePathWithAppPath()
#include <fbs.h>			// CFbsBitmap

// User includes
#include "MultimediaController.h"

// CONSTANTS
const TInt KJpgIndex = 0;

// ================= MEMBER FUNCTIONS =======================

/**
 * Symbian OS 2 phase constructor.
 * Constructs the CImageManipAdapter using the NewLC method, popping
 * the constructed object from the CleanupStack before returning it.
 *
 * @param aRect The rectangle for this window
 * @return The newly constructed CImageManipAdapter
 */
CImageManipAdapter* CImageManipAdapter::NewL(MMultimediaController& aMultimediaController, TDisplayMode aDeviceDisplayMode)
	{
	CImageManipAdapter* self = CImageManipAdapter::NewLC(  aMultimediaController,aDeviceDisplayMode);
	CleanupStack::Pop();
	return self;
	}

/**
 * Symbian OS 2 phase constructor.
 * Constructs the CImageManipAdapter using the constructor and ConstructL
 * method, leaving the constructed object on the CleanupStack before returning it.
 *
 * @param aRect The rectangle for this window
 * @return The newly constructed CImageManipAdapter
 */
CImageManipAdapter* CImageManipAdapter::NewLC(MMultimediaController& aMultimediaController,TDisplayMode aDeviceDisplayMode)
	{
	CImageManipAdapter* self = new (ELeave) CImageManipAdapter( aMultimediaController, aDeviceDisplayMode);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

/**
* Symbian OS 2nd phase constructor.
* Instantiates objects for encoding, decoding, rotating and scaling.
* A bitmap is also instantiated to be used for storing the result of a manipulation
* and used later for display on the screen.
*/
void CImageManipAdapter::ConstructL()
	{

	iFileToBitmap = CMdaImageFileToBitmapUtility::NewL(*this);

	ResetImageL();

	}

/**
* C++ constructor
*
* @param aMultimediaController The application controller object used to inform the view to redraw
* @param aDeviceDisplayMode The display mode (number of colors supported) of the device
*/
CImageManipAdapter::CImageManipAdapter(MMultimediaController& aMultimediaController,TDisplayMode aDeviceDisplayMode)
 :	iDeviceDisplayMode(aDeviceDisplayMode)
 ,iMultimediaController(aMultimediaController)
 ,iImage(NULL)
	{
		iManipulationState = EDecode;
	}

/**
 * Destructor.
 */
CImageManipAdapter::~CImageManipAdapter()
	{
	delete iFileToBitmap;

	delete iImage;

	}

/**
* Routes the commands obtained by the AppUi into the ImageManipAdapter to
* perform the user desired image manipulation
*
* @param aCommand The command Id from the AppUi used to decided which operation to perform
*/
void CImageManipAdapter::OpenImageByPath(const TDesC& aPath)
{

	iFileToBitmap->Cancel();

	iManipulationState = EDecode;
	DecodeOpenL(aPath);

}

/**
* From MMdaImageUtilObserver derivation.
* Used to inform client code that a conversion, rotation or scaling operation
* has completed.
* Following this the view is informed to redraw
*
* @param aError An error code value in case manipulation step failed
*/
void CImageManipAdapter::MiuoConvertComplete(TInt aError)
	{
	if (aError == KErrNone)
		{
		switch (iManipulationState)
			{
			case EDecode:
				iManipulationState = EDoNothing;

				iMultimediaController.RedrawView(0);

				break;
			default:
				break;
			}
		}
	else
		{
			iMultimediaController.RedrawView(aError);
		}
	}

/**
* From MMdaImageUtilObserver derivation.
* Used to inform client code that a file creation operation
* has completed.
* Following this the image data will be stored in the file created previously
*
* @param aError An error code value in case manipulation step failed
*/
void CImageManipAdapter::MiuoCreateComplete(TInt aError)
	{
	if (aError == KErrNone)
		{
		switch (iManipulationState)
			{
			default:
				break;
			}
		}
	else
		{
			iMultimediaController.RedrawView(aError);
		}
	}

/**
* From MMdaImageUtilObserver derivation.
* Used to inform client code that an open operation has completed allowing the
* subsequent encoding or decoding of an image file
*
* @param aError An error code value in case manipulation step failed
*/
void CImageManipAdapter::MiuoOpenComplete(TInt aError)
	{
	if (aError == KErrNone)
		{
		switch(iManipulationState)
			{
			case EDecode:
				TRAPD (err, DecodeConvertL());
				break;
			default:
				break;
			}
		}
	else
		{
			iMultimediaController.RedrawView(aError);
		}
	}

/**
* From MImageModel derivation
* Image that has been manipulated to be used for rendering by the view
* @return The manipulated image for display
*/
CFbsBitmap& CImageManipAdapter::Image() const
	{
	return *iImage;
	}

/**
* Opens up a handle to an existing GIF file.
* MuioOpenComplete() will be called when this is finished
*/
void CImageManipAdapter::DecodeOpenL(const TDesC& aPath)
	{
	TFileName fileName (aPath);
	TInt err=CompleteWithAppPath (fileName);
	iFileToBitmap->OpenL(fileName);
	}

/**
* Copies the GIF file into a CFbsBitmap that will be suitable for display.
* MuioConvertComplete() will be called when this operation is finished
*/
void CImageManipAdapter::DecodeConvertL()
	{
	TFrameInfo frmInfo;
	iFileToBitmap->FrameInfo(KJpgIndex, frmInfo);
	iImage->Create(frmInfo.iOverallSizeInPixels, iDeviceDisplayMode);
	iFileToBitmap->ConvertL(*iImage, KJpgIndex);
	}



/**
* Resets the image back to its original state
*/
void CImageManipAdapter::ResetImageL()
	{
		delete iImage;
		iImage = 0;
		iImage = new (ELeave) CFbsBitmap();
	}

void CImageManipAdapter::CancelDecode()
{
	iFileToBitmap->Cancel();
}

// End of File