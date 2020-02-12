#pragma once

namespace Hazel {

	class Timestep
	{
	public:
		Timestep(std::chrono::duration<float> time)
			: m_Time(time)
		{
		}

		operator float() const { return m_Time.count(); }

		std::chrono::duration<float> GetSeconds() const { return m_Time; }
		std::chrono::duration<float, std::milli> GetMilliseconds() const { return m_Time; }
	private:
		std::chrono::duration<float> m_Time;
	};

}