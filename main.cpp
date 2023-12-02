
#include <iostream>
#include <string>
#include <sstream>
#include <chrono>
#include <cassert>

#include "modulo_printer.hpp"
#include "modulo_printer_2_digits.hpp"
#include "lr_printer.hpp"
#include "lr_printer_2_digits.hpp"


/// Runs general tests for provided printer of natural numbers.
template< typename PrinterType >
void test_printer( PrinterType& p )
{
	using namespace std::string_literals;

	char buf[ 25 ];

	p.print( 43, buf );
	assert( buf == "43"s );

	p.print( 5'607, buf );
	assert( buf == "5607"s );

	p.print( 4, buf );
	assert( buf == "4"s );

	p.print( 2'147'483'647, buf );
	assert( buf == "2147483647"s );

	p.set_base( 8 );  // Printing in "base=8"

	p.print( 255, buf );
	assert( buf == "377"s );

	p.print( 10, buf );
	assert( buf == "12"s );

	p.set_base( 16 );  // Printing in "base=16"
	p.setup_default_alphabet();

	p.print( 512, buf );
	assert( buf == "200"s );

	p.print( 77, buf );
	assert( buf == "4d"s );

	p.set_base( 10 );  // Returning to base 10

	std::ostringstream ostr;  // Printing to output stream
	p.print( 123, ostr );
	ostr << " ";
	p.print( 0, ostr );
	ostr << " ";
	p.print( 10'000, ostr );
	assert( ostr.str() == "123 0 10000"s );
}


/// The clock-type, used to measure performance.
typedef std::chrono::high_resolution_clock clock_type;

// Buffer, large enough to fit 64-bit numbers, when printed in base=2.
char buf[ 64 + 7 ];

/// Invokes provided printer on printing all numbers in range 
/// [start_num, finish_num], into an internal buffer. Measures and returns 
/// time required for that.
template< typename PrinterType, typename NumberType >
clock_type::duration run_printer( PrinterType& p, 
		NumberType start_num, NumberType finish_num )
{
	clock_type::time_point start_time = clock_type::now();
	// Printing
	for ( NumberType num = start_num; num <= finish_num; ++num )
		p.print( num, buf );
	clock_type::duration dur = clock_type::now() - start_time;
	std::cout << std::chrono::duration_cast< std::chrono::milliseconds >( dur ).count()
			<< " msc" << std::endl;
	return dur;
}


int main( int argc, char* argv[] )
{
	// Testing "modulo printer"
	std::cout << "Modulo printer:" << std::endl;

	{
		std::cout << "\t Testing 'modulo_printer< int >' ..." << std::endl;
		ml::printers::modulo_printer< int > printer;
		test_printer( printer );
	}

	{
		std::cout << "\t Testing 'modulo_printer_2_digits< int >' ..." << std::endl;
		ml::printers::modulo_printer_2_digits< int > printer;
		test_printer( printer );
	}

	// Testing "LR-printer"
	std::cout << "LR-printer:" << std::endl;

	{
		std::cout << "\t Testing 'lr_printer< int >' ..." << std::endl;
		ml::printers::lr_printer< int > printer;
		test_printer( printer );
	}

	{
		std::cout << "\t Testing 'lr_printer< long long >' ..." << std::endl;
		ml::printers::lr_printer< long long > printer;
		test_printer( printer );
	}

	{
		std::cout << "\t Testing 'lr_printer_2_digits< int >' ..." << std::endl;
		ml::printers::lr_printer_2_digits< int > printer;
		test_printer( printer );
	}

	{
		std::cout << "\t Testing 'lr_printer_2_digits< long long >' ..." << std::endl;
		ml::printers::lr_printer_2_digits< long long > printer;
		test_printer( printer );
	}

	{
		// Compare printers' performance
		typedef int number_type;
				// Run on 32-bit integers
		const number_type start_num = 10'000'000, finish_num = 49'000'000;
				// 8-digit numbers
		std::cout << "Running the printers on numbers in ["
				<< start_num << ", " << finish_num << "], 32-bit, with base=10:" << std::endl;

		{
			std::cout << "\t modulo_printer: ";
			ml::printers::modulo_printer< number_type > printer;
			run_printer( printer, start_num, finish_num );
		}
		{
			std::cout << "\t modulo_printer_2_digits: ";
			ml::printers::modulo_printer_2_digits< number_type > printer;
			run_printer( printer, start_num, finish_num );
		}
		{
			std::cout << "\t lr_printer: ";
			ml::printers::lr_printer< number_type > printer;
			run_printer( printer, start_num, finish_num );
		}
		{
			std::cout << "\t lr_printer_2_digits: ";
			ml::printers::lr_printer_2_digits< number_type > printer;
			run_printer( printer, start_num, finish_num );
		}
	}

	{
		// Compare printers' performance
		typedef long long number_type;
				// Run on 64-bit integers
		const number_type 
				start_num = 52'109'000'000'000'000LL, 
				finish_num = 52'109'000'049'000'000LL;
				// 17-digit numbers
		std::cout << "Running the printers on numbers in ["
				<< start_num << ", " << finish_num << "], 64-bit, with base=10:" << std::endl;

		{
			std::cout << "\t modulo_printer: ";
			ml::printers::modulo_printer< number_type > printer;
			run_printer( printer, start_num, finish_num );
		}
		{
			std::cout << "\t modulo_printer_2_digits: ";
			ml::printers::modulo_printer_2_digits< number_type > printer;
			run_printer( printer, start_num, finish_num );
		}
		{
			std::cout << "\t lr_printer: ";
			ml::printers::lr_printer< number_type > printer;
			run_printer( printer, start_num, finish_num );
		}
		{
			std::cout << "\t lr_printer_2_digits: ";
			ml::printers::lr_printer_2_digits< number_type > printer;
			run_printer( printer, start_num, finish_num );
		}
	}

	std::cout << "Last converted number (to prevent unnecessary optimizations): " 
			<< buf << std::endl;

	return 0;
}

