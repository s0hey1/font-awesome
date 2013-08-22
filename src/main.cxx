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
	bool showGlyphInfo 		 = false;
	bool showEmptyInfo		 = false;
	std::string glyphChar;

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
		("glyphinfo", boost::program_options::value<std::string>(&glyphChar), "output information about a specific glyph index")
		("showempty", "display empty glyph information")
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
		std::wcout << "font-awesome version " << FONTAWESOME_VERSION << std::endl;
		return EXIT_SUCCESS;
	}

	if (argMap.count("metrics")) {
		showMetrics = true;
	}

	if (argMap.count("glyphinfo")) {
		showGlyphInfo = true;
	}
	if (argMap.count("showempty")) {
		showEmptyInfo = true;
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
	if (!showGlyphInfo) {
		if (useCodePoints) {
			if (debug) {
				std::wcout << "using codepoints" << std::endl;
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
			std::wcout << "Text Input: " << text << std::endl;
			std::wcout << "Font Name: " << info.faceInfo_.name_.c_str() << std::endl;
			std::wcout << "Font Family: " << info.faceInfo_.family_.c_str() << std::endl;
			std::wcout << "Font Style: " << info.faceInfo_.style_.c_str() << std::endl;
			std::wcout << "Available Glyphs: " << info.faceInfo_.glyphCount_ << std::endl;
			std::wcout << "Available Sizes: " << info.faceInfo_.sizes_ << std::endl;
			std::wcout << "Bold: ";

			if (info.faceInfo_.bold_) {
				std::wcout << "Yes";
			}
			else {
				std::wcout << "No";
			}
			std::cout << std::endl;

			std::wcout << "Italic: ";
			if (info.faceInfo_.italic_) {
				std::wcout << "Yes";
			}
			else {
				std::wcout << "No";
			}
			std::wcout << std::endl;

			std::wcout << "Kerning Available: ";
			if (info.faceInfo_.kerning_) {
				std::wcout << "Yes";
			}
			else {
				std::wcout << "No";
			}
			std::wcout << std::endl;

			std::wcout << "Text Length: " << info.length_ << std::endl;
			std::wcout << "Missing Empty Glyph: ";

			if (info.missingEmpty_) {
				std::wcout << "Yes";
			}
			else {
				std::wcout << "No";
			}
			std::wcout << std::endl;

			std::wcout << "Renderable Glyphs: " << info.hitCount_ << std::endl;
			std::wcout << "Non-renderable Glyphs: " << info.emptyCount_ << std::endl;
			std::wcout << "Character Map: " << info.charmap_ << std::endl;
			return EXIT_SUCCESS;
		}

		if (showGlyphInfo) {
			wchar_t gc;
			if (useCodePoints) {
				gc = atoi(glyphChar.c_str());
			}
			else {
				gc = glyphChar[0];
			}
			Font::Vector pen(0, 0);
			Font::Glyph glyph;
			if (showEmptyInfo) {
				glyph = font.emptyGlyph();
			}
			else {
				glyph = font.glyph(gc, pen);
			}
			std::wcout << "Glyph Character: " << glyphChar.c_str() << std::endl;
			std::wcout << "Glyph Codepoint: " << gc << std::endl;
			std::wcout << "Index: " << glyph.index_ << std::endl;
			std::wcout << "Advance: [" << glyph.advance_.first << ", " << glyph.advance_.second << "]" << std::endl;
			std::wcout << "Size: [" << glyph.size_.first << "x" << glyph.size_.second << "]" << std::endl;
			std::wcout << "Position: [" << glyph.position_.first << ", " << glyph.position_.second << "]" << std::endl;
			std::wcout << "Empty: ";
			if (glyph.empty_) {
				std::wcout << "Yes";
			}
			else {
				std::wcout << "No";
			}
			std::wcout << std::endl;
			return EXIT_SUCCESS;
		}

		boost::shared_ptr<Image> image;
		Renderer renderer(debug, gracefulEmptyOutput, fixMissingGlyph);

		image = renderer.render(font, textColor, text, emptyColor);
		if (writeFile && debug) {
			std::wcout << "Saving image to file [" << outfile.c_str() << "]" << std::endl;
		}
		FilePointer file(FilePointer::DIRECTION_OUT, outfile);
		PNGWriter writer;
		if (!writer.write(file, image)) {
			std::wcout << "Error writing image" << std::endl;
		}
	}
	catch (FontAwesomeException const& ex) {
		std::cerr << "Exception: " << ex.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

