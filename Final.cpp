//Include ArduPi library
#include "arduPi.h"

//Include the Math library
#include <math.h>

//Clickatell Message Code
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "clickatell/src/clickatell_sms/clickatell_debug.h"
#include "clickatell/src/clickatell_sms/clickatell_string.h"
#include "clickatell/src/clickatell_sms/clickatell_sms.h"

/* ----------------------------------------------------------------------------- *
 * Input configuration values                                                    *
 * NOTE: Please modify these values and replace them with your own credentials.  *
 * ----------------------------------------------------------------------------- */

// insert your HTTP API credentials here
#define CFG_HTTP_USERNAME           "myusernamehere" // insert your Clickatell account username here
#define CFG_HTTP_PASSWORD           "mypasswordhere" // insert your Clickatell account password here
#define CFG_HTTP_APIID              "myapiidhere"        // insert your Clickatell HTTP API ID here

// insert your destination addresses here
//For this project you only need to insert
#define CFG_MSISDN1          "" // insert your first desired destination mobile number here

// insert your SMS message sText here
#define CFG_MSG_TEXT         "WARNING: Your prepaid electricity is running low."

// timeout values - these can be modified or left as is
#define CFG_APICALL_TIMEOUT         5 // Config: Maximum time in seconds (long value) for API call to take
#define CFG_APICALL_CONNECT_TIMEOUT 2 // Config: maximum time in seconds (long value) that API call takes to connect to Clickatell server

/* ----------------------------------------------------------------------------- *
 * Fixed Macros/Types                                                            *
 * ----------------------------------------------------------------------------- */

// common print functions
#define PRINT_MAIN_TEST_SEPARATOR   { click_debug_print("\n===============================================================================================\n"); }
#define PRINT_SUB_TEST_SEPARATOR    { click_debug_print("\n\n"); }

static void run_common_setup(eClickApi eApiType);
static void run_common_api_call(eClickApi eApiType, ClickSmsHandle *oClickSms);

#define TH 690
#define triggeredMessage 0
#define coolDown 0


/*********************************************************
 *  IF YOUR ARDUINO CODE HAS OTHER FUNCTIONS APART FROM  *
 *  setup() AND loop() YOU MUST DECLARE THEM HERE        *
 * *******************************************************/

/*
 * Function:  run_common_setup
 * Info:      Runs series of API calls.
 * Inputs:    API type
 * Return:    void
 */
static void run_common_setup(eClickApi eApiType)
{
    ClickSmsHandle *oClickSms = NULL;

    PRINT_MAIN_TEST_SEPARATOR

    switch (eApiType) {
        case CLICK_API_HTTP:
            click_debug_print("Executing HTTP API Tests with Username+Password as authentication method\n\n");

            ClickSmsString *sHttpUser = click_string_create(CFG_HTTP_USERNAME);
            ClickSmsString *sHttpPassword = click_string_create(CFG_HTTP_PASSWORD);
            ClickSmsString *sHttpApiId = click_string_create(CFG_HTTP_APIID);

            oClickSms = clickatell_sms_handle_init(eApiType,
                                                   sHttpUser,
                                                   sHttpPassword,
                                                   NULL,
                                                   sHttpApiId,
                                                   CFG_APICALL_TIMEOUT,
                                                   CFG_APICALL_CONNECT_TIMEOUT);
            if (oClickSms == NULL)
                click_debug_print("ERROR: Clickatell SMS Module Initialization failed\n");
            else
                run_common_api_calls(eApiType, oClickSms);

            clickatell_sms_handle_shutdown(oClickSms);

            click_string_destroy(sHttpUser);
            click_string_destroy(sHttpPassword);
            click_string_destroy(sHttpApiId);
            break;

        default:
            click_debug_print("ERROR: Invalid API type selected!\n");
            break;
    }
}

/*
 * Function:  run_common_api_call
 * Info:      Runs common API call.
 *            Ensure function clickatell_sms_handle_init() has been called prior to this
 *            function.
 * Inputs:    eApiType  - API call type
 *            oClickSms - ClickSmsHandle handle returned from clickatell_sms_handle_init() call
 * Return:    void
 */
static void run_common_api_call(eClickApi eApiType, ClickSmsHandle *oClickSms)
{
    int i = 0;
    ClickSmsString *sResponse = NULL;
    ClickSmsString *sMsgText = click_string_create(CFG_MSG_TEXT);

    click_debug_print("[%s: Send SMS]\n\n", (eApiType == CLICK_API_HTTP ? "HTTP" : "REST"));
    ClickMsisdn *aMsisdnsSingle = (ClickMsisdn *)calloc(1, sizeof(ClickMsisdn));
    aMsisdnsSingle->aDests = calloc(1, sizeof(ClickSmsString *));
    aMsisdnsSingle->iNum = 1;        // send to 1 mobile number
    aMsisdnsSingle->aDests[0] = (ClickSmsString *)click_string_create(CFG_MSISDN1);

    ClickSmsString *sMsgIdResponse = clickatell_sms_message_send(oClickSms, sMsgText, aMsisdnsSingle);
    PRINT_SUB_TEST_SEPARATOR

    click_string_destroy((ClickSmsString *)(aMsisdnsSingle->aDests[0]));
    free(aMsisdnsSingle->aDests);
    free(aMsisdnsSingle);

    // retrieve apiMessageId field from response
    ClickSmsString *sMsgId = NULL;
    if (eApiType == CLICK_API_HTTP) {
        /* A successful response should look like this:   ID: 205e85d0578314037a96175249fc6a2b
         * which means we need to remove the 'ID:' prefix text and space character from the response
         */
        click_string_trim_prefix(sMsgIdResponse, 4);
        sMsgId = click_string_duplicate(sMsgIdResponse);
    }

    click_string_destroy(sMsgIdResponse);
    click_string_destroy(sMsgId);
    click_string_destroy(sMsgText);
}



/**************************
 * YOUR ARDUINO CODE HERE *
 * ************************/

int main (int argc, char *argv[]){

        setup();

        while(1){
                loop();
        }

        return (0);
}

void setup(void) {
}


void loop(int argc, char *argv[]){
  float analogReadingArduino;
  analogReadingArduino = analogRead(5);

  if(analogReadingArduino > TH){
    if(triggeredMessage == 0 && coolDown == 0){
      // start using Clickatell library
      clickatell_sms_init();

      // run Clickatell HTTP common API calls (with Username+Password as authentication)
      run_common_setup(CLICK_API_HTTP);

      // finished using Clickatell library
      clickatell_sms_shutdown();

  	  //Update variables
  	  triggeredMessage = 1;
  	  coolDown = 1;
    }
  } else {
    if(triggeredMessage == 1){
      triggeredMessage = 0;
    }

    if (coolDown < 100 && coolDown > 0) {
  	  coolDown++;
    } else {
  	  coolDown = 0;
    }
  }

  delay(3000);
}