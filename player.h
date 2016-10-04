#ifndef PLAYER_H
#define PLAYER_H

#include <QtWidgets/QDialog>
#include <QtMultimedia/QMediaPlayer>
#include <QtCore/QQueue>
// On Android videos can be only played in QML.
#ifdef Q_OS_ANDROID
/*
#include <QQuickView>
#include <QQuickItem>
#include <QQmlProperty>
*/
#include <QtAV>
#include <QtAVWidgets>
#else
#include <QtMultimediaWidgets/QVideoWidget>
#endif

#include "ui_player.h"

class fairytale;
class IconLabel;

/**
 * \brief The video player which allows to play video sequences which can be skipped or paused as well.
 */
class Player : public QDialog, protected Ui::Player
{
	Q_OBJECT

	signals:
		/**
		 * This signal is emitted when the video/sound and the parallel sounds have finished both.
		 */
		void finishVideoAndSounds();

		void stateChanged(QMediaPlayer::State state);

	public slots:
		void playVideo(fairytale *app, const QUrl &url, const QString &description);
		void playBonusVideo(fairytale *app, const QUrl &url, const QString &description);
		void playSound(fairytale *app, const QUrl &url, const QString &description, const QUrl &imageUrl, bool prefix);

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
		Player(QWidget *parent, fairytale *app);
		virtual ~Player();

		QMediaPlayer::State state() const;
		int volume() const;

/*
#ifdef Q_OS_ANDROID
		QObject*
#else
		QMediaPlayer*
#endif
*/
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

	private slots:
		void onChangeStateParallelSoundPlayer(QMediaPlayer::State state);
		void onChangeState(QMediaPlayer::State state);
#ifdef Q_OS_ANDROID
		void onChangeStateAndroid();
		void onChangeStateAndroidQtAv(QtAV::AVPlayer::State state);
#endif

	private:
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
		/*
		QQuickView *m_view;
		QQuickItem *m_item;
		QObject *m_mediaPlayer;
		QWidget *m_videoWidget;
		*/

		QtAV::AVPlayer *m_player;
		QtAV::WidgetRenderer *m_renderer;
#else
		QVideoWidget *m_videoWidget;
		QMediaPlayer *m_mediaPlayer;
#endif
};

#ifndef Q_OS_ANDROID
inline
/*
#ifndef Q_OS_ANDROID
QMediaPlayer*
#else
QObject*
#endif
*/
QMediaPlayer* Player::mediaPlayer() const
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
