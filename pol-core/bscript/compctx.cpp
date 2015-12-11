/*
History
=======

Notes
=======

*/

#include "compctx.h"

#include <cstring>
#include <ostream>

namespace Pol {
  namespace Bscript {
	CompilerContext::CompilerContext() :
	  s(),
	  cursor( s.begin() ),
	  line( 1 ),
	  filename( "" ),
	  dbg_filenum( 0 )
	{
	}

	CompilerContext::CompilerContext( const std::string& filename, int dbg_filenum, const Unicode& s ) :
	  s( s ),
	  cursor( s.begin() ),
	  line( 1 ),
	  filename( filename ),
	  dbg_filenum( dbg_filenum )
	{}

	CompilerContext::CompilerContext( const CompilerContext& ctx ) :
	  s( ctx.s ),
	  cursor( ctx.cursor ),
	  line( ctx.line ),
	  filename( ctx.filename ),
	  dbg_filenum( ctx.dbg_filenum )
	{}

	/**
	 * Skips whitespaces. Moves the cursor forward until a non-whitespace is found
	 */
	void CompilerContext::skipws()
	{
	  while ( cursor != s.end() && cursor->isSpace() )
	  {
		if ( *cursor == '\n' )
		  ++line;
		cursor++;
	  }
	}

	/**
	 * Skips comments. Looks for a comment start. If found, moves the cursor to the end of it.
	 *
	 * @return o an success
	 */
	int CompilerContext::skipcomments()
	{
	  // Backup current line and cursor
	  auto cursor_bck = cursor;
	  auto line_bck = line;

	  while ( cursor != s.end() && cursor+1 != s.end() )
	  {
		if ( *cursor == '/' )
		{
		  if ( cursor[1] == '*' ) // got a start of multiline comment
		  {
			cursor += 2;
			if( eatToCommentEnd() )
			  return 0;
			else
			  break;
		  }
		  else if ( cursor[1] == '/' ) // comment, one line only
		  {
			cursor += 2;
			eatToEndOfLine();
			return 0;
		  }
		  else
		  {
			break;
		  }
		}
		else
		{
		  break;
		}
	  }

	  cursor = cursor_bck;
	  line = line_bck;
	  return 1;
	}

	void CompilerContext::printOn( std::ostream& os ) const
	{
	  os << "File: " << filename << ", Line " << line << std::endl;
	}

    void CompilerContext::printOn( fmt::Writer& writer ) const
    {
      writer << "File: " << filename << ", Line " << line << "\n";
    }

	void CompilerContext::printOnShort( std::ostream& os ) const
	{
	  os << filename << ", Line " << line << std::endl;
	}

    void CompilerContext::printOnShort( fmt::Writer& writer ) const
    {
      writer << filename << ", Line " << line << "\n";
    }

	/**
	 * Skips to the end of the line (or to the end of the file, whathever comes first)
	 */
	void CompilerContext::eatToEndOfLine()
	{
	  while( cursor != s.end() )
	  {
		if( *(cursor++) == '\n' )
		  return;
	  }
	}

	/**
	 * Skips to the end of a multiline comment, moving cursor forward
	 *
	 * @note dropped support for nested comments, 12-09-2015 Bodom
	 * @return 0 on success
	 */
	int CompilerContext::eatToCommentEnd()
	{
	  // Backup current cursor and line
	  auto cursor_bck = cursor;
	  auto line_bck = line;

	  while ( cursor != s.end() && cursor+1 != s.end() )
	  {
		if ( *cursor == '\n' )
		  ++line;

		if ( *cursor == '*' && cursor[1] == '/' )
		{
		  cursor += 2;
		  return 0;
		}

		cursor++;
	  }

	  cursor = cursor_bck;
	  line = line_bck;
	  return -1;
	}

  }

}
