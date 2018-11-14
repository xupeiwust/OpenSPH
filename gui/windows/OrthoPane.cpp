#include "gui/windows/OrthoPane.h"
#include "gui/Controller.h"
#include "gui/Settings.h"
#include "gui/Utils.h"
#include "gui/objects/Bitmap.h"
#include "gui/objects/Camera.h"
#include "system/Profiler.h"
#include "thread/CheckFunction.h"
#include <wx/dcclient.h>
#include <wx/timer.h>

NAMESPACE_SPH_BEGIN

OrthoPane::OrthoPane(wxWindow* parent, Controller* controller, const GuiSettings& gui)
    : IGraphicsPane(parent)
    , controller(controller) {
    const int width = gui.get<int>(GuiSettingsId::VIEW_WIDTH);
    const int height = gui.get<int>(GuiSettingsId::VIEW_HEIGHT);
    this->SetMinSize(wxSize(width, height));
    this->Connect(wxEVT_PAINT, wxPaintEventHandler(OrthoPane::onPaint));
    this->Connect(wxEVT_MOTION, wxMouseEventHandler(OrthoPane::onMouseMotion));
    this->Connect(wxEVT_MOUSEWHEEL, wxMouseEventHandler(OrthoPane::onMouseWheel));
    this->Connect(wxEVT_RIGHT_DOWN, wxMouseEventHandler(OrthoPane::onRightDown));
    this->Connect(wxEVT_RIGHT_UP, wxMouseEventHandler(OrthoPane::onRightUp));
    this->Connect(wxEVT_LEFT_UP, wxMouseEventHandler(OrthoPane::onLeftUp));

    camera = controller->getCurrentCamera();
    particle.lastIdx = -1;
    arcBall.resize(Pixel(width, height));
}

OrthoPane::~OrthoPane() = default;

void OrthoPane::resetView() {
    dragging.initialMatrix = AffineMatrix::identity();
    camera->transform(AffineMatrix::identity());
}

void OrthoPane::onPaint(wxPaintEvent& UNUSED(evt)) {
    CHECK_FUNCTION(CheckFunction::MAIN_THREAD);

    wxPaintDC dc(this);
    const wxBitmap& bitmap = controller->getRenderedBitmap();
    if (!bitmap.IsOk()) {
        return;
    }

    dc.DrawBitmap(bitmap, wxPoint(0, 0));
}

void OrthoPane::onMouseMotion(wxMouseEvent& evt) {
    CHECK_FUNCTION(CheckFunction::MAIN_THREAD);
    Pixel position(evt.GetPosition());
    if (evt.Dragging()) {
        Pixel offset = Pixel(position.x - dragging.position.x, -(position.y - dragging.position.y));
        if (evt.RightIsDown()) {
            // right button, rotate view
            AffineMatrix matrix = arcBall.drag(position);
            camera->transform(dragging.initialMatrix * matrix);
        } else {
            // left button (or middle), pan
            camera->pan(offset);
        }
        controller->refresh(camera->clone());
    }
    dragging.position = position;
}

void OrthoPane::onRightDown(wxMouseEvent& evt) {
    CHECK_FUNCTION(CheckFunction::MAIN_THREAD);
    arcBall.click(Pixel(evt.GetPosition()));
}

void OrthoPane::onRightUp(wxMouseEvent& evt) {
    CHECK_FUNCTION(CheckFunction::MAIN_THREAD);
    AffineMatrix matrix = arcBall.drag(Pixel(evt.GetPosition()));
    dragging.initialMatrix = dragging.initialMatrix * matrix;
}

void OrthoPane::onLeftUp(wxMouseEvent& evt) {
    CHECK_FUNCTION(CheckFunction::MAIN_THREAD);
    Pixel position(evt.GetPosition());
    Optional<Size> selectedIdx = controller->getIntersectedParticle(position);
    if (selectedIdx.valueOr(-1) != particle.lastIdx.valueOr(-1)) {
        particle.lastIdx = selectedIdx;
        controller->setSelectedParticle(selectedIdx);
        controller->refresh(camera->clone());
    }
}

void OrthoPane::onMouseWheel(wxMouseEvent& evt) {
    CHECK_FUNCTION(CheckFunction::MAIN_THREAD);
    const float spin = evt.GetWheelRotation();
    const float amount = (spin > 0.f) ? 1.2f : 1.f / 1.2f;
    Pixel fixedPoint(evt.GetPosition());
    camera->zoom(Pixel(fixedPoint.x, this->GetSize().y - fixedPoint.y - 1), amount);
    controller->refresh(camera->clone());
}


NAMESPACE_SPH_END
