// Copyright (C) 2004-2021 Artifex Software, Inc.
//
// This file is part of MuPDF.
//
// MuPDF is free software: you can redistribute it and/or modify it under the
// terms of the GNU Affero General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option)
// any later version.
//
// MuPDF is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more
// details.
//
// You should have received a copy of the GNU Affero General Public License
// along with MuPDF. If not, see <https://www.gnu.org/licenses/agpl-3.0.en.html>
//
// Alternative licensing terms are available from the licensor.
// For commercial licensing, see <https://www.artifex.com/> or contact
// Artifex Software, Inc., 39 Mesa Street, Suite 108A, San Francisco,
// CA 94129, USA, for further information.

package com.artifex.mupdf.fitz;

import java.util.ArrayList;

public class StructuredText
{
	static {
		Context.init();
	}

	public static final int SELECT_CHARS = 0;
	public static final int SELECT_WORDS = 1;
	public static final int SELECT_LINES = 2;

	private long pointer;

	protected native void finalize();

	public void destroy() {
		finalize();
	}

	private StructuredText(long p) {
		pointer = p;
	}

	public native Quad[][] search(String needle);
	public native Quad[] highlight(Point a, Point b);
	public native Quad snapSelection(Point a, Point b, int mode);
	public native String copy(Point a, Point b);

	public native void walk(StructuredTextWalker walker);

	public TextBlock[] getBlocks() {
		BlockWalker walker = new BlockWalker();
		walk(walker);
		return walker.getBlocks();
	}

	static class BlockWalker implements StructuredTextWalker {
		ArrayList<TextBlock> blocks;
		ArrayList<TextLine> lines;
		ArrayList<TextChar> chrs;
		Rect lineBbox;
		Point lineDir;
		Rect blockBbox;

		BlockWalker() {
			blocks = new ArrayList<TextBlock>();
		}

		public void onImageBlock(Rect bbox, Matrix transform, Image image) {
		}

		public void beginTextBlock(Rect bbox) {
			lines = new ArrayList<TextLine>();
			blockBbox = bbox;
		}

		public void endTextBlock() {
			TextBlock block = new TextBlock();
			block.bbox = blockBbox;
			block.lines = lines.toArray(new TextLine[0]);
			blocks.add(block);
		}

		public void beginLine(Rect bbox, int wmode, Point dir) {
			chrs = new ArrayList<TextChar>();
			lineBbox = bbox;
			lineDir = dir;
		}

		public void endLine() {
			TextLine line = new TextLine();
			line.bbox = lineBbox;
			line.dir = lineDir;
			line.chars =  chrs.toArray(new TextChar[0]);
			lines.add(line);
		}

		public void onChar(int c, Point origin, Font font, float size, Quad quad) {
			TextChar chr = new TextChar();
			chr.c = c;
			chr.quad = quad;
			chr.origin = origin;
			chrs.add(chr);
		}

		TextBlock[] getBlocks() {
			return blocks.toArray(new TextBlock[0]);
		}
	}

	public static class TextBlock {
		public TextLine[] lines;
		public Rect bbox;
	}

	public static class TextLine {
		public TextChar[] chars;
		public Rect bbox;
		public Point dir;
	}

	public static class TextChar {
		public int c;
		public Quad quad;
		public Point origin;
		public boolean isWhitespace() {
			return Character.isWhitespace(c);
		}
	}

	/**
	 * FZ_STEXT_PRESERVE_LIGATURES: If this option is activated
	 * 	ligatures are passed through to the application in their
	 * 	original form. If this option is deactivated ligatures are
	 * 	expanded into their constituent parts, e.g. the ligature ffi is
	 * 	expanded into three separate characters f, f and i.
	 */
	public static final int FLAG_FZ_STEXT_PRESERVE_LIGATURES = 1;
	/**
	 * FZ_STEXT_PRESERVE_WHITESPACE: If this option is activated
	 * 	whitespace is passed through to the application in its original
	 * 	form. If this option is deactivated any type of horizontal
	 * 	whitespace (including horizontal tabs) will be replaced with
	 * 	space characters of variable width.
	 */
	public static final int FLAG_FZ_STEXT_PRESERVE_WHITESPACE = 2;
	/**
	 * FZ_STEXT_PRESERVE_IMAGES: If this option is set, then images
	 * 	will be stored in the structured text structure. The default is
	 * 	to ignore all images.
	 */
	public static final int FLAG_FZ_STEXT_PRESERVE_IMAGES = 4;
	/**
	 * FZ_STEXT_INHIBIT_SPACES: If this option is set, we will not try
	 * 	to add missing space characters where there are large gaps
	 * 	between characters.
	 */
	public static final int FLAG_FZ_STEXT_INHIBIT_SPACES = 8;
	/**
	 * FZ_STEXT_DEHYPHENATE: If this option is set, hyphens at the
	 * 	end of a line will be removed and the lines will be merged.
	 */
	public static final int FLAG_FZ_STEXT_DEHYPHENATE = 16;
	/**
	 * FZ_STEXT_PRESERVE_SPANS: If this option is set, spans on the same line
	 * 	will not be merged. Each line will thus be a span of text with the same
	 * 	font, colour, and size.
	 */
	public static final int FLAG_FZ_STEXT_PRESERVE_SPANS = 32;
	/**
	 * FZ_STEXT_MEDIABOX_CLIP: If this option is set, characters entirely
	 * 	outside each page's mediabox will be ignored.
	 */
	public static final int FLAG_FZ_STEXT_MEDIABOX_CLIP = 64;
}
