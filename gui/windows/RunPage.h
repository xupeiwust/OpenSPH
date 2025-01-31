#pragma once

#include "gui/Settings.h"
#include "gui/windows/Widgets.h"
#include "objects/containers/Array.h"
#include "objects/wrappers/LockingPtr.h"
#include "system/Settings.h"
#include "system/Timer.h"
#include <wx/panel.h>

class wxBoxSizer;
class wxGauge;
class wxCheckBox;
class wxTextCtrl;
class wxPanel;
class wxDialog;

class wxAuiManager;
class wxAuiNotebookEvent;

NAMESPACE_SPH_BEGIN

class IColorizer;
struct ColorizerData;
class IGraphicsPane;
class IPlot;
class Controller;
class OrthoPane;
class ParticleProbe;
class PlotView;
class Particle;
class Rgba;
class Statistics;
class Storage;
struct DiagnosticsError;
class SelectedParticlePlot;
class TimeLine;
class ProgressPanel;
class PaletteSimpleWidget;

/// \brief Main frame of the application.
///
/// Run is coupled with the window; currently there can only be one window and one run at the same time. Run
/// is ended when user closes the window.
class RunPage : public ClosablePage {
private:
    /// Parent control object
    RawPtr<Controller> controller;

    AutoPtr<wxAuiManager> manager;

    /// Gui settings
    GuiSettings& gui;

    /// Drawing pane (owned by wxWidgets)
    RawPtr<IGraphicsPane> pane;

    RawPtr<ParticleProbe> probe;

    Array<LockingPtr<IPlot>> plots;
    Array<RawPtr<PlotView>> plotViews;

    LockingPtr<SelectedParticlePlot> selectedParticlePlot;

    PaletteSimpleWidget* palettePanel = nullptr;

    wxTextCtrl* statsText = nullptr;
    Timer statsTimer;

    /// Additional wx controls
    ComboBox* quantityBox = nullptr;
    Size selectedIdx = 0;
    wxPanel* quantityPanel = nullptr;

    TimeLine* timelineBar = nullptr;
    ProgressPanel* progressBar = nullptr;
    wxPanel* statsBar = nullptr;

    /// Colorizers corresponding to the items in combobox
    Array<ColorizerData> colorizerList;

public:
    RunPage(wxWindow* window, Controller* controller, GuiSettings& guiSettings);

    ~RunPage();

    void refresh();

    void showTimeLine(const bool show);

    void runStarted(const Storage& storage, const Path& path);

    void onTimeStep(const Storage& storage, const Statistics& stats);

    void onRunEnd();

    void setProgress(const Statistics& stats);

    void newPhase(const String& className, const String& instanceName);

    void setColorizerList(Array<ColorizerData>&& colorizers);

    void setSelectedParticle(const Particle& particle, const Rgba color);

    void deselectParticle();

    wxSize getCanvasSize() const;

    bool isOk() const;

private:
    virtual bool isRunning() const override;
    virtual void stop() override;
    virtual void quit() override;

    /// Toolbar on the top, containing buttons for controlling the run.
    // wxPanel* createToolBar();

    /// Panel on the right with particle data
    wxPanel* createProbeBar();

    /// Panel on the right with plots
    wxPanel* createPlotBar();

    /// Panel on the left, with visualization controls
    wxPanel* createVisBar();

    /// Panel on the right, with run statistics and error reporting
    wxPanel* createStatsBar();

    wxWindow* createParticleBox(wxPanel* parent);
    wxWindow* createRaymarcherBox(wxPanel* parent);
    wxWindow* createVolumeBox(wxPanel* parent);

    void makeStatsText(const Size particleCnt, const Size pointCnt, const Statistics& stats);

    void setColorizer(const Size idx);

    void replaceQuantityBar(const Size idx);

    void addComponentIdBar(wxWindow* parent, wxSizer* sizer, SharedPtr<IColorizer> colorizer);

    void updateCutoff(const double cutoff);
};


NAMESPACE_SPH_END
