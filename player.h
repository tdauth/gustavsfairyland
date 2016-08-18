#ifndef PLAYER_H
#define PLAYER_H

#include <QtWidgets/QDialog>
// On Android videos can be only played in QML.
#ifdef Q_OS_ANDROID
#include <QQuickView>
#include <QQuickItem>
#include <QQmlProperty>
#else
#include <QtMultimedia/QMediaPlayer>
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

	public slots:
		void playVideo(fairytale *app, const QUrl &url, const QString &description);
		void playBonusVideo(fairytale *app, const QUrl &url, const QString &description);
		void playSound(fairytale *app, const QUrl &url, const QString &description, const QUrl &imageUrl, bool prefix);

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
	public:
		Player(QWidget *parent, fairytale *app);
		virtual ~Player();

		QMediaPlayer::State state() const;
		int volume() const;

#ifdef Q_OS_ANDROID
		QObject*
#else
		QMediaPlayer*
#endif
		mediaPlayer() const;

		/**
		 * \return Returns true if the video or sound has been skipped. This value is reset whenever a new media is played.
		 */
		bool skipped() const;
		/**
		 * \return Returns true if the currently played media is only the prefix for another media. This means after ending the current media the game mode does not continue yet.
		 */
		bool isPrefix() const;

		QPushButton* pauseButton() const;

	private:
		fairytale *m_app;
		/**
		 * The icon label is used for showing a clip picture when playing a sound only.
		 */
		IconLabel *m_iconLabel;
		bool m_skipped;
		bool m_isPrefix;

#ifdef Q_OS_ANDROID
		QQuickView *m_view;
		QQuickItem *m_item;
		QObject *m_mediaPlayer;
		QWidget *m_videoWidget;
#else
		QVideoWidget *m_videoWidget;
		QMediaPlayer *m_mediaPlayer;
#endif
};


inline
#ifndef Q_OS_ANDROID
QMediaPlayer*
#else
QObject*
#endif
Player::mediaPlayer() const
{
	return this->m_mediaPlayer;
}

inline bool Player::skipped() const
{
	return this->m_skipped;
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
