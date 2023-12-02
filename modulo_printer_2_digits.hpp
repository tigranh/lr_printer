
#ifndef ML__PRINTERS__NATURAL_PRINTER_BY_2_MODULOS_FROM_END_C_STYLE_HPP
#define ML__PRINTERS__NATURAL_PRINTER_BY_2_MODULOS_FROM_END_C_STYLE_HPP

#include <string>
#include <ostream>
#include <cstdio>
#include <cstring>
#include <cassert>

namespace ml {
namespace printers {


/// This class prints natural numbers, using calculation of their modulo 
/// over square of the base, so printing 2 digits in a single step, thus 
/// almost doubling performance.
/// Digits are written in the buffer from right to left, so there is no 
/// need to reverse them at the end.
template< typename NumberType >
class modulo_printer_2_digits
{
public:
	typedef NumberType number_type;
	typedef modulo_printer_2_digits< NumberType > this_type;

protected:
	/// The base, in which numbers will be printed.
	short _base = -1;

	/// Square of the base.
	/// This will be used for calculations, in order to not compute square 
	/// of the base every time.
	short _base_sqr;

	/// Maximal length of alphabet (maximal value for the base)
	static constexpr short BASE_MAX = 10 + 26;

	/// Digits of the alphabet.
	char _alphabet[ BASE_MAX ];

	/// This string contains 2*|N|*|N| characters, each pair correspoinding 
	/// to pair of alphabet characters, all ordered by alphanumerical.
	/// This is where from pairs of digits will be selected for printing, 
	/// instead of selecting digits individually.
	char _alphabet_sqr[ 2 * BASE_MAX * BASE_MAX ];

	/// Maximal count of digits for any printed number.
	/// Assuming that 64-bit integer is being printed in base 2.
	static constexpr short DIGITS_MAX = 64 + 5;

	/// Temporary buffer, used to store produced digits.
	mutable char _buffer[ DIGITS_MAX ];

protected:
	/// This method does the core work: extracting digits from given number
	/// in pairs and placing them in the internal buffer, but from the end, 
	/// so there will be no need to do any reverse.
	/// Last step might extract only 1 digit (if number of digits turns out 
	/// to be odd).
	/// Returns pointer to where the constructed string starts.
	/// Note, null-terminating character is not appended by this method.
	char* print_to_buffer( number_type x ) const {
		char* ptr = _buffer + DIGITS_MAX - 1;  // Start from the right
		//*--ptr = '\0';  // No need to place null-character here (already present)
		if ( x == 0 ) {  // Check zero case
			*--ptr = _alphabet[ 0 ];
			return ptr;
		}
		while ( x >= _base ) {  // We have 2 digits to print
			short pairs_index = 2 * (x % _base_sqr);
			// Copy 2 characters
			*--ptr = _alphabet_sqr[ pairs_index + 1 ];
			*--ptr = _alphabet_sqr[ pairs_index ];
			// Truncate last 2 digits
			x /= _base_sqr;
		}
		if ( x > 0 ) {  // We have 1 digit remaining
			assert( x < _base );
			*--ptr = _alphabet[ x ];
		}
		return ptr;
	}

public:
	/// Constructor with base specification.
	explicit modulo_printer_2_digits( short base_ = 10 )
		{ set_base( base_ );
		  setup_default_alphabet();
		  setup_buffer_size(); }

	/// Constructor with base & alphabet specification.
	modulo_printer_2_digits( short base_, const std::string& alphabet_ )
		{ set_base( base_ );
		  set_alphabet( alphabet_ );
		  setup_buffer_size(); }

	/// Setter / getter for the base.
	void set_base( short base_ )
		{ _base = base_;
		  _base_sqr = _base * _base;
		  calculate_alphabet_sqr();
		  setup_buffer_size(); }
	short get_base() const
		{ return _base; }

	/// Setter / getter for the alphabet.
	void set_alphabet( const std::string& alphabet_ )
		{ strcpy_s( _alphabet, alphabet_.c_str() );
		  calculate_alphabet_sqr(); }
	const std::string& get_alphabet() const
		{ return _alphabet; }

	/// Sets up default alphabet (at first decimal digits, then lower alpha 
	/// characters).
	void setup_default_alphabet() {
		assert( _base <= BASE_MAX );  // Maximal possible characters, 
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
		// Make further computations
		calculate_alphabet_sqr();
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
		  int length = (int)(_buffer + DIGITS_MAX - 1 - str);
		  fprintf( file, "%s", str );
		  return length; }

	/// Prints integer 'x' into output stream 'ostr'.
	std::ostream& print( const number_type& x, std::ostream& ostr ) const
		{ const char* str = print_to_buffer( x );
		  int length = (int)(_buffer + DIGITS_MAX - 1 - str);
		  return ostr.write( str, length ); }

protected:
	/// Assuming that we already have proper content in '_alphabe', calulates 
	/// and stores content of '_alphabet_sqr'.
	void calculate_alphabet_sqr() {
		// Clear
		int L = 0;  // Current length of '_alphabet_sqr'
		// Fill
		for ( short i = 0; i < _base; ++i ) {
			for ( short j = 0; j < _base; ++j ) {
				_alphabet_sqr[ L++ ] = _alphabet[ i ];
				_alphabet_sqr[ L++ ] = _alphabet[ j ];
			}
		}
		assert( L == 2*_base*_base );
	}

	/// Since digit writing is performed here from right to left, we must be sure 
	/// that our buffer has enough capacity to hold all characters.
	void setup_buffer_size() const {
		// Set the last null-character
		_buffer[ DIGITS_MAX - 1 ] = '\0';
	}
};


}
}

#endif // ML__PRINTERS__NATURAL_PRINTER_BY_2_MODULOS_FROM_END_C_STYLE_HPP
