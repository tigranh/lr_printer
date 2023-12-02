
#ifndef ML__PRINTERS__NATURAL_PRINTER_LEFT_TO_RIGHT_C_STYLE_HPP
#define ML__PRINTERS__NATURAL_PRINTER_LEFT_TO_RIGHT_C_STYLE_HPP

#include <string>
#include <ostream>
#include <cstdio>
#include <cstring>
#include <cassert>

namespace ml {
namespace printers {


/// This printer outputs natural numbers by obtaining digits from left
/// to right. For that it simply perofrms integer division over, let's say: 
/// 1'000'000, then 100'000, then 10'000, and so on.
/// In order to get what remains for next iteration, instead of calculating 
/// remainder it does subtraction.
template< typename NumberType >
class lr_printer
{
public:
	typedef NumberType number_type;
	typedef lr_printer< NumberType > this_type;

protected:
	/// The maximal value of base, which can be used during print
	static constexpr short MAX_BASE = 10 + 26;

	/// Maximal number of digits: assuming base=2, to have a bit more
	/// than 64-bit maximal value.
	static constexpr short MAX_DIGITS = 70;

	/// The base, by which numbers will be printed.
	short _base = -1;

	/// All the digits, used to print given numbers.
	char _alphabet[ MAX_BASE ];

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

	/// Actual length of '_powers[]' array.
	mutable short _powers_length = 0;

	/// Powers of the base, which will be used when calculating remainder 
	/// of the division (without actually calling remainder).
	mutable number_type _powers[ MAX_DIGITS ];

	/// The buffer to hold the digits, before sending them to output 
	/// stream or output file (in order to not send them digit by digit 
	/// there).
	mutable char _buffer[ MAX_DIGITS ];

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
		short digit;
		for ( ; power_ptr >= _powers; --power_ptr ) {
			// Find left-most digit
			digit = (short)(num / *power_ptr);
			assert( 0 <= digit );
			assert( digit < _base );
			// Print it
			*(out++) = _alphabet[ digit ];
			// Advance to remaining part
			num -= digit * (*power_ptr);
		}
		assert( num == 0 );  // Check that number is exhausted
		return out;
	}

public:

	/// Constructor with base specification.
	explicit lr_printer( short base_ = 10 )
		{ set_base( base_ );
		  setup_default_alphabet(); }

	/// Constructor with base & alphabet specification.
	lr_printer( short base_, const std::string& alphabet_ )
		{ set_base( base_ );
		  strcpy_s( _alphabet, alphabet_.c_str() ); }

	/// Setter / getter for the base.
	void set_base( short base_ )
		{ _base = base_;
		  init_helper_data(); }
	short get_base() const
		{ return _base; }

	/// Accessors to internal details
	const auto& get_powers() const
		{ return _powers; }

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
		short length = 0;
		// Attach digits
		for ( char ch = '0'; ch <= '9' 
				&& length < _base; 
				++ch, ++length )
			_alphabet[ length ] = ch;
		// Attach letters
		for ( char ch = 'a'; ch <= 'z'
				&& length < _base; 
				++ch, ++length )
			_alphabet[ length ] = ch;
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
		  return ostr.write( _buffer, (int)(buf_end - _buffer) ); }

protected:
	/// Initialized starting state of helper data, so it can be already 
	/// used for printing (not so large) numbers.
	void init_helper_data() const {
		const int DATA_INIT_LENGTH = 4;
		// Calculate first 'row's
		_powers_length = 1;
		_powers[ 0 ] = 1;
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
			return false;  // Overflow. No more powers can be appended.
		_powers[ _powers_length ] = new_power;
		++_powers_length;
		return true;
	}

	/// Returns pointer to the maximal power, starting from which 
	/// the helper data should be addressed, in order to print given number.
	/// Generally that will be equal to count of digits of the given 
	/// number minus 1.
	number_type* get_max_power_ptr( const number_type& num ) const {
		if ( _reached_max_power ) {
			if ( _powers[ _powers_length - 1 ] <= num )  // 'num' is a max-digit number
				return _powers + _powers_length - 1;
		}
		else {
			// Check that enough helper data is generated
			while ( _powers[ _powers_length - 1 ] <= num ) {
				if ( ! append_helper_data() ) {
					_reached_max_power = true;
					return _powers + _powers_length - 1;
				}
			}
		}
		// Find proper power incrementally
		number_type* power_ptr = _powers;
		while ( *power_ptr <= num )
			++power_ptr;
		--power_ptr;  // one less
		return power_ptr;
	}

};


}
}

#endif // ML__PRINTERS__NATURAL_PRINTER_LEFT_TO_RIGHT_C_STYLE_HPP
