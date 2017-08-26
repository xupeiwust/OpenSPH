#pragma once

#include "gui/Settings.h"
#include "objects/containers/Array.h"
#include "objects/wrappers/SharedPtr.h"
#include <wx/frame.h>

class wxComboBox;
class wxBoxSizer;
class wxGauge;
class wxCheckBox;

NAMESPACE_SPH_BEGIN

class IColorizer;
class Controller;
class OrthoPane;
class ParticleProbe;
class Particle;
class Color;

/// Main frame of the application. Run is coupled with the window; currently there can only be one window and
/// one run at the same time. Run is ended when user closes the window.
class MainWindow : public wxFrame {
private:
    /// Parent control object
    RawPtr<Controller> controller;

    /// Drawing pane
    /// \todo used some virtual base class instead of directly orthopane
    RawPtr<OrthoPane> pane;

    RawPtr<ParticleProbe> probe;

    /// Additional wx controls
    wxComboBox* quantityBox;
    Size selectedIdx = 0;
    wxGauge* gauge;
    wxCheckBox* shadingBox;

    Array<SharedPtr<IColorizer>> elementList;

public:
    MainWindow(Controller* controller, const GuiSettings& guiSettings);

    void setProgress(const float progress);

    void setElementList(Array<SharedPtr<IColorizer>>&& elements);

    void setSelectedParticle(const Particle& particle, const Color color);

    void deselectParticle();

private:
    wxBoxSizer* createToolbar(Controller* parent);

    /// wx event handlers

    void onComboBox(wxCommandEvent& evt);

    void onClose(wxCloseEvent& evt);
};


NAMESPACE_SPH_END
