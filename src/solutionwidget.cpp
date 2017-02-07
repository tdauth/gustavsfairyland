#include <QtGui>

#include "solutionwidget.h"
#include "floatingclip.h"
#include "iconlabel.h"
#include "clip.h"

SolutionWidget::SolutionWidget(fairytale *app, QWidget *parent) : QWidget(parent), m_app(app), m_solveCounter(0)
{
	this->setWindowFlags(this->windowFlags() | Qt::WindowStaysOnTopHint);
	this->setAcceptDrops(true);
	this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	this->setLayout(new QHBoxLayout());
}

void SolutionWidget::fail(const fairytale::ClipKey &clipKey)
{
	emit failed(clipKey);
}

void SolutionWidget::dragEnterEvent(QDragEnterEvent *event)
{
	FloatingClip *floatingClip = dynamic_cast<FloatingClip*>(event->source());

	if (floatingClip != nullptr)
	{
		qDebug() << "Accept drop!";
		event->acceptProposedAction();
	}
}

void SolutionWidget::dropEvent(QDropEvent *event)
{
	qDebug() << "Drop event!";
	FloatingClip *floatingClip = dynamic_cast<FloatingClip*>(event->source());

	if (floatingClip != nullptr)
	{
		bool found = false;

		for (IconLabels::iterator iterator = m_iconLabels.begin(); iterator != m_iconLabels.end() && !found; ++iterator)
		{
			const IconLabel *iconLabel = iterator.value();
			const QPoint iconLabelPos = iconLabel->mapToGlobal(QPoint(0, 0));
			const QRect rect = QRect(iconLabelPos.x(), iconLabelPos.y(), iconLabel->width(), iconLabel->height());
			const QPoint pos = this->mapToGlobal(event->pos());
			qDebug() << "Checking event position" << pos << "at rect" << rect;

			if (rect.contains(pos))
			{
				if (iterator.key() == floatingClip->clipKey())
				{
					iterator.value()->setEnabled(true); // show without grey

					event->acceptProposedAction();
					m_solveCounter++;

					emit solved(iterator.key());
				}
				else
				{
					// accept but reset it in the game mode later
					event->acceptProposedAction();
					emit failed(iterator.key());
				}

				found = true;
			}
		}

		if (!found)
		{
			qDebug() << "Found no matching icon label!";

			// accept but reset it in the game mode later
			event->acceptProposedAction();
			emit failed(floatingClip->clipKey());
		}
	}
	else
	{
		qDebug() << "Unable to find corresponding floating clip.";
	}

	QWidget::dropEvent(event);
}

void SolutionWidget::addClip(const fairytale::ClipKey &clipKey)
{
	IconLabel *iconLabel = new IconLabel(this);
	iconLabel->setMinimumSize(128, 128);
	iconLabel->setEnabled(false); // show with grey as long as the clip is not dragged and dropped
	iconLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	m_iconLabels.insert(clipKey, iconLabel);
	const Clip *clip = m_app->getClipByKey(clipKey);
	iconLabel->setFile(m_app->resolveClipUrl(clip->imageUrl()).toLocalFile());
	this->layout()->addWidget(iconLabel);
	this->layout()->setAlignment(iconLabel, Qt::AlignCenter);
}

void SolutionWidget::clearClips()
{
	m_solveCounter = 0;

	for (IconLabel *iconLabel : m_iconLabels.values())
	{
		delete iconLabel;
	}

	m_iconLabels.clear();
}

bool SolutionWidget::solvedAll() const
{
	return m_solveCounter == m_iconLabels.size();
}