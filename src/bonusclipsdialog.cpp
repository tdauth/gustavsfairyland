#include "bonusclipsdialog.h"
#include "fairytale.h"
#include "bonusclip.h"

BonusClipsDialog::BonusClipsDialog(fairytale *app, QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f), m_app(app)
{
	setupUi(this);

	connect(backPushButton, &QPushButton::clicked, this, &QDialog::reject);
}

void BonusClipsDialog::playBonusClip()
{
	QPushButton *button = dynamic_cast<QPushButton*>(QObject::sender());

	if (button != nullptr)
	{
		Buttons::iterator iterator = this->m_buttons.find(button);

		if (iterator != this->m_buttons.end())
		{
			Button *button = iterator.value();

			m_app->playBonusClip(button->clipKey());
		}
	}
}

void BonusClipsDialog::showEvent(QShowEvent *event)
{
	QDialog::showEvent(event);

	update();
}

void BonusClipsDialog::update()
{
	foreach (Button *button, m_buttons.values())
	{
		delete button;
	}

	m_buttons.clear();

	int i = 0;

	for (fairytale::BonusClipUnlocks::const_iterator iterator = m_app->bonusClipUnlocks().begin(); iterator != m_app->bonusClipUnlocks().end(); ++iterator)
	{
		if (iterator.value())
		{
			BonusClip *bonusClip = m_app->getBonusClipByKey(iterator.key());

			if (bonusClip != nullptr)
			{
				QPushButton *pushButton = new QPushButton(bonusClip->description(), this);
				pushButton->setIconSize(QSize(32, 32));
				pushButton->setIcon(QIcon(m_app->resolveClipUrl(bonusClip->imageUrl()).toLocalFile()));

				QVBoxLayout *layout = dynamic_cast<QVBoxLayout*>(contentWidget->layout());
				layout->insertWidget(i, pushButton, 0, Qt::AlignCenter);
				connect(pushButton, &QPushButton::clicked, this, &BonusClipsDialog::playBonusClip);
				m_buttons.insert(pushButton, new Button(iterator.key(), pushButton, this));

				++i;
			}
			else
			{
				qDebug() << "Bonus clip" << iterator.key() << "could not be found!";
			}
		}
	}
}

BonusClipsDialog::Button::Button(const fairytale::ClipKey &clipKey, QPushButton *button, QObject *parent) : QObject(parent), m_clipKey(clipKey), m_button(button)
{
}

BonusClipsDialog::Button::~Button()
{
	delete m_button;
}

fairytale::ClipKey BonusClipsDialog::Button::clipKey() const
{
	return this->m_clipKey;
}

#include "bonusclipsdialog.moc"
