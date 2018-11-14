#include "gui/Utils.h"
#include "common/Assert.h"
#include "objects/utility/StringUtils.h"
#include <iomanip>
#include <wx/dcmemory.h>

NAMESPACE_SPH_BEGIN

static Size getSubscriptSize(const std::wstring& text) {
    Size size = 0;
    if (!text.empty() && text[0] == L'-') {
        size++;
    }
    for (; size < text.size(); ++size) {
        const char c = text[size];
        if ((c < L'0' || c > L'9') && (c < L'a' || c > L'z') && (c < L'A' || c > L'Z')) {
            return size;
        }
    }
    return text.size();
}

void drawTextWithSubscripts(wxDC& dc, const std::wstring& text, const wxPoint point) {
    std::size_t n;
    std::size_t m = 0;
    wxPoint actPoint = point;
    const wxFont font = dc.GetFont();
    const wxFont subcriptFont = font.Smaller();

    while ((n = text.find_first_of(L"_^", m)) != std::string::npos) {
        const bool isSubscript = text[n] == '_';
        std::wstring part = text.substr(m, n - m);
        wxSize extent = dc.GetTextExtent(part);
        // draw part up to subscript using current font
        dc.DrawText(part, actPoint);

        actPoint.x += extent.x;
        const Size subscriptSize = getSubscriptSize(text.substr(n + 1));
        const std::wstring subscript = text.substr(n + 1, subscriptSize);
        dc.SetFont(subcriptFont);
        wxPoint subscriptPoint = isSubscript ? wxPoint(actPoint.x + 2, actPoint.y + extent.y / 3)
                                             : wxPoint(actPoint.x + 2, actPoint.y - extent.y / 4);

        dc.DrawText(subscript, subscriptPoint);
        actPoint.x = subscriptPoint.x + dc.GetTextExtent(subscript).x;

        dc.SetFont(font);
        m = n + 1 + subscriptSize; // skip _ and the subscript character
    }
    // draw last part of the text
    dc.DrawText(text.substr(m), actPoint);
}


std::wstring toPrintableString(const float value, const Size precision, const float decimalThreshold) {
    const float absValue = abs(value);
    std::stringstream ss;
    if (absValue == 0.f || (absValue >= 1.f / decimalThreshold && absValue <= decimalThreshold)) {
        ss << value;
    } else {
        ss << std::setprecision(precision) << std::scientific << value;
    }
    std::string s = ss.str();

    std::wstring printable;
    if (value > 0) {
        printable += ' ';
    }
    bool exponent = false;
    for (Size i = 0; i < s.size(); ++i) {
        // replace unary pluses with spaces (to keep alignment of numbers
        if (s[i] == '+') {
            continue;
        }
        // replace 'e' with 'x10^'
        if (s[i] == 'e') {
            exponent = true;
            printable += L"\u00D710^";
            continue;
        }
        // get rid of leading zeros in exponent
        if (exponent) {
            if (s[i] == '-') {
                printable += '-';
                continue;
            }
            if (s[i] == '0') {
                continue;
            }
        }
        printable += s[i];
        exponent = false;
    }
    return printable;
}

static Pixel getOriginOffset(wxDC& dc, Flags<TextAlign> align, const std::wstring& text) {
    wxSize extent = dc.GetTextExtent(text);
    if (text.find(L"^") != std::string::npos) {
        // number with superscript is actually a bit shorter, shrink it
        /// \todo this should be done more correctly
        extent.x -= 6;
    }
    Pixel offset(0, 0);
    if (align.has(TextAlign::LEFT)) {
        offset.x -= extent.x;
    }
    if (align.has(TextAlign::HORIZONTAL_CENTER)) {
        offset.x -= extent.x / 2;
    }
    if (align.has(TextAlign::TOP)) {
        offset.y -= extent.y;
    }
    if (align.has(TextAlign::VERTICAL_CENTER)) {
        offset.y -= extent.y / 2;
    }
    return offset;
}

void printLabels(wxBitmap& bitmap, ArrayView<const IRenderOutput::Label> labels) {
    wxMemoryDC dc(bitmap);
    wxFont font = dc.GetFont();
    for (const IRenderOutput::Label& label : labels) {
        dc.SetTextForeground(wxColour(label.color));
        font.SetPointSize(label.fontSize);
        dc.SetFont(font);
        const wxPoint origin(label.position + getOriginOffset(dc, label.align, label.text));
        drawTextWithSubscripts(dc, label.text, origin);
    }
    dc.SelectObject(wxNullBitmap);
}

NAMESPACE_SPH_END
