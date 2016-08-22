#include "clippackagedialog.h"
#include "gamemode.h"
#include "clippackage.h"

#include "clippackagedialog.moc"

ClipPackageDialog::ClipPackageDialog(QWidget* parent, Qt::WindowFlags f): QDialog(parent, f)
{
	this->setupUi(this);
}

void ClipPackageDialog::fill(const fairytale::ClipPackages &packages, const fairytale::GameModes &gameModes, fairytale *app)
{
	this->m_packages = packages;

	this->packagesComboBox->clear();

	for (fairytale::ClipPackages::const_iterator iterator = packages.begin(); iterator != packages.end(); ++iterator)
	{
		const ClipPackage *clipPackage = iterator.value();
		this->packagesComboBox->addItem(clipPackage->name(), clipPackage->id());
	}

	if (packages.isEmpty())
	{
		this->packagesComboBox->setEnabled(false);
	}
	else
	{
		this->packagesComboBox->setEnabled(true);
	}

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
	this->useMaxRoundsCheckBox->setChecked(app->defaultUseMaxRounds());
	this->maxRoundsSpinBox->setValue(app->defaultMaxRounds());

	if (gameModes.isEmpty() || packages.isEmpty())
	{
		this->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
	}
	else
	{
		this->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
	}
}

ClipPackage* ClipPackageDialog::clipPackage() const
{
	if (this->m_packages.isEmpty())
	{
		return nullptr;
	}

	return this->m_packages[this->packagesComboBox->currentData().toString()];
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
