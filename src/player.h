#ifndef PLAYER_H
#define PLAYER_H

#include <QtWidgets/QDialog>
#include <QtMultimedia/QMediaPlayer>
#include <QtCore/QQueue>
// On Android videos can be only played in QML.
#ifdef Q_OS_ANDROID
#include <QtAV>
#include <QtAVWidgets>
#include <OpenGLWidgetRenderer.h>
#else
#include <QtMultimediaWidgets/QVideoWidget>
#endif

#include "ui_player.h"

class fairytale;
class IconLabel;

/**
 * \brief The video player which allows to play video sequences which can be skipped or paused as well.
 *
 * \note There can only be played one video at a time but you can play parallel sounds to the video on another track.
 * \note On Android the backend is not QMultimedia and QMultimediaWidgets since the widgets are not supported on Android. On Android QtAV is used instead.
 */
class Player
#ifndef Q_OS_ANDROID
: public QDialog
#else
: public QWidget
#endif
, protected Ui::Player
{
	Q_OBJECT

	signals:
		/**
		 * This signal is emitted when the video/sound and the parallel sounds have finished both.
		 */
		void finishVideoAndSounds();

		void stateChanged(QMediaPlayer::State state);

	public slots:
		void playVideo(fairytale *app, const QUrl &url, const QString &description, bool duringGame = true, bool multipleVideos = true);
		void playBonusVideo(fairytale *app, const QUrl &url, const QString &description);
		void playSound(fairytale *app, const QUrl &url, const QString &description, const QUrl &imageUrl, bool prefix, bool duringGame = true);
		void showImage(fairytale *app, const QUrl &imageUrl, const QString &description);

		/**
		  * Plays a parallel sound which is mixed with the current output and which is waited for as well.
		  * If there is already played a parallel sound this one is queued.
		  */
		void playParallelSound(fairytale *app, const QUrl &url);

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

		QMediaPlayer::State state() const;
		int volume() const;

#ifndef Q_OS_ANDROID
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

	private slots:
		void onChangeStateParallelSoundPlayer(QMediaPlayer::State state);
		void onChangeState(QMediaPlayer::State state);
#ifdef Q_OS_ANDROID
		void onChangeStateAndroid();
		void onChangeStateAndroidQtAv(QtAV::AVPlayer::State state);
#endif

	private:
		/**
		 * Checks if the video and the parallel sounds have all been finished. If so the signal \ref finishVideoAndSounds() is emitted.
		 */
		void checkForFinish();

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

#ifdef Q_OS_ANDROID
		QtAV::AVPlayer *m_player;
		QtAV::OpenGLWidgetRenderer *m_renderer; // GLWidgetRenderer2

		QList<QWidget*> m_hiddenWidgets;
#else
		QVideoWidget *m_videoWidget;
		QMediaPlayer *m_mediaPlayer;
#endif
};

#ifndef Q_OS_ANDROID
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
