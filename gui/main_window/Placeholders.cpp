#include "Placeholders.h"

#include "stdafx.h"
#include "../utils/utils.h"
#include "../utils/InterConnector.h"
#include "../utils/features.h"
#include "../controls/TextEmojiWidget.h"
#include "../controls/DialogButton.h"
#include "../controls/TooltipWidget.h"
#include "../previewer/toast.h"

#include "../gui_settings.h"
#include "../core_dispatcher.h"
#include "../styles/ThemeParameters.h"

#include "../my_info.h"

#include "../common.shared/config/config.h"

using namespace Ui;

static void copyLink(const QString& _link)
{
    QApplication::clipboard()->setText(_link);
    Utils::showToastOverContactDialog(QT_TRANSLATE_NOOP("toast", "Link copied"));
}

static QString makeLink()
{
    auto nickOrUin = []() {
        const auto& nick = MyInfo()->nick();
        if (!nick.isEmpty())
            return nick;
        return MyInfo()->aimId();
    };

    const auto isEmail = MyInfo()->isEmailProfile();
    QString link;
    link += ql1s("https://");
    link += isEmail ? Features::getProfileDomainAgent() : Features::getProfileDomain();
    link += ql1c('/');
    link += isEmail ? MyInfo()->aimId() : nickOrUin();
    return link;
}

PlaceholderButton::PlaceholderButton(QWidget* _parent)
    : ClickableWidget(_parent)
{
    setFixedHeight(Utils::scale_value(40));

    text_ = MakeTextUnit(QString(), Data::MentionMap(), TextRendering::LinksVisible::DONT_SHOW_LINKS);
    text_->init(Fonts::appFontScaled(16, Fonts::FontWeight::SemiBold), Styling::getParameters().getColor(Styling::StyleVariable::TEXT_SOLID_PERMANENT), QColor(), QColor(), QColor(), TextRendering::HorAligment::CENTER);
}

PlaceholderButton::~PlaceholderButton() = default;

void PlaceholderButton::setText(const QString& _text)
{
    text_->setText(_text);
    text_->evaluateDesiredSize();
}

void PlaceholderButton::paintEvent(QPaintEvent* _event)
{
    QPainter p(this);
    const auto r = rect();
    p.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    {
        Utils::PainterSaver saver(p);

        p.setPen(Qt::NoPen);
        if (isPressed())
            p.setBrush(Styling::getParameters().getColor(Styling::StyleVariable::PRIMARY_ACTIVE));
        else if (isHovered())
            p.setBrush(Styling::getParameters().getColor(Styling::StyleVariable::PRIMARY_HOVER));
        else
            p.setBrush(Styling::getParameters().getColor(Styling::StyleVariable::PRIMARY));
        p.drawRoundedRect(r, Utils::scale_value(20), Utils::scale_value(20));
    }

    text_->setOffsets((r.width() - text_->cachedSize().width()) / 2, r.height() / 2 - 1);
    text_->draw(p, TextRendering::VerPosition::MIDDLE);
}

Placeholder::Placeholder(QWidget* _parent, Type _type)
    : QWidget(_parent)
    , type_(_type)
    , isPicrureOnly_(false)
{
    auto mainLayout = Utils::emptyVLayout(this);
    mainLayout->setAlignment(Qt::AlignCenter);
    mainLayout->setContentsMargins({});
    mainLayout->setSpacing(0);

    noRecentsWidget_ = new QWidget(this);
    auto noRecentsLayout = new QVBoxLayout(noRecentsWidget_);
    noRecentsWidget_->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
    noRecentsLayout->setAlignment(Qt::AlignCenter);
    noRecentsLayout->setContentsMargins(Utils::scale_value(20), 0, Utils::scale_value(20), 0);
    noRecentsLayout->setSpacing(0);
    noRecentsLayout->addSpacerItem(new QSpacerItem(Utils::scale_value(1), Utils::scale_value(1), QSizePolicy::Maximum, QSizePolicy::Maximum));

    const bool isSearchable = config::get().is_on(config::features::searchable_recents_placeholder);

    auto noRecentsPlaceholder = new QLabel(this);
    auto pixmap = QPixmap(Utils::parse_image_name(ql1s(":/product/placeholder/") % (type_ == Type::Recents ? ql1s("recents_100") : ql1s("contacts_100"))));
    Utils::check_pixel_ratio(pixmap);
    noRecentsPlaceholder->setFixedSize(Utils::scale_value(96), Utils::scale_value(96));
    noRecentsPlaceholder->setPixmap(pixmap);
    noRecentsPlaceholder->setAutoFillBackground(true);
    Testing::setAccessibleName(noRecentsPlaceholder, qsl("AS noRecentsPlaceholder"));
    noRecentsLayout->addWidget(noRecentsPlaceholder, 0, Qt::AlignCenter);
    noRecentsLayout->addSpacing(Utils::scale_value(36));

    const QString text = isSearchable ? QT_TRANSLATE_NOOP("placeholders", "Find a colleague by email and write to him")
        : (type_ == Type::Recents ? QT_TRANSLATE_NOOP("placeholders", "Chat list is empty") : QT_TRANSLATE_NOOP("placeholders", "Contact list is empty"));

    noRecentsTextWidget_ = new TextWidget(this, text, Data::MentionMap(), TextRendering::LinksVisible::DONT_SHOW_LINKS);
    noRecentsTextWidget_->init(Fonts::appFontScaled(20, Fonts::FontWeight::SemiBold), Styling::getParameters().getColor(Styling::StyleVariable::TEXT_SOLID), QColor(), QColor(), QColor(), TextRendering::HorAligment::CENTER);
    noRecentsTextWidget_->setContextMenuPolicy(Qt::ContextMenuPolicy::NoContextMenu);
    Testing::setAccessibleName(noRecentsTextWidget_, type_ == Type::Recents ? qsl("AS noRecentsLabel") : qsl("AS noContacstLabel"));
    noRecentsLayout->addWidget(noRecentsTextWidget_, 0, Qt::AlignCenter);

    if (!isSearchable)
    {
        noRecentsLayout->addSpacing(Utils::scale_value(24));

        noRecentsPromt_ = new TextWidget(this, QString(), Data::MentionMap(), TextRendering::LinksVisible::SHOW_LINKS);
        noRecentsPromt_->init(Fonts::appFontScaled(16), Styling::getParameters().getColor(Styling::StyleVariable::TEXT_SOLID), Styling::getParameters().getColor(Styling::StyleVariable::TEXT_PRIMARY_ACTIVE), QColor(), QColor(), TextRendering::HorAligment::CENTER);
        noRecentsPromt_->setLineSpacing(Utils::scale_value(4));
        noRecentsPromt_->setContextMenuPolicy(Qt::ContextMenuPolicy::NoContextMenu);
        noRecentsLayout->addWidget(noRecentsPromt_, 0, Qt::AlignCenter);
        Testing::setAccessibleName(noRecentsPromt_, qsl("AS noRecentsPromt"));

        QObject::connect(noRecentsPromt_, &TextWidget::linkActivated, this, [](const QString& _link) {
            QDesktopServices::openUrl(QUrl(_link));
        });

        updateLink();
        QObject::connect(MyInfo(), &my_info::received, this, &Placeholder::updateLink);
    }

    noRecentsLayout->addSpacing(Utils::scale_value(28));

    auto button = new PlaceholderButton(this);
    button->setText(isSearchable ? QT_TRANSLATE_NOOP("placeholders", "Find") : QT_TRANSLATE_NOOP("placeholders", "Copy"));
    noRecentsLayout->addWidget(button);
    connect(button, &Ui::ClickableTextWidget::clicked, this, [this, isSearchable]() {
        if (!isSearchable)
        {
            copyLink(link_);
        }
        else
        {
            if (type_ == Type::Recents)
            {
                emit Utils::InterConnector::instance().setSearchFocus();
                emit Utils::InterConnector::instance().showSearchDropdownFull();
            }
            else
            {
                emit Utils::InterConnector::instance().setContactSearchFocus();
            }
        }
    });

    noRecentsLayout->addSpacerItem(new QSpacerItem(Utils::scale_value(1), Utils::scale_value(1), QSizePolicy::Maximum, QSizePolicy::Maximum));

    Testing::setAccessibleName(noRecentsWidget_, type_ == Type::Recents ? qsl("AS noRecentsWidget") : qsl("AS noContactsWidget"));
    mainLayout->addWidget(noRecentsWidget_);
}

void Placeholder::setPictureOnlyView(bool _isPictureOnly)
{
    isPicrureOnly_ = _isPictureOnly;
    noRecentsWidget_->setVisible(!isPicrureOnly_);
}

void Placeholder::resizeEvent(QResizeEvent *_event)
{
    noRecentsWidget_->setVisible(!isPicrureOnly_);
    if (isPicrureOnly_)
        return;
    const auto m = noRecentsWidget_->layout()->contentsMargins();

    for (auto w : { noRecentsTextWidget_, noRecentsPromt_ })
        if (w)
            w->setMaxWidthAndResize(_event->size().width() - m.right() - m.left());
}

void Placeholder::updateLink()
{
    if (noRecentsPromt_ && !config::get().is_on(config::features::searchable_recents_placeholder))
    {
        link_ = makeLink();
        const auto text = QT_TRANSLATE_NOOP("placeholders", "Share the link %1 so people can write you").arg(link_);
        noRecentsPromt_->setText(text);
    }
}

RecentsPlaceholder::RecentsPlaceholder(QWidget *_parent)
    : Placeholder(_parent, Placeholder::Type::Recents)
{
}


////////////////////////////////////////////////////////////////////////////
ContactsPlaceholder::ContactsPlaceholder(QWidget *_parent)
    : Placeholder(_parent, Placeholder::Type::Contacts)
{
}


////////////////////////////////////////////////////////////////////////////
RotatingSpinner::RotatingSpinner(QWidget * _parent)
    : QWidget(_parent)
    , currentAngle_(0.)
{
    setFixedSize(Utils::scale_value(QSize(32, 32)));
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setStyleSheet(qsl("background: transparent;"));
}

RotatingSpinner::~RotatingSpinner()
{
    stopAnimation();
}

void RotatingSpinner::startAnimation(const QColor& _spinnerColor, const QColor& _bgColor)
{
    constexpr std::chrono::milliseconds duration = std::chrono::seconds(2);

    if (_spinnerColor.isValid())
        mainColor_ = _spinnerColor;
    else
        mainColor_ = Styling::getParameters().getColor(Styling::StyleVariable::PRIMARY);
    assert(mainColor_.alpha() == 255);

    if (_bgColor.isValid())
        bgColor_ = _bgColor;
    else
        bgColor_ = Styling::getParameters().getColor(Styling::StyleVariable::BASE_BRIGHT_INVERSE);

    anim_.finish();
    anim_.start([this]()
    {
        currentAngle_ = anim_.current();
        update();
    }, 0, 360, duration.count(), anim::linear, -1);
}

void RotatingSpinner::stopAnimation()
{
    anim_.finish();
}

void RotatingSpinner::paintEvent(QPaintEvent* _event)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setBrush(Qt::NoBrush);

    static const auto penWidth = Utils::scale_value(2);
    static const auto r2 = rect().adjusted(penWidth / 2, penWidth / 2, -(penWidth / 2), -(penWidth / 2));
    if (bgColor_.isValid())
    {
        p.setPen(QPen(bgColor_, penWidth));
        p.drawEllipse(r2);
    }

    constexpr auto QT_ANGLE_MULT = 16;
    constexpr double idleProgressValue = 0.75;
    const auto animAngle = anim_.isRunning() ? anim_.current() : 0.0;
    const auto baseAngle = (animAngle * QT_ANGLE_MULT);
    const auto progressAngle = (int)std::ceil(idleProgressValue * 360 * QT_ANGLE_MULT);

    p.setPen(QPen(mainColor_, penWidth));
    p.drawArc(r2, -baseAngle, -progressAngle);
}
