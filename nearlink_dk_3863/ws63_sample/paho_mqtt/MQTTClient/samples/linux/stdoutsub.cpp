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

/*
 
 stdout subscriber
 
 compulsory parameters:
 
  topic to subscribe to
 
 defaulted parameters:
 
	--host localhost
	--port 1883
	--qos 2
	--delimiter \n
	--clientid stdout_subscriber
	
	--userid none
	--password none
 
*/
#include <stdio.h>
#include <memory.h>
#define MQTT_DEBUG 1
#include "MQTTClient.h"

#define DEFAULT_STACK_SIZE -1

#include "linux.cpp"

#include <signal.h>
#include <sys/time.h>
#include <stdlib.h>


volatile int toStop = 0;


void usage()
{
	printf("MQTT stdout subscriber\n");
	printf("Usage: stdoutsub topicname <options>, where options are:\n");
	printf("  --host <hostname> (default is localhost)\n");
	printf("  --port <port> (default is 1883)\n");
	printf("  --qos <qos> (default is 2)\n");
	printf("  --delimiter <delim> (default is \\n)\n");
	printf("  --clientid <clientid> (default is hostname+timestamp)\n");
	printf("  --username none\n");
	printf("  --password none\n");
	printf("  --showtopics <on or off> (default is on if the topic has a wildcard, else off)\n");
	exit(-1);
}


void cfinish(int sig)
{
	signal(SIGINT, NULL);
	toStop = 1;
}


struct opts_struct
{
	char* clientid;
	int nodelimiter;
	char* delimiter;
	MQTT::QoS qos;
	char* username;
	char* password;
	char* host;
	int port;
	int showtopics;
} opts =
{
	(char*)"stdout-subscriber", 0, (char*)"\n", MQTT::QOS2, NULL, NULL, (char*)"localhost", 1883, 0
};


void getopts(int argc, char** argv)
{
	int count = 2;
	
	while (count < argc)
	{
		if (strcmp(argv[count], "--qos") == 0)
		{
			if (++count < argc)
			{
				if (strcmp(argv[count], "0") == 0)
					opts.qos = MQTT::QOS0;
				else if (strcmp(argv[count], "1") == 0)
					opts.qos = MQTT::QOS1;
				else if (strcmp(argv[count], "2") == 0)
					opts.qos = MQTT::QOS2;
				else
					usage();
			}
			else
				usage();
		}
		else if (strcmp(argv[count], "--host") == 0)
		{
			if (++count < argc)
				opts.host = argv[count];
			else
				usage();
		}
		else if (strcmp(argv[count], "--port") == 0)
		{
			if (++count < argc)
				opts.port = atoi(argv[count]);
			else
				usage();
		}
		else if (strcmp(argv[count], "--clientid") == 0)
		{
			if (++count < argc)
				opts.clientid = argv[count];
			else
				usage();
		}
		else if (strcmp(argv[count], "--username") == 0)
		{
			if (++count < argc)
				opts.username = argv[count];
			else
				usage();
		}
		else if (strcmp(argv[count], "--password") == 0)
		{
			if (++count < argc)
				opts.password = argv[count];
			else
				usage();
		}
		else if (strcmp(argv[count], "--delimiter") == 0)
		{
			if (++count < argc)
				opts.delimiter = argv[count];
			else
				opts.nodelimiter = 1;
		}
		else if (strcmp(argv[count], "--showtopics") == 0)
		{
			if (++count < argc)
			{
				if (strcmp(argv[count], "on") == 0)
					opts.showtopics = 1;
				else if (strcmp(argv[count], "off") == 0)
					opts.showtopics = 0;
				else
					usage();
			}
			else
				usage();
		}
		count++;
	}
	
}


void myconnect(IPStack& ipstack, MQTT::Client<IPStack, Countdown, 1000>& client, MQTTPacket_connectData& data)
{
	printf("Connecting to %s:%d\n", opts.host, opts.port);
	int rc = ipstack.connect(opts.host, opts.port);
	if (rc != 0)
	    printf("rc from TCP connect is %d\n", rc);

	rc = client.connect(data);
	if (rc != 0)
	{
		printf("Failed to connect, return code %d\n", rc);
		exit(-1);	
	}
	printf("Connected\n");
}


void messageArrived(MQTT::MessageData& md)
{
	MQTT::Message &message = md.message;

	if (opts.showtopics)
		printf("%.*s\t", md.topicName.lenstring.len, md.topicName.lenstring.data);
	if (opts.nodelimiter)
		printf("%.*s", (int)message.payloadlen, (char*)message.payload);
	else
		printf("%.*s%s", (int)message.payloadlen, (char*)message.payload, opts.delimiter);
	fflush(stdout);
}


int main(int argc, char** argv)
{
	int rc = 0;
	
	if (argc < 2)
		usage();
	
	char* topic = argv[1];

	if (strchr(topic, '#') || strchr(topic, '+'))
		opts.showtopics = 1;
	if (opts.showtopics)
		printf("topic is %s\n", topic);

	getopts(argc, argv);	

	IPStack ipstack = IPStack();
	MQTT::Client<IPStack, Countdown, 1000> client = MQTT::Client<IPStack, Countdown, 1000>(ipstack);

	signal(SIGINT, cfinish);
	signal(SIGTERM, cfinish);
 
	MQTTPacket_connectData data = MQTTPacket_connectData_initializer;       
	data.willFlag = 0;
	data.MQTTVersion = 3;
	data.clientID.cstring = opts.clientid;
	data.username.cstring = opts.username;
	data.password.cstring = opts.password;

	data.keepAliveInterval = 10;
	data.cleansession = 1;
	printf("will flag %d\n", data.willFlag);
	
	myconnect(ipstack, client, data);
    
	rc = client.subscribe(topic, opts.qos, messageArrived);
	printf("Subscribed %d\n", rc);

	while (!toStop)
	{
		client.yield(1000);	

		//if (!client.isconnected)
		//	myconnect(ipstack, client, data);
	}
	
	printf("Stopping\n");

	rc = client.disconnect();

	ipstack.disconnect();

	return 0;
}


