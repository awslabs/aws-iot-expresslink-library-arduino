#line 2 "test.ino"

#include <AUnit.h>

#include <Wire.h>
#include <ExpressLink.h>

using namespace aunit;

void setup() {
  Serial.begin(115200); // Serial in tests here is stdin/stdout, so baud rate doesn't matter!
}

void loop() {
  aunit::TestRunner::run();
}

class MockStream: public Stream {
  public:
    MockStream(String c, String r) : command(c), response(r) {}

    size_t write(uint8_t c) {
      if (c == command.charAt(command_index)) {
        command_index++;
        return 1;
      }

      Serial.printf("ERROR: write mismatch! Expected '0x%x' but got '0x%x' at position %d\n", command.charAt(command_index), c, command_index);
      exit(1);
    }

    int available() {
      return response_index < response.length();
    }

    int read() {
      char c = peek();
      response_index++;
      return c;
    }

    int peek() {
      if (response_index < response.length()) {
        return response.charAt(response_index);
      } else {
        return -1;
      }
    }

    bool valid() {
      if (command_index != command.length())
      {
        Serial.printf("Command: %d out of %d\n", command_index, command.length());
        return false;
      }
      if (response_index != response.length())
      {
        Serial.printf("Response: %d out of %d\n", response_index, response.length());
        return false;
      }
      return true;
    }

    String command;
    uint32_t command_index = 0;

    String response;
    uint32_t response_index = 0;
};

test(selfTest) {
  MockStream s("AT\nAT\n", "OK\nOK\r\n");

  ExpressLink el;
  assertTrue(el.begin(s));
  assertTrue(el.begin(s));

  assertTrue(s.valid());
}

test(successCommand) {
  MockStream s("AT\nAT+CONF X=y\n", "OK\r\nOK2 yes\r\nmaybe\r\nsure\r\n");

  ExpressLink el;
  assertTrue(el.begin(s));
  assertTrue(el.cmd("CONF X=y"));
  assertTrue(el.additionalLines == 2);
  assertEqual(el.response, "yes");
  assertEqual(el.readLine(el.additionalLines), "maybe\r\nsure");
  assertEqual(el.error, "");

  assertTrue(s.valid());
}

test(errorCommand) {
  MockStream s("AT\nAT+CONF INVALID null\n", "OK\r\nERR123 some error occured\r\n");

  ExpressLink el;
  assertTrue(el.begin(s));
  assertFalse(el.cmd("CONF INVALID null"));
  assertEqual(el.response, "");
  assertEqual(el.error, "ERR123 some error occured");

  assertTrue(s.valid());
}

test(confCommandGet) {
  MockStream s("AT\nAT+CONF? Endpoint\n", "OK\r\nOK example.com\r\n");

  ExpressLink el;
  assertTrue(el.begin(s));
  assertEqual(el.config.getEndpoint(), "example.com");

  assertTrue(s.valid());
}

test(confCommandSet) {
  MockStream s("AT\nAT+CONF Endpoint=example.com\n", "OK\r\nOK\r\n");

  ExpressLink el;
  assertTrue(el.begin(s));
  assertTrue(el.config.setEndpoint("example.com"));

  assertTrue(s.valid());
}

test(otaState) {
  MockStream s("AT\nAT+OTA?\nAT+OTA?\n", "OK\r\nOK 0\r\nOK 1 detail\r\n");

  ExpressLink el;
  assertTrue(el.begin(s));

  auto state = el.otaGetState();
  assertEqual(state.code, ExpressLink::OTACode::NoOTAInProgress);
  assertTrue(state.detail == "");

  state = el.otaGetState();
  assertEqual(state.code, ExpressLink::OTACode::UpdateProposed);
  assertTrue(state.detail == "detail");

  assertTrue(s.valid());
}
