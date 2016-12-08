#include <QtGui>
#include <QtWidgets>

#include "fairytalesdialog.h"
#include "fairytale.h"
#include "customfairytale.h"
#include "iconlabel.h"
#include "clip.h"

FairytalesDialog::FairytalesDialog(fairytale *app, QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f), m_app(app)
{
	setupUi(this);

	connect(okPushButton, &QPushButton::clicked, this, &QDialog::reject);
}

FairytalesDialog::~FairytalesDialog()
{
	clearFairytales();
}

void FairytalesDialog::playFairytale()
{
	QPushButton *button = dynamic_cast<QPushButton*>(QObject::sender());

	if (button != nullptr)
	{
		QWidget *parent = button->parentWidget();
		Fairytales::iterator iterator = this->m_fairytales.find(parent);

		if (iterator != this->m_fairytales.end())
		{
			Fairytale *fairytale = iterator.value();

			fairytale::CustomFairytales::const_iterator fairytaleIterator = m_app->customFairytales().find(fairytale->key());

			if (fairytaleIterator != m_app->customFairytales().end())
			{
				m_app->playCustomFairytale(fairytaleIterator.value());
			}
		}
	}
}

void FairytalesDialog::deleteFairytale()
{
	QPushButton *button = dynamic_cast<QPushButton*>(QObject::sender());

	if (button != nullptr)
	{
		QWidget *parent = button->parentWidget();
		Fairytales::iterator iterator = this->m_fairytales.find(parent);

		if (iterator != this->m_fairytales.end())
		{
			Fairytale *fairytale = iterator.value();

			fairytale::CustomFairytales::const_iterator fairytaleIterator = m_app->customFairytales().find(fairytale->key());

			if (fairytaleIterator != m_app->customFairytales().end())
			{
				// Delete it after asking for permission

				if (QMessageBox::question(this, tr("Delete Fairytale"), tr("Do you really want to delete the fairytale?")) == QMessageBox::Yes)
				{
					m_app->removeCustomFairytale(fairytaleIterator.value());
					m_fairytales.erase(iterator);

					delete fairytale;
					fairytale = nullptr;
				}
			}
		}
	}
}

void FairytalesDialog::showEvent(QShowEvent *event)
{
	QDialog::showEvent(event);

	update();
}

void FairytalesDialog::changeEvent(QEvent *event)
{
	switch(event->type())
	{
		// this event is send if a translator is loaded
		case QEvent::LanguageChange:
		{
			qDebug() << "Retranslate UI of fairytales dialog";
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

void FairytalesDialog::clearFairytales()
{
	foreach (Fairytale *fairytale, m_fairytales.values())
	{
		delete fairytale;
	}

	m_fairytales.clear();
}

void FairytalesDialog::update()
{
	clearFairytales();

	for (fairytale::CustomFairytales::const_iterator iterator = m_app->customFairytales().begin(); iterator != m_app->customFairytales().end(); ++iterator)
	{
		CustomFairytale *customFairytale = iterator.value();
		QScrollArea *scrollArea = new QScrollArea(); // dont use this as parent since it is deleted automatically when fairytale is deleted
		scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		QWidget *widget = new QWidget(scrollArea);
		widget->setLayout(new QHBoxLayout());
		widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

		QPushButton *pushButton = new QPushButton(tr("Play"), widget);
		widget->layout()->addWidget(pushButton);
		connect(pushButton, &QPushButton::clicked, this, &FairytalesDialog::playFairytale);
		pushButton->setIconSize(QSize(32, 32));
		pushButton->setIcon(QIcon(":/themes/oxygen/32x32/actions/media-playback-start.png"));

		QPushButton *deletePushButton = new QPushButton(tr("Delete"), widget);
		widget->layout()->addWidget(deletePushButton);
		connect(deletePushButton, &QPushButton::clicked, this, &FairytalesDialog::deleteFairytale);
		deletePushButton->setIconSize(QSize(32, 32));
		deletePushButton->setIcon(QIcon(":/themes/oxygen/32x32/actions/dialog-close.png"));

		QLabel *label = new QLabel(customFairytale->name(), widget);
		widget->layout()->addWidget(label);

		for (CustomFairytale::ClipIds::const_iterator clipIdIterator = customFairytale->clipIds().begin(); clipIdIterator != customFairytale->clipIds().end(); ++clipIdIterator)
		{
			IconLabel *iconLabel = new IconLabel(widget);
			widget->layout()->addWidget(iconLabel);
			iconLabel->setAlignment(Qt::AlignCenter);
			iconLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
			const QSize iconSize = QSize(128, 128);
			iconLabel->setMinimumSize(iconSize);
			iconLabel->setMaximumSize(iconSize);

			const Clip *clip = m_app->getClipByKey(*clipIdIterator);

			if (clip != nullptr)
			{
#ifndef Q_OS_ANDROID
				const QString imageFile = m_app->resolveClipUrl(clip->imageUrl()).toLocalFile();
#else
				const QString imageFile = m_app->resolveClipUrl(clip->imageUrl()).url();
#endif
				iconLabel->setFile(imageFile);
			}
			else
			{
				qDebug() << "Missing clip" << *clipIdIterator;
			}
		}

		scrollArea->setWidget(widget);
		QVBoxLayout *layout = dynamic_cast<QVBoxLayout*>(contentWidget->layout());
		layout->addWidget(scrollArea);
		m_fairytales.insert(widget, new Fairytale(customFairytale->name(), scrollArea, this));
	}
}

FairytalesDialog::Fairytale::Fairytale(const QString &key, QWidget *widget, QObject *parent) : QObject(parent), m_key(key), m_widget(widget)
{
}

FairytalesDialog::Fairytale::~Fairytale()
{
	delete m_widget;
}

QString FairytalesDialog::Fairytale::key() const
{
	return this->m_key;
}

#include "fairytalesdialog.moc"
