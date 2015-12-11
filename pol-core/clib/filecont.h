#ifndef CLIB_FILECONT_H
#define CLIB_FILECONT_H

#include <string>

#include "unicode.h"

namespace Pol {
  namespace Clib {

	class FileContentsBase
	{
	protected:
	  FILE* openFile( const std::string& filname );
	};

	/**
	 * Simple class to read a whole file as a char array
	 */
	class FileContents : FileContentsBase
	{
	public:
	  FileContents( const std::string& filname );

	  const char* contents() const;
	  void set_contents( const std::string& str );
	private:
	  std::string contents_;
	};

	/**
	 * Simple class to read a whole file as an Unicode object
	 */
	class FileContentsUnicode : FileContentsBase
	{
	public:
	  FileContentsUnicode( const std::string& filname );

	  Unicode contents() const;
	  void set_contents( const Unicode& str );
	private:
	  Unicode contents_;
	};

  }
}
#endif
