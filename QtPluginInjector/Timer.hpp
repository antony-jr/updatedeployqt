#ifndef TIMER_HPP_INCLUDED
#define TIMER_HPP_INCLUDED
#include <QObject>
#include <QThread>

class TimerWorker;
class Timer : public QObject {
	Q_OBJECT
public:
	Timer(QObject *parent = nullptr);
	~Timer();
public Q_SLOTS:
	void setInterval(int);
	void start();
	void stop();
Q_SIGNALS:
	void timeout();
private:
	QThread *m_Thread = nullptr;
	TimerWorker *m_Worker = nullptr;
};
#endif
