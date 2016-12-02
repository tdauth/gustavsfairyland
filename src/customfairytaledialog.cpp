#include <iostream>

#include <QtGui>
#include <QInputDialog>
#include <QMessageBox>

#include "customfairytaledialog.h"
#include "fairytale.h"
#include "iconlabel.h"
#include "clip.h"
#include "customfairytale.h"
#include "clippackage.h"
#include "gamemode.h"

void CustomFairytaleDialog::save()
{
	bool ok = false;
	const QString text = QInputDialog::getText(this, tr("Custom Fairytale Name"), tr("Name of custom fairytale:"), QLineEdit::Normal, "", &ok);

	if (ok && !text.isEmpty())
	{
		// Check if name is already used. It must be unique since it is used as key.
		if (m_app->customFairytales().find(text) != m_app->customFairytales().end())
		{
			if (QMessageBox::question(this, tr("Overwrite existing custom fairytale?"), tr("Do you want to overwrite the existing custom fairytale?")) == QMessageBox::No)
			{
				// Select another name.
				save();

				return;
			}
		}

		CustomFairytale *customFairytale = new CustomFairytale(m_app);
		customFairytale->setName(text);
		customFairytale->setClipIds(m_clips);
		m_app->addCustomFairytale(customFairytale);
	}
}

void CustomFairytaleDialog::retry()
{
	// Store this flag and let the game know that it should start a retry. Directly retrying from here leads to exec() recursion.
	this->m_retry = true;
	this->close();
}

void CustomFairytaleDialog::addClip(const CustomFairytale::ClipKey &clipKey)
{
	m_clips.push_back(clipKey);
	IconLabel *label = new IconLabel(this);
	this->horizontalLayout->addWidget(label);
	this->m_clipLabels.push_back(label);
	label->setAlignment(Qt::AlignCenter);
	label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	label->setMaximumSize(QSize(64, 64));

	const Clip *clip = m_app->getClipByKey(clipKey);

#ifndef Q_OS_ANDROID
	const QString imageFile = m_app->resolveClipUrl(clip->imageUrl()).toLocalFile();
#else
	const QString imageFile = m_app->resolveClipUrl(clip->imageUrl()).url();
#endif
	label->setFile(imageFile);
	label->setEnabled(true); // dont grey out the clip icon
	this->playFinalVideoPushButton->setEnabled(true);
	this->savePushButton->setEnabled(true);
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
	this->savePushButton->setEnabled(false);

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
		Clip *firstClip = m_app->getClipByKey(m_clips[0]);
		Clip *startPersonClip = m_app->getClipByKey(m_app->startPerson());

		QString text = tr("Once Upon a time there lived %1 and the following happened:<br/>").arg(firstClip->description());
		int i = 0;
		QStringList descriptionOfAllInvolved;

		foreach (fairytale::ClipKey clipKey, m_clips)
		{
			Clip *clip = m_app->getClipByKey(clipKey);

			if (i > 0)
			{
				text += " ";
			}

			text += m_app->description(startPersonClip, i, clip, false);

			if ((i > 0 && i % 2 == 0) || (i == m_clips.size() - 1))
			{
				text += "<br/>";
			}

			if (clip->isPerson())
			{
				QString description;
				description += clip->description();
				descriptionOfAllInvolved.push_back(description);
			}

			++i;
		}

		if (this->m_app->gameMode()->state() == GameMode::State::Won)
		{
			const QString formattedDescription = descriptionOfAllInvolved.join(tr(" and "));
			text += tr("And if %1 did not die then %2 %3 still alive today.<br/>End").arg(formattedDescription).arg(formattedDescription).arg((descriptionOfAllInvolved.size() > 1 ? tr("are") : tr("is")));
		}
		else
		{
			if (!text.endsWith("<br/>"))
			{
				text.append("<br/>");
			}

			text += tr("But nobody knows what happened next.<br/>End");
		}

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

