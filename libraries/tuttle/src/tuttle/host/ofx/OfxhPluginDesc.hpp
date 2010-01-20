#ifndef OFXH_PLUGINDESC_HPP
#define OFXH_PLUGINDESC_HPP


#include <boost/serialization/serialization.hpp>
#include <boost/algorithm/string.hpp>

namespace tuttle {
namespace host {
namespace ofx {


/**
 * C++ version of the information kept inside an OfxPlugin struct
 */
class OfxhPluginDesc
{
	typedef OfxhPluginDesc This;
protected:
	std::string _pluginApi; ///< the API I implement
	int _apiVersion; ///< the version of the API

	std::string _identifier; ///< the identifier of the plugin
	std::string _rawIdentifier; ///< the original identifier of the plugin
	int _versionMajor; ///< the plugin major version
	int _versionMinor; ///< the plugin minor version

public:

	OfxhPluginDesc() {}

	virtual ~OfxhPluginDesc() {}

	OfxhPluginDesc( const std::string& api,
	            int                apiVersion,
	            const std::string& identifier,
	            const std::string& rawIdentifier,
	            int                versionMajor,
	            int                versionMinor )
		: _pluginApi( api ),
		_apiVersion( apiVersion ),
		_identifier( identifier ),
		_rawIdentifier( rawIdentifier ),
		_versionMajor( versionMajor ),
		_versionMinor( versionMinor ) {}

	/**
	 * constructor for the case where we have already loaded the plugin binary and
	 * are populating this object from it
	 */
	OfxhPluginDesc( OfxPlugin* ofxPlugin )
		: _pluginApi( ofxPlugin->pluginApi ),
		_apiVersion( ofxPlugin->apiVersion ),
		_identifier( ofxPlugin->pluginIdentifier ),
		_rawIdentifier( ofxPlugin->pluginIdentifier ),
		_versionMajor( ofxPlugin->pluginVersionMajor ),
		_versionMinor( ofxPlugin->pluginVersionMinor )
	{
		boost::to_lower( _identifier );
	}

	bool operator==( const This& other ) const
	{
		if( _pluginApi != other._pluginApi ||
		    _apiVersion != other._apiVersion ||
		    _identifier != other._identifier ||
		    _rawIdentifier != other._rawIdentifier ||
		    _versionMajor != other._versionMajor ||
		    _versionMinor != other._versionMinor )
			return false;
		return true;
	}
	bool operator!=( const This& other ) const { return !This::operator==(other); }

public:
	const std::string& getPluginApi() const
	{
		return _pluginApi;
	}

	int getApiVersion() const
	{
		return _apiVersion;
	}

	const std::string& getIdentifier() const
	{
		return _identifier;
	}

	const std::string& getRawIdentifier() const
	{
		return _rawIdentifier;
	}

	int getVersionMajor() const
	{
		return _versionMajor;
	}

	int getVersionMinor() const
	{
		return _versionMinor;
	}
	
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize( Archive &ar, const unsigned int version )
	{
		ar & BOOST_SERIALIZATION_NVP(_pluginApi);
		ar & BOOST_SERIALIZATION_NVP(_apiVersion);
		ar & BOOST_SERIALIZATION_NVP(_identifier);
		ar & BOOST_SERIALIZATION_NVP(_rawIdentifier);
		ar & BOOST_SERIALIZATION_NVP(_versionMajor);
		ar & BOOST_SERIALIZATION_NVP(_versionMinor);
	}
};


}
}
}

#endif

