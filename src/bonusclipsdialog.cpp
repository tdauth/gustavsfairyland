#include "bonusclipsdialog.h"
#include "fairytale.h"
#include "bonusclip.h"

BonusClipsDialog::BonusClipsDialog(fairytale *app, QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f), m_app(app)
{
	setupUi(this);

	connect(backPushButton, &QPushButton::clicked, this, &QDialog::reject);
}

BonusClipsDialog::~BonusClipsDialog()
{
	clearButtons();
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

void BonusClipsDialog::changeEvent(QEvent *event)
{
	switch(event->type())
	{
		// this event is send if a translator is loaded
		case QEvent::LanguageChange:
		{
			qDebug() << "Retranslate UI of bonus clips dialog";
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

void BonusClipsDialog::clearButtons()
{
	foreach (Button *button, m_buttons.values())
	{
		delete button;
	}

	m_buttons.clear();
}

void BonusClipsDialog::update()
{
	clearButtons();

	/*
	 * Always sort the bonus clips the same way.
	 */
	QList<fairytale::ClipKey> clipKeys = m_app->bonusClipUnlocks().toList();

	qDebug() << "Clip keys:" << clipKeys;

	std::sort(clipKeys.begin(), clipKeys.end(), [](const fairytale::ClipKey &a, const fairytale::ClipKey &b)
		{
			if (a.first == b.first)
			{
				return QString::compare(a.second, b.second) < 0;
			}

			return QString::compare(a.first, b.first) < 0;
		});

	int i = 0;

	for (const fairytale::ClipKey &clipKey : clipKeys)
	{
		BonusClip *bonusClip = m_app->getBonusClipByKey(clipKey);

		if (bonusClip != nullptr)
		{
			QPushButton *pushButton = new QPushButton(bonusClip->description());
			pushButton->setIconSize(QSize(32, 32));
			pushButton->setIcon(QIcon(m_app->resolveClipUrl(bonusClip->imageUrl()).toLocalFile()));
			pushButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

			QVBoxLayout *layout = dynamic_cast<QVBoxLayout*>(contentWidget->layout());
			layout->insertWidget(i, pushButton);
			connect(pushButton, &QPushButton::clicked, this, &BonusClipsDialog::playBonusClip);
			m_buttons.insert(pushButton, new Button(clipKey, pushButton, this));

			++i;
		}
		else
		{
			qDebug() << "Bonus clip" << clipKey << "could not be found!";
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
