
#ifndef ML__PRINTERS__NATURAL_PRINTER_BY_MODULO_FROM_END_C_STYLE_HPP
#define ML__PRINTERS__NATURAL_PRINTER_BY_MODULO_FROM_END_C_STYLE_HPP

#include <string>
#include <ostream>
#include <cstdio>
#include <cstring>
#include <cassert>

namespace ml {
namespace printers {


/// This class prints natural numbers, using calculation of their modulo 
/// over the base, and truncation of division over the base.
/// Digits are written in the buffer from right to left, so there is no 
/// need to reverse them at the end.
template< typename NumberType >
class modulo_printer
{
public:
	typedef NumberType number_type;
	typedef modulo_printer< NumberType > this_type;

protected:
	/// The base, in which numbers will be printed.
	short _base = -1;

	/// Maximal value of '_base' for this printer.
	static constexpr short BASE_MAX = 10 + 26;

	/// Digits of the alphabet.
	char _alphabet[ BASE_MAX ];

	/// Maximal count of digits that a number can have.
	/// Assuming 64-bit integer in base 2.
	static constexpr unsigned short DIGITS_MAX = 64 + 7;

	/// Temporary buffer, used to store produced digits in reverse order.
	mutable char _buffer[ DIGITS_MAX ];

protected:
	/// This method does the core work: extracting digits from given number
	/// and placing them in the internal buffer, but from the end, so there 
	/// will be no need to do any reverse.
	/// Returns pointer to where the constructed string starts.
	/// Note, null-terminating character is not appended by this method.
	char* print_to_buffer( number_type x ) const {
		char* ptr = _buffer + DIGITS_MAX - 1;  // Start from the right
		//*--ptr = '\0';  // No need to place null-character here
		if ( x == 0 ) {  // Check zero case
			*--ptr = _alphabet[ 0 ];
			return ptr;
		}
		do {  // Regular case
			*--ptr = _alphabet[ x % _base ];
			x /= _base;
		} while ( x != 0 );
		return ptr;
	}

public:
	/// Constructor with base specification.
	explicit modulo_printer( short base_ = 10 )
		: _base( base_ )
		{ setup_default_alphabet();
		  prepare_buffer(); }

	/// Constructor with base & alphabet specification.
	modulo_printer( short base_, const std::string& alphabet_ )
		: _base( base_ ), 
		{ strcpy_s( _alphabet, alphabet_.c_str() );
		  prepare_buffer(); }

	/// Setter / getter for the base.
	void set_base( short base_ )
		{ _base = base_;
		  prepare_buffer(); }
	short get_base() const
		{ return _base; }

	/// Setter / getter for the alphabet.
	void set_alphabet( const std::string& alphabet_ )
		{ strcpy_s( _alphabet, alphabet_.c_str() ); }
	const auto& get_alphabet() const
		{ return _alphabet; }

	/// Sets up default alphabet (at first decimal digits, then lower alpha 
	/// characters).
	void setup_default_alphabet() {
		assert( _base <= 10 + 26 );  // Maximal possible characters, 
		                             // that can be used
		int L = 0;  // Current length of '_alphabet'
		// Attach digits
		for ( char ch = '0'; ch <= '9' 
				&& L < _base; 
				++ch )
			_alphabet[ L++ ] = ch;
		// Attach letters
		for ( char ch = 'a'; ch <= 'z'
				&& L < _base;
				++ch )
			_alphabet[ L++ ] = ch;
		assert( L == _base );
	}

	/// Prints integer 'x' into buffer 'buf', and appends null-character.
	/// Returns number of digits printed (null-character not included).
	int print( const number_type& x, char* buf ) const
		{ const char* str = print_to_buffer( x );
		  strcpy( buf, str );
		  return (int)(_buffer + DIGITS_MAX - 1 - str); }

	/// Prints integer 'x' into specified file, without appending any other 
	/// character.
	/// Returns number of digits printed.
	int print( const number_type& x, FILE* file ) const
		{ const char* str = print_to_buffer( x );
		  fprintf( file, "%s", str );
		  return (int)(_buffer + DIGITS_MAX - 1 - str); }

	/// Prints integer 'x' into output stream 'ostr'.
	std::ostream& print( const number_type& x, std::ostream& ostr ) const
		{ const char* str = print_to_buffer( x );
		  int length = (int)(_buffer + DIGITS_MAX - 1 - str);
		  return ostr.write( str, length ); }

protected:
	/// Since digit writing is performed here from right to left, we must be sure 
	/// that our buffer is null-terminated.
	void prepare_buffer() const {
		_buffer[ DIGITS_MAX - 1 ] = '\0';
	}
};


}
}

#endif // ML__PRINTERS__NATURAL_PRINTER_BY_MODULO_FROM_END_C_STYLE_HPP
