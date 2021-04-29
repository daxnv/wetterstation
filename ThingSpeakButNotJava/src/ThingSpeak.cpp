/*
  ThingSpeak Communication Library For Arduino, ESP8266 and ESP32
  
  ThingSpeak ( https://www.thingspeak.com ) is an analytic IoT platform service that allows you to aggregate, visualize and 
  analyze live data streams in the cloud.
  
  Copyright 2017, The MathWorks, Inc.
 
  See the accompaning licence file for licensing information.
*/

#include "ThingSpeak.h"
ThingSpeakClass ThingSpeak;

ThingSpeakClass::ThingSpeakClass() {
    resetWriteFields();
    this->lastReadStatus = TS_OK_SUCCESS;
}

bool ThingSpeakClass::begin(Client &client) {
#ifdef PRINT_DEBUG_MESSAGES
    Serial.println("ts::tsBegin");
#endif

    this->setClient(&client);

    this->setPort(THINGSPEAK_PORT_NUMBER);
#if defined(TS_ENABLE_SSL)
    #if defined(WIFISSLCLIENT_H) || defined(wificlientbearssl_h) || defined(WiFiClientSecure_h)
                    this->setPort(THINGSPEAK_HTTPS_PORT_NUMBER);
                #else
                    Serial.println("WARNING: This library doesn't support SSL connection to ThingSpeak. Default HTTP Connection used.");
                #endif
#endif

    resetWriteFields();
    this->lastReadStatus = TS_OK_SUCCESS;
    return true;
}

int ThingSpeakClass::writeField(unsigned long channelNumber, unsigned int field, int value, const char *writeAPIKey) {
    char valueString[10];  // int range is -32768 to 32768, so 7 bytes including terminator, plus a little extra
    itoa(value, valueString, 10);
    return writeField(channelNumber, field, valueString, writeAPIKey);
}

int ThingSpeakClass::writeField(unsigned long channelNumber, unsigned int field, long value, const char *writeAPIKey) {
    char valueString[15];  // long range is -2147483648 to 2147483647, so 12 bytes including terminator
    ltoa(value, valueString, 10);
    return writeField(channelNumber, field, valueString, writeAPIKey);
}

int ThingSpeakClass::writeField(unsigned long channelNumber, unsigned int field, float value, const char *writeAPIKey) {
#ifdef PRINT_DEBUG_MESSAGES
    Serial.print("ts::writeField (channelNumber: "); Serial.print(channelNumber); Serial.print(" writeAPIKey: "); Serial.print(writeAPIKey); Serial.print(" field: "); Serial.print(field); Serial.print(" value: "); Serial.print(value,5); Serial.println(")");
#endif
    char valueString[20]; // range is -999999000000.00000 to 999999000000.00000, so 19 + 1 for the terminator
    int status = convertFloatToChar(value, valueString);
    if(status != TS_OK_SUCCESS) return status;

    return writeField(channelNumber, field, valueString, writeAPIKey);
}

int ThingSpeakClass::writeField(unsigned long channelNumber, unsigned int field, String value, const char *writeAPIKey) {
    // Invalid field number specified
    if(field < FIELDNUM_MIN || field > FIELDNUM_MAX) return TS_ERR_INVALID_FIELD_NUM;
    // Max # bytes for ThingSpeak field is 255
    if(value.length() > FIELDLENGTH_MAX) return TS_ERR_OUT_OF_RANGE;

#ifdef PRINT_DEBUG_MESSAGES
    Serial.print("ts::writeField (channelNumber: "); Serial.print(channelNumber); Serial.print(" writeAPIKey: "); Serial.print(writeAPIKey); Serial.print(" field: "); Serial.print(field); Serial.print(" value: \""); Serial.print(value); Serial.println("\")");
#endif
    String postMessage = String("field");
    postMessage.concat(field);
    postMessage.concat("=");
    postMessage.concat(value);

    return writeRaw(channelNumber, postMessage, writeAPIKey);
}

int ThingSpeakClass::setField(unsigned int field, int value) {
    char valueString[10];  // int range is -32768 to 32768, so 7 bytes including terminator
    itoa(value, valueString, 10);

    return setField(field, valueString);
}

int ThingSpeakClass::setField(unsigned int field, long value) {
    char valueString[15];  // long range is -2147483648 to 2147483647, so 12 bytes including terminator
    ltoa(value, valueString, 10);

    return setField(field, valueString);
}

int ThingSpeakClass::setField(unsigned int field, float value) {
    char valueString[20]; // range is -999999000000.00000 to 999999000000.00000, so 19 + 1 for the terminator
    int status = convertFloatToChar(value, valueString);
    if(status != TS_OK_SUCCESS) return status;

    return setField(field, valueString);
}

int ThingSpeakClass::setField(unsigned int field, String value) {
#ifdef PRINT_DEBUG_MESSAGES
    Serial.print("ts::setField   (field: "); Serial.print(field); Serial.print(" value: \""); Serial.print(value); Serial.println("\")");
#endif
    if(field < FIELDNUM_MIN || field > FIELDNUM_MAX) return TS_ERR_INVALID_FIELD_NUM;
    // Max # bytes for ThingSpeak field is 255 (UTF-8)
    if(value.length() > FIELDLENGTH_MAX) return TS_ERR_OUT_OF_RANGE;
    this->nextWriteField[field - 1] = value;

    return TS_OK_SUCCESS;
}

int ThingSpeakClass::setLatitude(float latitude) {
#ifdef PRINT_DEBUG_MESSAGES
    Serial.print("ts::setLatitude(latitude: "); Serial.print(latitude,3); Serial.println("\")");
#endif
    this->nextWriteLatitude = latitude;

    return TS_OK_SUCCESS;
}

int ThingSpeakClass::setLongitude(float longitude) {
#ifdef PRINT_DEBUG_MESSAGES
    Serial.print("ts::setLongitude(longitude: "); Serial.print(longitude,3); Serial.println("\")");
#endif
    this->nextWriteLongitude = longitude;

    return TS_OK_SUCCESS;
}

int ThingSpeakClass::setElevation(float elevation) {
#ifdef PRINT_DEBUG_MESSAGES
    Serial.print("ts::setElevation(elevation: "); Serial.print(elevation,3); Serial.println("\")");
#endif
    this->nextWriteElevation = elevation;

    return TS_OK_SUCCESS;
}

int ThingSpeakClass::setStatus(String status) {
#ifdef PRINT_DEBUG_MESSAGES
    Serial.print("ts::setStatus(status: "); Serial.print(status); Serial.println("\")");
#endif
    // Max # bytes for ThingSpeak field is 255 (UTF-8)
    if(status.length() > FIELDLENGTH_MAX) return TS_ERR_OUT_OF_RANGE;
    this->nextWriteStatus = status;

    return TS_OK_SUCCESS;
}

int ThingSpeakClass::setTwitterTweet(String twitter, String tweet) {
#ifdef PRINT_DEBUG_MESSAGES
    Serial.print("ts::setTwitterTweet(twitter: "); Serial.print(twitter); Serial.print(", tweet: "); Serial.print(tweet); Serial.println("\")");
#endif
    // Max # bytes for ThingSpeak field is 255 (UTF-8)
    if((twitter.length() > FIELDLENGTH_MAX) || (tweet.length() > FIELDLENGTH_MAX)) return TS_ERR_OUT_OF_RANGE;

    this->nextWriteTwitter = twitter;
    this->nextWriteTweet = tweet;

    return TS_OK_SUCCESS;
}

int ThingSpeakClass::setCreatedAt(String createdAt) {
#ifdef PRINT_DEBUG_MESSAGES
    Serial.print("ts::setCreatedAt(createdAt: "); Serial.print(createdAt); Serial.println("\")");
#endif

    // the ISO 8601 format is too complicated to check for valid timestamps here
    // we'll need to reply on the api to tell us if there is a problem
    // Max # bytes for ThingSpeak field is 255 (UTF-8)
    if(createdAt.length() > FIELDLENGTH_MAX) return TS_ERR_OUT_OF_RANGE;
    this->nextWriteCreatedAt = createdAt;

    return TS_OK_SUCCESS;
}

int ThingSpeakClass::writeFields(unsigned long channelNumber, const char *writeAPIKey) {
    if(!connectThingSpeak()){
        // Failed to connect to ThingSpeak
        return TS_ERR_CONNECT_FAILED;
    }

    // Get the content length of the payload
    int contentLen = getWriteFieldsContentLength();

    if(contentLen == 0){
        // setField was not called before writeFields
        return TS_ERR_SETFIELD_NOT_CALLED;
    }

#ifdef PRINT_DEBUG_MESSAGES
    Serial.print("ts::writeFields   (channelNumber: "); Serial.print(channelNumber); Serial.print(" writeAPIKey: "); Serial.println(writeAPIKey);
#endif

    // Post data to thingspeak
    if(!this->client->print("POST /update HTTP/1.1\r\n")) return abortWriteRaw();
    if(!writeHTTPHeader(writeAPIKey)) return abortWriteRaw();
    if(!this->client->print("Content-Type: application/x-www-form-urlencoded\r\n")) return abortWriteRaw();
    if(!this->client->print("Content-Length: ")) return abortWriteRaw();
    if(!this->client->print(contentLen)) return abortWriteRaw();
    if(!this->client->print("\r\n\r\n")) return abortWriteRaw();

    bool fFirstItem = true;
    for(size_t iField = 0; iField < FIELDNUM_MAX; iField++){
        if(this->nextWriteField[iField].length() > 0){
            if(!fFirstItem){
                if(!this->client->print("&")) return abortWriteRaw();
            }
            if(!this->client->print("field")) return abortWriteRaw();
            if(!this->client->print(iField + 1)) return abortWriteRaw();
            if(!this->client->print("=")) return abortWriteRaw();
            if(!this->client->print(this->nextWriteField[iField])) return abortWriteRaw();
            fFirstItem = false;
        }
    }

    if(!isnan(this->nextWriteLatitude)){
        if(!fFirstItem){
            if(!this->client->print("&")) return abortWriteRaw();
        }
        if(!this->client->print("lat=")) return abortWriteRaw();
        if(!this->client->print(this->nextWriteLatitude)) return abortWriteRaw();
        fFirstItem = false;
    }

    if(!isnan(this->nextWriteLongitude)){
        if(!fFirstItem){
            if(!this->client->print("&")) return abortWriteRaw();
        }
        if(!this->client->print("long=")) return abortWriteRaw();
        if(!this->client->print(this->nextWriteLongitude)) return abortWriteRaw();
        fFirstItem = false;
    }

    if(!isnan(this->nextWriteElevation)){
        if(!fFirstItem){
            if(!this->client->print("&")) return abortWriteRaw();
        }
        if(!this->client->print("elevation=")) return abortWriteRaw();
        if(!this->client->print(this->nextWriteElevation)) return abortWriteRaw();
        fFirstItem = false;
    }

    if(this->nextWriteStatus.length() > 0){
        if(!fFirstItem){
            if(!this->client->print("&")) return abortWriteRaw();
        }
        if(!this->client->print("status=")) return abortWriteRaw();
        if(!this->client->print(this->nextWriteStatus)) return abortWriteRaw();
        fFirstItem = false;
    }

    if(this->nextWriteTwitter.length() > 0){
        if(!fFirstItem){
            if(!this->client->print("&")) return abortWriteRaw();
        }
        if(!this->client->print("twitter=")) return abortWriteRaw();
        if(!this->client->print(this->nextWriteTwitter)) return abortWriteRaw();
        fFirstItem = false;
    }

    if(this->nextWriteTweet.length() > 0){
        if(!fFirstItem){
            if(!this->client->print("&")) return abortWriteRaw();
        }
        if(!this->client->print("tweet=")) return abortWriteRaw();
        if(!this->client->print(this->nextWriteTweet)) return abortWriteRaw();
        fFirstItem = false;
    }

    if(this->nextWriteCreatedAt.length() > 0){
        if(!fFirstItem){
            if(!this->client->print("&")) return abortWriteRaw();
        }
        if(!this->client->print("created_at=")) return abortWriteRaw();
        if(!this->client->print(this->nextWriteCreatedAt)) return abortWriteRaw();
        fFirstItem = false;
    }

    if(!this->client->print("&headers=false")) return abortWriteRaw();

    resetWriteFields();

    return finishWrite();
}

int ThingSpeakClass::writeRaw(unsigned long channelNumber, String postMessage, const char *writeAPIKey) {
#ifdef PRINT_DEBUG_MESSAGES
    Serial.print("ts::writeRaw   (channelNumber: "); Serial.print(channelNumber); Serial.print(" writeAPIKey: "); Serial.println(writeAPIKey);
#endif

    if(!connectThingSpeak())
    {
        // Failed to connect to ThingSpeak
        return TS_ERR_CONNECT_FAILED;
    }

    postMessage.concat("&headers=false");

#ifdef PRINT_DEBUG_MESSAGES
    Serial.print("               POST \"");Serial.print(postMessage);Serial.println("\"");
#endif


    // Post data to thingspeak
    if(!this->client->print("POST /update HTTP/1.1\r\n")) return abortWriteRaw();
    if(!writeHTTPHeader(writeAPIKey)) return abortWriteRaw();
    if(!this->client->print("Content-Type: application/x-www-form-urlencoded\r\n")) return abortWriteRaw();
    if(!this->client->print("Content-Length: ")) return abortWriteRaw();
    if(!this->client->print(postMessage.length())) return abortWriteRaw();
    if(!this->client->print("\r\n\r\n")) return abortWriteRaw();
    if(!this->client->print(postMessage)) return abortWriteRaw();

    resetWriteFields();

    return finishWrite();
}

String ThingSpeakClass::readStringField(unsigned long channelNumber, unsigned int field, const char *readAPIKey) {
    if(field < FIELDNUM_MIN || field > FIELDNUM_MAX)
    {
        this->lastReadStatus = TS_ERR_INVALID_FIELD_NUM;
        return("");
    }
#ifdef PRINT_DEBUG_MESSAGES
    Serial.print("ts::readStringField(channelNumber: "); Serial.print(channelNumber);
                if(NULL != readAPIKey)
                {
                    Serial.print(" readAPIKey: "); Serial.print(readAPIKey);
                }
                Serial.print(" field: "); Serial.print(field); Serial.println(")");
#endif
    String suffixURL = String("/fields/");
    suffixURL.concat(field);
    suffixURL.concat("/last");

    return readRaw(channelNumber, suffixURL, readAPIKey);
}

String ThingSpeakClass::readStringField(unsigned long channelNumber, unsigned int field) {
    return readStringField(channelNumber, field, NULL);
}

float ThingSpeakClass::readFloatField(unsigned long channelNumber, unsigned int field, const char *readAPIKey) {
    return convertStringToFloat(readStringField(channelNumber, field, readAPIKey));
}

float ThingSpeakClass::readFloatField(unsigned long channelNumber, unsigned int field) {
    return readFloatField(channelNumber, field, NULL);
}

long ThingSpeakClass::readLongField(unsigned long channelNumber, unsigned int field, const char *readAPIKey) {
    // Note that although the function is called "toInt" it really returns a long.
    return readStringField(channelNumber, field, readAPIKey).toInt();
}

long ThingSpeakClass::readLongField(unsigned long channelNumber, unsigned int field) {
    return readLongField(channelNumber, field, NULL);
}

int ThingSpeakClass::readIntField(unsigned long channelNumber, unsigned int field, const char *readAPIKey) {
    return readLongField(channelNumber, field, readAPIKey);
}

int ThingSpeakClass::readIntField(unsigned long channelNumber, unsigned int field) {
    return readLongField(channelNumber, field, NULL);
}

String ThingSpeakClass::readStatus(unsigned long channelNumber, const char *readAPIKey) {
    String content = readRaw(channelNumber, "/feeds/last.txt?status=true", readAPIKey);

    if(getLastReadStatus() != TS_OK_SUCCESS){
        return String("");
    }

    return getJSONValueByKey(content, "status");
}

String ThingSpeakClass::readStatus(unsigned long channelNumber) {
    return readStatus(channelNumber, NULL);
}

String ThingSpeakClass::readCreatedAt(unsigned long channelNumber, const char *readAPIKey) {
    String content = readRaw(channelNumber, "/feeds/last.txt", readAPIKey);

    if(getLastReadStatus() != TS_OK_SUCCESS){
        return String("");
    }

    return getJSONValueByKey(content, "created_at");
}

String ThingSpeakClass::readCreatedAt(unsigned long channelNumber) {
    return readCreatedAt(channelNumber, NULL);
}

String ThingSpeakClass::readRaw(unsigned long channelNumber, String suffixURL, const char *readAPIKey) {
#ifdef PRINT_DEBUG_MESSAGES
    Serial.print("ts::readRaw   (channelNumber: "); Serial.print(channelNumber);
                if(NULL != readAPIKey)
                {
                    Serial.print(" readAPIKey: "); Serial.print(readAPIKey);
                }
                Serial.print(" suffixURL: \""); Serial.print(suffixURL); Serial.println("\")");
#endif

    if(!connectThingSpeak())
    {
        this->lastReadStatus = TS_ERR_CONNECT_FAILED;
        return String("");
    }

    String readURL = String("/channels/");
    readURL.concat(channelNumber);
    readURL.concat(suffixURL);

#ifdef PRINT_DEBUG_MESSAGES
    Serial.print("               GET \"");Serial.print(readURL);Serial.println("\"");
#endif

    // Get data from thingspeak
    if(!this->client->print("GET ")) return abortReadRaw();
    if(!this->client->print(readURL)) return abortReadRaw();
    if(!this->client->print(" HTTP/1.1\r\n")) return abortReadRaw();
    if(!writeHTTPHeader(readAPIKey)) return abortReadRaw();
    if(!this->client->print("\r\n")) return abortReadRaw();

    String content = String();
    int status = getHTTPResponse(content);

    this->lastReadStatus = status;

    emptyStream();

#ifdef PRINT_DEBUG_MESSAGES
    if(status == TS_OK_SUCCESS)
                {
                    Serial.print("Read: \""); Serial.print(content); Serial.println("\"");
                }
#endif

    this->client->stop();
#ifdef PRINT_DEBUG_MESSAGES
    Serial.println("disconnected.");
#endif

    if(status != TS_OK_SUCCESS)
    {
        return String("");
    }

    return content;
}

String ThingSpeakClass::readRaw(unsigned long channelNumber, String suffixURL) {
    return readRaw(channelNumber, suffixURL, NULL);
}

#ifndef ARDUINO_AVR_UNO //No Arduino here ----------

int ThingSpeakClass::readMultipleFields(unsigned long channelNumber, const char *readAPIKey) {
    String readCondition = "/feeds/last.txt?status=true&location=true";

    String multiContent = readRaw(channelNumber, readCondition, readAPIKey);

    if(getLastReadStatus() != TS_OK_SUCCESS){
        return getLastReadStatus();
    }

    this->lastFeed.nextReadField[0] = parseValues(multiContent, "field1");
    this->lastFeed.nextReadField[1] = parseValues(multiContent, "field2");
    this->lastFeed.nextReadField[2] = parseValues(multiContent, "field3");
    this->lastFeed.nextReadField[3] = parseValues(multiContent, "field4");
    this->lastFeed.nextReadField[4] = parseValues(multiContent, "field5");
    this->lastFeed.nextReadField[5] = parseValues(multiContent, "field6");
    this->lastFeed.nextReadField[6] = parseValues(multiContent, "field7");
    this->lastFeed.nextReadField[7] = parseValues(multiContent, "field8");
    this->lastFeed.nextReadCreatedAt = parseValues(multiContent, "created_at");
    this->lastFeed.nextReadLatitude = parseValues(multiContent, "latitude");
    this->lastFeed.nextReadLongitude = parseValues(multiContent, "longitude");
    this->lastFeed.nextReadElevation = parseValues(multiContent, "elevation");
    this->lastFeed.nextReadStatus = parseValues(multiContent, "status");

    return TS_OK_SUCCESS;
}

int ThingSpeakClass::readMultipleFields(unsigned long channelNumber) {
    return readMultipleFields(channelNumber, NULL);
}

String ThingSpeakClass::getFieldAsString(unsigned int field) {
    if(field < FIELDNUM_MIN || field > FIELDNUM_MAX)
    {
        this->lastReadStatus = TS_ERR_INVALID_FIELD_NUM;
        return("");
    }

    this->lastReadStatus = TS_OK_SUCCESS;
    return this->lastFeed.nextReadField[field-1];
}

float ThingSpeakClass::getFieldAsFloat(unsigned int field) {
    return convertStringToFloat(getFieldAsString(field));
}

long ThingSpeakClass::getFieldAsLong(unsigned int field) {
    // Note that although the function is called "toInt" it really returns a long.
    return getFieldAsString(field).toInt();
}

int ThingSpeakClass::getFieldAsInt(unsigned int field) {
    // int and long are same
    return getFieldAsLong(field);
}

String ThingSpeakClass::getStatus() {
    return this->lastFeed.nextReadStatus;
}

String ThingSpeakClass::getLatitude() {
    return this->lastFeed.nextReadLatitude;
}

String ThingSpeakClass::getLongitude() {
    return this->lastFeed.nextReadLongitude;
}

String ThingSpeakClass::getElevation() {
    return this->lastFeed.nextReadElevation;
}

String ThingSpeakClass::getCreatedAt() {
    return this->lastFeed.nextReadCreatedAt;
}

#endif //Arduino from here on again ----------

int ThingSpeakClass::getLastReadStatus() {
    return this->lastReadStatus;
}

int ThingSpeakClass::getWriteFieldsContentLength() {
    size_t iField;
    int contentLen = 0;

    for(iField = 0; iField < FIELDNUM_MAX; iField++){
        if(this->nextWriteField[iField].length() > 0){
            contentLen = contentLen + 8 + this->nextWriteField[iField].length();	// &fieldX=[value]

            // future-proof in case ThingSpeak allows 999 fields someday
            if(iField > 9){
                contentLen = contentLen + 1;
            }
            else if(iField > 99){
                contentLen = contentLen + 2;
            }

        }
    }

    if(!isnan(this->nextWriteLatitude)){
        contentLen = contentLen + 5 + String(this->nextWriteLatitude).length(); // &lat=[value]
    }

    if(!isnan(this->nextWriteLongitude)){
        contentLen = contentLen + 6 + String(this->nextWriteLongitude).length(); // &long=[value]
    }

    if(!isnan(this->nextWriteElevation)){
        contentLen = contentLen + 11 + String(this->nextWriteElevation).length(); // &elevation=[value]
    }

    if(this->nextWriteStatus.length() > 0){
        contentLen = contentLen + 8 + this->nextWriteStatus.length();	// &status=[value]
    }

    if(this->nextWriteTwitter.length() > 0){
        contentLen = contentLen + 9 + this->nextWriteTwitter.length();	// &twitter=[value]
    }

    if(this->nextWriteTweet.length() > 0){
        contentLen = contentLen + 7 + this->nextWriteTweet.length();	// &tweet=[value]
    }

    if(this->nextWriteCreatedAt.length() > 0){
        contentLen = contentLen + 12 + this->nextWriteCreatedAt.length();	// &created_at=[value]
    }

    if(contentLen == 0){
        return 0;
    }

    contentLen = contentLen + 13; // add 14 for '&headers=false', subtract 1 for missing first '&'

    return contentLen;
}

void ThingSpeakClass::emptyStream() {
    while(this->client->available() > 0){
        this->client->read();
    }
}

int ThingSpeakClass::finishWrite() {
    String entryIDText = String();
    int status = getHTTPResponse(entryIDText);

    emptyStream();

    if(status != TS_OK_SUCCESS)
    {
        this->client->stop();
        return status;
    }
    long entryID = entryIDText.toInt();

#ifdef PRINT_DEBUG_MESSAGES
    Serial.print("               Entry ID \"");Serial.print(entryIDText);Serial.print("\" (");Serial.print(entryID);Serial.println(")");
#endif

    this->client->stop();

#ifdef PRINT_DEBUG_MESSAGES
    Serial.println("disconnected.");
#endif
    if(entryID == 0)
    {
        // ThingSpeak did not accept the write
        status = TS_ERR_NOT_INSERTED;
    }

    return status;
}

String ThingSpeakClass::getJSONValueByKey(String textToSearch, String key) {
    if(textToSearch.length() == 0){
        return String("");
    }

    String searchPhrase = String("\"");
    searchPhrase.concat(key);
    searchPhrase.concat("\":\"");

    int fromPosition = textToSearch.indexOf(searchPhrase,0);

    if(fromPosition == -1){
        // return because there is no status or it's null
        return String("");
    }

    fromPosition = fromPosition + searchPhrase.length();

    int toPosition = textToSearch.indexOf("\"", fromPosition);


    if(toPosition == -1){
        // return because there is no end quote
        return String("");
    }

    textToSearch.remove(toPosition);

    return textToSearch.substring(fromPosition);
}

#ifndef ARDUINO_AVR_UNO

String ThingSpeakClass::parseValues(String &multiContent, String key) {
    if(multiContent.length() == 0){
        return String("");
    }

    String searchPhrase = String("\"");
    searchPhrase.concat(key);
    searchPhrase.concat("\":\"");

    int fromPosition = multiContent.indexOf(searchPhrase,0);

    if(fromPosition == -1){
        // return because there is no status or it's null
        return String("");
    }

    fromPosition = fromPosition + searchPhrase.length();

    int toPosition = multiContent.indexOf("\"", fromPosition);


    if(toPosition == -1){
        // return because there is no end quote
        return String("");
    }

    return multiContent.substring(fromPosition, toPosition);
}

#endif

int ThingSpeakClass::abortWriteRaw() {
    while(this->client->available() > 0){
        this->client->read();
    }
    this->client->stop();
    resetWriteFields();

    return TS_ERR_UNEXPECTED_FAIL;
}

String ThingSpeakClass::abortReadRaw() {
    while(this->client->available() > 0){
        this->client->read();
    }
    this->client->stop();
#ifdef PRINT_DEBUG_MESSAGES
    Serial.println("ReadRaw abort - disconnected.");
#endif
    this->lastReadStatus = TS_ERR_UNEXPECTED_FAIL;
    return String("");
}

void ThingSpeakClass::setPort(unsigned int port) {
    this->port = port;
}

void ThingSpeakClass::setClient(Client *client) {
    this->client = client;
}

bool ThingSpeakClass::connectThingSpeak() {
    bool connectSuccess = false;

#ifdef PRINT_DEBUG_MESSAGES
    Serial.print("               Connect to default ThingSpeak: ");
                Serial.print(THINGSPEAK_URL);
                Serial.print(":");
                Serial.print(this->port);
                Serial.print("...");
#endif

    connectSuccess = client->connect(const_cast<char *>(THINGSPEAK_URL), this->port);

#ifdef PRINT_DEBUG_MESSAGES
    if (connectSuccess)
                {
                    Serial.println("Success.");
                }
                else
                {
                    Serial.println("Failed.");
                }
#endif

    return connectSuccess;
}

bool ThingSpeakClass::writeHTTPHeader(const char *APIKey) {

    if (!this->client->print("Host: api.thingspeak.com\r\n")) return false;
    if (!this->client->print("User-Agent: ")) return false;
    if (!this->client->print(TS_USER_AGENT)) return false;
    if (!this->client->print("\r\n")) return false;
    if(NULL != APIKey)
    {
        if (!this->client->print("X-THINGSPEAKAPIKEY: ")) return false;
        if (!this->client->print(APIKey)) return false;
        if (!this->client->print("\r\n")) return false;
    }

    return true;
}

int ThingSpeakClass::getHTTPResponse(String &response) {
    // make sure all of the HTTP request is pushed out of the buffer before looking for a response
    this->client->flush();

    long timeoutTime = millis() + TIMEOUT_MS_SERVERRESPONSE;

    while(this->client-> available() < 17){
        delay(2);
        if(millis() > timeoutTime){
            return TS_ERR_TIMEOUT;
        }
    }

    if(!this->client->find(const_cast<char *>("HTTP/1.1")))
    {
#ifdef PRINT_HTTP
        Serial.println("ERROR: Didn't find HTTP/1.1");
#endif
        return TS_ERR_BAD_RESPONSE; // Couldn't parse response (didn't find HTTP/1.1)
    }

    int status = this->client->parseInt();
#ifdef PRINT_HTTP
    Serial.print("Got Status of ");Serial.println(status);
#endif
    if(status != TS_OK_SUCCESS)
    {
        return status;
    }

    // Find Content-Length
    if(!this->client->find(const_cast<char *>("Content-Length:"))){
#ifdef PRINT_HTTP
        Serial.println("ERROR: Didn't find Content-Length header");
#endif
        return TS_ERR_BAD_RESPONSE; // Couldn't parse response (didn't find HTTP/1.1)
    }
    int contentLength = this->client->parseInt();

#ifdef PRINT_HTTP
    Serial.print("Content Length: ");
                Serial.println(contentLength);
#endif

    if(!this->client->find(const_cast<char *>("\r\n\r\n")))
    {
#ifdef PRINT_HTTP
        Serial.println("ERROR: Didn't find end of headers");
#endif
        return TS_ERR_BAD_RESPONSE;
    }
#ifdef PRINT_HTTP
    Serial.println("Found end of header");
#endif

    timeoutTime = millis() + TIMEOUT_MS_SERVERRESPONSE;

    while(this->client->available() < contentLength){
        delay(2);
        if(millis() > timeoutTime){
            return TS_ERR_TIMEOUT;
        }
    }

    String tempString = String("");
    char y = 0;
    for(int i = 0; i < contentLength; i++){
        y = client->read();
        tempString.concat(y);
    }
    response = tempString;

#ifdef PRINT_HTTP
    Serial.print("Response: \"");Serial.print(response);Serial.println("\"");
#endif

    return status;
}

int ThingSpeakClass::convertFloatToChar(float value, char *valueString) {
    // Supported range is -999999000000 to 999999000000
    if(0 == isinf(value) && (value > 999999000000 || value < -999999000000))
    {
        // Out of range
        return TS_ERR_OUT_OF_RANGE;
    }
    // assume that 5 places right of decimal should be sufficient for most applications

#if defined(ARDUINO_ARCH_SAMD) || defined(ARDUINO_ARCH_SAM)
    sprintf(valueString, "%.5f", value);
#else
    dtostrf(value,1,5, valueString);
#endif

    return TS_OK_SUCCESS;
}

float ThingSpeakClass::convertStringToFloat(String value) {
    // There's a bug in the AVR function strtod that it doesn't decode -INF correctly (it maps it to INF)
    float result = value.toFloat();

    if(1 == isinf(result) && *value.c_str() == '-')
    {
        result = (float)-INFINITY;
    }

    return result;
}

void ThingSpeakClass::resetWriteFields() {
    for(size_t iField = 0; iField < FIELDNUM_MAX; iField++)
    {
        this->nextWriteField[iField] = "";
    }
    this->nextWriteLatitude = NAN;
    this->nextWriteLongitude = NAN;
    this->nextWriteElevation = NAN;
    this->nextWriteStatus = "";
    this->nextWriteTwitter = "";
    this->nextWriteTweet = "";
    this->nextWriteCreatedAt = "";
}
