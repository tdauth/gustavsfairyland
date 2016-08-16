#include <QtGui>

#include "customfairytaledialog.h"
#include "fairytale.h"
#include "iconlabel.h"
#include "clip.h"

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

CustomFairytaleDialog::CustomFairytaleDialog(fairytale *app, QWidget *parent) : QDialog(parent), m_app(app)
{
	setupUi(this);
	this->setModal(true);

	connect(this->playFinalVideoPushButton, &QPushButton::clicked, m_app, &fairytale::playFinalVideo);
	connect(this->buttonBox->button(QDialogButtonBox::Cancel), &QPushButton::clicked, this, &CustomFairytaleDialog::reject);
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

void CustomFairytaleDialog::showEvent(QShowEvent *event)
{
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

