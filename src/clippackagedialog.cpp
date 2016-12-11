#include <QtWidgets>

#include "clippackagedialog.h"
#include "gamemode.h"
#include "clippackage.h"

#include "clippackagedialog.moc"

ClipPackageDialog::ClipPackageDialog(QWidget* parent, Qt::WindowFlags f): QDialog(parent, f)
{
	this->setupUi(this);

	connect(this->gameModesComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &ClipPackageDialog::currentGameModeIndexChanged);

	this->advancedGroupBox->setChecked(false);
	this->advancedWidget->hide();

	connect(this->okPushButton, &QPushButton::clicked, this, &QDialog::accept);
	connect(this->cancelPushButton, &QPushButton::clicked, this, &QDialog::reject);
}

void ClipPackageDialog::fill(const fairytale::ClipPackages &packages, const fairytale::GameModes &gameModes, fairytale *app)
{
	this->m_app = app;
	this->m_packages = packages;

	foreach (QCheckBox *checkBox, m_checkBoxes)
	{
		delete checkBox;
	}

	m_checkBoxes.clear();

	const QStringList defaultClipPackageKeys = app->defaultClipPackages().keys();
	const QSet<QString> defaultClipPackageKeysSet = QSet<QString>::fromList(defaultClipPackageKeys);

	for (fairytale::ClipPackages::const_iterator iterator = packages.begin(); iterator != packages.end(); ++iterator)
	{
		const ClipPackage *clipPackage = iterator.value();
		//this->packagesComboBox->addItem(clipPackage->name(), clipPackage->id());
		QCheckBox *checkBox = new QCheckBox(this);
		QFont font = checkBox->font();
		font.setPointSize(15);
		checkBox->setFont(font);
		checkBox->setText(clipPackage->name());
		clipPackagesWidget->layout()->addWidget(checkBox);

		checkBox->setChecked(defaultClipPackageKeysSet.contains(clipPackage->id()));
		m_checkBoxes.push_back(checkBox);

		connect(checkBox, &QCheckBox::stateChanged, this, &ClipPackageDialog::validate);
	}

	this->clipPackagesWidget->setEnabled(!packages.isEmpty());

	this->m_gameModes = gameModes;

	this->gameModesComboBox->clear();
	int i = 0;

	for (fairytale::GameModes::const_iterator iterator = gameModes.begin(); iterator != gameModes.end(); ++iterator)
	{
		const GameMode *gameMode = iterator.value();
		this->gameModesComboBox->addItem(gameMode->name(), gameMode->id());

		// Make sure that the default game mode is chosen by default in the combo box.
		if (app->defaultGameMode() == gameMode)
		{
			this->gameModesComboBox->setCurrentIndex(i);
		}

		++i;
	}

	this->difficultyComboBox->setCurrentIndex((int)app->defaultDifficulty());
	this->difficultyComboBox->setEnabled(app->defaultGameMode()->useDifficulty());
	this->useMaxRoundsCheckBox->setChecked(app->defaultUseMaxRounds());
	this->useMaxRoundsCheckBox->setEnabled(app->defaultGameMode()->useMaxRounds());
	this->maxRoundsSpinBox->setValue(app->defaultMaxRounds());
	this->maxRoundsSpinBox->setEnabled(app->defaultGameMode()->useMaxRounds());

	validate();
}

fairytale::ClipPackages ClipPackageDialog::clipPackages() const
{
	fairytale::ClipPackages result;
	fairytale::ClipPackages::const_iterator iterator = m_packages.begin();

	for (int i = 0; i < m_checkBoxes.size() && iterator != m_packages.end(); ++i, ++iterator)
	{
		if (m_checkBoxes[i]->isChecked())
		{
			result.insert(iterator.key(), iterator.value());
		}
	}

	return result;
}

GameMode* ClipPackageDialog::gameMode() const
{
	if (this->m_gameModes.isEmpty())
	{
		return nullptr;
	}

	return this->m_gameModes[this->gameModesComboBox->currentData().toString()];
}

fairytale::Difficulty ClipPackageDialog::difficulty() const
{
	return (fairytale::Difficulty)this->difficultyComboBox->currentIndex();
}

int ClipPackageDialog::maxRounds() const
{
	return this->maxRoundsSpinBox->value();
}

bool ClipPackageDialog::useMaxRounds() const
{
	return this->useMaxRoundsCheckBox->isChecked();
}

void ClipPackageDialog::currentGameModeIndexChanged(int index)
{
	qDebug() << "Changing index to" << index;

	if (index >= 0 && index < this->m_gameModes.size())
	{
		const QString id = this->gameModesComboBox->itemData(index).toString();
		qDebug() << "ID:" << id;
		GameMode *gameMode = this->m_gameModes[id];
		this->difficultyComboBox->setEnabled(gameMode->useDifficulty());
		this->useMaxRoundsCheckBox->setEnabled(gameMode->useMaxRounds());
		this->maxRoundsSpinBox->setEnabled(gameMode->useMaxRounds());
	}
}

void ClipPackageDialog::validate()
{
	const bool gameModesAreEmpty = m_gameModes.isEmpty();
	const bool packagesAreEmpty = m_packages.isEmpty();
	const bool maxRoundsNotBiggerThanZero = m_app->maxRoundsByMultipleClipPackages(clipPackages()) <= 0;

	QString message;

	if (gameModesAreEmpty)
	{
		message = tr("No game modes available.");
	}
	else if (packagesAreEmpty)
	{
		message = tr("No clip packages available.");
	}
	else if (maxRoundsNotBiggerThanZero)
	{
		message = tr("Clip Packages have not enough clips.");
	}

	errorLabel->setText(tr("Error: %1").arg(message));
	errorLabel->setVisible(!message.isEmpty());

	okPushButton->setEnabled(!gameModesAreEmpty && !packagesAreEmpty && !maxRoundsNotBiggerThanZero);
}

void ClipPackageDialog::changeEvent(QEvent *event)
{
	switch(event->type())
	{
		// this event is send if a translator is loaded
		case QEvent::LanguageChange:
		{
			qDebug() << "Retranslate UI of clip package dialog";
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
