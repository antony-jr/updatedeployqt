#ifndef TIMER_PRIVATE_HPP_INCLUDED
#define TIMER_PRIVATE_HPP_INCLUDED
#include <QObject>
#include <QTimer>

class TimerWorker : public QObject {
	Q_OBJECT
public:
	TimerWorker(QObject *parent = nullptr);
	~TimerWorker();
public Q_SLOTS:
	void setInterval(int);
	void start();
	void stop();
Q_SIGNALS:
	void timeout();
private:
	QTimer *m_Timer = nullptr;
};
#endif
