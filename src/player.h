#ifndef PLAYER_H
#define PLAYER_H

#include "config.h"

#include <QtWidgets/QDialog>
#include <QtMultimedia/QMediaPlayer>
#include <QtCore/QQueue>
// On Android videos can be only played in QML or with QtAV.
#ifdef USE_QTAV
#include <QtAV>
#include <QtAVWidgets>
#include <OpenGLWidgetRenderer.h>
#else
#include <QtMultimediaWidgets/QVideoWidget>
#endif

#include "ui_player.h"

class fairytale;
class IconLabel;

using PlayerParentType =
#ifndef Q_OS_ANDROID
QDialog
#else
QWidget
#endif
;

/**
 * \brief The video player which allows to play video sequences which can be skipped or paused as well.
 *
 * \note There can only be played one video at a time but you can play parallel sounds to the video on another track.
 * \note On Android the backend is not QMultimedia and QMultimediaWidgets since the widgets are not supported on Android. On Android QtAV is used instead.
 *
 * The backend depends on the options set during the compile time of the game.
 */
class Player : public PlayerParentType, protected Ui::Player
{
	Q_OBJECT

	signals:
		/**
		 * This signal is emitted when the state of the video/sound player changes (not of the parallel sounds player).
		 */
		void stateChanged(QMediaPlayer::State state);

		/**
		 * This signal is emitted when the state of the video/sound player AND the parallel sounds player changes.
		 */
		void stateChangedVideoAndSounds(QMediaPlayer::State state);

	public slots:
		void playVideo(const QUrl &url, const QString &description, bool duringGame = true, bool multipleVideos = true);
		void playBonusVideo(const QUrl &url, const QString &description);
		void playSound(const QUrl &url, const QString &description, const QUrl &imageUrl, bool prefix, bool duringGame = true);
		void showImage(const QUrl &imageUrl, const QString &description);

		/**
		  * Plays a parallel sound which is mixed with the current output and which is waited for as well.
		  * If there is already played a parallel sound this one is queued.
		  */
		void playParallelSound(const QUrl &url);

		/**
		 * Starts playing the current video or sound.
		 */
		void play();
		/**
		 * Pauses the current video or sound.
		 */
		void pause();
		/**
		 * Stops the current video or sound completely.
		 */
		void stop();
		/**
		 * Sets the volume of the sound output.
		 * \param volume A value between 0 and 100.
		 */
		void setVolume(int volume);
		void setMuted(bool muted);

		/**
		 * Skips the currently played video or sound.
		 */
		void skip();

		/**
		 * Skips the currently played video or sound and indicates to the game that everything should be skipped.
		 */
		void skipAll();

	public:
		typedef
#ifndef Q_OS_ANDROID
		QDialog
#else
		QWidget
#endif
		Base;

		Player(QWidget *parent, fairytale *app);
		virtual ~Player();

		fairytale* app() const;

		QMediaPlayer::State state() const;
		int volume() const;
		bool isMuted() const;

#ifndef USE_QTAV
		QMediaPlayer* mediaPlayer() const;
#endif

		/**
		 * \return Returns true if the video or sound has been skipped. This value is reset whenever a new media is played.
		 */
		bool skipped() const;
		/**
		 * \return Returns true if the video or sound has been skipped and everything following should be skipped as well.
		 */
		bool skippedAll() const;
		/**
		 * \return Returns true if the currently played media is only the prefix for another media. This means after ending the current media the game mode does not continue yet.
		 */
		bool isPrefix() const;

		QPushButton* pauseButton() const;

	protected:
		virtual void changeEvent(QEvent *event) override;
		virtual void showEvent(QShowEvent *event) override;
		virtual void hideEvent(QHideEvent *event) override;

	private slots:
		void onChangeStateParallelSoundPlayer(QMediaPlayer::State state);
		void onChangeState(QMediaPlayer::State state);
#ifdef USE_QTAV
		void onChangeStateAndroidQtAv(QtAV::AVPlayer::State state);
		void onError(const QtAV::AVError &e);
#endif

	private:
		/**
		 * Checks if the video and the parallel sounds have all been finished. If so the signal \ref finishVideoAndSounds() is emitted.
		 */
		void checkForFinish();
		void checkForStart();
		void checkForPause();
#ifdef USE_QTAV
		void showCentral();
		void hideCentral();
#endif

		void setupButtons();

		fairytale *m_app;

		/**
		 * The icon label is used for showing a clip picture when playing a sound only.
		 */
		IconLabel *m_iconLabel;
		bool m_skipped;
		bool m_skippedAll;
		bool m_isPrefix;

		/// A queue with sounds waiting for the parallel sound player to become available.
		typedef QQueue<QUrl> SoundQueue;
		SoundQueue m_parallelSounds;
		/// Player for background music.
		QMediaPlayer *m_parallelSoundsMediaPlayer;

#ifdef USE_QTAV
		QtAV::AVPlayer *m_player;
		QtAV::OpenGLWidgetRenderer *m_renderer; // GLWidgetRenderer2

		QList<QWidget*> m_hiddenWidgets;
#else
		QVideoWidget *m_videoWidget;
		QMediaPlayer *m_mediaPlayer;
#endif

		bool m_musicWasMutedBefore = false;
};

inline fairytale* Player::app() const
{
	return this->m_app;
}

#ifndef USE_QTAV
inline QMediaPlayer* Player::mediaPlayer() const
{
	return this->m_mediaPlayer;
}
#endif

inline bool Player::skipped() const
{
	return this->m_skipped;
}

inline bool Player::skippedAll() const
{
	return this->m_skippedAll;
}

inline bool Player::isPrefix() const
{
	return this->m_isPrefix;
}

inline QPushButton* Player::pauseButton() const
{
	return this->pausePushButton;
}

#endif // PLAYER_H