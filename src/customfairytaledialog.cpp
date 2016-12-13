#include <QtGui>
#include <QtWidgets>

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

		QMessageBox::information(this, tr("Saved Custom Fairytale"), tr("Successfully saved custom fairytale \"%1\".").arg(text));
	}
}

void CustomFairytaleDialog::retry()
{
	// Store this flag and let the game know that it should start a retry. Directly retrying from here leads to exec() recursion.
	this->m_retry = true;
	// Retry with the same difficulty.
	this->m_retryDifficulty = this->m_app->difficulty();
	this->close();
}

void CustomFairytaleDialog::retryEasier()
{
	// Store this flag and let the game know that it should start a retry. Directly retrying from here leads to exec() recursion.
	this->m_retry = true;
	// Retry with a easier difficulty.
	switch (this->m_app->difficulty())
	{
		case fairytale::Difficulty::Easy:
		{
			this->m_retryDifficulty = fairytale::Difficulty::Easy;
			break;
		}

		case fairytale::Difficulty::Normal:
		{
			this->m_retryDifficulty = fairytale::Difficulty::Easy;
			break;
		}

		case fairytale::Difficulty::Hard:
		{
			this->m_retryDifficulty = fairytale::Difficulty::Normal;
			break;
		}

		case fairytale::Difficulty::Mahlerisch:
		{
			this->m_retryDifficulty = fairytale::Difficulty::Hard;
			break;
		}
	}

	this->close();
}

void CustomFairytaleDialog::retryHarder()
{
	// Store this flag and let the game know that it should start a retry. Directly retrying from here leads to exec() recursion.
	this->m_retry = true;
	// Retry with a harder difficulty.
	switch (this->m_app->difficulty())
	{
		case fairytale::Difficulty::Easy:
		{
			this->m_retryDifficulty = fairytale::Difficulty::Normal;
			break;
		}

		case fairytale::Difficulty::Normal:
		{
			this->m_retryDifficulty = fairytale::Difficulty::Hard;
			break;
		}

		case fairytale::Difficulty::Hard:
		{
			this->m_retryDifficulty = fairytale::Difficulty::Mahlerisch;
			break;
		}

		case fairytale::Difficulty::Mahlerisch:
		{
			this->m_retryDifficulty = fairytale::Difficulty::Mahlerisch;
			break;
		}
	}

	this->close();
}

void CustomFairytaleDialog::addClip(const fairytale::ClipKey &clipKey)
{
	m_clips.push_back(clipKey);
	IconLabel *label = new IconLabel(this);
	this->horizontalLayout->addWidget(label);
	this->m_clipLabels.push_back(label);
	label->setAlignment(Qt::AlignCenter);
	label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	const QSize iconSize = QSize(64, 64);
	label->setMinimumSize(iconSize);
	label->setMaximumSize(iconSize);

	const Clip *clip = m_app->getClipByKey(clipKey);

#ifndef Q_OS_ANDROID
	const QString imageFile = m_app->resolveClipUrl(clip->imageUrl()).toLocalFile();
#else
	const QString imageFile = m_app->resolveClipUrl(clip->imageUrl()).url();
#endif
	label->setFile(imageFile);
	label->setEnabled(true); // dont grey out the clip icon
	this->playFinalVideoPushButton->show();
	this->savePushButton->show();
}

CustomFairytaleDialog::CustomFairytaleDialog(fairytale *app, QWidget *parent) : QDialog(parent), m_app(app), m_retry(false), m_retryDifficulty(fairytale::Difficulty::Normal)
{
	setupUi(this);
	this->setModal(true);

	connect(this->playFinalVideoPushButton, &QPushButton::clicked, m_app, &fairytale::playFinalVideo);
	connect(this->savePushButton, &QPushButton::clicked, this, &CustomFairytaleDialog::save);
	connect(this->okPushButton, &QPushButton::clicked, this, &CustomFairytaleDialog::accept);
	connect(this->retryPushButton, &QPushButton::clicked, this, &CustomFairytaleDialog::retry);
	connect(this->retryEasierPushButton, &QPushButton::clicked, this, &CustomFairytaleDialog::retryEasier);
	connect(this->retryHarderPushButton, &QPushButton::clicked, this, &CustomFairytaleDialog::retryHarder);
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
	this->playFinalVideoPushButton->hide();
	this->savePushButton->hide();

	qDebug() << "After the rest";
}

void CustomFairytaleDialog::changeEvent(QEvent *event)
{
	switch(event->type())
	{
		// this event is send if a translator is loaded
		case QEvent::LanguageChange:
		{
			qDebug() << "Retranslate UI of custom fairytale dialog";
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

			++i;
		}

		if (this->m_app->gameMode()->state() == GameMode::State::Won)
		{
			text += tr("And if %1 did not die then %2 is still alive today.<br/>End").arg(firstClip->description()).arg(firstClip->description());
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

	/*
	 * If no clips exist allow fast retrying.
	 */
	if (m_clips.isEmpty())
	{
		retryPushButton->setFocus(Qt::TabFocusReason);
	}
	/*
	 * Otherwise allow fast playing of the video.
	 */
	else
	{
		playFinalVideoPushButton->setFocus(Qt::TabFocusReason);
	}

	this->retryEasierPushButton->setVisible(this->m_app->difficulty() != fairytale::Difficulty::Easy);
	this->retryHarderPushButton->setVisible(this->m_app->difficulty() != fairytale::Difficulty::Mahlerisch);

	QWidget::showEvent(event);
}

