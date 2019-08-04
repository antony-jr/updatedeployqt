#include <Timer_p.hpp>

TimerWorker::TimerWorker(QObject *parent)
		: QObject(parent)
{
	m_Timer = new QTimer(this);
	m_Timer->setSingleShot(true);
	connect(m_Timer , &QTimer::timeout , this , &TimerWorker::timeout , Qt::DirectConnection);
}

TimerWorker::~TimerWorker()
{
	m_Timer->stop();
	m_Timer->deleteLater();
}

void TimerWorker::setInterval(int msecs)
{
	m_Timer->setInterval(msecs);
}

void TimerWorker::start()
{
	m_Timer->start();
}

void TimerWorker::stop()
{
	m_Timer->stop();
}
