#include <iostream>

#include <boost/filesystem/path.hpp>

using namespace boost::unit_test;
using namespace tuttle::host;
namespace bfs = boost::filesystem;

BOOST_AUTO_TEST_CASE( process_writer )
{
	TUTTLE_COUT( "******** PROCESS WRITER " << pluginName << " ********" );
	Graph g;

	TUTTLE_COUT( "-------- PLUGINS CREATION --------" );
	Graph::Node& constant = g.createNode( "tuttle.constant" );
	Graph::Node& writer   = g.createNode( pluginName );

	TUTTLE_COUT( "-------- PLUGINS CONFIGURATION --------" );

	constant.getParam( "width" ).setValue( 500 );
	constant.getParam( "height" ).setValue( 500 );
	
	std::string tuttleOFXData = "TuttleOFX-data";
	if( const char* env_test_data = std::getenv("TUTTLE_TEST_DATA") )
	{
		tuttleOFXData = env_test_data;
	}
	
	const std::string pluginFilename = ( bfs::path(tuttleOFXData) / "image" / filename ).string();
	writer.getParam( "filename" ).setValue( pluginFilename );

	TUTTLE_COUT( "-------- GRAPH CONNECTION --------" );
	g.connect( constant, writer );

	TUTTLE_COUT( "-------- GRAPH PROCESSING --------" );
	boost::posix_time::ptime t1a(boost::posix_time::microsec_clock::local_time());
	memory::MemoryCache outputCache;
	g.compute( outputCache, writer );
	boost::posix_time::ptime t2a(boost::posix_time::microsec_clock::local_time());

	TUTTLE_COUT( "Process took: " << t2a - t1a );

	std::cout << outputCache << std::endl;

	memory::CACHE_ELEMENT imgRes = outputCache.get( writer.getName(), 0 );

	TUTTLE_TCOUT_VAR( imgRes->getROD() );
	BOOST_CHECK_EQUAL( imgRes->getROD().x1, 0 );
	BOOST_CHECK_EQUAL( imgRes->getROD().y1, 0 );
	BOOST_CHECK_EQUAL( imgRes->getROD().x2, 499 );
	BOOST_CHECK_EQUAL( imgRes->getROD().y2, 499 );

	TUTTLE_TCOUT_VAR( imgRes->getBounds() );
	BOOST_CHECK_EQUAL( imgRes->getBounds().x1, 0 );
	BOOST_CHECK_EQUAL( imgRes->getBounds().y1, 0 );
	BOOST_CHECK_EQUAL( imgRes->getBounds().x2, 499 );
	BOOST_CHECK_EQUAL( imgRes->getBounds().y2, 499 );
}

BOOST_AUTO_TEST_CASE( process_unconnected )
{
	TUTTLE_COUT( "******** PROCESS WRITER " << pluginName << " UNCONNECTED ********" );
	Graph g;

	TUTTLE_COUT( "--> PLUGINS CREATION" );
	Graph::Node& write = g.createNode( pluginName );

	TUTTLE_COUT( "--> PLUGINS CONFIGURATION" );
	std::string filename = "data/no-such-file";
	write.getParam( "filename" ).setValue( filename );

	TUTTLE_COUT( "---> GRAPH PROCESSING" );
	BOOST_REQUIRE_THROW( g.compute( write ), boost::exception );
}
