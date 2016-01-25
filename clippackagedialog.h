/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2016  Barade <barade.barade@web.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef CLIPPACKAGEDIALOG_H
#define CLIPPACKAGEDIALOG_H

#include <qt5/QtWidgets/QDialog>

#include "ui_clippackagedialog.h"

#include "fairytale.h"

class ClipPackageDialog : public QDialog, protected Ui::ClipPackageDialog
{
	Q_OBJECT

	public:
		ClipPackageDialog(QWidget* parent, Qt::WindowFlags f = 0);

		void fill(const fairytale::ClipPackages &packages);

		ClipPackage* clipPackage() const;

	private:
		fairytale::ClipPackages m_packages;
};

#endif // CLIPPACKAGEDIALOG_H
