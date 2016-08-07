#include <QtGui>

#include "customfairytaledialog.h"
#include "fairytale.h"
#include "iconbutton.h"

#include "clip.h"

void CustomFairytaleDialog::addClip(Clip *clip)
{
	IconButton *button = new IconButton(this);
	this->scrollAreaWidgetContents->layout()->addWidget(button);
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
	this->setModal(true);

	connect(this->playFinalVideoPushButton, &QPushButton::clicked, m_app, &fairytale::playFinalVideo);
	connect(this->buttonBox->button(QDialogButtonBox::Cancel), &QPushButton::clicked, this, &CustomFairytaleDialog::reject);
}

void CustomFairytaleDialog::clear()
{
	qDebug() << "Clearing custom fairytale dialog";

	foreach (QAbstractButton *button, this->m_clipButtons)
	{
		delete button;
	}

	qDebug() << "After deleting icon buttons";

	this->m_clipButtons.clear();
	this->playFinalVideoPushButton->setEnabled(false);

	qDebug() << "After the rest";
}
