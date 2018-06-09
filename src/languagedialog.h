#ifndef LANGUAGEDIALOG_H
#define LANGUAGEDIALOG_H

#include <QDialog>

#include "ui_languagedialog.h"

/**
 * \brief A dialog to select a language tag for a clip package.
 *
 * After calling the exec() method you can use \ref getLanguage() to get the selected language.
 */
class LanguageDialog : public QDialog, protected Ui::LanguageDialog
{
	public:
		explicit LanguageDialog(QWidget *parent);

		/**
		 * \return Returns the selected language tag such as "de" or "en".
		 */
		QString getLanguage() const;
};

#endif // LANGUAGEDIALOG_H
