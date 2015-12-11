/*
History
=======

Notes
=======

*/

#ifndef __TOKEN_H
#define __TOKEN_H

#ifndef __TOKENS_H
#include "tokens.h"
#endif
#ifndef __MODULES_H
#include "modules.h"
#endif

#include "options.h"
#include "../../lib/format/format.h"
#include "../clib/unicode.h"

#include <iosfwd>
#include <set>
namespace Pol {
  namespace Bscript {
	typedef struct
	{
	  unsigned sourceFile;
	  unsigned offset;
	  unsigned strOffset;
	} DebugToken;

	class UserFunction;

	class Token
	{
	public:
	  BTokenId id;
	  BTokenType type;
	  double dval;
	  union
	  {
		int precedence;
		int sourceFile;
	  };
	  int dbg_filenum;
	  int dbg_linenum;
	  union
	  {
		int lval;
		const unsigned char* dataptr;
	  };
	  UserFunction *userfunc;
	  bool deprecated;
	  unsigned char module;

	  static unsigned int instances();
	  static void show_instances();
	protected:
	  Unicode token;
#if STORE_INSTANCELIST
	  typedef set<Token*> Instances;
	  static Instances _instancelist;
#endif
	  static unsigned int _instances;
	  void register_instance();
	  void unregister_instance();

	public:
	  const Unicode *tokval() const { return &token; }

	  Token();
	  Token( const Token& tok );
	  Token& operator=( const Token& tok );

	  Token( ModuleID module, BTokenId id, BTokenType type );
	  Token( BTokenId id, BTokenType type );
	  Token( ModuleID module, BTokenId id, BTokenType type, UserFunction *userfunc );

	  void nulStr();
	  void copyStr( const Unicode& s );
	  inline void copyStr( const Unicode *s ) { copyStr(*s); };
	  void copyStr( const char *s );

	  ~Token();

	  void printOn( std::ostream& outputStream ) const;
	};

	std::ostream& operator << ( std::ostream&, const Token& );
    fmt::Writer& operator << ( fmt::Writer&, const Token& );
  }
}
#endif
