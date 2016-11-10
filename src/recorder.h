#ifndef RECORDER_H
#define RECORDER_H

#include <QObject>
#include <QMediaRecorder>

class Recorder : public QObject
{
	public slots:
		void record(const QString &file);
		void stop();

	public:
		Recorder(QObject *object = nullptr);

		QMediaRecorder::State state() const;

		QMediaRecorder* recorder() const;

	private:
		QMediaRecorder *m_recorder;
};

inline QMediaRecorder::State Recorder::state() const
{
	return this->m_recorder->state();
}

inline QMediaRecorder* Recorder::recorder() const
{
	return this->m_recorder;
}

#endif // RECORDER_H
