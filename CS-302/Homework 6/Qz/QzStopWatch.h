//////////////////////////////////////////////////////////////////////////////
//
//	File: QzStopWatch.h
//
//	$Header: /Projects/Qz/QzStopWatch.h  8  2009/9/7 11:34:25a  Lee $
//
//	This utility class is used for performance measurements using the highest
//	precision clock available on system.
//
//////////////////////////////////////////////////////////////////////////////


#pragma once


class QzStopWatch
{
private:
	U64 m_Frequency;
	U64 m_Start;
	U64 m_Stop;
	U64 m_Accumulate;

public:
	QzStopWatch(void)
		:	m_Frequency(0),
			m_Start(0),
			m_Stop(0),
			m_Accumulate(0)
	{
		m_Frequency = QzPrecisionClockFrequency();
	}

	void Start(void)
	{
		m_Start = QzPrecisionClockRead();
	}

	void Stop(void)
	{
		m_Stop = QzPrecisionClockRead();
		m_Accumulate += m_Stop - m_Start;
	}

	void Reset(void)
	{
		m_Accumulate = 0;
	}

	U32 iSeconds(void)
	{
		if (m_Frequency != 0) {
			return U32(m_Accumulate / m_Frequency);
		}

		return 0;
	}

	float fSeconds(void)
	{
		if (m_Frequency != 0) {
			return float(double(m_Accumulate) / double(m_Frequency));
		}

		return 0;
	}

	// Return the computed delta from Start(), without requiring timer to be stopped.
	U32 iDeltaMilliseconds(void)
	{
		if (m_Frequency != 0) {
			U64 current = QzPrecisionClockRead();
			return U32(((m_Accumulate + (current - m_Start)) * 1000) / m_Frequency);
		}

		return 0;
	}

	// Return the computed delta from Start(), without requiring timer to be stopped.
	U32 iDeltaMicroseconds(void)
	{
		if (m_Frequency != 0) {
			U64 current = QzPrecisionClockRead();
			return U32(((m_Accumulate + (current - m_Start)) * 1000000) / m_Frequency);
		}

		return 0;
	}

	U32 iMilliseconds(void)
	{
		if (m_Frequency != 0) {
			return U32((m_Accumulate * 1000) / m_Frequency);
		}

		return 0;
	}

	float fMilliseconds(void)
	{
		if (m_Frequency != 0) {
			return float((double(m_Accumulate) * 1000.0) / double(m_Frequency));
		}

		return 0;
	}

	U32 iMicroseconds(void)
	{
		if (m_Frequency != 0) {
			return U32((m_Accumulate * 1000000) / m_Frequency);
		}

		return 0;
	}

	float fMicroseconds(void)
	{
		if (m_Frequency != 0) {
			return float((double(m_Accumulate) * 1000000.0) / double(m_Frequency));
		}

		return 0;
	}
};

