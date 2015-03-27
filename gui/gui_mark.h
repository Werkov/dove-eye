#ifndef GUI_GUI_MARK_H_
#define GUI_GUI_MARK_H_

#include <QPoint>

namespace gui {

struct GuiMark {
  QPoint pos;
};

} // namespace gui

#ifdef HAVE_GUI
/*
 * Make CameraIndex available as argument for Qt's queued connections.
 */
#include <QMetaType>
Q_DECLARE_METATYPE(gui::GuiMark)
#endif

#endif // GUI_GUI_MARK_H_
