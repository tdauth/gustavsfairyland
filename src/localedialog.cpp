#include "localedialog.h"
#include "fairytale.h"
#include "bonusclip.h"

LocaleDialog::LocaleDialog(fairytale *app, QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f), m_app(app)
{
	setupUi(this);

	connect(okPushButton, &QPushButton::clicked, this, &QDialog::accept);
}

LocaleDialog::~LocaleDialog()
{
	clearButtons();
}

void LocaleDialog::changeLocale()
{
	QRadioButton *button = dynamic_cast<QRadioButton*>(QObject::sender());

	if (button != nullptr)
	{
		Buttons::iterator iterator = this->m_buttons.find(button);

		if (iterator != this->m_buttons.end())
		{
			Button *button = iterator.value();

			m_app->loadLanguage(button->locale());
		}
	}
}

void LocaleDialog::showEvent(QShowEvent *event)
{
	QDialog::showEvent(event);

	update();
}

void LocaleDialog::changeEvent(QEvent *event)
{
	switch(event->type())
	{
		// this event is send if a translator is loaded
		case QEvent::LanguageChange:
		{
			qDebug() << "Retranslate UI of locale dialog";
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

void LocaleDialog::update()
{
	clearButtons();

	int i = 0;

	for (const QString &locale : m_app->languages())
	{
		QRadioButton *pushButton = new QRadioButton(fairytale::localeToName(locale), this);
		pushButton->setIconSize(QSize(32, 32));
		pushButton->setIcon(QIcon(QString(":/resources/") + locale + ".svg"));
		qDebug() << "Locale" << locale << "Current translation" << m_app->currentTranslation();

		if (locale == m_app->currentTranslation())
		{
			pushButton->setChecked(true);
		}

		QVBoxLayout *layout = dynamic_cast<QVBoxLayout*>(contentWidget->layout());
		layout->insertWidget(i, pushButton, 0, Qt::AlignCenter);
		connect(pushButton, &QPushButton::clicked, this, &LocaleDialog::changeLocale);
		m_buttons.insert(pushButton, new Button(locale, pushButton, this));

		++i;
	}
}

void LocaleDialog::clearButtons()
{
	foreach (Button *button, m_buttons.values())
	{
		delete button;
	}

	m_buttons.clear();
}

LocaleDialog::Button::Button(const QString &locale, QRadioButton *button, QObject *parent) : QObject(parent), m_locale(locale), m_button(button)
{
}

LocaleDialog::Button::~Button()
{
	delete m_button;
}

QString LocaleDialog::Button::locale() const
{
	return this->m_locale;
}

#include "localedialog.moc"
