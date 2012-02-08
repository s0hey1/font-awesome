import os
import math
import sys

import Image, ImageDraw, ImageFont
import freetype as ft

from optparse import OptionParser

FREETYPE_DPI = 100
DEFAULT_TEXT = "The Quick Brown Fox Jumped Over the Lazy Dog"

avail_sizes = (12, 24, 36, 62)

def draw_bitmap(target, source, x, y):
	x_max = x + source.width
	y_max = y + source.rows
	width, height = target.size
	pix = target.load()
	for p, i in enumerate(range(x, x_max)):
		for q, j in enumerate(range(y, y_max)):
			v = int(source.buffer[q * source.width + p])
			if v != 0:
				a = pix[i, j][3]
				pixel = (0, 0, 0, max(v, a))
				pix[i, j] = pixel

def draw(font, text, size):
	print font, text, size
	face = ft.Face(font)
	face.set_char_size(size * 64, 0, FREETYPE_DPI, 0)
	slot = face.glyph

	#matrix = ft.Matrix()
	#matrix.xx = (int)( math.cos(0) * 0x10000L)
	#matrix.xy = (int)(-math.sin(0) * 0x10000L)
	#matrix.yx = (int)( math.sin(0) * 0x10000L)
	#matrix.yy = (int)( math.cos(0) * 0x10000L)
	pen = ft.Vector(0, 0)

	minx = 0
	miny = 0
	maxx = 0
	maxy = 0
	for i, c in enumerate(text):
		face.set_transform(ft.Matrix(), pen)
		face.load_char(c, ft.FT_LOAD_RENDER)
		bitmap = slot.bitmap
		_l = slot.bitmap_left
		_t = 0 - slot.bitmap_top
		minx = min(minx, _l)
		miny = min(miny, _t)
		maxx = max(maxx, _l + bitmap.width)
		maxy = max(maxy, _t + bitmap.rows)
		pen.x += slot.advance.x
		pen.y += slot.advance.y
	
	width = maxx - minx
	height = maxy - miny

	pen.x = (0 - minx) * 64;
	pen.y = (maxy) * 64;
	image = Image.new('RGBA', (width, height), (255, 255, 255, 0))
	for i, c in enumerate(text):
		face.set_transform(ft.Matrix(), pen)
		face.load_char(c, ft.FT_LOAD_RENDER)
		draw_bitmap(image, slot.bitmap, slot.bitmap_left, height - slot.bitmap_top)
		pen.x += slot.advance.x
		pen.y += slot.advance.y

	return image

def render_preview(fontName, fontSize):
	if fontName is None:
		raise Exception("you gave me a bad font")
	fontSize = int(fontSize)
	if fontSize not in avail_sizes:
		fontSize = avail_sizes[-1]

	text = "Hello world yo"

	image = draw(fontName, text, fontSize)

	image.save("output.png", "PNG")



if __name__ == "__main__":
	parser = OptionParser()
	parser.add_option("-f", "--file", dest="filename", action="store", type="string", help="Font File", metavar="FILE")
	parser.add_option("-s", "--size", dest="size", action="store", type="int", help="Font Size")

	(options, args) = parser.parse_args()
	render_preview(options.filename, options.size)



