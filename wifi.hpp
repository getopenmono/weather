// This software is part of OpenMono, see http://developer.openmono.com
// Released under the MIT license, see LICENSE.txt
#ifndef __com_openmono_wifi_h
#define __com_openmono_wifi_h
#include <mbed.h>
#include <mono.h>
#include <wireless/module_communication.h>
#include <wireless/redpine_module.h>
#include <network_request.h>
#include "lib/bytestring.hpp"
#include "lib/ibytebuffer.hpp"
#include "lib/iconfiguration.hpp"
using mono::network::HttpClient;
using mono::network::INetworkRequest;

#define MONO_WIFI_SSID (uint8_t const *)(CONF_KEY_ROOT "/wifi/ssid.txt")
#define MONO_WIFI_PASSWORD (uint8_t const *)(CONF_KEY_ROOT "/wifi/password.txt")

class Wifi
{
public:

    enum Status
    {
        Wifi_NotConnected, // 0
        Wifi_BrokenConfiguration, // 1
        Wifi_Connecting, // 2
        Wifi_Ready, // 3
        Wifi_NotReady, // 4
        Wifi_BrokenUrl, // 5
        Wifi_Receiving, // 6
        Wifi_BrokenCommunication, // 7
        Wifi_DnsResolutionFailed, // 8
        Wifi_UnknownError // 9
    };

    /**
     * Prepare a wireless connection to an access point which is determined
     * by the values of the configuration keys "/wifi/ssid.txt" and
     * "/wifi/password.txt".
     */
    Wifi (IConfiguration const & configuration);

    /**
     * Start up a wireless connection.
     * @param  context object that will handle the connection.
     * @param  handler method in context that will be called when connection is established.
     * @return         status of connection when starting connection.
     */
    template <typename Class>
    Status connect (Class * context, void (Class::*handler)())
    {
        connectHandler.attach(context,handler);
        return startConnection();
    }

    /**
     * Close the wireless connection.
     */
    ~Wifi ();

    /**
     * Retrieve a document via HTTP GET.
     * @param  url             URL to request document from.
     * @param  buffer          byte buffer that can store the document.
     * @param  context         object that will handle responses and statuses.
     * @param  documentHandler method in object that will be called when the document has been retrieved.
     * @param  changeHandler   optional method in object that will be called whenever the status changes.
     */
    template <typename Class>
    Status get
    (
        uint8_t const * url,
        IByteBuffer * buffer,
        Class * context,
        void (Class::*documentHandler)(IByteBuffer*),
        void (Class::*changeHandler)(Status) = 0
    ) {
        if (changeHandler != 0) statusHandler.attach(context,changeHandler);
        if (_status != Wifi_Ready)
        {
            statusHandler.call(_status);
            return Wifi_NotReady;
        }
        _buffer = buffer;
        receivedHandler.attach(context,documentHandler);
        return startRequest(url);
    }

    Status status () const;

private:
    Status startConnection ();
    Status startRequest (uint8_t const * url);
    void networkReadyHandler ();
    void httpHandleData (HttpClient::HttpResponseData const & data);
    void destroyClient ();

    mbed::SPI spi;
    mono::redpine::ModuleSPICommunication spiComm;
    HttpClient client;
    ByteString ssid;
    ByteString password;
    IByteBuffer * _buffer;
    Status _status;
    IConfiguration const & configuration;
    void networkErrorHandler (INetworkRequest::ErrorEvent * error);
    mbed::FunctionPointerArg1<void,IByteBuffer*> receivedHandler;
    mbed::FunctionPointerArg1<void,Status> statusHandler;
    mbed::FunctionPointerArg1<void,void> connectHandler;
};

#endif // __com_openmono_wifi_h
