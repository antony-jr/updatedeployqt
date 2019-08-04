#include <QMetaObject>
#include <QMetaMethod>
#include <Timer.hpp>
#include <Timer_p.hpp>

Timer::Timer(QObject *parent)
	: QObject(parent)	
{
	m_Thread = new QThread;
	m_Thread->start();
	
	m_Worker = new TimerWorker;
	m_Worker->moveToThread(m_Thread);
	connect(m_Worker , &TimerWorker::timeout , this , &Timer::timeout , Qt::DirectConnection);
}

Timer::~Timer()
{
	stop();
	m_Worker->deleteLater();
	m_Thread->quit();
	m_Thread->wait();
	m_Thread->deleteLater();
}

void Timer::setInterval(int msecs)
{
	auto metaObject = m_Worker->metaObject();
	metaObject->method(metaObject->indexOfMethod(QMetaObject::normalizedSignature("setInterval(int)")))
		.invoke(m_Worker , Qt::QueuedConnection , Q_ARG(int , msecs));
}

void Timer::start()
{
	auto metaObject = m_Worker->metaObject();
	metaObject->method(metaObject->indexOfMethod(QMetaObject::normalizedSignature("start()")))
		.invoke(m_Worker , Qt::QueuedConnection);
}

void Timer::stop()
{
	auto metaObject = m_Worker->metaObject();
	metaObject->method(metaObject->indexOfMethod(QMetaObject::normalizedSignature("stop()")))
		.invoke(m_Worker , Qt::QueuedConnection);
}
