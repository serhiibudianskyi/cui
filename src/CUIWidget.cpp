#include "CUIWidget.h"

uint32_t CUI::Widget::nextId_ = 0;

CUI::Widget::Widget(CUI::Widget *parent) : id_(++nextId_), parent_(parent)
{
    TRC::Trace(TRC::Type::INFO, "%s: id(%ld)", __PRETTY_FUNCTION__, id_);
}

CUI::Widget::~Widget()
{
    TRC::Trace(TRC::Type::INFO, "%s: id(%ld)", __PRETTY_FUNCTION__, id_);
}

std::uint32_t CUI::Widget::getId() const
{
    return id_;
}

CUI::Widget *CUI::Widget::getParent() const
{
    return parent_;
}

void CUI::Widget::setSize(const CUI::Size &size)
{
    size_ = size;
}

CUI::Size CUI::Widget::getSize() const
{
    return size_;
}

void CUI::Widget::setMinSize(const CUI::Size &size)
{
    minSize_ = size;
}

CUI::Size CUI::Widget::getMinSize() const
{
    return minSize_;
}

void CUI::Widget::setMaxSize(const CUI::Size &size)
{
    maxSize_ = size;
}

CUI::Size CUI::Widget::getMaxSize() const
{
    return maxSize_;
}

void CUI::Widget::setPadding(const CUI::Padding &padding)
{
    padding_ = padding;
}

CUI::Padding CUI::Widget::getPadding() const
{
    return padding_;
}

void CUI::Widget::setPosition(const CUI::Position &position)
{
    position_ = position;
}

CUI::Position CUI::Widget::getPosition() const
{
    return position_;
}

CUI::Position CUI::Widget::getAbsolutePosition() const
{
    CUI::Position result = getPosition();

    if (parent_)
    {
        result += parent_->getAbsolutePosition() + CUI::Position{parent_->getPadding().left_,  parent_->getPadding().top_};
    }

    return result;
}

void CUI::Widget::setDirection(const CUI::Direction &direction)
{
    direction_ = direction;
}

CUI::Direction CUI::Widget::getDirection() const
{
    return direction_;
}

void CUI::Widget::setActive(bool active)
{
    active_ = active;
}

bool CUI::Widget::isActive() const
{
    return active_;
}

void CUI::Widget::activate()
{
    setActive(true);
    
    getParent() ? getParent()->refresh() : refresh();
}

void CUI::Widget::deactivate()
{
    setActive(false);

    getParent() ? getParent()->refresh() : refresh();
}