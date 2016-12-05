#include "localedialog.h"
#include "fairytale.h"
#include "bonusclip.h"

LocaleDialog::LocaleDialog(fairytale *app, QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f), m_app(app)
{
	setupUi(this);

	connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
	connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
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

void LocaleDialog::update()
{
	foreach (Button *button, m_buttons.values())
	{
		delete button;
	}

	m_buttons.clear();

	int i = 0;

	for (const QString &locale : m_app->languages())
	{
		QRadioButton *pushButton = new QRadioButton(fairytale::localeToName(locale), this);
		pushButton->setIconSize(QSize(32, 32));
		// TODO set locale icon
		//pushButton->setIcon(QIcon(m_app->resolveClipUrl(bonusClip->imageUrl()).toLocalFile()));

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
