
cmake_minimum_required (VERSION 3.23)

project (lr_printers_test)

set(CMAKE_CXX_STANDARD 14)

set (HEADER_FILES
	lr_printer.hpp
	lr_printer_2_digits.hpp 
	modulo_printer.hpp 
	modulo_printer_2_digits.hpp 
	)
	
set (SOURCE_FILES
	main.cpp
	)
	
set (ML_DIR "../../libraries")

add_executable ( lr_printers_test ${HEADER_FILES} ${SOURCE_FILES} )
target_include_directories( lr_printers_test PRIVATE ${ML_DIR} )
