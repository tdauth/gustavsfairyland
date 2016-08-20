#include <iostream>

#include <QtGui>
#include <QInputDialog>

#include "customfairytaledialog.h"
#include "fairytale.h"
#include "iconlabel.h"
#include "clip.h"
#include "customfairytale.h"
#include "clippackage.h"

void CustomFairytaleDialog::save()
{
	bool ok = false;
	const QString text = QInputDialog::getText(this, tr("Custom Fairytale Name"), tr("Name of custom fairytale:"), QLineEdit::Normal, "", &ok);

	if (ok && !text.isEmpty())
	{
		// TODO check if name is already used. It must be unique?

		CustomFairytale *customFairytale = new CustomFairytale(m_app);
		customFairytale->setPackageId(this->m_app->clipPackage()->id());
		customFairytale->setName(text);
		CustomFairytale::ClipIds clipIds;

		for (int i = 0; i < m_clips.size(); ++i)
		{
			Clip *clip = m_clips.at(i);
			clipIds.push_back(clip->id());
		}

		customFairytale->setClipIds(clipIds);
		m_app->addCustomFairytale(customFairytale);
	}
}

void CustomFairytaleDialog::retry()
{
	// Store this flag and let the game know that it should start a retry. Directly retrying from here leads to exec() recursion.
	this->m_retry = true;
	this->close();
}

void CustomFairytaleDialog::addClip(Clip *clip)
{
	m_clips.push_back(clip);
	IconLabel *label = new IconLabel(this);
	this->horizontalLayout->addWidget(label);
	this->m_clipLabels.push_back(label);
	label->setAlignment(Qt::AlignCenter);
	label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	label->setMaximumSize(QSize(64, 64));
#ifndef Q_OS_ANDROID
	const QString imageFile = m_app->resolveClipUrl(clip->imageUrl()).toLocalFile();
#else
	const QString imageFile = m_app->resolveClipUrl(clip->imageUrl()).url();
#endif
	label->setFile(imageFile);
	label->setEnabled(true); // dont grey out the clip icon
	this->playFinalVideoPushButton->setEnabled(true);
}

CustomFairytaleDialog::CustomFairytaleDialog(fairytale *app, QWidget *parent) : QDialog(parent), m_app(app), m_retry(false)
{
	setupUi(this);
	this->setModal(true);

	connect(this->playFinalVideoPushButton, &QPushButton::clicked, m_app, &fairytale::playFinalVideo);
	connect(this->savePushButton, &QPushButton::clicked, this, &CustomFairytaleDialog::save);
	connect(this->okPushButton, &QPushButton::clicked, this, &CustomFairytaleDialog::accept);
	connect(this->retryPushButton, &QPushButton::clicked, this, &CustomFairytaleDialog::retry);
}

void CustomFairytaleDialog::clear()
{
	qDebug() << "Clearing custom fairytale dialog";

	this->m_clips.clear();
	this->textBrowser->clear();

	foreach (IconLabel *label, this->m_clipLabels)
	{
		delete label;
	}

	qDebug() << "After deleting icon buttons";

	this->m_clipLabels.clear();
	this->playFinalVideoPushButton->setEnabled(false);

	qDebug() << "After the rest";
}

void CustomFairytaleDialog::changeEvent(QEvent* event)
{
	switch(event->type())
	{
		// this event is send if a translator is loaded
		case QEvent::LanguageChange:
		{
			std::cerr << "Retranslate UI of custom fairytale dialog" << std::endl;
			this->retranslateUi(this);

			break;
		}

		default:
		{
			break;
		}
	}

	QDialog::changeEvent(event);
}

void CustomFairytaleDialog::showEvent(QShowEvent *event)
{
	m_retry = false;

	if (!m_clips.empty())
	{
		QString text = tr("Once Upon a time there lived %1 and the following happened:<br/>").arg(m_clips[0]->description());
		int i = 0;

		foreach (Clip *clip, m_clips)
		{
			if (i > 0)
			{
				text += " ";
			}

			text += m_app->description(i, clip, false);

			if (i > 0 && i % 2 == 0)
			{
				text += "<br>";
			}

			++i;
		}

		text += tr("And if %1 did not die then %1 is still alive today.<br/>End").arg(m_clips[0]->description());

		textBrowser->setText(text);

		// Align text at center
		QTextCursor cursor = textBrowser->textCursor();
		QTextBlockFormat textBlockFormat = cursor.blockFormat();
		textBlockFormat.setAlignment(Qt::AlignCenter);
		cursor.mergeBlockFormat(textBlockFormat);
		textBrowser->setTextCursor(cursor);
	}

	QWidget::showEvent(event);
}

