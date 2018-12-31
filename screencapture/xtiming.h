#pragma once

#include <windows.h>


class XTiming
{
private:
	bool m_f_initialized;
	double m_perfFrequency;
	double m_currentModelTime;
	double m_lastModelTime;
	double m_deltaModelTime;

public:
	XTiming()
	{
		m_f_initialized = false;
		m_perfFrequency = 0;
		m_currentModelTime = 0;
		m_lastModelTime = 0;
		m_deltaModelTime = 0;
	}

    double getTime()
    {
        LARGE_INTEGER li;
        QueryPerformanceCounter(&li);
        double currentCounter = (double)li.QuadPart;
        return (currentCounter / m_perfFrequency);
    }

	void init()
	{
		if (!m_f_initialized)
		{
			// Detect frequency
			LARGE_INTEGER li;
			QueryPerformanceFrequency(&li);
			m_perfFrequency = (double)li.QuadPart;

			m_lastModelTime = getTime()-0.001f;
			m_currentModelTime = getTime();

			m_f_initialized = true;
		}
	}

	double getTimeDelta()
	{
		if (m_currentModelTime==0)
			this->updateWorldTime();

		return m_deltaModelTime;
	}

	double getWorldTime()
	{
		if (m_currentModelTime==0)
			this->updateWorldTime();

		return m_currentModelTime;
	}

	void updateWorldTime()
	{
		if (m_f_initialized)
		{
			m_lastModelTime = m_currentModelTime;

			m_currentModelTime = getTime();
		}
		else
		{
			init();
		}

		m_deltaModelTime = m_currentModelTime - m_lastModelTime;
	}

    // use only for save / load game
	void setCurrentModelTime(double model_time_setup_value)
	{
		m_currentModelTime = model_time_setup_value;
	}
}; // class XTiming
