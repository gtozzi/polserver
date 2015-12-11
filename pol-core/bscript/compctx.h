/*
History
=======

Notes
=======

*/

#ifndef BSCRIPT_COMPCTX_H
#define BSCRIPT_COMPCTX_H

#include <iosfwd>
#include "../../lib/format/format.h"
#include "../clib/unicode.h"

namespace Pol {
  namespace Bscript {

	/**
	 * Represents a single compile context.
	 *
	 * A new compile context is created for every source file being compiled,
	 * but a context can spawn itself many sub-contexts when needed
	 */
	class CompilerContext
	{
	public:
	  CompilerContext();
	  CompilerContext( const std::string& filename, int dbg_filenum, const Unicode& s );
	  CompilerContext( const CompilerContext& );
	  //CompilerContext& operator=( const CompilerContext& );

      void printOn( std::ostream& os ) const;
      void printOn( fmt::Writer& writer ) const;
	  void printOnShort( std::ostream& os ) const;
      void printOnShort( fmt::Writer& writer ) const;

	  void skipws();
	  int skipcomments();

	  /** The code, as string */
	  const Unicode s;
	  /** The cursor while reading the code (iterator over characters) */
	  Unicode::const_iterator cursor;
	  int line;
	  std::string filename;

	  int dbg_filenum;

	protected:
	  int eatToCommentEnd();
	  void eatToEndOfLine();
	};

	inline std::ostream& operator<<( std::ostream& os, const CompilerContext& ctx )
	{
	  ctx.printOn( os );
	  return os;
	}

    inline fmt::Writer& operator<<( fmt::Writer& writer, const CompilerContext& ctx )
    {
      ctx.printOn( writer );
      return writer;
    }
  }
}
#endif
