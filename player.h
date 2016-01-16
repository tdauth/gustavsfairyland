#ifndef PLAYER_H
#define PLAYER_H

#include <QtWidgets/QDialog>

#include <QtMultimedia/QMediaPlayer>

#include <QtMultimediaWidgets/QVideoWidget>

#include "ui_player.h"

class Player : public QDialog, protected Ui::Player
{
	Q_OBJECT

	public slots:
		void playVideo(const QUrl &url);

		void skip();
	public:
		Player(QWidget *parent);

		QMediaPlayer* mediaPlayer() const;
		bool skipped() const;

	private:
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

#endif // PLAYER_H
