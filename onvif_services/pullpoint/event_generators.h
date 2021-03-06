#pragma once

#include "../Logger.h"
#include "../onvif_services/physical_components/IDigitalInput.h"

#include <functional>
#include <deque>

#include <boost/signals2.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/steady_timer.hpp>

namespace osrv
{
	namespace event
	{
		struct NotificationMessage;

		class IEventGenerator
		{
		public:
			IEventGenerator(int interval, const std::string& topic, boost::asio::io_context& io_context, const ILogger& logger)
				:
				event_interval_(interval)
				,notifications_topic_(topic)
				,io_context_(io_context)
				,alarm_timer_(io_context_)
				,logger_(logger)
			{
			}

			virtual ~IEventGenerator() {}

			//Before calling this method, no any events should be generated
			void Run()
			{
				schedule_next_alarm();
			}

			void Stop()
			{
				alarm_timer_.cancel();
			}

			//This will be connected
			boost::signals2::connection Connect(std::function<void(NotificationMessage)> f)
			{
				return event_signal_.connect(f);
			}

			// returns a NotificationMessage with 'PropertyOperation' equals "Initialized"
			virtual std::deque<NotificationMessage> GenerateSynchronizationEvent() const = 0;

		protected:
			// This method is should be overrided by implementors.
			// Implementors should fill a NotificationMessage and emit the signal with it
			virtual void generate_event()
			{
				//	NotificationMessage event_description;
				// /*do somehow filling event_description...*/
				// event_signal_(NotificationMessage);
			}

			void schedule_next_alarm()
			{
				alarm_timer_.expires_after(std::chrono::seconds(event_interval_));
				alarm_timer_.async_wait([this](const boost::system::error_code& error) {
							if(error == boost::asio::error::operation_aborted)
								return;

							generate_event();

							schedule_next_alarm();
						}
					);
			}

		protected:
			boost::signals2::signal<void(NotificationMessage)> event_signal_;

		protected:
			const int event_interval_;
			const std::string notifications_topic_;

			boost::asio::io_context& io_context_;
			boost::asio::steady_timer alarm_timer_;

			const ILogger& logger_;
		};

		class DInputEventGenerator : public IEventGenerator
		{
		public:
			DInputEventGenerator(int /*interval*/, const std::string& /*topic*/,
				boost::asio::io_context& /*io_context*/, const ILogger& /*logger_*/);

			// If the member DigitalInputsList is not initialized, events will not be generated
			void SetDigitalInputsList(const DigitalInputsList& /*di_list*/);

			// Inherited via IEventGenerator
			std::deque<NotificationMessage> GenerateSynchronizationEvent() const override;

		protected:
			void generate_event() override;

		private:
			bool state = false;
			const DigitalInputsList* di_list_ = nullptr;
		};
		
		class MotionAlarmEventGenerator : public IEventGenerator
		{
		public:
			MotionAlarmEventGenerator(const std::string& /*source_token*/, int /*interval*/, const std::string& /*topic*/,
				boost::asio::io_context& /*io_context*/, const ILogger& /*logger_*/);

			// Inherited via IEventGenerator
			std::deque<NotificationMessage> GenerateSynchronizationEvent() const override;

		protected:
			void generate_event() override;

		private:
			bool InvertState()
			{
				return state_ = !state_;
			}

		private:
			bool state_ = false;
			std::string source_token_;
		};

		class CellMotionEventGenerator : public IEventGenerator
		{
		public:
			CellMotionEventGenerator(const std::string& /*vsc_token*/, const std::string& /*vac_token*/,
				const std::string& /*rule*/,
				const std::string& /*data_item_name*/,
				int /*interval*/, const std::string& /*topic*/,
				boost::asio::io_context& /*io_context*/, const ILogger& /*logger_*/);

			// Inherited via IEventGenerator
			std::deque<NotificationMessage> GenerateSynchronizationEvent() const override;

		protected:
			void generate_event() override;

		private:
			bool InvertState()
			{
				return state_ = !state_;
			}

		private:
			bool state_ = false;
			std::string video_source_configuration_token_;
			std::string video_analytics_configuration_token_;
			std::string rule_;
			std::string data_item_name_;
		};

		class AudioDetectectionEventGenerator : public IEventGenerator
		{
		public:
			AudioDetectectionEventGenerator(const std::string& /*asc_token*/, const std::string& /*ac_token*/,
				const std::string& /*rule*/,
				const std::string& /*data_item_name*/,
				int /*interval*/, const std::string& /*topic*/,
				boost::asio::io_context& /*io_context*/, const ILogger& /*logger_*/);

			// Inherited via IEventGenerator
			std::deque<NotificationMessage> GenerateSynchronizationEvent() const override;

		protected:
			void generate_event() override;

		private:
			bool InvertState()
			{
				return state_ = !state_;
			}

		private:
			bool state_ = false;
			std::string source_configuration_token_;
			std::string analytics_configuration_token_;
			std::string rule_;
			std::string data_item_name_;
		};
	}
}