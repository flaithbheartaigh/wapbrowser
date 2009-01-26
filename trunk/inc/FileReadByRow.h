/*
============================================================================
 Name        : FileReadByRow.h
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : CFileReadByRow declaration
============================================================================
*/

#ifndef FILEREADBYROW_H
#define FILEREADBYROW_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <badesca.h>
// CLASS DECLARATION

/**
*  CFileReadByRow
* 
*/
class CFileReadByRow : public CBase
{
public: // Constructors and destructor

	/**
        * Destructor.
        */
	~CFileReadByRow();

        /**
        * Two-phased constructor.
        */
	static CFileReadByRow* NewL();

        /**
        * Two-phased constructor.
        */
	static CFileReadByRow* NewLC();

public:

	/**
        * Constructor for performing 1st stage construction
        */
	CFileReadByRow();

	/**
        * EPOC default constructor for performing 2nd stage construction
        */
	void ConstructL();
	void LoadItem(const TDesC& aFileName);

	const TDesC & GetTextById(TInt aId);

public:
	CDesCArray* iItem;

};

#endif // FileReadByRow_H
