#include "customfairytaledialog.h"
#include "fairytale.h"
#include "iconbutton.h"
#include <QtGui>

#include "clip.h"

void CustomFairytaleDialog::addClip(Clip* clip)
{
	QPushButton *button = new IconButton(this);
	this->scrollAreaWidgetContents->layout()->addWidget(button);
	this->m_clipButtons.push_back(button);
	button->setIcon(QIcon(clip->imageUrl().toLocalFile()));
	button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	button->setEnabled(false);
	this->playFinalVideoPushButton->setEnabled(true);
}

CustomFairytaleDialog::CustomFairytaleDialog(fairytale* app, QWidget* parent) : QDialog(parent), m_app(app)
{
	setupUi(this);
	this->setModal(true);

	connect(this->playFinalVideoPushButton, &QPushButton::clicked, m_app, &fairytale::playFinalVideo);
	connect(this->buttonBox->button(QDialogButtonBox::Cancel), &QPushButton::clicked, this, &CustomFairytaleDialog::close);
}

void CustomFairytaleDialog::clear()
{
	foreach (QAbstractButton *button, this->m_clipButtons)
	{
		delete button;
	}

	this->m_clipButtons.clear();
	this->playFinalVideoPushButton->setEnabled(false);
}

void CustomFairytaleDialog::closeEvent(QCloseEvent* event)
{
	QDialog::closeEvent(event);

	if (this->m_app->isRunning() && this->m_app->isPaused())
	{
		// unpause game
		this->m_app->pauseGame();
	}
}
