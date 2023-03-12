#include "ExpressLink.h"

ExpressLinkConfig::ExpressLinkConfig(ExpressLink &el) : expresslink(el)
{
    // constructor
}

String ExpressLinkConfig::getTopic(uint8_t index)
{
    expresslink.cmd("CONF? Topic" + String(index));
    return expresslink.response;
}

bool ExpressLinkConfig::setTopic(uint8_t index, String topic)
{
    return expresslink.cmd("CONF Topic" + String(index) + "=" + topic);
}

/// @brief equivalent to: AT+CONF? About
/// @return value from the configuration dictionary
String ExpressLinkConfig::getAbout()
{
    expresslink.cmd("CONF? About");
    return expresslink.response;
}

/// @brief equivalent to: AT+CONF? Version
/// @return value from the configuration dictionary
String ExpressLinkConfig::getVersion()
{
    expresslink.cmd("CONF? Version");
    return expresslink.response;
}

/// @brief equivalent to: AT+CONF? TechSpec
/// @return value from the configuration dictionary
String ExpressLinkConfig::getTechSpec()
{
    expresslink.cmd("CONF? TechSpec");
    return expresslink.response;
}

/// @brief equivalent to: AT+CONF? ThingName
/// @return value from the configuration dictionary
String ExpressLinkConfig::getThingName()
{
    expresslink.cmd("CONF? ThingName");
    return expresslink.response;
}

/// @brief equivalent to: AT+CONF? Certificate pem
/// @return value from the configuration dictionary
String ExpressLinkConfig::getCertificate()
{
    expresslink.cmd("CONF? Certificate pem");
    expresslink.response = expresslink.readLine(expresslink.additionalLines);
    return expresslink.response;
}

/// @brief equivalent to: AT+CONF? CustomName
/// @return value from the configuration dictionary
String ExpressLinkConfig::getCustomName()
{
    expresslink.cmd("CONF? CustomName");
    return expresslink.response;
}

/// @brief equivalent to: AT+CONF CustomName={value}
/// @param value to be written to configuration dictionary
/// @return true on success, false on error
bool ExpressLinkConfig::setCustomName(const String &value)
{
    return expresslink.cmd("CONF CustomName=" + value);
}

/// @brief equivalent to: AT+CONF? Endpoint
/// @return value from the configuration dictionary
String ExpressLinkConfig::getEndpoint()
{
    expresslink.cmd("CONF? Endpoint");
    return expresslink.response;
}

/// @brief equivalent to: AT+CONF Endpoin={value}
/// @param value to be written to configuration dictionary
/// @return true on success, false on error
bool ExpressLinkConfig::setEndpoint(const String &value)
{
    return expresslink.cmd("CONF Endpoint=" + value);
}

/// @brief equivalent to: AT+CONF? Endpoint pem
/// @return value from the configuration dictionary as multi-line PEM-formatted string
String ExpressLinkConfig::getRootCA()
{
    expresslink.cmd("CONF? RootCA pem");
    expresslink.response = expresslink.readLine(expresslink.additionalLines);
    return expresslink.response;
}

/// @brief equivalent to: AT+CONF RootCA={value}
/// @param value to be written to configuration dictionary as multi-line PEM-formatted string
/// @return true on success, false on error
bool ExpressLinkConfig::setRootCA(const String &value)
{
    return expresslink.cmd("CONF RootCA=" + value);
}

/// @brief equivalent to: AT+CONF? ShadowToken
/// @return value from the configuration dictionary
String ExpressLinkConfig::getShadowToken()
{
    expresslink.cmd("CONF? ShadowToken");
    return expresslink.response;
}

/// @brief equivalent to: AT+CONF? DefenderPeriod
/// @return value from the configuration dictionary
uint32_t ExpressLinkConfig::getDefenderPeriod()
{
    expresslink.cmd("CONF? DefenderPeriod");
    return expresslink.response.toInt();
}

/// @brief equivalent to: AT+CONF? HOTAcertificate pem
/// @return value from the configuration dictionary as multi-line PEM-formatted string
String ExpressLinkConfig::getHOTAcertificate()
{
    expresslink.cmd("CONF? HOTAcertificate pem");
    expresslink.response = expresslink.readLine(expresslink.additionalLines);
    return expresslink.response;
}

/// @brief equivalent to: AT+CONF? OTAcertificate pem
/// @return value from the configuration dictionary as multi-line PEM-formatted string
String ExpressLinkConfig::getOTAcertificate()
{
    expresslink.cmd("CONF? OTAcertificate pem");
    expresslink.response = expresslink.readLine(expresslink.additionalLines);
    return expresslink.response;
}

/// @brief equivalent to: AT+CONF? SSID
/// @return value from the configuration dictionary
String ExpressLinkConfig::getSSID()
{
    expresslink.cmd("CONF? SSID");
    return expresslink.response;
}

/// @brief equivalent to: AT+CONF SSID={value}
/// @param value to be written to configuration dictionary
/// @return true on success, false on error
bool ExpressLinkConfig::setSSID(const String &value)
{
    return expresslink.cmd("CONF SSID=" + value);
}

/// @brief equivalent to: AT+CONF Passphrase={value}
/// @param value to be written to configuration dictionary
/// @return true on success, false on error
bool ExpressLinkConfig::setPassphrase(const String &value)
{
    return expresslink.cmd("CONF Passphrase=" + value);
}

/// @brief equivalent to: AT+CONF? APN
/// @return value from the configuration dictionary
String ExpressLinkConfig::getAPN()
{
    expresslink.cmd("CONF? APN");
    return expresslink.response;
}

ExpressLink::ExpressLink(void) : config(*this)
{
    // constructor
}

/// @brief Initializes an ExpressLink interface
/// @param u serial uart for AT commands (initialize separately with RX/TX pins)
/// @param event GPIO pin where ExpressLink EVENT pin is connected, set to -1 if not connected (default)
/// @param wake GPIO pin where ExpressLink WAKE pin is connected, set to -1 if not connected (default)
/// @param reset GPIO pin where ExpressLink RESET pin is connected, set to -1 if not connected (default)
/// @param debug uses the default `Serial` stream to print AT commands and responses. Only enable if `Serial` is connected to a different UART than the ExpressLink UART.
/// @return true on success, false on error
bool ExpressLink::begin(Stream &u, int event, int wake, int reset, bool d)
{
    debug = d;
    uart = &u;
    uart->setTimeout(120 * 1000); // 120 seconds

    if (resetPin >= 0)
    {
        resetPin = reset;
        pinMode(resetPin, OUTPUT);
    }
    if (eventPin >= 0)
    {
        eventPin = event;
        pinMode(eventPin, INPUT);
    }
    if (wakePin >= 0)
    {
        wakePin = wake;
        pinMode(wakePin, OUTPUT);
    }
    return selfTest();
}

String ExpressLink::readLine(uint32_t count)
{
    unsigned long start = millis();
    String response;
    uint16_t line_count = 0;
    for (uint8_t i = 0; i < count; i++)
    {
        do
        {
            int c = uart->read();
            if (c >= 0)
            {
                response += (char)c;
            }
            if (c == '\n')
            {
                line_count++;
                break;
            }
        } while (millis() - start < 120000);
    }
    if (line_count != count)
    {
        // error, not enough lines read - potential UART timeout happened
        return "";
    }

    unescape(response);
    response.trim();
    return response;
}

/// @brief Escapes string in-place so it can be written to ExpressLink UART
/// @param value string (will be modified)
void ExpressLink::escape(String &value)
{
    // see https://docs.aws.amazon.com/iot-expresslink/latest/programmersguide/elpg-commands.html#elpg-delimiters
    value.replace("\n", "\\A");
    value.replace("\r", "\\D");
    value.replace("\\", "\\\\");
}

/// @brief Unescapes string in-place after reading it from ExpressLink UART
/// @param value string (will be modified)
void ExpressLink::unescape(String &value)
{
    // see https://docs.aws.amazon.com/iot-expresslink/latest/programmersguide/elpg-commands.html#elpg-delimiters
    value.replace("\\A", "\n");
    value.replace("\\D", "\r");
    value.replace("\\\\", "\\");
}

/// @brief Execute AT command and reads all response lines. Escaping and unescaping is handled automatically. Check class attribute `response` (if true returned) and `error` (if false returned).
/// @param command: e.g., AT+CONNECT or SUBSCRIBE1 (with or without the `AT+` prefix)
/// @return true on success, false on error
bool ExpressLink::cmd(String command)
{
    escape(command);
    if (!command.startsWith("AT+"))
    {
        command = "AT+" + command;
    }

    if (debug)
    {
        Serial.println("> " + command);
    }
    uart->print(command + "\n");

    response = readLine();

    if (debug)
    {
        Serial.println("< " + response);
    }

    additionalLines = 0;
    if (response.startsWith("OK "))
    {
        response = response.substring(3); // trim off the `OK ` prefix
        error = "";
        return true;
    }
    else if (response.startsWith("OK"))
    {
        response = response.substring(2); // trim off the `OK` prefix
        if (response.length() > 0)
        {
            String l;
            for (auto &&c : response)
            {
                if (isDigit(c))
                {
                    l += c;
                }
                else
                {
                    break;
                }
            }
            additionalLines = l.toInt();
            response = response.substring(l.length() + 1); // trim off the number
        }
        error = "";
        return true;
    }
    else if (response.startsWith("ERR"))
    {
        error = response;
        response = "";
        return false;
    }
    else
    {
        error = response;
        response = "";
        return false;
    }
}

/// @brief equivalent to sending AT and checking for OK response line
/// @return true on success, false on error
bool ExpressLink::selfTest()
{
    uart->print("AT\n");
    auto response = readLine();
    return response == "OK";
}

/// @brief equivalent to: AT+CONNECT or AT+CONNECT!
/// @param async true to use non-blocking CONNECT!
/// @return true on success, false on error
bool ExpressLink::connect(bool async)
{
    if (async)
    {
        return cmd("CONNECT!");
    }
    else
    {
        return cmd("CONNECT");
    }
}

/// @brief equivalent to: AT+CONNECT? and parsing the response for CONNECTED/DISCONNECTED
/// @return true if connected, false if disconnected
bool ExpressLink::isConnected()
{
    // OK {status} {onboarded} [CONNECTED/DISCONNECTED] [STAGING/CUSTOMER]
    cmd("CONNECT?");
    return response.substring(0, 1) == "1";
}

/// @brief equivalent to: AT+CONNECT? and parsing the response for STAGING/CUSTOMER
/// @return true if onboarded to customer endpoint, false if staging endpoint
bool ExpressLink::isOnboarded()
{
    // OK {status} {onboarded} [CONNECTED/DISCONNECTED] [STAGING/CUSTOMER]
    cmd("CONNECT?");
    return response.substring(2, 3) == "1";
}

/// @brief equivalent to: AT+DISCONNECT
/// @return true on success, false on error
bool ExpressLink::disconnect()
{
    return cmd("DISCONNECT");
}

/// @brief soft-reboot of the module, equivalent to: AT+RESET
/// @return true on success, false on error
bool ExpressLink::reset()
{
    return cmd("RESET");
}

/// @brief wipe all data and config, equivalent to: AT+FACTORY_RESET
/// @return true on success, false on error
bool ExpressLink::factoryReset()
{
    return cmd("FACTORY_RESET");
}

/// @brief Gets the next pending Event, if available
/// @param checkPin true (default) if the EVENT pin should be read; false if the AT+EVENT? command should be used
/// @return Event struct with event code and parameter, `code`==NONE if no event is pending
ExpressLink::Event ExpressLink::getEvent(bool checkPin)
{
    // response format:
    //   OK [{event_identifier} {parameter} {mnemonic [detail]}]{EOL}

    Event event;
    if (checkPin && eventPin >= 0 && digitalRead(eventPin) == LOW)
    {
        event.code = NONE;
        event.parameter = 0;
        return event;
    }
    if (!cmd("EVENT?"))
    {
        event.code = UNKNOWN;
        event.parameter = 0;
        return event;
    }
    if (response == "") // OK prefix has already been parsed by cmd()
    {
        event.code = NONE;
        event.parameter = 0;
        return event;
    }

    char *next;
    auto code = strtol(response.c_str(), &next, 10);
    if (code >= FIRST_EVENT_CODE && code < LAST_EVENT_CODE)
    {
        event.code = EventCode(strtol(response.c_str(), &next, 10));
    }
    else
    {
        event.code = UNKNOWN;
        event.parameter = 0;
        return event;
    }

    char *rest;
    event.parameter = strtol(next, &rest, 10);

    response = String(rest);
    response = response.substring(response.indexOf(' ')); // make optional `detail` available

    return event;
}

/// @brief Subscribe to Topic#.
///
/// Equivalent to `AT+CONF Topic{topic_index}={topic_name}` followed by `AT+SUBSCRIBE{topic_index}`.
/// @param topic_index index to subscribe to
/// @param topic_name name of topic (empty to skip setting topic in configuration dictionary)
/// @return true on success, false on error
bool ExpressLink::subscribe(uint8_t topic_index, String topic_name)
{
    if (topic_name.length() > 0)
    {
        config.setTopic(topic_index, topic_name);
    }
    return cmd("SUBSCRIBE" + String(topic_index));
}

/// @brief Unsubscribe from Topic#.
///
/// Equivalent to `AT+UNSUBSCRIBE{topic_index}`.
/// @param topic_index index to unsubscribe from
/// @return true on success, false on error
bool ExpressLink::unsubscribe(uint8_t topic_index)
{
    return cmd("UNSUBSCRIBE" + String(topic_index));
}

/// @brief Request next message pending on the indicated topic.
///
/// Equivalent to `AT+GET{topic_index}`.
/// @param topic_index use -1 (default) for `GET`, or value for `GETx`
/// @return true on success, false on error
bool ExpressLink::get(uint8_t topic_index)
{
    return cmd("GET" + String(topic_index));
}

/// @brief Same as `ExpressLink::publish - use it instead.`
/// @param topic_index
/// @param message
/// @return true on success, false on error
bool ExpressLink::send(uint8_t topic_index, String message)
{
    return publish(topic_index, message);
}

/// @brief Publish msg on a topic selected from topic list.
///
/// Equivalent to `AT+SEND{topic_index} {message}`.
/// @param topic_index the topic index to publish to
/// @param message raw message to publish, typically JSON-encoded
/// @return true on success, false on error
bool ExpressLink::publish(uint8_t topic_index, String message)
{
    return cmd("SEND" + String(topic_index) + " " + message);
}

/// @brief Fetches the current state of the OTA process.
///
/// Equivalent to `AT+OTA?`.
/// @return OTA state with code and detail
ExpressLink::OTAState ExpressLink::otaGetState()
{
    OTAState s;
    cmd("OTA?");
    s.code = OTACode(response.charAt(0) - '0'); // get numerical digit value from string character
    if (response.length() > 2)
    {
        s.detail = String(response.substring(2));
    }
    else
    {
        s.detail = "";
    }
    return s;
}

/// @brief Allow the OTA operation to proceed.
///
/// Equivalent to `AT+OTA ACCEPT<EOL>`.
/// @return true on success, false on error
bool ExpressLink::otaAccept()
{
    return cmd("OTA ACCEPT");
}

/// @brief Requests the next # bytes from the OTA buffer.
///
/// Equivalent to `AT+OTA READ {count}<EOL>`.
///
/// Retreive payload from `ExpressLink::response`
/// @param count decimal value of number of bytes to read
/// @return true on success, false on error
bool ExpressLink::otaRead(uint32_t count)
{
    return cmd("OTA READ " + String(count));
}

/// @brief Moves the read pointer to an absolute address.
///
/// Equivalent to `AT+OTA SEEK<EOL>` or `AT+OTA SEEK {address}<EOL>`.
/// @param address decimal value for read pointer to seek to, or -1 to seek to beginning
/// @return true on success, false on error
bool ExpressLink::otaSeek(uint32_t address)
{
    if (address == (uint32_t)-1)
    {
        return cmd("OTA SEEK");
    }
    else
    {
        return cmd("OTA SEEK " + String(address));
    }
}

/// @brief Authorize the ExpressLink module to apply the new image.
///
/// Equivalent to `AT+OTA APPLY<EOL>`.
/// @return true on success, false on error
bool ExpressLink::otaApply()
{
    return cmd("OTA APPLY");
}

/// @brief The host OTA operation is completed.
///
/// Equivalent to `AT+OTA CLOSE<EOL>`.
/// @return true on success, false on error
bool ExpressLink::otaClose()
{
    return cmd("OTA CLOSE");
}

/// @brief The contents of the OTA buffer are emptied.
///
/// Equivalent to `AT+OTA FLUSH<EOL>`.
/// @return true on success, false on error
bool ExpressLink::otaFlush()
{
    return cmd("OTA FLUSH");
}

/// @brief Initialize communication with the Device Shadow service.
///
/// Equivalent to `AT+SHADOW{index} INIT<EOL>`.
/// @param index shadow index. Use -1 (default), to select the unnamed shadow.
/// @return true on success, false on error
bool ExpressLink::shadowInit(uint8_t index)
{
    String i = (index >= 0) ? String(index) : "";
    return cmd("SHADOW" + i + " INIT");
}

/// @brief Request a Device Shadow document.
///
/// Equivalent to `AT+SHADOW{index} DOC<EOL>`.
/// @param index shadow index. Use -1 (default), to select the unnamed shadow.
/// @return true on success, false on error
bool ExpressLink::shadowDoc(uint8_t index)
{
    String i = (index >= 0) ? String(index) : "";
    return cmd("SHADOW" + i + " DOC");
}

/// @brief Retrieve a device shadow document.
///
/// Equivalent to `AT+SHADOW{index} GET DOC<EOL>`.
/// @param index shadow index. Use -1 (default), to select the unnamed shadow.
/// @return true on success, false on error
bool ExpressLink::shadowGetDoc(uint8_t index)
{
    String i = (index >= 0) ? String(index) : "";
    return cmd("SHADOW" + i + " GET DOC");
}

/// @brief Request a device shadow document update.
///
/// Equivalent to `AT+SHADOW{index} UPDATE {new_state}<EOL>`.
/// @param index shadow index. Use -1 (default), to select the unnamed shadow.
/// @return true on success, false on error
bool ExpressLink::shadowUpdate(String new_state, uint8_t index)
{
    String i = (index >= 0) ? String(index) : "";
    return cmd("SHADOW" + i + " UPDATE " + new_state);
}

/// @brief Retrieve a device shadow update response.
///
/// Equivalent to `AT+SHADOW{index} GET UPDATE<EOL>`.
/// @param index shadow index. Use -1 (default), to select the unnamed shadow.
/// @return true on success, false on error
bool ExpressLink::shadowGetUpdate(uint8_t index)
{
    String i = (index >= 0) ? String(index) : "";
    return cmd("SHADOW" + i + " GET UPDATE");
}

/// @brief Subscribe to a device shadow document.
///
/// Equivalent to `AT+SHADOW{index} SUBSCRIBE<EOL>`.
/// @param index shadow index. Use -1 (default), to select the unnamed shadow.
/// @return true on success, false on error
bool ExpressLink::shadowSubscribe(uint8_t index)
{
    String i = (index >= 0) ? String(index) : "";
    return cmd("SHADOW" + i + " SUBSCRIBE");
}

/// @brief Unsubscribe from a device shadow document.
///
/// Equivalent to `AT+SHADOW{index} UNSUBSCRIBE<EOL>`.
/// @param index shadow index. Use -1 (default), to select the unnamed shadow.
/// @return true on success, false on error
bool ExpressLink::shadowUnsubscribe(uint8_t index)
{
    String i = (index >= 0) ? String(index) : "";
    return cmd("SHADOW" + i + " UNSUBSCRIBE");
}

/// @brief Retrieve a Shadow Delta message.
///
/// Equivalent to `AT+SHADOW{index} GET DELTA<EOL>`.
/// @param index shadow index. Use -1 (default), to select the unnamed shadow.
/// @return true on success, false on error
bool ExpressLink::shadowGetDelta(uint8_t index)
{
    String i = (index >= 0) ? String(index) : "";
    return cmd("SHADOW" + i + " GET DELTA");
}

/// @brief Request the deletion of a Shadow document.
///
/// Equivalent to `AT+SHADOW{index} DELETE<EOL>`.
/// @param index shadow index. Use -1 (default), to select the unnamed shadow.
/// @return true on success, false on error
bool ExpressLink::shadowDelete(uint8_t index)
{
    String i = (index >= 0) ? String(index) : "";
    return cmd("SHADOW" + i + " DELETE");
}

/// @brief Request a Shadow delete response.
///
/// Equivalent to `AT+SHADOW{index} GET DELETE<EOL>`.
/// @param index shadow index. Use -1 (default), to select the unnamed shadow.
/// @return true on success, false on error
bool ExpressLink::shadowGetDelete(uint8_t index)
{
    String i = (index >= 0) ? String(index) : "";
    return cmd("SHADOW" + i + " GET DELETE");
}

/// @brief Enters Serial/UART passthrough mode.
/// All serial communication is bridged between the ExpressLink UART and the passed `destination`.
/// This function never returns. You can use it for debugging or over-the-wire firmware upgrades.
void ExpressLink::passthrough(Stream *destination)
{
    // inspired by https://docs.arduino.cc/built-in-examples/communication/SerialPassthrough
    while (true)
    {
        if (destination->available())
        {
            uart->write(destination->read());
        }
        if (uart->available())
        {
            destination->write(uart->read());
        }
    }
}
