#pragma once

#include "Arduino.h"
#include "Stream.h"

class ExpressLink;

class ExpressLinkConfig
{
public:
    ExpressLinkConfig(ExpressLink &el);

    bool get(String key);
    bool set(String key, String value);

    String getAbout();
    String getVersion();
    String getTechSpec();
    String getThingName();
    String getCertificate();

    String getCustomName();
    bool setCustomName(const String &value);

    String getEndpoint();
    bool setEndpoint(const String &value);

    String getRootCA();
    bool setRootCA(const String &value);

    String getShadowToken();
    bool setShadowToken(const String &value);

    uint32_t getDefenderPeriod();
    bool setDefenderPeriod(const uint32_t value);

    String getHOTAcertificate();
    bool setHOTAcertificate(const String &value);

    String getOTAcertificate();
    bool setOTAcertificate(const String &value);

    String getSSID();
    bool setSSID(const String &value);

    bool setPassphrase(const String &value);

    String getAPN();
    bool setAPN(const String &value);

    String getTopic(uint8_t index);
    bool setTopic(uint8_t index, String topic);

    String getShadow(uint8_t index);
    bool setShadow(uint8_t index, String topic);

private:
    ExpressLink &expresslink;
};

class ExpressLink
{
    friend class ExpressLinkConfig;

public:
    /// @brief see https://docs.aws.amazon.com/iot-expresslink/latest/programmersguide/elpg-event-handling.html#elpg-event-handling-commands
    enum EventCode : int16_t
    {
        UNKNOWN = -2, /// Failed to parse response, Check `response` for full event. Not defined in ExpressLink TechSpec.
        NONE = -1,    /// No event retrieved, queue was empty. Not defined in ExpressLink TechSpec.
        FIRST_EVENT_CODE = 1,
        MSG = 1,      /// A message was received on the topic #. Parameter=Topic Index
        STARTUP = 2,  /// The module has entered the active state. Parameter=0
        CONLOST = 3,  /// Connection unexpectedly lost. Parameter=0
        OVERRUN = 4,  /// Receive buffer Overrun (topic in detail). Parameter=0
        OTA = 5,      /// OTA event (see the OTA? command for details). Parameter=0
        CONNECT = 6,  /// A connection was established or failed. Parameter=Connection Hint
        CONFMODE = 7, /// CONFMODE exit with success. Parameter=0
        SUBACK = 8,   /// A subscription was accepted. Parameter=Topic Index
        SUBNACK = 9,  /// A subscription was rejected. Parameter=Topic Index
        // 10..19 RESERVED
        SHADOW_INIT = 20,        /// Shadow[Shadow Index] interface was initialized successfully. Parameter=Shadow Index
        SHADOW_INIT_FAILED = 21, /// The SHADOW[Shadow Index] interface initialization failed. Parameter=Shadow Index
        SHADOW_DOC = 22,         /// A Shadow document was received. Parameter=Shadow Index
        SHADOW_UPDATE = 23,      /// A Shadow update result was received. Parameter=Shadow Index
        SHADOW_DELTA = 24,       /// A Shadow delta update was received. Parameter=Shadow Index
        SHADOW_DELETE = 25,      /// A Shadow delete result was received. Parameter=Shadow Index
        SHADOW_SUBACK = 26,      /// A Shadow delta subscription was accepted. Parameter=Shadow Index
        SHADOW_SUBNACK = 27,     /// A Shadow delta subscription was rejected. Parameter=Shadow Index
        LAST_EVENT_CODE,
        // <= 999 reserved
        // >= 1000 available for custom implementation
    };

    /// @brief see https://docs.aws.amazon.com/iot-expresslink/latest/programmersguide/elpg-ota-updates.html#elpg-ota-commands
    enum OTACode : int16_t
    {
        NoOTAInProgress = 0,          /// No OTA in progress.
        UpdateProposed = 1,           /// A new module OTA update is being proposed. The host can inspect the version number and decide to accept or reject it. The {detail} field provides the version information (string).
        HostUpdateProposed = 2,       /// A new Host OTA update is being proposed. The host can inspect the version details and decide to accept or reject it. The {detail} field provides the metadata that is entered by the operator (string).
        OTAInProgress = 3,            /// OTA in progress. The download and signature verification steps have not been completed yet.
        NewExpressLinkImageReady = 4, /// A new module firmware image has arrived. The signature has been verified and the ExpressLink module is ready to reboot. (Also, an event was generated.)
        NewHostImageReady = 5,        /// A new host image has arrived. The signature has been verified and the ExpressLink module is ready to read its contents to the host. The size of the file is indicated in the response detail. (Also, an event was generated.)
    };

    struct Event
    {
        EventCode code;
        int parameter;
    };

    struct OTAState
    {
        OTACode code;
        String detail;
    };

    ExpressLink(void);
    bool begin(Stream &s, int event = -1, int wake = -1, int reset = -1, bool debug = false);

    bool cmd(String command);

    bool selfTest();

    bool connect(bool async = false);
    bool isConnected();
    bool isOnboarded();
    bool disconnect();

    bool reset();
    bool factoryReset();
    bool sleep(uint32_t duration, uint8_t sleep_mode = 0);

    Event getEvent(bool checkPin = true);

    bool subscribe(uint8_t topic_index, String topic_name);
    bool unsubscribe(uint8_t topic_index);
    bool get(uint8_t topic_index = -1); // -1 = GET, 0...X = GETX
    bool send(uint8_t topic_index, String message);
    bool publish(uint8_t topic_index, String message);

    OTAState otaGetState();
    bool otaAccept();
    bool otaRead(uint32_t count);
    bool otaSeek(uint32_t address = -1);
    bool otaApply();
    bool otaClose();
    bool otaFlush();

    bool shadowInit(uint8_t index = -1);
    bool shadowDoc(uint8_t index = -1);
    bool shadowGetDoc(uint8_t index = -1);
    bool shadowUpdate(String new_state, uint8_t index = -1);
    bool shadowGetUpdate(uint8_t index = -1);
    bool shadowSubscribe(uint8_t index = -1);
    bool shadowUnsubscribe(uint8_t index = -1);
    bool shadowGetDelta(uint8_t index = -1);
    bool shadowDelete(uint8_t index = -1);
    bool shadowGetDelete(uint8_t index = -1);

    void passthrough(Stream* destination);

    ExpressLinkConfig config;

    String readLine(uint32_t count = 1);
    String response;
    String error;
    uint32_t additionalLines;

    /// @brief The default UART configuration shall be 115200, 8, N, 1
    /// (baud rate: 115200; data bits: 8; parity: none; stop bits: 1).
    /// There is no hardware or software flow control for UART communications.
    /// See https://docs.aws.amazon.com/iot-expresslink/latest/programmersguide/elpg-commands.html#elpg-commands-introduction
    static const uint32_t BAUDRATE = 115200;

    /// @brief The maximum runtime for every command must be listed in the datasheet.
    /// No command can take more than 120 seconds to complete (the maximum time for a TCP connection timeout).
    /// See https://docs.aws.amazon.com/iot-expresslink/latest/programmersguide/elpg-commands.html#elpg-response-timeout
    static const uint32_t TIMEOUT = 120000; // milliseconds

protected:
    void escape(String &value);
    void unescape(String &value);

private:
    bool debug;
    Stream *uart;
    int resetPin = -1;
    int eventPin = -1;
    int wakePin = -1;
};
