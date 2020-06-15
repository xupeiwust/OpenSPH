#pragma once

#include "post/Analysis.h"
#include "quantities/Storage.h"
#include <thread>
#include <wx/grid.h>
#include <wx/panel.h>

NAMESPACE_SPH_BEGIN

enum class CheckFlag;

class GridPage : public wxPanel {
private:
    wxGrid* grid = nullptr;
    std::thread thread;
    const Storage& storage;

    wxSpinCtrl* countSpinner;

public:
    GridPage(wxWindow* parent, const wxSize size, const Storage& storage);

    ~GridPage();

private:
    struct Config {
        Float moonLimit = 0.1f;
        Float radiiLimit = 2.f;
    };

    void update(const Storage& storage, const Config& config);

    void updateAsync(const Storage& storage,
        const Size fragmentCnt,
        const Flags<CheckFlag> checks,
        const Config& config);

    template <typename T>
    void updateCell(const Size rowIdx, const Size colIdx, const T& value);

    wxCheckBox* getCheck(const CheckFlag check) const;

    Size getCheckedCount() const;
};

NAMESPACE_SPH_END
