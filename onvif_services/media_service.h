#pragma once

#include "../Types.inl"

#include <boost/property_tree/ptree_fwd.hpp>

class ILogger;

namespace osrv
{
	struct ServerConfigs;

	namespace media
	{
		void init_service(HttpServer& /*srv*/, const osrv::ServerConfigs& /*configs*/,
			const std::string& /*configs_path*/, ILogger& /*logger*/);

		namespace util
		{
			namespace pt = boost::property_tree;
			void fill_soap_videosource_configuration(const pt::ptree& config_node, pt::ptree& videosource_node);

			void fill_analytics_configuration(/*const pt::ptree& config_node,*/ pt::ptree& result);

			std::string generate_rtsp_url(const ServerConfigs& /*server_configs*/, const std::string& /*profile_stream_url*/);
		}
	}
}