/**
 * Copyright (c) 2020 HiSilicon (Shanghai) Technologies CO., LIMITED.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.  *
 *
 * History: \n
 * 2022-09-16， Create file. \n
 */


/**
 * @file
 * Tests for the Paho embedded C "high" level client
 */

 #include <stdio.h>
 #include <string.h>
 #include <memory.h>
 //#define MQTT_DEBUG
 #include "MQTTClient.h"

 #define DEFAULT_STACK_SIZE -1

 #include "linux.cpp"

 #include <sys/time.h>
 #include <stdlib.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

void usage(void)
{
	printf("help!!\n");
	exit(EXIT_FAILURE);
}

struct Options
{
	char* host;         /**< connection to system under test. */
  int port;
	char* proxy_host;
  int proxy_port;
	int verbose;
	int test_no;
	int MQTTVersion;
	int iterations;
} options =
{
	(char*)"localhost",
  1883,
	(char*)"localhost",
  1885,
	0,
	0,
	4,
	1,
};

void getopts(int argc, char** argv)
{
	int count = 1;

	while (count < argc)
	{
		if (strcmp(argv[count], "--test_no") == 0)
		{
			if (++count < argc)
				options.test_no = atoi(argv[count]);
			else
				usage();
		}
		else if (strcmp(argv[count], "--host") == 0)
		{
			if (++count < argc)
			{
				options.host = argv[count];
				printf("\nSetting host to %s\n", options.host);
			}
			else
				usage();
		}
    else if (strcmp(argv[count], "--port") == 0)
    {
      if (++count < argc)
      {
        options.port = atoi(argv[count]);
        printf("\nSetting port to %d\n", options.port);
      }
      else
        usage();
    }
		else if (strcmp(argv[count], "--proxy_host") == 0)
		{
			if (++count < argc)
				options.proxy_host = argv[count];
			else
				usage();
		}
    else if (strcmp(argv[count], "--proxy_port") == 0)
    {
      if (++count < argc)
      {
        options.proxy_port = atoi(argv[count]);
        printf("\nSetting proxy port to %d\n", options.proxy_port);
      }
      else
        usage();
    }
		else if (strcmp(argv[count], "--MQTTversion") == 0)
		{
			if (++count < argc)
			{
				options.MQTTVersion = atoi(argv[count]);
				printf("setting MQTT version to %d\n", options.MQTTVersion);
			}
			else
				usage();
		}
		else if (strcmp(argv[count], "--iterations") == 0)
		{
			if (++count < argc)
				options.iterations = atoi(argv[count]);
			else
				usage();
		}
		else if (strcmp(argv[count], "--verbose") == 0)
		{
			options.verbose = 1;
			printf("\nSetting verbose on\n");
		}
		count++;
	}
}


#define LOGA_DEBUG 0
#define LOGA_INFO 1
#include <stdarg.h>
#include <time.h>
#include <sys/timeb.h>
void MyLog(int LOGA_level, const char* format, ...)
{
	static char msg_buf[256];
	va_list args;
	struct timeb ts;

	struct tm *timeinfo;

	if (LOGA_level == LOGA_DEBUG && options.verbose == 0)
	  return;

	ftime(&ts);
	timeinfo = localtime(&ts.time);
	strftime(msg_buf, 80, "%Y%m%d %H%M%S", timeinfo);

	sprintf(&msg_buf[strlen(msg_buf)], ".%.3hu ", ts.millitm);

	va_start(args, format);
	vsnprintf(&msg_buf[strlen(msg_buf)], sizeof(msg_buf) - strlen(msg_buf), format, args);
	va_end(args);

	printf("%s\n", msg_buf);
	fflush(stdout);
}


#if defined(WIN32) || defined(_WINDOWS)
#define mqsleep(A) Sleep(1000*A)
#define START_TIME_TYPE DWORD
static DWORD start_time = 0;
START_TIME_TYPE start_clock(void)
{
	return GetTickCount();
}
#elif defined(AIX)
#define mqsleep sleep
#define START_TIME_TYPE struct timespec
START_TIME_TYPE start_clock(void)
{
	static struct timespec start;
	clock_gettime(CLOCK_REALTIME, &start);
	return start;
}
#else
#define mqsleep sleep
#define START_TIME_TYPE struct timeval
/* TODO - unused - remove? static struct timeval start_time; */
START_TIME_TYPE start_clock(void)
{
	struct timeval start_time;
	gettimeofday(&start_time, NULL);
	return start_time;
}
#endif


#if defined(WIN32)
long elapsed(START_TIME_TYPE start_time)
{
	return GetTickCount() - start_time;
}
#elif defined(AIX)
#define assert(a)
long elapsed(struct timespec start)
{
	struct timespec now, res;

	clock_gettime(CLOCK_REALTIME, &now);
	ntimersub(now, start, res);
	return (res.tv_sec)*1000L + (res.tv_nsec)/1000000L;
}
#else
long elapsed(START_TIME_TYPE start_time)
{
	struct timeval now, res;

	gettimeofday(&now, NULL);
	timersub(&now, &start_time, &res);
	return (res.tv_sec)*1000 + (res.tv_usec)/1000;
}
#endif


#define assert(a, b, c, d) myassert(__FILE__, __LINE__, a, b, c, d)
#define assert1(a, b, c, d, e) myassert(__FILE__, __LINE__, a, b, c, d, e)

int tests = 0;
int failures = 0;
FILE* xml;
START_TIME_TYPE global_start_time;
char output[3000];
char* cur_output = output;


void write_test_result(void)
{
	long duration = elapsed(global_start_time);

	fprintf(xml, " time=\"%ld.%.3ld\" >\n", duration / 1000, duration % 1000);
	if (cur_output != output)
	{
		fprintf(xml, "%s", output);
		cur_output = output;
	}
	fprintf(xml, "</testcase>\n");
}


void myassert(const char* filename, int lineno, const char* description, int value, const char* format, ...)
{
	++tests;
	if (!value)
	{
		va_list args;

		++failures;
		MyLog(LOGA_INFO, (char*)"Assertion failed, file %s, line %d, description: %s\n", filename, lineno, description);

		va_start(args, format);
		vprintf(format, args);
		va_end(args);

		cur_output += sprintf(cur_output, "<failure type=\"%s\">file %s, line %d </failure>\n",
                        description, filename, lineno);
	}
	else
		MyLog(LOGA_DEBUG, "Assertion succeeded, file %s, line %d, description: %s", filename, lineno, description);
}


static volatile MQTT::MessageData* test1_message_data = NULL;
static MQTT::Message pubmsg;

void messageArrived(MQTT::MessageData& md)
{
    test1_message_data = &md;
	  MQTT::Message &m = md.message;

    assert("Good message lengths", pubmsg.payloadlen == m.payloadlen,
         "payloadlen was %d", m.payloadlen);

    if (pubmsg.payloadlen == m.payloadlen)
        assert("Good message contents", memcmp(m.payload, pubmsg.payload, m.payloadlen) == 0,
          "payload was %s", m.payload);
}


/*********************************************************************

Test1: single-threaded client

*********************************************************************/
void test1_sendAndReceive(MQTT::Client<IPStack, Countdown, 1000>& client, int qos, const char* test_topic)
{
	char* topicName = NULL;
	int topicLen;
	int i = 0;
	int iterations = 50;
	int rc;
  int wait_seconds;

	MyLog(LOGA_DEBUG, "%d messages at QoS %d", iterations, qos);
  memset(&pubmsg, '\0', sizeof(pubmsg));
	pubmsg.payload = (void*)"a much longer message that we can shorten to the extent that we need to payload up to 11";
	pubmsg.payloadlen = 11;
	pubmsg.qos = (MQTT::QoS)qos;
	pubmsg.retained = false;
  pubmsg.dup = false;

	for (i = 0; i < iterations; ++i)
	{
    test1_message_data = NULL;
		rc = client.publish(test_topic, pubmsg);
		assert("Good rc from publish", rc == MQTT::SUCCESS, "rc was %d", rc);

    /* wait for the message to be received */
    wait_seconds = 10;
		while ((test1_message_data == NULL) && (wait_seconds-- > 0))
		{
      client.yield(100);
		}
		assert("Message Arrived", wait_seconds > 0, "Time out waiting for message %d\n", i);

		if (!test1_message_data)
			printf("No message received within timeout period\n");
	}

	/* wait to receive any outstanding messages */
  wait_seconds = 2;
  while (wait_seconds-- > 0)
  {
      client.yield(1000);
  }
}


int test1(struct Options options)
{
	MQTT::QoS subsqos = MQTT::QOS2;
	int rc = 0;
	const char* test_topic = "C client test1";

	fprintf(xml, "<testcase classname=\"test1\" name=\"single threaded client using receive\"");
	global_start_time = start_clock();
	failures = 0;
	MyLog(LOGA_INFO, "Starting test 1 - single threaded client using receive");

  IPStack ipstack = IPStack();
	MQTT::Client<IPStack, Countdown, 1000> client = MQTT::Client<IPStack, Countdown, 1000>(ipstack);

  MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
  data.willFlag = 1;
  data.MQTTVersion = options.MQTTVersion;
  data.clientID.cstring = (char*)"single-threaded-test";
  data.username.cstring = (char*)"testuser";
  data.password.cstring = (char*)"testpassword";

  data.keepAliveInterval = 20;
  data.cleansession = 1;

	data.will.message.cstring = (char*)"will message";
	data.will.qos = 1;
	data.will.retained = 0;
	data.will.topicName.cstring = (char*)"will topic";

	MyLog(LOGA_DEBUG, "Connecting");
  rc = ipstack.connect(options.host, options.port);
  assert("Good rc from TCP connect", rc == MQTT::SUCCESS, "rc was %d", rc);
  if (rc != MQTT::SUCCESS)
    goto exit;

  rc = client.connect(data);
	assert("Good rc from connect", rc == MQTT::SUCCESS, "rc was %d", rc);
	if (rc != MQTT::SUCCESS)
		goto exit;

	rc = client.subscribe(test_topic, subsqos, messageArrived);
	assert("Good rc from subscribe", rc == MQTT::SUCCESS, "rc was %d", rc);

	test1_sendAndReceive(client, 0, test_topic);
	test1_sendAndReceive(client, 1, test_topic);
	test1_sendAndReceive(client, 2, test_topic);

	MyLog(LOGA_DEBUG, "Stopping\n");

	rc = client.unsubscribe(test_topic);
	assert("Unsubscribe successful", rc == MQTT::SUCCESS, "rc was %d", rc);
	rc = client.disconnect();
	assert("Disconnect successful", rc == MQTT::SUCCESS, "rc was %d", rc);
  ipstack.disconnect();

	/* Just to make sure we can connect again */
  rc = ipstack.connect(options.host, options.port);
  assert("TCP connect successful",  rc == MQTT::SUCCESS, "rc was %d", rc);
  rc = client.connect(data);
	assert("Connect successful",  rc == MQTT::SUCCESS, "rc was %d", rc);
	rc = client.disconnect();
	assert("Disconnect successful", rc == MQTT::SUCCESS, "rc was %d", rc);
  ipstack.disconnect();

exit:
	MyLog(LOGA_INFO, "TEST1: test %s. %d tests run, %d failures.",
			(failures == 0) ? "passed" : "failed", tests, failures);
	write_test_result();
	return failures;
}


/*********************************************************************

Test 2: connack return data

*********************************************************************/
int test2(struct Options options)
{
  MQTT::QoS subsqos = MQTT::QOS2;
	int rc;
  const char* test_topic = "C client test2";

  fprintf(xml, "<testcase classname=\"test2\" name=\"connack return data\"");
  global_start_time = start_clock();
  failures = 0;
  MyLog(LOGA_INFO, "Starting test 2 - connack return data");

  IPStack ipstack = IPStack();
  MQTT::Client<IPStack, Countdown, 1000> client = MQTT::Client<IPStack, Countdown, 1000>(ipstack);

  MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
  data.willFlag = 1;
  data.MQTTVersion = options.MQTTVersion;
  data.clientID.cstring = (char*)"connack-return-data";
  data.username.cstring = (char*)"testuser";
  data.password.cstring = (char*)"testpassword";

  data.keepAliveInterval = 20;
  data.cleansession = 1;

  data.will.message.cstring = (char*)"will message";
  data.will.qos = 1;
  data.will.retained = 0;
  data.will.topicName.cstring = (char*)"will topic";

  MyLog(LOGA_DEBUG, "Connecting");
  rc = ipstack.connect(options.host, options.port);
  assert("Good rc from TCP connect", rc == MQTT::SUCCESS, "rc was %d", rc);
  if (rc != MQTT::SUCCESS)
    goto exit;

  MQTT::connackData connack;
  rc = client.connect(data, connack);
  assert("Good rc from connect", rc == MQTT::SUCCESS, "rc was %d", rc);
  if (rc != MQTT::SUCCESS)
    goto exit;

  assert("Good rc in connack", connack.rc == 0, "rc was %d", connack.rc);
  assert("Session present is 0", connack.sessionPresent == 0,
         "sessionPresent was %d", connack.sessionPresent);

  rc = client.disconnect();
  assert("Disconnect successful", rc == MQTT::SUCCESS, "rc was %d", rc);
  ipstack.disconnect();

  /* reconnect with cleansession false */
  data.cleansession = 0;
  rc = ipstack.connect(options.host, options.port);
  assert("TCP connect successful",  rc == MQTT::SUCCESS, "rc was %d", rc);
  rc = client.connect(data, connack);
  assert("Connect successful",  rc == MQTT::SUCCESS, "rc was %d", rc);

  assert("Good rc in connack", connack.rc == 0, "rc was %d", connack.rc);
  assert("Session present is 0", connack.sessionPresent == 0,
           "sessionPresent was %d", connack.sessionPresent);

  MQTT::subackData suback;
  rc = client.subscribe(test_topic, subsqos, messageArrived, suback);
  assert("Good rc from subscribe", rc == MQTT::SUCCESS, "rc was %d", rc);
  assert("Granted QoS rc from subscribe", suback.grantedQoS == MQTT::QOS2,
         "rc was %d", suback.grantedQoS);

  rc = client.disconnect();
  assert("Disconnect successful", rc == MQTT::SUCCESS, "rc was %d", rc);
  ipstack.disconnect();

  /* reconnect with cleansession false */
  data.cleansession = 0;
  rc = ipstack.connect(options.host, options.port);
  assert("TCP connect successful",  rc == MQTT::SUCCESS, "rc was %d", rc);
  rc = client.connect(data, connack);
  assert("Connect successful",  rc == MQTT::SUCCESS, "rc was %d", rc);

  assert("Good rc in connack", connack.rc == 0, "rc was %d", connack.rc);
  assert("Session present is 1", connack.sessionPresent == 1,
           "sessionPresent was %d", connack.sessionPresent);

  rc = client.disconnect();
  assert("Disconnect successful", rc == MQTT::SUCCESS, "rc was %d", rc);
  ipstack.disconnect();

  /* reconnect with cleansession true */
  data.cleansession = 1;
  rc = ipstack.connect(options.host, options.port);
  assert("TCP connect successful",  rc == MQTT::SUCCESS, "rc was %d", rc);
  rc = client.connect(data, connack);
  assert("Connect successful",  rc == MQTT::SUCCESS, "rc was %d", rc);

  assert("Good rc in connack", connack.rc == 0, "rc was %d", connack.rc);
  assert("Session present is 0", connack.sessionPresent == 0,
           "sessionPresent was %d", connack.sessionPresent);

  rc = client.disconnect();
  assert("Disconnect successful", rc == MQTT::SUCCESS, "rc was %d", rc);
  ipstack.disconnect();

exit:
  MyLog(LOGA_INFO, "TEST2: test %s. %d tests run, %d failures.",
      (failures == 0) ? "passed" : "failed", tests, failures);
  write_test_result();
  return failures;
}


/*********************************************************************

Test 3: client session state

*********************************************************************/
static volatile MQTT::MessageData* test2_message_data = NULL;

void messageArrived2(MQTT::MessageData& md)
{
    test2_message_data = &md;
	  MQTT::Message &m = md.message;

    assert("Good message lengths", pubmsg.payloadlen == m.payloadlen,
         "payloadlen was %d", m.payloadlen);

    if (pubmsg.payloadlen == m.payloadlen)
        assert("Good message contents", memcmp(m.payload, pubmsg.payload, m.payloadlen) == 0,
          "payload was %s", m.payload);
}


int check_subs_exist(MQTT::Client<IPStack, Countdown, 1000>& client, const char* test_topic, int which)
{
    int rc = MQTT::FAILURE;
    int wait_seconds = 0;

    memset(&pubmsg, '\0', sizeof(pubmsg));
    pubmsg.payload = (void*)"a much longer message that we can shorten to the extent that we need to payload up to 11";
    pubmsg.payloadlen = 11;
    pubmsg.qos = MQTT::QOS2;
    pubmsg.retained = false;
    pubmsg.dup = false;

    test1_message_data = test2_message_data = NULL;
    rc = client.publish(test_topic, pubmsg);
    assert("Good rc from publish", rc == MQTT::SUCCESS, "rc was %d", rc);

    /* wait for the message to be received */
    wait_seconds = 10;
    while (wait_seconds-- > 0)
    {
        client.yield(100);
    }

    rc = (((which == 1 || which == 3) && test1_message_data) ||
         (which == 2 && test1_message_data == NULL)) ? MQTT::SUCCESS : MQTT::FAILURE;
    assert("test1 subscription", rc == MQTT::SUCCESS, "test1_message_data %p\n",
            test1_message_data);
    rc = (((which == 2 || which == 3) && test2_message_data) ||
         (which == 1 && test2_message_data == NULL)) ? MQTT::SUCCESS : MQTT::FAILURE;
    assert("test2 subscription", rc == MQTT::SUCCESS, "test2_message_data %p\n",
             test2_message_data);
    return rc;
}


int test3(struct Options options)
{
  MQTT::QoS subsqos = MQTT::QOS2;
	int rc;
  const char* test_topic = "C client test3";
  int wait_seconds = 0;

  fprintf(xml, "<testcase classname=\"test3\" name=\"session state\"");
  global_start_time = start_clock();
  failures = 0;
  MyLog(LOGA_INFO, "Starting test 3 - session state");

  IPStack ipstack = IPStack();
  MQTT::Client<IPStack, Countdown, 1000> client = MQTT::Client<IPStack, Countdown, 1000>(ipstack);

  MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
  data.willFlag = 1;
  data.MQTTVersion = options.MQTTVersion;
  data.clientID.cstring = (char*)"connack-return-data";
  data.username.cstring = (char*)"testuser";
  data.password.cstring = (char*)"testpassword";

  data.keepAliveInterval = 10;
  data.cleansession = 1;

  data.will.message.cstring = (char*)"will message";
  data.will.qos = 1;
  data.will.retained = 0;
  data.will.topicName.cstring = (char*)"will topic";

  assert("Good rc in connack", client.isConnected() == false,
         "isconnected was %d", client.isConnected());

  MyLog(LOGA_DEBUG, "Connecting");
  rc = ipstack.connect(options.host, options.port);
  assert("Good rc from TCP connect", rc == MQTT::SUCCESS, "rc was %d", rc);
  if (rc != MQTT::SUCCESS)
    goto exit;

  MQTT::connackData connack;
  rc = client.connect(data, connack);
  assert("Good rc from connect", rc == MQTT::SUCCESS, "rc was %d", rc);
  if (rc != MQTT::SUCCESS)
    goto exit;

  assert("Good rc in connack", connack.rc == 0, "rc was %d", connack.rc);
  assert("Session present is 0", connack.sessionPresent == 0,
         "sessionPresent was %d", connack.sessionPresent);

  assert("Good rc in connack", client.isConnected() == true,
                "isconnected was %d", client.isConnected());

  rc = client.disconnect();
  assert("Disconnect successful", rc == MQTT::SUCCESS, "rc was %d", rc);
  ipstack.disconnect();

  /* reconnect with cleansession false */
  data.cleansession = 0;
  rc = ipstack.connect(options.proxy_host, options.proxy_port);
  assert("TCP connect successful",  rc == MQTT::SUCCESS, "rc was %d", rc);
  rc = client.connect(data, connack);
  assert("Connect successful",  rc == MQTT::SUCCESS, "rc was %d", rc);

  assert("Good rc in connack", connack.rc == 0, "rc was %d", connack.rc);
  assert("Session present is 0", connack.sessionPresent == 0,
           "sessionPresent was %d", connack.sessionPresent);

  MQTT::subackData suback;
  rc = client.subscribe(test_topic, subsqos, messageArrived, suback);
  assert("Good rc from subscribe", rc == MQTT::SUCCESS, "rc was %d", rc);
  assert("Granted QoS rc from subscribe", suback.grantedQoS == MQTT::QOS2,
         "rc was %d", suback.grantedQoS);

  check_subs_exist(client, test_topic, 1);

  rc = client.subscribe(test_topic, subsqos, messageArrived2, suback);
  assert("Good rc from subscribe", rc == MQTT::SUCCESS, "rc was %d", rc);
  assert("Granted QoS rc from subscribe", suback.grantedQoS == MQTT::QOS2,
                  "rc was %d", suback.grantedQoS);

  check_subs_exist(client, test_topic, 2);

  rc = client.disconnect();
  assert("Disconnect successful", rc == MQTT::SUCCESS, "rc was %d", rc);
  ipstack.disconnect();

  /* reconnect with cleansession false */
  data.cleansession = 0;
  rc = ipstack.connect(options.proxy_host, options.proxy_port);
  assert("TCP connect successful",  rc == MQTT::SUCCESS, "rc was %d", rc);
  rc = client.connect(data, connack);
  assert("Connect successful",  rc == MQTT::SUCCESS, "rc was %d", rc);

  assert("Good rc in connack", connack.rc == 0, "rc was %d", connack.rc);
  assert("Session present is 1", connack.sessionPresent == 1,
           "sessionPresent was %d", connack.sessionPresent);

  check_subs_exist(client, test_topic, 2);

  rc = client.subscribe(test_topic, subsqos, messageArrived, suback);
  assert("Good rc from subscribe", rc == MQTT::SUCCESS, "rc was %d", rc);
  assert("Granted QoS rc from subscribe", suback.grantedQoS == MQTT::QOS2,
            "rc was %d", suback.grantedQoS);

  check_subs_exist(client, test_topic, 1);

  // cause a connection FAILURE
  memset(&pubmsg, '\0', sizeof(pubmsg));
  pubmsg.payload = (void*)"TERMINATE";
  pubmsg.payloadlen = strlen((char*)pubmsg.payload);
  pubmsg.qos = MQTT::QOS0;
  pubmsg.retained = false;
  pubmsg.dup = false;
  rc = client.publish("MQTTSAS topic", pubmsg);
  assert("Good rc from publish", rc == MQTT::SUCCESS, "rc was %d", rc);

  // wait for failure to be noticed by keepalive
  wait_seconds = 20;
  while (client.isConnected() && (wait_seconds-- > 0))
  {
      client.yield(1000);
  }
  assert("Disconnected", !client.isConnected(), "isConnected was %d",
         client.isConnected());
  ipstack.disconnect();

  /* reconnect with cleansession false */
  data.cleansession = 0;
  rc = ipstack.connect(options.host, options.port);
  assert("TCP connect successful",  rc == MQTT::SUCCESS, "rc was %d", rc);
  rc = client.connect(data, connack);
  assert("Connect successful",  rc == MQTT::SUCCESS, "rc was %d", rc);

  assert("Good rc in connack", connack.rc == 0, "rc was %d", connack.rc);
  assert("Session present is 1", connack.sessionPresent == 1,
           "sessionPresent was %d", connack.sessionPresent);

  check_subs_exist(client, test_topic, 1);

  rc = client.subscribe(test_topic, subsqos, messageArrived2, suback);
  assert("Good rc from subscribe", rc == MQTT::SUCCESS, "rc was %d", rc);
  assert("Granted QoS rc from subscribe", suback.grantedQoS == MQTT::QOS2,
                  "rc was %d", suback.grantedQoS);

  check_subs_exist(client, test_topic, 2);

  rc = client.disconnect();
  assert("Disconnect successful", rc == MQTT::SUCCESS, "rc was %d", rc);
  ipstack.disconnect();

  /* reconnect with cleansession true to clean up both server and client state */
  data.cleansession = 1;
  rc = ipstack.connect(options.host, options.port);
  assert("TCP connect successful",  rc == MQTT::SUCCESS, "rc was %d", rc);
  rc = client.connect(data, connack);
  assert("Connect successful",  rc == MQTT::SUCCESS, "rc was %d", rc);

  assert("Good rc in connack", connack.rc == 0, "rc was %d", connack.rc);
  assert("Session present is 0", connack.sessionPresent == 0,
           "sessionPresent was %d", connack.sessionPresent);

  rc = client.subscribe(test_topic, subsqos, messageArrived2, suback);
  assert("Good rc from subscribe", rc == MQTT::SUCCESS, "rc was %d", rc);
  assert("Granted QoS rc from subscribe", suback.grantedQoS == MQTT::QOS2,
                  "rc was %d", suback.grantedQoS);

  check_subs_exist(client, test_topic, 2);

  rc = client.disconnect();
  assert("Disconnect successful", rc == MQTT::SUCCESS, "rc was %d", rc);
  ipstack.disconnect();

exit:
  MyLog(LOGA_INFO, "TEST2: test %s. %d tests run, %d failures.",
      (failures == 0) ? "passed" : "failed", tests, failures);
  write_test_result();
  return failures;
}


#if 0
/*********************************************************************

Test 4: connectionLost and will message

*********************************************************************/
MQTTClient test6_c1, test6_c2;
volatile int test6_will_message_arrived = 0;
volatile int test6_connection_lost_called = 0;

void test6_connectionLost(void* context, char* cause)
{
	MQTTClient c = (MQTTClient)context;
	printf("%s -> Callback: connection lost\n", (c == test6_c1) ? "Client-1" : "Client-2");
	test6_connection_lost_called = 1;
}

void test6_deliveryComplete(void* context, MQTTClient_deliveryToken token)
{
	printf("Client-2 -> Callback: publish complete for token %d\n", token);
}

char* test6_will_topic = "C Test 2: will topic";
char* test6_will_message = "will message from Client-1";

int test6_messageArrived(void* context, char* topicName, int topicLen, MQTTClient_message* m)
{
	MQTTClient c = (MQTTClient)context;
	printf("%s -> Callback: message received on topic '%s' is '%.*s'.\n",
			 (c == test6_c1) ? "Client-1" : "Client-2", topicName, m->payloadlen, (char*)(m->payload));
	if (c == test6_c2 && strcmp(topicName, test6_will_topic) == 0 && memcmp(m->payload, test6_will_message, m->payloadlen) == 0)
		test6_will_message_arrived = 1;
	MQTTClient_free(topicName);
	MQTTClient_freeMessage(&m);
	return 1;
}


int test6(struct Options options)
{
	char* testname = "test6";
	MQTTClient_connectOptions opts = MQTTClient_connectOptions_initializer;
	MQTTClient_willOptions wopts =  MQTTClient_willOptions_initializer;
	MQTTClient_connectOptions opts2 = MQTTClient_connectOptions_initializer;
	int rc, count;
	char* mqttsas_topic = "MQTTSAS topic";

	failures = 0;
	MyLog(LOGA_INFO, "Starting test 6 - connectionLost and will messages");
	fprintf(xml, "<testcase classname=\"test1\" name=\"connectionLost and will messages\"");
	global_start_time = start_clock();

	opts.keepAliveInterval = 2;
	opts.cleansession = 1;
	opts.MQTTVersion = MQTTVERSION_3_1_1;
	opts.will = &wopts;
	opts.will->message = test6_will_message;
	opts.will->qos = 1;
	opts.will->retained = 0;
	opts.will->topicName = test6_will_topic;
	if (options.haconnections != NULL)
	{
		opts.serverURIs = options.haconnections;
		opts.serverURIcount = options.hacount;
	}

	/* Client-1 with Will options */
	rc = MQTTClient_create(&test6_c1, options.proxy_connection, "Client_1", MQTTCLIENT_PERSISTENCE_DEFAULT, NULL);
	assert("good rc from create", rc == MQTTCLIENT_SUCCESS, "rc was %d\n", rc);
	if (rc != MQTTCLIENT_SUCCESS)
		goto exit;

	rc = MQTTClient_setCallbacks(test6_c1, (void*)test6_c1, test6_connectionLost, test6_messageArrived, test6_deliveryComplete);
	assert("good rc from setCallbacks",  rc == MQTTCLIENT_SUCCESS, "rc was %d\n", rc);
	if (rc != MQTTCLIENT_SUCCESS)
		goto exit;

	/* Connect to the broker */
	rc = MQTTClient_connect(test6_c1, &opts);
	assert("good rc from connect",  rc == MQTTCLIENT_SUCCESS, "rc was %d\n", rc);
	if (rc != MQTTCLIENT_SUCCESS)
		goto exit;

	/* Client - 2 (multi-threaded) */
	rc = MQTTClient_create(&test6_c2, options.connection, "Client_2", MQTTCLIENT_PERSISTENCE_DEFAULT, NULL);
	assert("good rc from create",  rc == MQTTCLIENT_SUCCESS, "rc was %d\n", rc);

	/* Set the callback functions for the client */
	rc = MQTTClient_setCallbacks(test6_c2, (void*)test6_c2, test6_connectionLost, test6_messageArrived, test6_deliveryComplete);
	assert("good rc from setCallbacks",  rc == MQTTCLIENT_SUCCESS, "rc was %d\n", rc);

	/* Connect to the broker */
	opts2.keepAliveInterval = 20;
	opts2.cleansession = 1;
	MyLog(LOGA_INFO, "Connecting Client_2 ...");
	rc = MQTTClient_connect(test6_c2, &opts2);
	assert("Good rc from connect", rc == MQTTCLIENT_SUCCESS, "rc was %d\n", rc);

	rc = MQTTClient_subscribe(test6_c2, test6_will_topic, 2);
	assert("Good rc from subscribe", rc == MQTTCLIENT_SUCCESS, "rc was %d\n", rc);

	/* now send the command which will break the connection and cause the will message to be sent */
	rc = MQTTClient_publish(test6_c1, mqttsas_topic, (int)strlen("TERMINATE"), "TERMINATE", 0, 0, NULL);
	assert("Good rc from publish", rc == MQTTCLIENT_SUCCESS, "rc was %d\n", rc);

	MyLog(LOGA_INFO, "Waiting to receive the will message");
	count = 0;
	while (++count < 40)
	{
		#if defined(WIN32)
			Sleep(1000L);
		#else
			sleep(1);
		#endif
		if (test6_will_message_arrived == 1 && test6_connection_lost_called == 1)
			break;
	}
	assert("will message arrived", test6_will_message_arrived == 1,
							"will_message_arrived was %d\n", test6_will_message_arrived);
	assert("connection lost called", test6_connection_lost_called == 1,
			         "connection_lost_called %d\n", test6_connection_lost_called);

	rc = MQTTClient_unsubscribe(test6_c2, test6_will_topic);
	assert("Good rc from unsubscribe", rc == MQTTCLIENT_SUCCESS, "rc was %d", rc);

	rc = MQTTClient_isConnected(test6_c2);
	assert("Client-2 still connected", rc == 1, "isconnected is %d", rc);

	rc = MQTTClient_isConnected(test6_c1);
	assert("Client-1 not connected", rc == 0, "isconnected is %d", rc);

	rc = MQTTClient_disconnect(test6_c2, 100L);
	assert("Good rc from disconnect", rc == MQTTCLIENT_SUCCESS, "rc was %d", rc);

	MQTTClient_destroy(&test6_c1);
	MQTTClient_destroy(&test6_c2);

exit:
	MyLog(LOGA_INFO, "%s: test %s. %d tests run, %d failures.\n",
			(failures == 0) ? "passed" : "failed", testname, tests, failures);
	write_test_result();
	return failures;
}
#endif

int main(int argc, char** argv)
{
	int rc = 0;
 	int (*tests[])(Options) = {NULL, test1, test2, test3, /*test4, test5, test6, test6a*/};
	int i;

	xml = fopen("TEST-test1.xml", "w");
	fprintf(xml, "<testsuite name=\"test1\" tests=\"%d\">\n", (int)(ARRAY_SIZE(tests) - 1));

	//setenv("MQTT_C_CLIENT_TRACE", "ON", 1);
	//setenv("MQTT_C_CLIENT_TRACE_LEVEL", "ERROR", 0);

	getopts(argc, argv);

	for (i = 0; i < options.iterations; ++i)
	{
	 	if (options.test_no == 0)
		{ /* run all the tests */
 		   	for (options.test_no = 1; options.test_no < ARRAY_SIZE(tests); ++options.test_no)
				rc += tests[options.test_no](options); /* return number of failures.  0 = test succeeded */
		}
		else
 		   	rc = tests[options.test_no](options); /* run just the selected test */
	}

 	if (rc == 0)
		MyLog(LOGA_INFO, "verdict pass");
	else
		MyLog(LOGA_INFO, "verdict fail");

	fprintf(xml, "</testsuite>\n");
	fclose(xml);
	return rc;
}
