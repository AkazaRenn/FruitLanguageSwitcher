#include <exception>
#include <iostream>
#include <memory>
#include <ole2.h>
#include <wrl/client.h>
#pragma warning(push)
#pragma warning(disable : 5260)
#include <UIAutomation.h>
#pragma warning(pop)
#include <Windows.h>

using Microsoft::WRL::ComPtr;

struct CaretInfo {
    double X;
    double Y;
    double Height;
};

class SafeArray {
private:
    SAFEARRAY* pSafeArray = nullptr;

public:
    SafeArray(SAFEARRAY* _pSafeArray)
        : pSafeArray(_pSafeArray) {}
    ~SafeArray() {
        if (!pSafeArray) {
            return;
        }
        SafeArrayDestroy(pSafeArray);
    }
    bool GetCaretInfo(CaretInfo& outCaretInfo) {
        LONG lBound = 0, uBound = -1;
        SafeArrayGetLBound(pSafeArray, 1, &lBound);
        SafeArrayGetUBound(pSafeArray, 1, &uBound);
        LONG count = uBound - lBound + 1;

        if (count >= 4) {
            double* pData;
            SafeArrayAccessData(pSafeArray, (void**)&pData);
            double left = pData[0];
            double top = pData[1];
            double width = pData[2];
            double height = pData[3];
            SafeArrayUnaccessData(pSafeArray);

            outCaretInfo = {
                .X = (left + width / 2),
                .Y = top,
                .Height = height,
            };
            return true;
        }

        return false;
    }
    static inline bool GetCaretInfo(ComPtr<IUIAutomationTextRange>& textRange, CaretInfo& outCaretInfo) {
        SAFEARRAY* pSafeArray;
        textRange->GetBoundingRectangles(&pSafeArray);
        if (pSafeArray) {
            return SafeArray(pSafeArray).GetCaretInfo(outCaretInfo);
        }
        return false;
    }
};

extern "C" __declspec(dllexport)
bool GetCaretPosition(CaretInfo* caretInfo) {
    if (!caretInfo) {
        return false;
    }

    HRESULT hr = 0;
    int compValue;
    try {
        do {
            ComPtr<IUIAutomation> uiAutomation;
            hr = CoCreateInstance(CLSID_CUIAutomation8, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(uiAutomation.GetAddressOf()));
            if (FAILED(hr)) {
                return false;
            }
            ComPtr<IUIAutomationElement> focusedElement;
            hr = uiAutomation->GetFocusedElement(focusedElement.GetAddressOf());
            if (FAILED(hr) || (!focusedElement)) {
                return false;
            }

            do {
                ComPtr<IUIAutomationTextPattern2> textPattern2;
                hr = focusedElement->GetCurrentPatternAs(UIA_TextPattern2Id, IID_PPV_ARGS(textPattern2.GetAddressOf()));
                if (FAILED(hr) || (!textPattern2)) {
                    break;
                }
                ComPtr<IUIAutomationTextRangeArray> selection;
                hr = textPattern2->GetSelection(selection.GetAddressOf());
                if (FAILED(hr) || (!selection)) {
                    break;
                }
                int selectionLength;
                hr = selection->get_Length(&selectionLength);
                if (FAILED(hr) || (!selectionLength)) {
                    break;
                }
                ComPtr<IUIAutomationTextRange> textRange;
                hr = selection->GetElement(0, textRange.GetAddressOf());
                if (FAILED(hr) || (!textRange)) {
                    break;
                }
                hr = textRange->CompareEndpoints(TextPatternRangeEndpoint::TextPatternRangeEndpoint_Start, textRange.Get(), TextPatternRangeEndpoint::TextPatternRangeEndpoint_End, &compValue);
                if (compValue == 0) {
                    // Not a selection anyways, just use this value
                    if (SafeArray::GetCaretInfo(textRange, *caretInfo)) {
                        break;
                    }
                }
                ComPtr<IUIAutomationTextRange> caretRange;
                BOOL isActive;
                hr = textPattern2->GetCaretRange(&isActive, caretRange.GetAddressOf());
                if (FAILED(hr) || (!caretRange)) {
                    break;
                }
                hr = caretRange->CompareEndpoints(TextPatternRangeEndpoint::TextPatternRangeEndpoint_Start, textRange.Get(), TextPatternRangeEndpoint::TextPatternRangeEndpoint_Start, &compValue);
                if (compValue == 0) {
                    hr = textRange->MoveEndpointByRange(TextPatternRangeEndpoint::TextPatternRangeEndpoint_Start, textRange.Get(), TextPatternRangeEndpoint::TextPatternRangeEndpoint_End);
                } else {
                    hr = textRange->MoveEndpointByRange(TextPatternRangeEndpoint::TextPatternRangeEndpoint_End, textRange.Get(), TextPatternRangeEndpoint::TextPatternRangeEndpoint_Start);
                }
                if (SafeArray::GetCaretInfo(textRange, *caretInfo)) {
                    break;
                }
            } while (false);

            if (caretInfo->Height > 0) {
                return true;
            }

            do {
                ComPtr<IUIAutomationTextPattern> textPattern;
                hr = focusedElement->GetCurrentPatternAs(UIA_TextPatternId, IID_PPV_ARGS(textPattern.GetAddressOf()));
                if (FAILED(hr) || (!textPattern)) {
                    break;
                }
                ComPtr<IUIAutomationTextRangeArray> selection;
                hr = textPattern->GetSelection(selection.GetAddressOf());
                if (FAILED(hr) || (!selection)) {
                    break;
                }
                int selectionLength;
                hr = selection->get_Length(&selectionLength);
                if (FAILED(hr) || (!selectionLength)) {
                    break;
                }
                ComPtr<IUIAutomationTextRange> textRangeStart;
                hr = selection->GetElement(0, textRangeStart.GetAddressOf());
                if (FAILED(hr) || (!textRangeStart)) {
                    break;
                }
                ComPtr<IUIAutomationTextRange> textRangeEnd;
                hr = textRangeStart->Clone(textRangeEnd.GetAddressOf());
                if (FAILED(hr) || (!textRangeEnd)) {
                    break;
                }
                hr = textRangeStart->MoveEndpointByRange(TextPatternRangeEndpoint::TextPatternRangeEndpoint_End, textRangeStart.Get(), TextPatternRangeEndpoint::TextPatternRangeEndpoint_Start);
                if (SafeArray::GetCaretInfo(textRangeStart, *caretInfo)) {
                    break;
                }
                hr = textRangeEnd->MoveEndpointByRange(TextPatternRangeEndpoint::TextPatternRangeEndpoint_Start, textRangeEnd.Get(), TextPatternRangeEndpoint::TextPatternRangeEndpoint_End);
                if (SafeArray::GetCaretInfo(textRangeEnd, *caretInfo)) {
                    break;
                }
            } while (false);
        } while (false);
    } catch (std::exception e) {
    }

    return true;
}
