/*
History
=======

Notes
=======

*/

#include "filecont.h"
#include "fileutil.h"

#include "logfacility.h"
#include <cstdio>
#include <stdexcept>

// disables unsafe warning for fopen
#ifdef _MSC_VER
#pragma warning(disable:4996) 
#define _CRT_SECURE_NO_WARNINGS 
#endif

namespace Pol {
  namespace Clib {

	/**
	 * Opens the given file, handles checks and errors
	 */
	FILE* FileContentsBase::openFile( const std::string& filename )
	{
#ifdef _WIN32 // Unix does this automatically
	  std::string truename = Clib::GetTrueName( filename );
	  std::string filepart = Clib::GetFilePart( filename );
	  if ( truename != filepart && Clib::FileExists( filename ) )
	  {
		ERROR_PRINT << "Case mismatch: \n"
		  << "  Specified:  " << filepart << "\n"
		  << "  Filesystem: " << truename << "\n";
		throw std::runtime_error( "Error opening file" );
	  }
#endif

	  FILE *fp = fopen( filename.c_str(), "rb" );
	  if( fp == NULL )
	  {
        ERROR_PRINT << "Unable to open '" << filename << "' for reading.\n";
		throw std::runtime_error( "Error opening file" );
	  }
	  return fp;
	}

	FileContents::FileContents( const std::string& filename )
	{
	  FILE *fp = openFile( filename );

	  char buf[1024];
	  while( !ferror( fp ) && !feof( fp ) )
	  {
		size_t nread = fread( buf, 1, sizeof buf, fp );
		if( nread )
		  contents_.append( buf, nread );
	  }

	  fclose( fp );
	}

	const char* FileContents::contents() const
	{
	  return contents_.c_str();
	}

	void FileContents::set_contents( const std::string& str )
	{
	  contents_ = str;
	}

	/**
	 * Reads the given file name, files is supposed to be utf8
	 */
	FileContentsUnicode::FileContentsUnicode( const std::string& filename )
	{
	  FILE *fp = openFile( filename );
	  Utf8CharValidator validator;

	  char buf[1024];
	  while( !ferror( fp ) && !feof( fp ) )
	  {
		size_t nread = fread( buf, 1, sizeof buf, fp );
		for( size_t i = 0; i < nread; i++ )
		{
		  switch( validator.addByte(&buf[i]) )
		  {
		  case Pol::Utf8CharValidator::MORE:
			continue;
		  case Pol::Utf8CharValidator::INVALID:
			ERROR_PRINT << "Error while reading '" << filename << "': character coding is not utf8.\n";
			throw std::runtime_error( "Error reading file" );
		  default:
			assert(false); //This should never happen
		  case Pol::Utf8CharValidator::DONE:
			Pol::Utf8Char uchar = validator.getChar();
			char16_t u16char = uchar.asUtf16();
			if( ! u16char )
			{
			  ERROR_PRINT << "Error while reading '" << filename << "': found UO-incompatible character out of range 0x0001-0xFFFF.\n";
			  throw std::runtime_error( "Error reading file" );
			}
			contents_ += u16char;
		  }
		}
	  }

	  fclose( fp );
	}

	Unicode FileContentsUnicode::contents() const
	{
	  return contents_;
	}

	void FileContentsUnicode::set_contents( const Unicode& str )
	{
	  contents_ = str;
	}

  }

}
