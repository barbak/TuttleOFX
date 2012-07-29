#include <iostream>
#define BOOST_TEST_MODULE plugin_ImageMagick
#include <boost/test/unit_test.hpp>

#include <tuttle/host/Graph.hpp>
#include <tuttle/host/InputBufferNode.hpp>

#include <boost/preprocessor/stringize.hpp>

#include <boost/timer.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

/**
 * Simple functional test, to read and write an image.
 */

using namespace boost::unit_test;
using namespace tuttle::host;

BOOST_AUTO_TEST_SUITE( plugin_Dpx_reader )

BOOST_AUTO_TEST_CASE( loading_openfx_plugins )
{
	TUTTLE_COUT( "-------- LOADING OPENFX PLUGINS --------" );
	Core::instance().getPluginCache().addDirectoryToPath( BOOST_PP_STRINGIZE(TUTTLE_PLUGIN_PATH) );
	Core::instance().preload();
	//TUTTLE_COUT( Core::instance().getImageEffectPluginCache() );
	TUTTLE_COUT( "----------------- DONE -----------------" );
}

BOOST_AUTO_TEST_CASE( process_reader )
{
	TUTTLE_COUT( "******** PROCESS READER IMAGE MAGICK ********" );
	Graph g;

	TUTTLE_COUT( "--> PLUGINS CREATION" );
	Graph::Node& read = g.createNode( "tuttle.imagemagickreader" );

	TUTTLE_COUT( "--> PLUGINS CONFIGURATION" );

	read.getParam( "filename" ).setValue( "data/file.dpx" );
	
	TUTTLE_COUT( "--> GRAPH PROCESSING" );
	boost::posix_time::ptime t1a(boost::posix_time::microsec_clock::local_time());
	ComputeOptions options;
	options._returnBuffers = true;
	memory::MemoryCache outputCache;
	g.compute( outputCache, read, options );
	boost::posix_time::ptime t2a(boost::posix_time::microsec_clock::local_time());

	TUTTLE_COUT( "Process took: " << t2a - t1a );

	std::cout << outputCache << std::endl;

	memory::CACHE_ELEMENT imgRes = outputCache.get( read.getName(), 0 );

	TUTTLE_TCOUT_VAR( imgRes->getROD() );
	BOOST_CHECK_EQUAL( imgRes->getROD().x1, 0 );
	BOOST_CHECK_EQUAL( imgRes->getROD().y1, 0 );
	BOOST_CHECK_NE( imgRes->getROD().x2, 0 );
	BOOST_CHECK_NE( imgRes->getROD().y2, 0 );

	TUTTLE_TCOUT_VAR( imgRes->getBounds() );
	BOOST_CHECK_EQUAL( imgRes->getBounds().x1, 0 );
	BOOST_CHECK_EQUAL( imgRes->getBounds().y1, 0 );
	BOOST_CHECK_NE( imgRes->getBounds().x2, 0 );
	BOOST_CHECK_NE( imgRes->getBounds().y2, 0 );
}

BOOST_AUTO_TEST_CASE( process_nofile )
{
	TUTTLE_COUT( "******** PROCESS READER IMAGE MAGICK NO FILE ********" );
	Graph g;

	TUTTLE_COUT( "--> PLUGINS CREATION" );
	Graph::Node& read = g.createNode( "tuttle.imagemagickreader" );

	TUTTLE_COUT( "--> PLUGINS CONFIGURATION" );
	read.getParam( "filename" ).setValue( "data/no-such-file.dpx" );

	TUTTLE_COUT( "--> GRAPH CONNECTION" );
//	g.connect( read, write );

	TUTTLE_COUT( "---> GRAPH PROCESSING" );
	BOOST_REQUIRE_THROW( g.compute( read ), boost::exception );
}

BOOST_AUTO_TEST_SUITE_END()
