#ifndef ROOMWIDGET_H
#define ROOMWIDGET_H

#include <random>

#include <QOpenGLWidget>
#include <QtCore/QTimer>
#include <QtCore/QElapsedTimer>
#include <QtMultimedia/QSoundEffect>
#include <QSvgRenderer>

class GameModeMoving;
class Door;
class FloatingClip;
class Clip;

// Use QOpenGLWidget for better performance but there is a bug when resizing the widget with OpenGL. It is painted over the sizes.
typedef QWidget RoomWidgetParent;

/**
 * \brief A widget which displays a room with four windows from the top.
 *
 * Every window can open randomly and the wind can blow from that window and move the current piece of paper which contains the clip.
 * This makes it much harder for the player to select it without any hotkey.
 *
 * \note Inherits QOpenGLWidget therefore painting should be faster on platforms where OpenGL is available.
 */
class RoomWidget : public RoomWidgetParent
{
	Q_OBJECT

	signals:
		void gotIt();

	public slots:
		void changeWind();
		void updatePaint();

	public:
		typedef QVector<Door*> Doors;
		typedef QVector<FloatingClip*> FloatingClips;

		/**
		 * The size of a floating clip depends on the room widget's size.
		 * A bigger room widget leads to bigger floating clips to keep the fairness.
		 * Otherwise small room widgets with big floating clips would give you an advantage of clicking the floating clip.
		 */
		int floatingClipWidth() const;
		/**
		 * The speed depends on the size as well. It does also depend on the difficulty of the game (\ref fairytale::difficulty()).
		 * The more difficult the game is the faster the clips move.
		 * \return Returns the pixels per S which the floating clip is moved.
		 */
		int floatingClipSpeed() const;

		/**
		 * \return Returns the maximum distance in pixels which the floating clip moves from a collision. This distance is smaller if the room widget is smaller.
		 */
		int maxCollisionDistance() const;

		RoomWidget(GameModeMoving *gameMode, QWidget* parent);

		GameModeMoving* gameMode() const;

		void pause();
		void start();
		void resume();

		void addFloatingClip(Clip *clip, int width, int speed);
		void clearFloatingClipsExceptFirst();
		const Doors& doors() const;
		const FloatingClips& floatingClips() const;

	protected:
		/// Repaints the room widget and the doors as well as the floating clip.
		virtual void paintEvent(QPaintEvent *event) override;
		/// Catches all clicks.
		/// Use the press event to detect if the player has won or to play specific sounds but do never trigger the signal which would lead to a crash.
		virtual void mousePressEvent(QMouseEvent *event) override;
		/**
		 * Use a release event to emit the signal, otherwise the mouse position is corrupted after showing the player in the slot.
		 */
		virtual void mouseReleaseEvent(QMouseEvent *event) override;
		/**
		 * The resize event updates the room widget background image and updates the size of all floating clips to \ref floatingClipWidth().
		 */
		virtual void resizeEvent(QResizeEvent *event) override;

	private:
		/**
		 * Only plays a random sound from the given list \p soundEffects if the last played sound did already finish.
		 * Otherwise too many sounds will overlay each other.
		 * \param soundEffects A list of possible file paths for the sound which is played. The path of the actual played sound is choosen randomly.
		 */
		void playSoundFromList(const QStringList &soundEffects);

		GameModeMoving *m_gameMode;
		bool m_won;
		std::random_device rd; // obtain a random number from hardware
		QTimer *m_windTimer;
		QTimer *m_paintTimer; // repaints the whole room widget with all doors and the floating clip
		/// Measures the time the painting takes to prevent wrong distances when it takes longer than the repaint timer interval.
		qint64 m_paintTime;
		Doors m_doors;
		FloatingClips m_floatingClips;
		QStringList m_failSoundPaths;
		QStringList m_successSoundPaths;
		QSvgRenderer m_woodSvg;
		QImage m_woodImage;
		QImage m_woodImageDisabled;
};

inline GameModeMoving* RoomWidget::gameMode() const
{
	return this->m_gameMode;
}

inline const RoomWidget::Doors& RoomWidget::doors() const
{
	return this->m_doors;
}

inline const RoomWidget::FloatingClips& RoomWidget::floatingClips() const
{
	return this->m_floatingClips;
}

#endif // ROOMWIDGET_H
