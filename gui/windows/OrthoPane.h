#pragma once

#include "gui/ArcBall.h"
#include "gui/Settings.h"
#include "gui/objects/Point.h"
#include "gui/windows/IGraphicsPane.h"
#include "objects/wrappers/Optional.h"

class wxTimer;

NAMESPACE_SPH_BEGIN

class Controller;

class OrthoPane : public IGraphicsPane {
private:
    Controller* controller;

    /// Helper for rotation
    ArcBall arcBall;

    struct {
        /// Cached last mouse position when dragging the window
        Point position;

        /// Camera rotation matrix when dragging started.
        AffineMatrix initialMatrix = AffineMatrix::identity();
    } dragging;

    struct {
        Optional<Size> lastIdx;
    } particle;

public:
    OrthoPane(wxWindow* parent, Controller* controller, const GuiSettings& gui);

    ~OrthoPane();

    virtual void resetView() override {
        dragging.initialMatrix = AffineMatrix::identity();
    }

private:
    /// wx event handlers
    void onPaint(wxPaintEvent& evt);

    void onMouseMotion(wxMouseEvent& evt);

    void onLeftUp(wxMouseEvent& evt);

    void onRightDown(wxMouseEvent& evt);

    void onRightUp(wxMouseEvent& evt);

    void onMouseWheel(wxMouseEvent& evt);
};

NAMESPACE_SPH_END
