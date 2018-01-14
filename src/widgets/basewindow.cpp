#include "basewindow.hpp"

#include "singletons/settingsmanager.hpp"
#include "util/nativeeventhelper.hpp"
#include "widgets/tooltipwidget.hpp"

#include <QDebug>
#include <QIcon>

#include <windows.h>

namespace chatterino {
namespace widgets {

BaseWindow::BaseWindow(singletons::ThemeManager &_themeManager, QWidget *parent)
    : BaseWidget(_themeManager, parent)
{
    this->init();
}

BaseWindow::BaseWindow(BaseWidget *parent)
    : BaseWidget(parent)
{
    this->init();
}

BaseWindow::BaseWindow(QWidget *parent)
    : BaseWidget(parent)
{
    this->init();
}

void BaseWindow::init()
{
    this->setWindowIcon(QIcon(":/images/icon.png"));

#ifdef USEWINSDK
    auto dpi = util::getWindowDpi(this->winId());

    if (dpi) {
        this->dpiMultiplier = dpi.value() / 96.f;
    }

    this->dpiMultiplierChanged(1, this->dpiMultiplier);
#endif

    if (singletons::SettingManager::getInstance().windowTopMost.getValue()) {
        this->setWindowFlags(this->windowFlags() | Qt::WindowStaysOnTopHint);
    }
}

void BaseWindow::changeEvent(QEvent *)
{
    TooltipWidget::getInstance()->hide();
}

void BaseWindow::leaveEvent(QEvent *)
{
    TooltipWidget::getInstance()->hide();
}

#ifdef USEWINSDK
bool BaseWindow::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
    MSG *msg = reinterpret_cast<MSG *>(message);

    // WM_DPICHANGED
    if (msg->message == 0x02E0) {
        qDebug() << "dpi changed";
        int dpi = HIWORD(msg->wParam);

        float oldDpiMultiplier = this->dpiMultiplier;
        this->dpiMultiplier = dpi / 96.f;
        float scale = this->dpiMultiplier / oldDpiMultiplier;

        this->dpiMultiplierChanged(oldDpiMultiplier, this->dpiMultiplier);

        this->resize(static_cast<int>(this->width() * scale),
                     static_cast<int>(this->height() * scale));

        return true;
    }

    return QWidget::nativeEvent(eventType, message, result);
}  // namespace widgets

#endif

}  // namespace widgets
}  // namespace chatterino