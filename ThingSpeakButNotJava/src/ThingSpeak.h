/*
  ThingSpeak(TM) Communication Library For Arduino, ESP8266 and ESP32

  Enables an Arduino or other compatible hardware to write or read data to or from ThingSpeak,
  an open data platform for the Internet of Things with MATLAB analytics and visualization. 

  ThingSpeak ( https://www.thingspeak.com ) is an analytic IoT platform service that allows you to aggregate, visualize and 
  analyze live data streams in the cloud.
  
  Copyright 2020, The MathWorks, Inc.
 
  See the accompaning licence file for licensing information.
*/

// #define PRINT_DEBUG_MESSAGES
// #define PRINT_HTTP

#ifndef ThingSpeak_h
    #define ThingSpeak_h

    #define TS_VER "2.0.0"

    #include "Arduino.h"
    #include <Client.h>

    #define THINGSPEAK_URL "api.thingspeak.com"
    #define THINGSPEAK_PORT_NUMBER 80
    #define THINGSPEAK_HTTPS_PORT_NUMBER 443

    #ifdef ARDUINO_ARCH_AVR
        #ifdef ARDUINO_AVR_YUN
            #define TS_USER_AGENT "tslib-arduino/" TS_VER " (arduino yun)"
        #else
            #define TS_USER_AGENT "tslib-arduino/" TS_VER " (arduino uno or mega)"
        #endif
    #elif defined(ARDUINO_ARCH_ESP8266)
        #define TS_USER_AGENT "tslib-arduino/" TS_VER " (ESP8266)"
    #elif defined(ARDUINO_SAMD_MKR1000)
        #define TS_USER_AGENT "tslib-arduino/" TS_VER " (arduino mkr1000)"
    #elif defined(ARDUINO_SAM_DUE)
        #define TS_USER_AGENT "tslib-arduino/" TS_VER " (arduino due)"
    #elif defined(ARDUINO_ARCH_SAMD)
        #define TS_USER_AGENT "tslib-arduino/" TS_VER " (arduino samd)"
    #elif defined(ARDUINO_ARCH_SAM)
        #define TS_USER_AGENT "tslib-arduino/" TS_VER " (arduino sam)"
    #elif defined(ARDUINO_ARCH_SAMD_BETA)
        #define TS_USER_AGENT "tslib-arduino/" TS_VER " (arduino samd_beta )"
    #elif defined(ARDUINO_ARCH_ESP32)
        #define TS_USER_AGENT "tslib-arduino/" TS_VER " (ESP32)"
    #elif defined(ARDUINO_ARCH_SAMD_BETA)
        #define TS_USER_AGENT "tslib-arduino/" TS_VER " (arduino vidor)"
    #else
        #define TS_USER_AGENT "tslib-arduino/" TS_VER " (unknown)"
    #endif

    #define FIELDNUM_MIN 1
    #define FIELDNUM_MAX 8
    #define FIELDLENGTH_MAX 255  // Max length for a field in ThingSpeak is 255 bytes (UTF-8)

    #define TIMEOUT_MS_SERVERRESPONSE 5000  // Wait up to five seconds for server to respond

    #define TS_OK_SUCCESS              200     // OK / Success
    #define TS_ERR_BADAPIKEY           400     // Incorrect API key (or invalid ThingSpeak server address)
    #define TS_ERR_BADURL              404     // Incorrect API key (or invalid ThingSpeak server address)
    #define TS_ERR_OUT_OF_RANGE        -101    // Value is out of range or string is too long (> 255 bytes)
    #define TS_ERR_INVALID_FIELD_NUM   -201    // Invalid field number specified
    #define TS_ERR_SETFIELD_NOT_CALLED -210    // setField() was not called before writeFields()
    #define TS_ERR_CONNECT_FAILED      -301    // Failed to connect to ThingSpeak
    #define TS_ERR_UNEXPECTED_FAIL     -302    // Unexpected failure during write to ThingSpeak
    #define TS_ERR_BAD_RESPONSE        -303    // Unable to parse response
    #define TS_ERR_TIMEOUT             -304    // Timeout waiting for server to respond
    #define TS_ERR_NOT_INSERTED        -401    // Point was not inserted (most probable cause is the rate limit of once every 15 seconds)

    
    // variables to store the values from the readMultipleFields functionality
    #ifndef ARDUINO_AVR_UNO
        typedef struct feedRecord
        {
            String nextReadField[8];
            String nextReadStatus;
            String nextReadLatitude;
            String nextReadLongitude;
            String nextReadElevation;
            String nextReadCreatedAt;
        }feed;
    #endif


    // Enables an Arduino, ESP8266, ESP32 or other compatible hardware to write or read data to or from ThingSpeak, an open data platform for the Internet of Things with MATLAB analytics and visualization.
    class ThingSpeakClass
    {
      public:
        ThingSpeakClass();


        /*
        Function: begin
        
        Summary:
        Initializes the ThingSpeak library and network settings using the ThingSpeak.com service.
        
        Parameters:
        client - EthernetClient, YunClient, TCPClient, or WiFiClient for HTTP connection and WiFiSSLClient, WiFiClientSecure for HTTPS connection created earlier in the sketch.
        
        Returns:
        Always returns true
        
        Notes:
        This does not validate the information passed in, or generate any calls to ThingSpeak.
        Include this ThingSpeak header file after including the client header file and the TS_ENABLE_SSL macro in the user sketch.
        */
        bool begin(Client & client);
        
        
        /*
        Function: writeField
        
        Summary:
        Write a value to a single field in a ThingSpeak channel
        
        Parameters:
        channelNumber - Channel number
        field - Field number (1-8) within the channel to write to.
        value - value to write.
        writeAPIKey - Write API key associated with the channel.  *If you share code with others, do _not_ share this key*
        
        Returns:
        HTTP status code of 200 if successful.
        
        Notes:
        See getLastReadStatus() for other possible return values.
        */
        int writeField(unsigned long channelNumber, unsigned int field, int value, const char * writeAPIKey);

        int writeField(unsigned long channelNumber, unsigned int field, long value, const char * writeAPIKey);

        int writeField(unsigned long channelNumber, unsigned int field, float value, const char * writeAPIKey);

        int writeField(unsigned long channelNumber, unsigned int field, String value, const char * writeAPIKey);

             
        /*
        Function: setField
        
        Summary:
        Set the value of a single field that will be part of a multi-field update.
        
        Parameters:
        field - Field number (1-8) within the channel to set.
        value - value to set.
        
        Returns:
        Code of 200 if successful.
        Code of -101 if value is out of range or string is too long (> 255 bytes)
        */
        int setField(unsigned int field, int value);

        int setField(unsigned int field, long value);

        int setField(unsigned int field, float value);

        int setField(unsigned int field, String value);

         
        /*
        Function: setLatitude
        
        Summary:
        Set the latitude of a multi-field update.
        
        Parameters:
        latitude - Latitude of the measurement as a floating point value (degrees N, use negative values for degrees S)
        
        Returns:
        Always return 200
        
        Notes:
        To record latitude, longitude and elevation of a write, call setField() for each of the fields you want to write. Then setLatitude(), setLongitude(), setElevation() and then call writeFields()
        */
        int setLatitude(float latitude);

     
        /*
        Function: setLongitude
        
        Summary:
        Set the longitude of a multi-field update.
        
        Parameters:
        longitude - Longitude of the measurement as a floating point value (degrees E, use negative values for degrees W)
        
        Returns:
        Always return 200
        
        Notes:
        To record latitude, longitude and elevation of a write, call setField() for each of the fields you want to write. Then setLatitude(), setLongitude(), setElevation() and then call writeFields()
        */
        int setLongitude(float longitude);

         
        /*
        Function: setElevation
        
        Summary:
        Set the elevation of a multi-field update.
        
        Parameters:
        elevation - Elevation of the measurement as a floating point value (meters above sea level)
        
        Returns:
        Always return 200
        
        Notes:
        To record latitude, longitude and elevation of a write, call setField() for each of the fields you want to write. Then setLatitude(), setLongitude(), setElevation() and then call writeFields()
        
        */
        int setElevation(float elevation);

         
        /*
        Function: setStatus
        
        Summary:
        Set the status field of a multi-field update.
        
        Parameters:
        status - String to write (UTF8).  ThingSpeak limits this to 255 bytes.
        
        Returns:
        Code of 200 if successful.
        Code of -101 if string is too long (> 255 bytes)
        
        Notes:
        To record a status message on a write, call setStatus() then call writeFields().
        Use status to provide additonal details when writing a channel update.
        Additonally, status can be used by the ThingTweet App to send a message to Twitter.
        */
        int setStatus(String status);
        

        /*
        Function: setTwitterTweet
        
        Summary:
        Set the Twitter account and message to use for an update to be tweeted.
        
        Parameters:
        twitter - Twitter account name as a String.
        tweet - Twitter message as a String (UTF-8) limited to 140 character.
        
        Returns:
        Code of 200 if successful.
        Code of -101 if string is too long (> 255 bytes)
        
        Notes:
        To send a message to twitter call setTwitterTweet() then call writeFields().
        Prior to using this feature, a twitter account must be linked to your ThingSpeak account. Do this by logging into ThingSpeak and going to Apps, then ThingTweet and clicking Link Twitter Account.
        */
        int setTwitterTweet(String twitter, String tweet);
        
            
        /*
        Function: setCreatedAt
        
        Summary:
        Set the created-at date of a multi-field update.
        
        Parameters:
        createdAt - Desired timestamp to be included with the channel update as a String.  The timestamp string must be in the ISO 8601 format. Example "2017-01-12 13:22:54"
        
        Returns:
        Code of 200 if successful.
        Code of -101 if string is too long (> 255 bytes)
        
        Notes:
        Timezones can be set using the timezone hour offset parameter. For example, a timestamp for Eastern Standard Time is: "2017-01-12 13:22:54-05".
        If no timezone hour offset parameter is used, UTC time is assumed.
        */
        int setCreatedAt(String createdAt);
        
     
        /*
        Function: writeFields
        
        Summary:
        Write a multi-field update.
        
        Parameters:
        channelNumber - Channel number
        writeAPIKey - Write API key associated with the channel.  *If you share code with others, do _not_ share this key*
        
        Returns:
        200 - successful.
        404 - Incorrect API key (or invalid ThingSpeak server address)
        -101 - Value is out of range or string is too long (> 255 characters)
        -201 - Invalid field number specified
        -210 - setField() was not called before writeFields()
        -301 - Failed to connect to ThingSpeak
        -302 - Unexpected failure during write to ThingSpeak
        -303 - Unable to parse response
        -304 - Timeout waiting for server to respond
        -401 - Point was not inserted (most probable cause is the rate limit of once every 15 seconds)
        
        Notes:
        Call setField(), setLatitude(), setLongitude(), setElevation() and/or setStatus() and then call writeFields()
        */
        int writeFields(unsigned long channelNumber, const char * writeAPIKey);

         
        /*
        Function: writeRaw
        
        Summary:
        Write a raw POST to a ThingSpeak channel
        
        Parameters:
        channelNumber - Channel number
        postMessage - Raw URL to write to ThingSpeak as a string.  See the documentation at https://thingspeak.com/docs/channels#update_feed.
        writeAPIKey - Write API key associated with the channel.  *If you share code with others, do _not_ share this key*
        
        Returns:
        200 - successful.
        404 - Incorrect API key (or invalid ThingSpeak server address)
        -101 - Value is out of range or string is too long (> 255 characters)
        -201 - Invalid field number specified
        -210 - setField() was not called before writeFields()
        -301 - Failed to connect to ThingSpeak
        -302 - Unexpected failure during write to ThingSpeak
        -303 - Unable to parse response
        -304 - Timeout waiting for server to respond
        -401 - Point was not inserted (most probable cause is the rate limit of once every 15 seconds)
        
        Notes:
        This is low level functionality that will not be required by most users.
        */
        int writeRaw(unsigned long channelNumber, String postMessage, const char * writeAPIKey);
        
         
        /*
        Function: read*Field
        
        Summary:
        Read the latest * from a private ThingSpeak channel
        
        Parameters:
        channelNumber - Channel number
        field - Field number (1-8) within the channel to read from.
        readAPIKey - Read API key associated with the channel.  *If you share code with others, do _not_ share this key*
        
        Returns:
        Value read (UTF8 string), or empty string if there is an error.  Use getLastReadStatus() to get more specific information.
        */
        String readStringField(unsigned long channelNumber, unsigned int field, const char * readAPIKey);

        float readFloatField(unsigned long channelNumber, unsigned int field, const char * readAPIKey);

        long readLongField(unsigned long channelNumber, unsigned int field, const char * readAPIKey);

        int readIntField(unsigned long channelNumber, unsigned int field, const char * readAPIKey);


        /*
        Function: read*Field
        
        Summary:
        Read the latest * from a public ThingSpeak channel
        
        Parameters:
        channelNumber - Channel number
        field - Field number (1-8) within the channel to read from.
        
        Returns:
        Value read (UTF8 string), or empty string if there is an error.  Use getLastReadStatus() to get more specific information.
        */
        String readStringField(unsigned long channelNumber, unsigned int field);

        float readFloatField(unsigned long channelNumber, unsigned int field);

        long readLongField(unsigned long channelNumber, unsigned int field);

        int readIntField(unsigned long channelNumber, unsigned int field);


        /*
        Function: readStatus
        
        Summary:
        Read the latest status from a private/public ThingSpeak channel
        
        Parameters:
        channelNumber - Channel number
        readAPIKey - Read API key associated with the channel.  *If you share code with others, do _not_ share this key*
        
        Results:
        Value read (UTF8 string). An empty string is returned if there was no status written to the channel or in case of an error.  Use getLastReadStatus() to get more specific information.
        */
        String readStatus(unsigned long channelNumber, const char * readAPIKey);

        String readStatus(unsigned long channelNumber);
        

        /*
        Function: readCreatedAt
        
        Summary:
        Read the created-at timestamp associated with the latest update to a private/public ThingSpeak channel
        
        Parameters:
        channelNumber - Channel number
        readAPIKey - Read API key associated with the channel.  *If you share code with others, do _not_ share this key*
        
        Results:
        Value read (UTF8 string). An empty string is returned if there was no created-at timestamp written to the channel or in case of an error.  Use getLastReadStatus() to get more specific information.
        */
        String readCreatedAt(unsigned long channelNumber, const char * readAPIKey);

        String readCreatedAt(unsigned long channelNumber);
           
            
        /*
        Function: readRaw
        
        Summary:
        Read a raw response from a private/pulic ThingSpeak channel
        
        Parameters:
        channelNumber - Channel number
        suffixURL - Raw URL to write to ThingSpeak as a String.  See the documentation at https://thingspeak.com/docs/channels#get_feed
        readAPIKey - Read API key associated with the channel.  *If you share code with others, do _not_ share this key*
        
        Returns:
        Response if successful, or empty string. Use getLastReadStatus() to get more specific information.
        
        Notes:
        This is low level functionality that will not be required by most users.
        */
        String readRaw(unsigned long channelNumber, String suffixURL, const char * readAPIKey);

        String readRaw(unsigned long channelNumber, String suffixURL);
        
        
        #ifndef ARDUINO_AVR_UNO // Arduino Uno doesn't have enough memory to perform the following functionalities.
            
            /*
            Function: readMultipleFields
             
            Summary:
            Read all the field values, status message, location coordinates, and created-at timestamp associated with the latest feed to a private ThingSpeak channel and store the values locally in variables within a struct.
             
            Parameters:
            channelNumber - Channel number
            readAPIKey - Read API key associated with the channel. *If you share code with others, do _not_ share this key*
             
            Returns:
            HTTP status code of 200 if successful.
            
            Notes:
            See getLastReadStatus() for other possible return values.
            */
            int readMultipleFields(unsigned long channelNumber, const char * readAPIKey);
            
            
            /*
            Function: readMultipleFields
            
            Summary:
            Read all the field values, status message, location coordinates, and created-at timestamp associated with the latest update to a private ThingSpeak channel and store the values locally in variables within a struct.
             
            Parameters:
            channelNumber - Channel number
            readAPIKey - Read API key associated with the channel. *If you share code with others, do _not_ share this key*
             
            Returns:
            HTTP status code of 200 if successful.
            
            Notes:
            See getLastReadStatus() for other possible return values.
            */
            int readMultipleFields(unsigned long channelNumber);
            
            
            /*
            Function: getFieldAsString
             
            Summary:
            Fetch the value as string from the latest stored feed record.
            
            Parameters:
            field - Field number (1-8) within the channel to read from.
            
            Returns:
            Value read (UTF8 string), empty string if there is an error, or old value read (UTF8 string) if invoked before readMultipleFields().  Use getLastReadStatus() to get more specific information.
            */
            String getFieldAsString(unsigned int field);
            
            
            /*
            Function: getFieldAsFloat
             
            Summary:
            Fetch the value as float from the latest stored feed record.
            
            Parameters:
            field - Field number (1-8) within the channel to read from.
            
            Returns:
            Value read, 0 if the field is text or there is an error, or old value read if invoked before readMultipleFields(). Use getLastReadStatus() to get more specific information.  Note that NAN, INFINITY, and -INFINITY are valid results.
            */
            float getFieldAsFloat(unsigned int field);
            
            
            /*
            Function: getFieldAsLong
            
            Summary:
            Fetch the value as long from the latest stored feed record.
            
            Parameters:
            field - Field number (1-8) within the channel to read from.
            
            Returns:
            Value read, 0 if the field is text or there is an error, or old value read if invoked before readMultipleFields(). Use getLastReadStatus() to get more specific information.
            */
            long getFieldAsLong(unsigned int field);
            
            
            /*
            Function: getFieldAsInt
             
            Summary:
            Fetch the value as int from the latest stored feed record.
            
            Parameters:
            field - Field number (1-8) within the channel to read from.
            
            Returns:
            Value read, 0 if the field is text or there is an error, or old value read if invoked before readMultipleFields(). Use getLastReadStatus() to get more specific information.
            */
            int getFieldAsInt(unsigned int field);
            
                
            /*
            Function: getStatus
             
            Summary:
            Fetch the status message associated with the latest stored feed record.
            
            Results:
            Value read (UTF8 string). An empty string is returned if there was no status written to the channel or in case of an error.  Use getLastReadStatus() to get more specific information.
            */
            String getStatus();
            
            
            /*
            Function: getLatitude
             
            Summary:
            Fetch the latitude associated with the latest stored feed record.
            
            Results:
            Value read (UTF8 string). An empty string is returned if there was no latitude written to the channel or in case of an error.  Use getLastReadStatus() to get more specific information.
            */
            String getLatitude();
            
            
            /*
            Function: getLongitude
             
            Summary:
            Fetch the longitude associated with the latest stored feed record.
            
            Results:
            Value read (UTF8 string). An empty string is returned if there was no longitude written to the channel or in case of an error.  Use getLastReadStatus() to get more specific information.
            */
            String getLongitude();
            
            
            /*
            Function: getElevation
             
            Summary:
            Fetch the longitude associated with the latest stored feed record.
            
            Results:
            Value read (UTF8 string). An empty string is returned if there was no elevation written to the channel or in case of an error.  Use getLastReadStatus() to get more specific information.
            */
            String getElevation();
            
            
            /*
            Function: getCreatedAt
             
            Summary:
            Fetch the created-at timestamp associated with the latest stored feed record.
            
            Results:
            Value read (UTF8 string). An empty string is returned if there was no created-at timestamp written to the channel or in case of an error.  Use getLastReadStatus() to get more specific information.
            */
            String getCreatedAt();
        
        #endif
        
     
        /*
        Function: getLastReadStatus
        
        Summary:
        Get the status of the previous read.
        
        Returns:
        Generally, these are HTTP status codes.  Negative values indicate an error generated by the library.
        Possible response codes...
        200 - OK / Success
        404 - Incorrect API key (or invalid ThingSpeak server address)
        -101 - Value is out of range or string is too long (> 255 characters)
        -201 - Invalid field number specified
        -210 - setField() was not called before writeFields()
        -301 - Failed to connect to ThingSpeak
        -302 -  Unexpected failure during write to ThingSpeak
        -303 - Unable to parse response
        -304 - Timeout waiting for server to respond
        -401 - Point was not inserted (most probable cause is exceeding the rate limit)
        
        Notes:
        The read functions will return zero or empty if there is an error.  Use this function to retrieve the details.
        */
        int getLastReadStatus();
        
        
    private:
            
        int getWriteFieldsContentLength();
        
        void emptyStream();
        
        int finishWrite();
        
        String getJSONValueByKey(String textToSearch, String key);
        
        #ifndef ARDUINO_AVR_UNO
            String parseValues(String & multiContent, String key);
        #endif
        
        int abortWriteRaw();

        String abortReadRaw();

        void setPort(unsigned int port);
        
        void setClient(Client * client);
        
        Client * client = NULL;
        unsigned int port = THINGSPEAK_PORT_NUMBER;
        String nextWriteField[8];
        float nextWriteLatitude;
        float nextWriteLongitude;
        float nextWriteElevation;
        int lastReadStatus;
        String nextWriteStatus;
        String nextWriteTwitter;
        String nextWriteTweet;
        String nextWriteCreatedAt;
        #ifndef ARDUINO_AVR_UNO
            feed lastFeed;
        #endif

        bool connectThingSpeak();

        bool writeHTTPHeader(const char * APIKey);

        int getHTTPResponse(String & response);
        
        
        int convertFloatToChar(float value, char *valueString);

        float convertStringToFloat(String value);

        void resetWriteFields();
    };

extern ThingSpeakClass ThingSpeak;

#endif //ThingSpeak_h
