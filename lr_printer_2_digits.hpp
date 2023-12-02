
#ifndef ML__PRINTERS__NATURAL_PRINTER_LEFT_TO_RIGHT_2_DIGITS_C_STYLE_HPP
#define ML__PRINTERS__NATURAL_PRINTER_LEFT_TO_RIGHT_2_DIGITS_C_STYLE_HPP

#include <string>
#include <ostream>
#include <cstring>
#include <cstdio>
#include <cassert>

namespace ml {
namespace printers {


/// This printer outputs natural numbers by obtaining digits from left
/// to right, in pairs. For that it simply perofrms integer division over, 
/// let's say: 1'000'000, then 10'000, then 100, and so on.
/// In order to get what remains for next iteration, instead of calculating 
/// remainder it does subtraction.
template< typename NumberType >
class lr_printer_2_digits
{
public:
	typedef NumberType number_type;
	typedef lr_printer_2_digits< NumberType > this_type;

protected:
	/// The base, by which numbers will be printed.
	short _base = -1;

	/// Square of the base.
	/// This will be used for calculations, in order to not compute square 
	/// of the base every time.
	short _base_sqr;

	/// Maximal value of the base for numbers.
	static constexpr short BASE_MAX = 10 + 6 + 1;

	/// All the digits, used to print given numbers.
	char _alphabet[ BASE_MAX ];

	/// This string contains 2*|N|*|N| characters, each pair corresponding 
	/// to pair of alphabet characters, all ordered by alphanumerical.
	/// This is where from pairs of digits will be selected for printing, 
	/// instead of selecting digits individually.
	char _alphabet_sqr[ BASE_MAX * BASE_MAX * 2 ];

	/// Some integer types are bounded while some others are not.
	/// This class calculates (amoung other) all powers of 'base', which fit 
	/// in "number_type". So for unbounded integers this calculation might 
	/// span infinitely. Also for the bounded integers perhapes there will 
	/// be no need to print large numbers.
	/// Considering that facts, here we calculate necessary powers (an other
	/// data) incrementally, when that is needed to print currently received
	/// number.
	/// This member variable indicates if we have already calculated all 
	/// powers of '_base', which will fit in 'number_type'. For unbounded 
	/// integer types, it will always remain 'false'.
	mutable bool _reached_max_power = false;

	/// Maximal number of digits for printed number.
	/// Assuming printing 64-bit number in base 2, with a bit more digits.
	static constexpr short DIGITS_MAX = 64 + 5;

	/// How many values of '_powers[]' array are currently calculated.
	mutable short _powers_length;

	/// Powers of the base, which will be used when calculating remainder 
	/// of the division (without actually calling remainder).
	mutable number_type _powers[ DIGITS_MAX ];

	/// The buffer to hold the digits, before sending them to output 
	/// stream or output file (in order to not send them digit by digit 
	/// there).
	mutable char _buffer[ DIGITS_MAX ];

protected:
	/// This is the base printing routine.
	template< typename OutIt >
	OutIt print_to_out_iter( number_type num, OutIt out ) const {
		// Check zero case
		if ( num == 0 ) {
			*(out++) = _alphabet[ 0 ];
			return out;
		}
		// Start with the most significant digit
		number_type* power_ptr = get_max_power_ptr( num );
		number_type* power_ptr_lim = _powers + 1;
		short digits_2;
		for ( ; power_ptr >= power_ptr_lim; power_ptr -= 2 ) {
			// Find 2 left-most digits
			digits_2 = (short)(num / *power_ptr);
			assert( 0 <= digits_2 );
			assert( digits_2 < _base_sqr );
			// Print them
			*(out++) = _alphabet_sqr[ digits_2 * 2 ];
			*(out++) = _alphabet_sqr[ (digits_2 * 2) + 1 ];
			// Advance to remaining part
			num -= digits_2 * (*power_ptr);
		}
		// Print last 1 or 2 digit(s)
		assert( power_ptr == _powers || power_ptr == _powers - 1 );
		if ( power_ptr == _powers ) {
			// 2 digits remain
			assert( 0 <= num );
			assert( num < _base_sqr );
			// Print them
			*(out++) = _alphabet_sqr[ num * 2 ];
			*(out++) = _alphabet_sqr[ (num * 2) + 1 ];
		}
		else if ( power_ptr == _powers - 1 ) {
			// 1 digit remains
			assert( 0 <= num );
			assert( num < _base );
			// Print it
			*(out++) = _alphabet[ num ];
		}
		return out;
	}

public:

	/// Constructor with base specification.
	explicit lr_printer_2_digits( short base_ = 10 )
		{ set_base( base_ );
		  setup_default_alphabet(); }

	/// Constructor with base & alphabet specification.
	lr_printer_2_digits( short base_, const std::string& alphabet_ )
		{ set_base( base_ );
		  set_alphabet( alphabet_ ); }

	/// Setter / getter for the base.
	void set_base( short base_ )
		{ _base = base_;
		  _base_sqr = base_ * base_;
		  calculate_alphabet_sqr();
		  init_helper_data(); }
	short get_base() const
		{ return _base; }

	/// Accessors to internal details
	const auto& get_powers() const
		{ return _powers; }

	/// Setter / getter for the alphabet.
	void set_alphabet( const std::string& alphabet_ )
		{ strcpy_s( _alphabet, alphabet_.c_str() );
		  calculate_alphabet_sqr(); }
	const auto& get_alphabet() const
		{ return _alphabet; }

	/// Sets up default alphabet (at first decimal digits, then lower alpha 
	/// characters).
	void setup_default_alphabet() {
		assert( _base <= 10 + 6 );  // Maximal possible characters, 
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
		{ char* buf_end = print_to_out_iter( x, buf );
		  *buf_end = '\0';
		  return (int)(buf_end - buf); }

	/// Prints integer 'x' into specified file, without appending any other 
	/// character.
	/// Returns number of digits printed.
	int print( const number_type& x, FILE* file ) const
		{ char* buf_end = print_to_out_iter( x, _buffer );
		  *buf_end = '\0';
		  fprintf( file, "%s", _buffer );
		  return (int)(buf_end - _buffer); }

	/// Prints integer 'x' into output stream 'ostr'.
	std::ostream& print( const number_type& x, std::ostream& ostr ) const
		{ char* buf_end = print_to_out_iter( x, _buffer );
		  return ostr.write( _buffer, (buf_end - _buffer) ); }

protected:
	/// Assuming that we already have proper content in '_alphabet', calulates 
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

	/// Initialized starting state of helper data, so it can be already 
	/// used for printing (not so large) numbers.
	void init_helper_data() const {
		const int DATA_INIT_LENGTH = 4;
		// Calculate first 'row's
		//   powers
		_powers[ 0 ] = 1;
		_powers_length = 1;
		_reached_max_power = false;
		// Append next rows
		for ( int i = 1; i < DATA_INIT_LENGTH; ++i )
			append_helper_data();
	}

	/// Appends one more row of helper data.
	/// Generally this method will be called when the number which is 
	/// received for printing is larger than values of current helper data.
	/// Returns if successfully appened. Failer might happen only on overflow.
	bool append_helper_data() const {
		// Append powers
		const number_type new_power = _powers[ _powers_length - 1 ] * _base;
		if ( new_power / _base != _powers[ _powers_length - 1 ] )
			return false;
		_powers[ _powers_length ] = new_power;
		++_powers_length;
		return true;
	}

	/// Returns pointer to the maximal power, starting from which 
	/// the helper data should be addressed, in order to print given number.
	/// Generally that will be equal to count of digits of the given 
	/// number minus 2.
	number_type* get_max_power_ptr( const number_type& num ) const {
		if ( _reached_max_power ) {
			if ( _powers[ _powers_length - 2 ] <= num ) {
				// 'num' is either a max-digit value, or 1-less digit value
				if ( _powers[ _powers_length - 1 ] <= num )
					return _powers + _powers_length - 2;  // 'num' is a max-digit number
				else
					return _powers + _powers_length - 3;  // 'num' is 1-digit less
			}
		}
		else {
			// Check that enough helper data is generated
			while ( _powers[ _powers_length - 2 ] <= num ) {
				if ( ! append_helper_data() ) {
					_reached_max_power = true;
					if ( _powers[ _powers_length - 1 ] <= num )
						return _powers + _powers_length - 2;  // 'num' is a max-digit number
					else
						return _powers + _powers_length - 3;  // 'num' is 1-digit less
				}
			}
		}
		// Find proper power
		number_type* power_ptr = _powers;
		//   go with double steps
		while ( *power_ptr <= num )
			power_ptr += 2;
		//   then possibly one more step
		--power_ptr;
		if ( *power_ptr <= num )
			++power_ptr;
		assert( *power_ptr > num );
		power_ptr -= 2;  // two less
		return power_ptr;
	}

};


}
}

#endif // ML__PRINTERS__NATURAL_PRINTER_LEFT_TO_RIGHT_2_DIGITS_C_STYLE_HPP
