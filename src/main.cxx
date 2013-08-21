/**
 * font-awesome (c) creativemarket.com
 * Author: Josh Farr <josh@creativemarket.com>
 */

#include "../version.h"
#include "Color.h"
#include "Font.h"
#include "FontAwesomeException.h"
#include "FilePointer.h"
#include "Image.h"
#include "PNGWriter.h"
#include "Renderer.h"

#include <iostream>
#include <string>
#include <clocale>
#include <sstream>

#include <boost/program_options.hpp>


int main (int argc, char * argv[]) {
	int fontSize;
	std::string filename;
	bool verbose 		 	 = false;
	bool useCodePoints 		 = false;
	std::string hexColor;
	std::string hexEmptyColor;
	bool fixMissingGlyph 	 = false;
	bool gracefulEmptyOutput = false;
	bool debug 				 = false;
	Color textColor;
	Color emptyColor;
	std::wstring text;
	std::string outfile;
	bool writeFile 			 = false;
	bool showMetrics		 = false;

	// setup option parser
	boost::program_options::options_description optsDesc("Allowed options");
	optsDesc.add_options()
		("verbose,v", "more verbose output")
		("help,h", "show this help")
		("version", "show version")
		("gracefulempty", "output 1x1 empty png when image has 0 area")
		("fix0glyph", "draw box if glyph 0 (missing glyph) has no substance")
		("color", boost::program_options::value<std::string>(&hexColor), "text color (in RGBA hex)")
		("emptycolor", boost::program_options::value<std::string>(&hexEmptyColor), "empty glyph color (in RGBA hex)")
		("filename,f", boost::program_options::value<std::string>(&filename), "font filename")
		("fontsize,s", boost::program_options::value<int>(&fontSize)->default_value(32), "font size in points")
		("codepoints", "text specified as unicode code points")
		("debug", "only query the required canvas size to render text")
		("metrics", "output font metrics instead of a rendered image")
		("outfile", boost::program_options::value<std::string>(&outfile), "save image to filename")
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

	if (argMap.count("metrics")) {
		showMetrics = true;
	}

	if (!argMap.count("filename")) {
		std::cerr << "no file (use -f flag)" << std::endl;
		return EXIT_FAILURE;
	}
	if (argMap.count("outfile")) {
		writeFile = true;
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
	if (argMap.count("codepoints")) {
		useCodePoints = true;
	}

	setlocale(LC_ALL, "");

	// set text color
	if (argMap.count("color")) {
		if (!textColor.validateHex(hexColor)) {
			std::cerr << "Invalid --color: " << hexColor << std::endl;
			return EXIT_FAILURE;
		}
		textColor.fromHex(hexColor);
	}
	if (argMap.count("emptycolor")) {
		if (!emptyColor.validateHex(hexEmptyColor)) {
			std::cerr << "Invalid --emptycolor: " << hexEmptyColor << std::endl;
			return EXIT_FAILURE;
		}
		emptyColor.fromHex(hexEmptyColor);
	}

	// make sure stdin is in binary read mode
	FilePointer input(FilePointer::DIRECTION_IN);
	input.mode("rb");

	// get text from stdin
	if (useCodePoints) {
		if (debug) {
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
	if (debug) {
		std::wcout << "Rendering text [" << text << "]" << std::endl;
	}

	// create font
	try {
		Font font(filename, fontSize);

		if (fixMissingGlyph) {
			if (font.missingExists()) {
				if (verbose) {
					std::cerr << "Missing glyph exists. Unsetting flag." << std::endl;
				}
				fixMissingGlyph = false;
			}
			else {
				if (verbose) {
					std::cerr << "Missing glyph has no substance. Will hack in box." << std::endl;
				}
			}
		}

		if (showMetrics) {
			Font::TextInfo info = font.metrics(text);
			std::cout << "Text Length: " << info.length_ << std::endl;
			std::cout << "Missing Empty Glyph: ";
			if (info.missingEmpty_) {
				std::cout << "Yes";
			}
			else {
				std::cout << "No";
			}
			std::cout << std::endl;
			std::cout << "Renderable Glyphs: " << info.hitCount_ << std::endl;
			std::cout << "Non-renderable Glyphs: " << info.emptyCount_ << std::endl;
			std::cout << "Character Map:";
			std::wcout << info.charmap_ << std::endl;
			return EXIT_SUCCESS;
		}

		boost::shared_ptr<Image> image;
		Renderer renderer(debug, gracefulEmptyOutput, fixMissingGlyph);

		image = renderer.render(font, textColor, text, emptyColor);
		if (writeFile && debug) {
			std::cout << "Saving image to file [" << outfile << "]" << std::endl;
		}
		FilePointer file(FilePointer::DIRECTION_OUT, outfile);
		PNGWriter writer;
		if (!writer.write(file, image)) {
			std::cout << "Error writing image" << std::endl;
		}
	}
	catch (FontAwesomeException const& ex) {
		std::cerr << "Exception: " << ex.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

