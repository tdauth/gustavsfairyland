#ifndef SOLUTIONWIDGET_H
#define SOLUTIONWIDGET_H

#include <QWidget>

#include "fairytale.h"

class IconLabel;

/**
 * \brief This widget lists icons for a fairytale which has to be solved. It accepts drop events from floating clips to solve the complete fairytale.
 *
 * Every drop in the proper clip widget solves one clip.
 */
class SolutionWidget : public QWidget
{
	Q_OBJECT

	signals:
		void solved(const fairytale::ClipKey &clipKey);
		void failed(const fairytale::ClipKey &clipKey);

	public:
		SolutionWidget(fairytale *app, QWidget *parent);

		fairytale* app() const;

		void addClip(const fairytale::ClipKey &clipKey);
		void clearClips();

		bool solvedAll() const;

		void fail(const fairytale::ClipKey &clipKey);

	protected:
		virtual void dragEnterEvent(QDragEnterEvent *event) override;
		virtual void dropEvent(QDropEvent *event) override;
		virtual void showEvent(QShowEvent *event) override;

	private:
		fairytale *m_app;
		typedef QMap<fairytale::ClipKey, IconLabel*> IconLabels;
		IconLabels m_iconLabels;
		int m_solveCounter;
		QWidget *m_centralWidget;
};

inline fairytale* SolutionWidget::app() const
{
	return this->m_app;
}

#endif // SOLUTIONWIDGET_H
