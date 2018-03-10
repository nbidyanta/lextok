cmake_minimum_required(VERSION 3.10)
project(LexTok VERSION 0.1.0 LANGUAGES CXX DESCRIPTION "A lexical tokenization header-only library")
find_package(Doxygen REQUIRED dot)

option(BUILD_LEXTOK_DOCS "Generate HTML documentation for the Lexical Tokenizer (requires Doxygen and dot)" OFF)

if (BUILD_LEXTOK_DOCS)
	if (NOT DOXYGEN_FOUND)
		message(FATAL_ERROR "Doxygen and dot are needed to build the documentation.")
	endif()
	set(DOXYGEN_TAB_SIZE 2)
	set(DOXYGEN_WARN_NO_PARAMDOC YES)
	set(DOXYGEN_WARN_AS_ERROR YES)
	set(DOXYGEN_SOURCE_BROWSER YES)
	set(DOXYGEN_HTML_DYNAMIC_SECTIONS YES)
	set(DOXYGEN_DOCSET_FEEDNAME "Lexical Tokenizer documentation")
	set(DOXYGEN_DOCSET_BUNDLE_ID org.nbidyanta.lextok)
	set(DOXYGEN_DOCSET_PUBLISHER_ID org.nbidyanta.doxygen)
	set(DOXYGEN_DOCSET_PUBLISHER_NAME Nilangshu)
	set(DOXYGEN_GENERATE_TREEVIEW YES)
	doxygen_add_docs(lextok-docs inc/ COMMENT "Generate Doxygen HTML documentation for the Lexical Tokenizer")
endif()

set(CMAKE_CXX_STANDARD 17)

add_executable(lextok ${CMAKE_SOURCE_DIR}/src/main.cpp)

target_include_directories(lextok PRIVATE ${CMAKE_SOURCE_DIR}/inc)

set(CMAKE_CXX_FLAGS "-Wall -Werror -Wconversion -fno-rtti -fno-exceptions")
