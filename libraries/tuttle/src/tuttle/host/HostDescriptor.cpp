#include "HostDescriptor.hpp"
#include "ImageEffectNode.hpp"
#include "attribute/ClipImage.hpp"

// ofx host
#include <tuttle/host/ofx/OfxhBinary.hpp>
#include <tuttle/host/ofx/OfxhMemory.hpp>
#include <tuttle/host/ofx/OfxhImageEffectNode.hpp>
#include <tuttle/host/ofx/OfxhPluginAPICache.hpp>
#include <tuttle/host/ofx/OfxhPluginCache.hpp>
#include <tuttle/host/ofx/OfxhHost.hpp>
#include <tuttle/host/ofx/OfxhImageEffectPlugin.hpp>
#include <tuttle/host/ofx/property/OfxhSet.hpp>
#include <tuttle/host/ofx/attribute/OfxhClip.hpp>
#include <tuttle/host/ofx/attribute/OfxhParam.hpp>

// ofx
#include <ofxCore.h>
#include <ofxImageEffect.h>

#include <iostream>
#include <fstream>
#include <cstring>

/// environment variable to override the host kOfxPropName
/// This is useful if the plugin defines specific behavior depending on the host
#define kHostPropNameEnvVar "TUTTLE_OVERRIDE_HOST_OFXPROPNAME"

namespace tuttle
{
namespace host
{

Host::Host()
{
    /// @todo tuttle set host properties correctly...
    const std::string hostPropName(std::getenv(kHostPropNameEnvVar) ? std::getenv(kHostPropNameEnvVar) : "TuttleOfx");
    _properties.setStringProperty(kOfxPropName, hostPropName);
    _properties.setStringProperty(kOfxPropLabel, "TuttleOfx Alpha");
    _properties.setIntProperty(kOfxImageEffectHostPropIsBackground, false);
    _properties.setIntProperty(kOfxImageEffectPropSupportsOverlays, true);
    _properties.setIntProperty(kOfxImageEffectPropSupportsMultiResolution, true);
    _properties.setIntProperty(kOfxImageEffectPropSupportsTiles, true); ///< @todo tuttle: we hope to do this !
    _properties.setIntProperty(kOfxImageEffectPropTemporalClipAccess, true);
    _properties.setStringProperty(kOfxImageEffectPropSupportedComponents, kOfxImageComponentRGBA, 0);
    _properties.setStringProperty(kOfxImageEffectPropSupportedComponents, kOfxImageComponentRGB, 1);
    _properties.setStringProperty(kOfxImageEffectPropSupportedComponents, kOfxImageComponentAlpha, 2);
    _properties.setStringProperty(kOfxImageEffectPropSupportedContexts, kOfxImageEffectContextFilter, 0);
    _properties.setStringProperty(kOfxImageEffectPropSupportedContexts, kOfxImageEffectContextGenerator, 1);
    _properties.setStringProperty(kOfxImageEffectPropSupportedContexts, kOfxImageEffectContextRetimer, 2);
    _properties.setStringProperty(kOfxImageEffectPropSupportedContexts, kOfxImageEffectContextTransition, 3);
    _properties.setStringProperty(kOfxImageEffectPropSupportedContexts, kOfxImageEffectContextGeneral, 4);
    //	_properties.setStringProperty( kOfxImageEffectPropSupportedContexts, kOfxImageEffectContextReader, 5 );
    //	_properties.setStringProperty( kOfxImageEffectPropSupportedContexts, kOfxImageEffectContextWriter, 6 );
    _properties.setStringProperty(kOfxImageEffectPropSupportedPixelDepths, kOfxBitDepthFloat, 0);
    _properties.setStringProperty(kOfxImageEffectPropSupportedPixelDepths, kOfxBitDepthShort, 1);
    _properties.setStringProperty(kOfxImageEffectPropSupportedPixelDepths, kOfxBitDepthByte, 2);
    _properties.setIntProperty(kOfxImageEffectPropSupportsMultipleClipDepths, true);
    _properties.setIntProperty(kOfxImageEffectPropSupportsMultipleClipPARs, true);
    _properties.setIntProperty(kOfxImageEffectPropSetableFrameRate, true);
    _properties.setIntProperty(kOfxImageEffectPropSetableFielding, false);
    _properties.setIntProperty(kOfxParamHostPropSupportsStringAnimation, false);
    _properties.setIntProperty(kOfxParamHostPropSupportsCustomInteract, true);
    _properties.setIntProperty(kOfxParamHostPropSupportsChoiceAnimation, true);
    _properties.setIntProperty(kOfxParamHostPropSupportsBooleanAnimation, true);
    _properties.setIntProperty(kOfxParamHostPropSupportsCustomAnimation, false);
    _properties.setIntProperty(kOfxParamHostPropMaxParameters, -1);
    _properties.setIntProperty(kOfxParamHostPropMaxPages, 0);
    _properties.setIntProperty(kOfxParamHostPropPageRowColumnCount, 0, 0);
    _properties.setIntProperty(kOfxParamHostPropPageRowColumnCount, 0, 1);
}

tuttle::host::ImageEffectNode* Host::newInstance(tuttle::host::ofx::imageEffect::OfxhImageEffectPlugin& plugin,
                                                 tuttle::host::ofx::imageEffect::OfxhImageEffectNodeDescriptor& desc,
                                                 const std::string& context) const
{
    return new tuttle::host::ImageEffectNode(plugin, desc, context);
}

/// Override this to create a descriptor, this makes the 'root' descriptor
tuttle::host::ofx::imageEffect::OfxhImageEffectNodeDescriptor*
Host::makeDescriptor(tuttle::host::ofx::imageEffect::OfxhImageEffectPlugin& plugin) const
{
    tuttle::host::ofx::imageEffect::OfxhImageEffectNodeDescriptor* desc =
        new tuttle::host::ofx::imageEffect::OfxhImageEffectNodeDescriptor(plugin);

    return desc;
}

/// used to construct a context description, rootContext is the main context
tuttle::host::ofx::imageEffect::OfxhImageEffectNodeDescriptor*
Host::makeDescriptor(const tuttle::host::ofx::imageEffect::OfxhImageEffectNodeDescriptor& rootContext,
                     tuttle::host::ofx::imageEffect::OfxhImageEffectPlugin& plugin) const
{
    return new tuttle::host::ofx::imageEffect::OfxhImageEffectNodeDescriptor(rootContext, plugin);
}

/// used to construct populate the cache
tuttle::host::ofx::imageEffect::OfxhImageEffectNodeDescriptor*
Host::makeDescriptor(const std::string& bundlePath, tuttle::host::ofx::imageEffect::OfxhImageEffectPlugin& plugin) const
{
    return new tuttle::host::ofx::imageEffect::OfxhImageEffectNodeDescriptor(bundlePath, plugin);
}

/// message
OfxStatus Host::vmessage(const char* type, const char* id, const char* format, va_list args) const
{
    bool isQuestion = false;
    const char* prefix = "Message : ";

    if(strcmp(type, kOfxMessageLog) == 0)
    {
        prefix = "Log : ";
    }
    else if(strcmp(type, kOfxMessageFatal) == 0 || strcmp(type, kOfxMessageError) == 0)
    {
        prefix = "Error : ";
    }
    else if(strcmp(type, kOfxMessageQuestion) == 0)
    {
        prefix = "Question : ";
        isQuestion = true;
    }

    // Just dump our message to stdout, should be done with a proper
    // UI in a full ap, and post a dialogue for yes/no questions.
    fputs(prefix, stdout);
    vprintf(format, args);
    printf("\n");

    if(isQuestion)
    {
        /// cant do this properly inour example, as we need to raise a dialogue to ask a question, so just return yes
        return kOfxStatReplyYes;
    }
    else
    {
        return kOfxStatOK;
    }
}
}
}
