#ifndef ROOMWIDGET_H
#define ROOMWIDGET_H

#include <random>

#include <QtWidgets/QWidget>
#include <QtCore/QTimer>
#include <QtMultimedia/QSoundEffect>

class GameModeMoving;
class Door;
class FloatingClip;

/**
 * \brief A widget which displays a room with four windows from the top.
 *
 * Every window can open randomly and the wind can blow from that window and move the current piece of paper which contains the clip.
 * This makes it much harder for the player to select it without any hotkey.
 */
class RoomWidget : public QWidget
{
	Q_OBJECT

	signals:
		void gotIt();

	public slots:
		void changeWind();
		void updatePaint();

	public:
		typedef QVector<Door*> Doors;

		RoomWidget(GameModeMoving *gameMode, QWidget* parent);

		GameModeMoving* gameMode() const;

		void pause();
		void start();
		void resume();

		const Doors doors() const;
		FloatingClip *floatingClip() const;

	protected:
		/// Repaints the room widget and the doors as well as the floating clip.
		virtual void paintEvent(QPaintEvent *event) override;
		/// Catches all clicks.
		/**
		 * Use a release event, otherwise the mouse position is corrupted after showing the player in the slot.
		 */
		virtual void mouseReleaseEvent(QMouseEvent *event) override;

	private slots:
		void failSoundPlayingChanged();

	private:
		/**
		 * Only plays a random sound from the given list \p soundEffects if the last played sound did already finish.
		 * Otherwise too many sounds will overlay each other.
		 * \param soundEffects A list of possible file paths for the sound which is played. The path of the actual played sound is choosen randomly.
		 */
		void playSoundFromList(const QStringList &soundEffects);

		GameModeMoving *m_gameMode;
		std::random_device rd; // obtain a random number from hardware
		QTimer *m_windTimer;
		QTimer *m_paintTimer; // repaints the whole room widget with all doors and the floating clip
		Doors m_doors;
		FloatingClip *m_floatingClip;
		/// This sound effect is played whenever the player misses a click.
		QSoundEffect m_failSound;
		bool m_playNewFailSound;
		QStringList m_failSoundPaths;
		QStringList m_successSoundPaths;
};

inline GameModeMoving* RoomWidget::gameMode() const
{
	return this->m_gameMode;
}

inline const RoomWidget::Doors RoomWidget::doors() const
{
	return this->m_doors;
}

inline FloatingClip* RoomWidget::floatingClip() const
{
	return this->m_floatingClip;
}

#endif // ROOMWIDGET_H
