/**
 * font-awesome (c) creativemarket.com
 * Author: Josh Farr <josh@creativemarket.com>
 */

#include "../version.h"
#include "Color.h"
#include "Font.h"
#include "FontAwesomeException.h"
#include "FilePointer.h"

#include <iostream>
#include <string>
#include <clocale>
#include <sstream>

#include <boost/program_options.hpp>


int main (int argc, char * argv[]) {
	int fontSize;
	std::string filename;
	bool verbose = false;
	bool useCodePoints = false;
	std::string hexColor;
	bool fixMissingGlyph = false;
	bool gracefulEmptyOutput = false;
	bool debug = false;
	Color textColor;

	// setup option parser
	boost::program_options::options_description optsDesc("Allowed options");
	optsDesc.add_options()
		("verbose,v", "more verbose output")
		("help,h", "show this help")
		("version", "show version")
		("gracefulempty", "output 1x1 empty png when image has 0 area")
		("fix0glyph", "draw box if glyph 0 (missing glyph) has no substance")
		("color", boost::program_options::value<std::string>(&hexColor), "text color (in RGBA hex)")
		("filename,f", boost::program_options::value<std::string>(&filename), "font filename")
		("fontsize,s", boost::program_options::value<int>(&fontSize)->default_value(32), "font size in points")
		("codepoints", "text specified as unicode code points")
		("debug", "only query the required canvas size to render text")
	;

	// parse options
	boost::program_options::variables_map argMap;
	boost::program_options::store(boost::program_options::parse_command_line(argc, argv, optsDesc), argMap);
	boost::program_options::notify(argMap);

	// process options
	if (argMap.count("help")) {
		std::cout << optsDesc << std::endl;
		return EXIT_SUCCESS;
	}
	if (argMap.count("version")) {
		std::cout << "font-awesome version " << FONTAWESOME_VERSION << std::endl;
		return EXIT_SUCCESS;
	}

	if (!argMap.count("filename")) {
		std::cerr << "no file (use -f flag)" << std::endl;
		return EXIT_FAILURE;
	}

	// flags
	if (argMap.count("verbose")) {
		verbose = true;
	}
	if (argMap.count("fix0glyph")) {
		fixMissingGlyph = true;
	}
	if (argMap.count("gracefulempty")) {
		gracefulEmptyOutput = true;
	}
	if (argMap.count("debug")) {
		debug = true;
	}

	setlocale(LC_ALL, "");

	if (argMap.count("color")) {
		if (!textColor.validateHex(hexColor)) {
			std::cerr << "Invalid --color: " << hexColor << std::endl;
			return EXIT_FAILURE;
		}
		textColor.fromHex(hexColor);
	}

	// make sure stdin is in binary read mode
	FilePointer input(FilePointer::DIRECTION_IN);
	input.mode("rb");

	std::wstring text;
	if (useCodePoints) {
		if (verbose) {
			std::cout << "using codepoints" << std::endl;
		}
		std::string characters;
		while (std::cin.good()) {
			std::cin >> characters;
			wchar_t wc = atoi(characters.c_str());
			text.push_back(wc);
		}
	}
	else {
		std::wstring wstr;
		while (std::wcin.good()) {
			std::getline(std::wcin, wstr);
			text.append(wstr);
		}
	}
/*
	wchar_t character;
	do {
		if (useCodePoints) {
			// Take input as series of long integars
			if (fscanf(stdin, "%s", buffer) != 1) {
				break;
			}
			character = atoi(buffer);
		}
		else{
			// Take input from stdin as in (locale-aware)
			character = fgetwc(stdin);
		}
		if (character == 10){ // line feed
			continue;
		}
		if (character == WEOF){
			break;
		}
		text.push_back(character);
		text[text_length] = character;
		if (verbose) {
			// fprintf(stderr, "%d: 0x%X (%lc)\n", text_length, (wint_t)c, (wint_t)c);
		}
	} while (character != WEOF);
*/

	try {
		Font font(filename, fontSize);
	}
	catch (FontAwesomeException const& ex) {
		std::cerr << "Exception: " << ex.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}