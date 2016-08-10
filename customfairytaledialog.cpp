#include <QtGui>

#include "customfairytaledialog.h"
#include "fairytale.h"
#include "iconbutton.h"

#include "clip.h"

void CustomFairytaleDialog::addClip(Clip *clip)
{
	m_clips.push_back(clip);
	IconButton *button = new IconButton(this);
	this->horizontalLayout->addWidget(button);
	this->m_clipButtons.push_back(button);
	button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	button->setMaximumSize(QSize(64, 64));
	button->setFile(m_app->resolveClipUrl(clip->imageUrl()).toLocalFile());
	button->setEnabled(true); // dont grey out the clip icon
	this->playFinalVideoPushButton->setEnabled(true);
}

CustomFairytaleDialog::CustomFairytaleDialog(fairytale *app, QWidget *parent) : QDialog(parent), m_app(app)
{
	setupUi(this);
	this->textBrowser->setAlignment(Qt::AlignCenter);
	this->setModal(true);

	connect(this->playFinalVideoPushButton, &QPushButton::clicked, m_app, &fairytale::playFinalVideo);
	connect(this->buttonBox->button(QDialogButtonBox::Cancel), &QPushButton::clicked, this, &CustomFairytaleDialog::reject);
}

void CustomFairytaleDialog::clear()
{
	qDebug() << "Clearing custom fairytale dialog";

	this->m_clips.clear();
	this->textBrowser->clear();

	foreach (QAbstractButton *button, this->m_clipButtons)
	{
		delete button;
	}

	qDebug() << "After deleting icon buttons";

	this->m_clipButtons.clear();
	this->playFinalVideoPushButton->setEnabled(false);

	qDebug() << "After the rest";
}

void CustomFairytaleDialog::showEvent(QShowEvent *event)
{
	if (!m_clips.empty())
	{
		QString text = tr("Es war einmal vor langer Zeit, da lebte %1 und es begab sich Folgendes:<br/>").arg(m_clips[0]->description());
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

		text += tr("Und wenn %1 nicht gestorben ist, dann lebt %1 noch heute.<br/>Ende").arg(m_clips[0]->description());

		textBrowser->setText(text);
	}

	QWidget::showEvent(event);
}

