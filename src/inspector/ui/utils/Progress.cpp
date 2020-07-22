#include "Progress.h"

namespace inspector
{

namespace ui
{

ProgressHandler::ProgressHandler(QObject *parent)
  : QObject(parent),
    mMin(0),
    mMax(1),
    mValue(0)
{

}

ProgressHandler::~ProgressHandler()
{

}

void ProgressHandler::setRange(int min, int max)
{
  mMin = min;
  mMax = max;
  emit rangeChange(min, max);
}

void ProgressHandler::setValue(int value)
{
  mValue = value;
  emit valueChange(value);
}

void ProgressHandler::next()
{
  setValue(++mValue);
}

void ProgressHandler::init()
{
  emit initialized();
}

void ProgressHandler::finish()
{
  emit finished();
}

void ProgressHandler::setTitle(const QString &title)
{
  emit titleChange(title);
}

void ProgressHandler::setDescription(const QString &description)
{
  emit descriptionChange(description);
}


} // namespace ui

} // namespace inspector


