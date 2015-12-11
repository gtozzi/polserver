/*
History
=======
2006/09/16 Shinigami: fixed Memory Overwrite Bug in convertArrayToUC

Notes
=======

*/

#include "unicode.h"
#include "endian.h"

#include <boost/algorithm/string/case_conv.hpp>

namespace Pol {
  namespace Clib {
	// Warning: Make sure that your buffer if large enough! Don't forget to add space for null terminator if requested.
	bool convertArrayToUC( Bscript::ObjArray*& in_text, u16* out_wtext,
						   size_t textlen, bool ConvToBE /*false*/, bool nullterm /*true*/ )
	{
	  u16 value;
	  size_t pos = 0;
	  for( size_t i = 0; i < textlen; i++ )
	  {
		Bscript::BObject* bo = in_text->ref_arr[i].get( );
		if( bo == NULL )
		  continue;
		if ( !bo->isa( Bscript::BObjectImp::OTLong ) )
		  return false;
		Bscript::BObjectImp *imp = bo->impptr( );
		Bscript::BLong* blong = static_cast<Bscript::BLong*>( imp );
		value = (u16)( blong->value() & 0xFFFF );
		out_wtext[pos++] = ConvToBE ? ctBEu16( value ) : value;
		// Jump out on a NULL (EOS) value (should stop exploits, too?)
		if( value == 0L )
		  return true;
	  }
	  if( nullterm )
		out_wtext[pos] = (u16)0L;
	  return true;
	}

	bool convertUCtoArray( const u16* in_wtext, Bscript::ObjArray*& out_text,
						   size_t textlen, bool ConvFromBE /*false*/ )
	{
	  u16 value;
	  out_text = new Bscript::ObjArray( );
	  for( size_t i = 0; i < textlen; i++ )
	  {
		value = ConvFromBE ? cfBEu16( in_wtext[i] ) : in_wtext[i];
		// Jump out on a NULL (EOS) value (should stop exploits, too?)
		if( value == 0L )
		  return true;
		out_text->addElement( new Bscript::BLong( value ) );
	  }
	  return true;
	}
  }


  /**
   * Create an utf8 char from a standard char.
   */
  Utf8Char::Utf8Char( const char* c )
  {
    if( *c < 0x80 )
    {
      bytes_ += *c;
    }
    else
    {
      bytes_ += static_cast<char>(0xC0 | ( *c >> 6 )); // first byte: 110XXXXX
      bytes_ += static_cast<char>(0x80 | ( *c & 0x3f )); // second byte: 10XXXXXX
    }
  }

  bool Utf8Char::operator==( const char c ) const
  {
    return bytes_.size() == 1 && bytes_[0] == c;
  }

  /**
   * Returns number of bytes represented by this utf8 char
   */
  u8 Utf8Char::getByteLen() const
  {
    if( bytes_[0] < 0x80 )
      return 1;

    u8 ret = 0;
    for( u8 i = 7; i >= 0; i-- )
      if( bytes_[0] & ( 1 << i ) )
        ret++;
      else
        return ret;
  }

   /**
   * Returns an UTF32 representation of this utf8 char
   *
   * @return The u32 value, always > 0
   */
  char32_t Utf8Char::asUtf32() const
  {
    u8 len = getByteLen();
    if( len <= 1 )
      return bytes_[0];

    char32_t out = 0;
    // Copy bits from first byte
    for( u8 i = 7-3-len; i >= 0; i-- )
      if( bytes_[0] & ( 1 << i ) )
        out |= 1 << ( (6 * ( len - 1 ) ) + 1 + i );
    // Copy bits from remaining bytes
    for( u8 bn = 1; bn < len; bn++ )
      for( u8 i = 5; i >= 0; i-- )
        if( bytes_[bn] & ( 1 << i ) )
          out |= 1 << ( (6 * ( len - bn - 1)) + i );

    return out;
  }

  /**
   * Returns an UTF16 representation of this utf8 char, when possible
   *
   * @param failsafe When true, will replace invalid chars with 0xFFFD replacement character instead of returning 0
   * @return The char value, 0 when the char has no UTF16 representation
   */
  char16_t Utf8Char::asUtf16( const bool failsafe ) const
  {
    char32_t out = asUtf32();
    if( out > 0xFFFF )
      if( failsafe )
        return 0xFFFD;
      else
        return 0;
    return out;
  }

  /**
   * Returns an ANSI representation of this utf8 char, when possible
   *
   * @param failsafe When true, will replace invalid chars with "?" instead of returning 0
   * @return The char value, 0 when the char has no ANSI representation
   */
  char Utf8Char::asAnsi( const bool failsafe ) const
  {
    char32_t out = asUtf32();
    if( out > 0xFF )
      if( failsafe )
        return '?';
      else
        return 0;
    return out;
  }


  /**
   * Creates a new char from a given char
   */
  UnicodeChar::UnicodeChar( const char16_t c ) :
  val_( c )
  {
  }

  bool UnicodeChar::operator==( const char16_t c ) const
  {
    return val_ == c;
  }

  /**
   * Returns minumum number of bytes this char can fit in
   */
  u8 UnicodeChar::getByteLen() const
  {
    if( val_ > 0xFF )
      return 2;
    return 1;
  }

  /**
   * Returns an UTF16 representation
   */
  char16_t UnicodeChar::asUtf16( ) const
  {
    return val_;
  }

  /**
   * Returns an ANSI representation of this utf8 char, when possible
   *
   * @param failsafe When true, will replace invalid chars with "?" instead of returning 0
   * @return The char value, 0 when the char has no ANSI representation
   */
  char UnicodeChar::asAnsi( const bool failsafe ) const
  {
    if( val_ > 0xFF )
      if( failsafe )
        return '?';
      else
        return 0;
    return val_;
  }

  /**
   * Tells wether this is a space character
   */
  bool UnicodeChar::isSpace() const
  {
    return isspace(val_);
  }

  /**
   * Tells wether this is a digit character
   */
  bool UnicodeChar::isDigit() const
  {
    return isdigit(val_);
  }

  /**
   * Tells wether this is an alphabetic character
   */
  bool UnicodeChar::isAlpha() const
  {
    return isalpha(val_);
  }


  Unicode& Unicode::operator+=( const char c )
  {
    append(&UnicodeChar(c));
    return *this;
  }
  bool Unicode::operator==( const char c ) const
  {
    return size() == 1 && front() == c;
  }

  /**
   * Makes an ANSI string representation of this Unicode object, when possible
   *
   * @param outStr pointer to the string object that will be appended with the unicode output
   *               when an error is encountered, the string will be only partially filled.
   * @return TRUE on success, FALSE when non ANSI-compatible characters were encountered.
   */
  bool Unicode::asAnsi( std::string* outStr ) const
  {
    for( auto it = begin(); it != end(); ++it )
    {
      char c = it->asAnsi();
      if( c )
        *outStr += c;
      else
        return false;
    }
    return true;
  }

  /**
   * Returns an ANSI string representation of this Unicode object
   *
   * @param failsafe When true, will replace invalid chars with "?" instead of throwing
   * @return the ANSI string
   * @throws badcast
   */
  std::string Unicode::asAnsi( const bool failsafe ) const
  {
    std::string ret;

    for( auto it = begin(); it != end(); ++it )
    {
      char c = it->asAnsi(failsafe);
      if( c )
        ret += c;
      else
        throw badcast;
    }

    return ret;
  }

  /**
   * In-place convert this string to lowercase
   */
  void Unicode::toLower()
  {
    boost::to_lower(this);
  }

  /**lowercase in-place
   * In-place convert this string to uppercase
   */
  void Unicode::toUpper()
  {
    boost::to_upper(this);
  }


  Utf8CharValidator::Utf8CharValidator() :
    bytesNext_(0),
    completed_(false)
  {
  }

  /**
   * Adds a byte to the current character
   *
   * @return AddByteResult (INVALID when the byte is not valid,
   *         MORE when the byte is valid, but will need more,
   *         DONE 0 when char is complete)
   */
  Utf8CharValidator::AddByteResult Utf8CharValidator::addByte(const char* byte)
  {
    assert( ! completed_ );

    if( bytesNext_ == 0 )
    {
      // this is the first byte
      if( *byte < 0x80 )
      {
        // this is a single byte character, nothing special to do
        char_.bytes_ += *byte;
        completed_ = true;
        return AddByteResult::DONE;
      }
      else
      {
        // this is the start of a multibyte utf8 char
        // number of most significant bits set to 1 tells how many bytes is the
        // character composed in total (first byte is this one)
        for( u8 i = 6; i >= 0; i-- )
          if( *byte & ( 1 << i ) )
            bytesNext_++;
          else
            break;
         if( bytesNext_ < 1 || bytesNext_ > 3 )
          return AddByteResult::INVALID;

        char_.bytes_ += *byte;
        return AddByteResult::MORE;
      }
    } else {
      // waiting for next byte in a multibyte char, it must be 10XXXXXX
      if( ( *byte & 0xC0 ) != 0x80 )
        return AddByteResult::INVALID;

      bytesNext_--;
      char_.bytes_ += *byte;

      if( bytesNext_ == 0 )
      {
        completed_ = true;
        return AddByteResult::DONE;
      }
      return AddByteResult::MORE;
    }
  }

  /**
   * Resets the validator to its initial status
   */
  Utf8Char Utf8CharValidator::getChar()
  {
    assert( completed_ );
    return char_;
  }

}
