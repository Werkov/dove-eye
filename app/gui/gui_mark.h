#ifndef GUI_GUI_MARK_H_
#define GUI_GUI_MARK_H_

#include <algorithm>

#include <QPoint>
#include <QSize>

namespace gui {

struct GuiMark {
  enum Flags {
    kNoFlags = 0,
    kCtrl = 0x1,
    kShift = 0x2
  };

  QPoint press_pos;
  QPoint release_pos;
  Flags flags;

  GuiMark() : flags(kNoFlags) {
  }

  inline QPoint TopLeft() const {
    return QPoint(std::min(press_pos.x(), release_pos.x()),
                  std::min(press_pos.y(), release_pos.y()));
  }

  inline QSize Size() const {
    return QSize(std::abs(press_pos.x() - release_pos.x()),
                 std::abs(press_pos.y() - release_pos.y()));
  }
};

inline GuiMark::Flags operator|(const GuiMark::Flags lhs, const GuiMark::Flags rhs) {
  return static_cast<GuiMark::Flags>(
      static_cast<int>(lhs) | static_cast<int>(rhs));
}

inline GuiMark::Flags& operator|=(GuiMark::Flags &lhs, const GuiMark::Flags rhs) {
  lhs = lhs | rhs;
  return lhs;
}

inline GuiMark::Flags operator&(const GuiMark::Flags lhs, const GuiMark::Flags rhs) {
  return static_cast<GuiMark::Flags>(
      static_cast<int>(lhs) & static_cast<int>(rhs));
}

inline GuiMark::Flags& operator&=(GuiMark::Flags &lhs, const GuiMark::Flags rhs) {
  lhs = lhs & rhs;
  return lhs;
}

} // namespace gui

#ifdef HAVE_GUI
/*
 * Make CameraIndex available as argument for Qt's queued connections.
 */
#include <QMetaType>
Q_DECLARE_METATYPE(gui::GuiMark)
#endif

#endif // GUI_GUI_MARK_H_
