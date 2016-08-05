#ifndef PLAYER_H
#define PLAYER_H

#include <QtWidgets/QDialog>

#include <QtMultimedia/QMediaPlayer>

#include <QtMultimediaWidgets/QVideoWidget>

#include "ui_player.h"

class fairytale;

/**
 * \brief The video player which allows to play video sequences which can be skipped or paused as well.
 */
class Player : public QDialog, protected Ui::Player
{
	Q_OBJECT

	public slots:
		void playVideo(fairytale *app, const QUrl &url, const QString &description);

		void skip();
	public:
		Player(QWidget *parent, fairytale *app);

		QMediaPlayer* mediaPlayer() const;
		bool skipped() const;

		QPushButton* pauseButton() const;

	private:
		fairytale *m_app;
		QVideoWidget *m_videoWidget;
		QMediaPlayer *m_mediaPlayer;

		bool m_skipped;
};

inline QMediaPlayer* Player::mediaPlayer() const
{
	return this->m_mediaPlayer;
}

inline bool Player::skipped() const
{
	return this->m_skipped;
}

inline QPushButton* Player::pauseButton() const
{
	return this->pausePushButton;
}

#endif // PLAYER_H
