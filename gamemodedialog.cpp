#include "gamemodedialog.h"
#include "gamemode.h"

#include "gamemodedialog.moc"

GameModeDialog::GameModeDialog(QWidget* parent, Qt::WindowFlags f): QDialog(parent, f)
{
	this->setupUi(this);
}

void GameModeDialog::fill(const fairytale::GameModes &gameModes)
{
	this->m_gameModes = gameModes;

	this->comboBox->clear();

	for (fairytale::GameModes::const_iterator iterator = gameModes.begin(); iterator != gameModes.end(); ++iterator)
	{
		const GameMode *gameMode = iterator.value();
		this->comboBox->addItem(gameMode->name(), gameMode->id());
	}

	if (gameModes.isEmpty())
	{
		this->comboBox->setEnabled(false);
		this->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
	}
	else
	{
		this->comboBox->setEnabled(true);
		this->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
	}
}

GameMode* GameModeDialog::gameMode() const
{
	if (this->m_gameModes.isEmpty())
	{
		return nullptr;
	}

	return this->m_gameModes[this->comboBox->currentData().toString()];
}
