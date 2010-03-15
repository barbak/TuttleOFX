#include "ImageStatisticsPluginFactory.hpp"
#include "ImageStatisticsPlugin.hpp"
#include "ImageStatisticsDefinitions.hpp"

#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/Progress.hpp>
#include <tuttle/plugin/PluginException.hpp>

#include <string>
#include <iostream>
#include <stdio.h>
#include <cmath>
#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>
#include <boost/scoped_ptr.hpp>

namespace tuttle {
namespace plugin {
namespace average {

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out]   desc     Effect descriptor
 */
void ImageStatisticsPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
	// basic labels
	desc.setLabels( "ImageStatistics", "ImageStatistics",
		            "ImageStatistics" );
	desc.setPluginGrouping( "tuttle" );

	// add the supported contexts, only filter at the moment
	desc.addSupportedContext( OFX::eContextGeneral );
	desc.addSupportedContext( OFX::eContextFilter );

	// add supported pixel depths
	desc.addSupportedBitDepth( OFX::eBitDepthUByte );
	desc.addSupportedBitDepth( OFX::eBitDepthUShort );
	desc.addSupportedBitDepth( OFX::eBitDepthFloat );

	// set a few flags
	desc.setSingleInstance( false );
	desc.setHostFrameThreading( true );
	desc.setSupportsMultiResolution( false );
	desc.setSupportsTiles( kSupportTiles );
	desc.setTemporalClipAccess( kSupportTemporalClipAccess );
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void ImageStatisticsPluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc,
                                                  OFX::ContextEnum context )
{
	OFX::ClipDescriptor* srcClip = desc.defineClip( kOfxImageEffectSimpleSourceClipName );
	srcClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	srcClip->addSupportedComponent( OFX::ePixelComponentAlpha );
	srcClip->setSupportsTiles( kSupportTiles );

	// Create the mandated output clip
	OFX::ClipDescriptor* dstClip = desc.defineClip( kOfxImageEffectOutputClipName );
	dstClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	dstClip->addSupportedComponent( OFX::ePixelComponentAlpha );
	dstClip->setSupportsTiles( kSupportTiles );

	OFX::Double2DParamDescriptor* cornerA = desc.defineDouble2DParam( kCornerA );
	cornerA->setLabel( "A" );
	cornerA->setDoubleType( OFX::eDoubleTypeNormalisedXYAbsolute );
    cornerA->setDefault( 0.4, 0.4 );

	OFX::Double2DParamDescriptor* cornerB = desc.defineDouble2DParam( kCornerB );
	cornerB->setLabel( "B" );
	cornerB->setDoubleType( OFX::eDoubleTypeNormalisedXYAbsolute );
    cornerB->setDefault( 0.6, 0.6 );

	OFX::ChoiceParamDescriptor* chooseOutput = desc.defineChoiceParam( kChooseOutput );
	chooseOutput->setLabel( "Choose output" );
	chooseOutput->appendOption( kChooseOutputSource );
	chooseOutput->appendOption( kChooseOutputAverage );
	chooseOutput->appendOption( kChooseOutputChannelMin );
	chooseOutput->appendOption( kChooseOutputChannelMax );
	chooseOutput->appendOption( kChooseOutputLuminosityMin );
	chooseOutput->appendOption( kChooseOutputLuminosityMax );
	chooseOutput->setDefault( 0 );
	chooseOutput->setEvaluateOnChange( false );

	OFX::GroupParamDescriptor* outputGroup = desc.defineGroupParam( kOutputGroup );
	outputGroup->setLabel( "Output" );

	OFX::RGBAParamDescriptor* outputAverage = desc.defineRGBAParam( kOutputAverage );
	outputAverage->setLabel( "Average" );
	outputAverage->setParent( outputGroup );

	OFX::RGBAParamDescriptor* outputChannelMin = desc.defineRGBAParam( kOutputChannelMin );
	outputChannelMin->setLabel( "Channels' min" );
	outputChannelMin->setHint( "Minimum value per channel" );
	outputChannelMin->setParent( outputGroup );

	OFX::RGBAParamDescriptor* outputChannelMax = desc.defineRGBAParam( kOutputChannelMax );
	outputChannelMax->setLabel( "Channels' max" );
	outputChannelMax->setParent( outputGroup );

	OFX::RGBAParamDescriptor* outputLuminosityMin = desc.defineRGBAParam( kOutputLuminosityMin );
	outputLuminosityMin->setLabel( "Luminosity min" );
	outputLuminosityMin->setParent( outputGroup );

	OFX::RGBAParamDescriptor* outputLuminosityMax = desc.defineRGBAParam( kOutputLuminosityMax );
	outputLuminosityMax->setLabel( "Luminosity max" );
	outputLuminosityMax->setParent( outputGroup );
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  effect handle
 * @param[in] context    Application context
 * @return  plugin instance
 */
OFX::ImageEffect* ImageStatisticsPluginFactory::createInstance( OfxImageEffectHandle handle,
                                                            OFX::ContextEnum context )
{
	return new ImageStatisticsPlugin(handle);
}

}
}
}

namespace OFX {
namespace Plugin {

void getPluginIDs( OFX::PluginFactoryArray& ids )
{
	static tuttle::plugin::average::ImageStatisticsPluginFactory p("fr.tuttle.average", 1, 0);
	ids.push_back(&p);
}

}
}
