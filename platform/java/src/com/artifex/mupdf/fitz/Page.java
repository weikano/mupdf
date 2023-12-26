// Copyright (C) 2004-2023 Artifex Software, Inc.
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

public class Page
{
	static {
		Context.init();
	}

	public static final int MEDIA_BOX = 0;
	public static final int CROP_BOX = 1;
	public static final int BLEED_BOX = 2;
	public static final int TRIM_BOX = 3;
	public static final int ART_BOX = 4;
	public static final int UNKNOWN_BOX = 5;

	private long pointer;

	protected native void finalize();

	public void destroy() {
		finalize();
	}

	protected Page(long p) {
		pointer = p;
	}

	private native Rect getBoundsNative(int box);

	public Rect getBounds(int box) {
		return getBoundsNative(box);
	}

	public Rect getBounds() {
		return getBoundsNative(Page.CROP_BOX);
	}

	public native void run(Device dev, Matrix ctm, Cookie cookie);
	public native void runPageContents(Device dev, Matrix ctm, Cookie cookie);
	public native void runPageAnnots(Device dev, Matrix ctm, Cookie cookie);
	public native void runPageWidgets(Device dev, Matrix ctm, Cookie cookie);

	public void run(Device dev, Matrix ctm) {
		run(dev, ctm, null);
	}

	public native Link[] getLinks();

	public native Pixmap toPixmap(Matrix ctm, ColorSpace cs, boolean alpha, boolean showExtras);
	public Pixmap toPixmap(Matrix ctm, ColorSpace cs, boolean alpha) {
		return toPixmap(ctm, cs, alpha, true);
	}

	public native DisplayList toDisplayList(boolean showExtras);
	public DisplayList toDisplayList() {
		return toDisplayList(true);
	}

	public native StructuredText toStructuredText(String options);
	public StructuredText toStructuredText() {
		return toStructuredText(null);
	}

	public native Quad[][] search(String needle);

	public native byte[] textAsHtml();

	public native Document getDocument();

	public native Link createLink(Rect bbox, String uri);
	public Link createLink(Rect bbox, LinkDestination dest) {
		return createLink(bbox, getDocument().formatLinkURI(dest));
	}
	public native void deleteLink(Link link);

	public native String getLabel();

	public native String getTextContent();

	/**
	 * 自动裁边功能，功能参考自koreader
	 * <p>
	 * document.lua
	 * function page_mt.__index:getUsedBBox()
	 *
	 * @return null if exception else contents rect
	 */
	public native Rect getUsedBBox();

	public StructuredText toStructuredTextAdvance(int flags) {
		return toStructuredTextAdvance(flags, 1);
	}

	/**
	 * @param flags 0或者{@link StructuredText#FLAG_FZ_STEXT_DEHYPHENATE}等标记组合
	 * @param scale 缩放比例，默认为1
	 * @return
	 */
	public native StructuredText toStructuredTextAdvance(int flags, float scale);

	/**
	 * pdfium中FPDF_DeviceToPage
	 *
	 * if (!page || !page_x || !page_y)
	 * return false;
	 * <p>
	 * IPDF_Page* pPage = IPDFPageFromFPDFPage(page);
	 * const FX_RECT rect(start_x, start_y, start_x + size_x, start_y + size_y);
	 * absl::optional<CFX_PointF> pos =
	 * pPage->DeviceToPage(rect, rotate, CFX_PointF(device_x, device_y));
	 * if (!pos.has_value())
	 * return false;
	 * <p>
	 * *page_x = pos->x;
	 * *page_y = pos->y;
	 * return true;
	 * @param bbox 页面尺寸
	 * @param startX 偏移量x
	 * @param startY 偏移量Y
	 * @param size_x 映射后的宽度
	 * @param size_y 映射后的高度
	 * @param rotate 旋转
	 * @param device_x 在size_x上的横坐标
	 * @param device_y 在size_y上的纵坐标
	 * @return 映射到pdf文档坐标后的x,y null if context exception
	 */
	public native Point deviceToPage(Rect bbox, int startX, int startY, int size_x, int size_y, int rotate, double device_x, double device_y);

	/**
	 * pdfium中FPDF_PageToDevice
	 *
	 * if (!page || !device_x || !device_y)
	 *     return false;
	 *
	 *   IPDF_Page* pPage = IPDFPageFromFPDFPage(page);
	 *   const FX_RECT rect(start_x, start_y, start_x + size_x, start_y + size_y);
	 *   CFX_PointF page_point(static_cast<float>(page_x), static_cast<float>(page_y));
	 *   absl::optional<CFX_PointF> pos =
	 *       pPage->PageToDevice(rect, rotate, page_point);
	 *   if (!pos.has_value())
	 *     return false;
	 *
	 *   *device_x = FXSYS_roundf(pos->x);
	 *   *device_y = FXSYS_roundf(pos->y);
	 *   return true;
	 * @param bbox 页面尺寸
	 * @param startX 偏移量x
	 * @param startY 偏移量Y
	 * @param size_x 映射后的宽度
	 * @param size_y 映射后的高度
	 * @param rotate 旋转
	 * @param device_x 在size_x上的横坐标
	 * @param device_y 在size_y上的纵坐标
	 * @return 映射到pdf文档坐标后的x,y null if context exception
	 */
	public native Point pageToDevice(Rect bbox, int startX, int startY, int size_x, int size_y, int rotate, double device_x, double device_y);

	/**
	 * 获取页面尺寸，参考pdfium cpdf_page.cpp CPDF_Page::UpdateDimensions实现
	 * @return
	 */
	public native Rect getPdfiumBBox();
}
