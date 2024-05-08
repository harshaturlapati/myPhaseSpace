// example_imu1.cc -*- C++ -*-
// simple imu example

/***
Copyright (c) PhaseSpace, Inc 2017

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL PHASESPACE, INC
BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
***/

// 2024-04-16 Quick updates to include (i) unix epoch, (ii) IMU feedback and (iii) Marker data

#if defined(_MSC_VER)
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include <time.h>
#include <ctime>
#include <chrono>
#include "myPhaseSpaceUDP.h"
#include <sstream>    // header file for stringstream

using namespace std::chrono;
int num_visible = 0;
int num_IMUs = 0;

uint64_t nanoseconds_since_epoch = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

int64_t GetTickUs()
{
#if defined(_MSC_VER)

	nanoseconds_since_epoch = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

	return nanoseconds_since_epoch;

#else
	struct timespec start;
	clock_gettime(CLOCK_MONOTONIC, &start);

	return (start.tv_sec * 1000000LLU) + (start.tv_nsec / 1000);
#endif
}


#include <iostream>
#include <iomanip>

#include "owl.hpp"

using namespace std;

/***
Custom includes
***/
#include <sys/types.h>
#include <stdio.h>
#include <conio.h>
#include <sys/stat.h>
#include <inttypes.h>

#include <fstream>
#include <direct.h>

string convertToString(char* a, int size)
{
	int i;
	string s = "";
	for (i = 0; i < size; i++) {
		s = s + a[i];
	}
	return s;
}

/***
Custom includes
***/

void imu_info(OWL::Context &owl, const OWL::Event *frame, ofstream &log_file)
{
	const OWL::Event *inputs = frame->find(OWL::Type::INPUT, "inputs");
	num_IMUs = 0;

	if (!inputs)
	{
		std::cout << num_IMUs;
		return;
	}


	for (const OWL::Input *i = inputs->begin(); i != inputs->end(); i++)
	{
		OWL::DeviceInfo dev = owl.deviceInfo(i->hw_id);
		if (dev.type == "microdriver" && i->data.size() == 16)
		{
			const uint8_t *data = i->data.data();

			int16_t addr = int16_t(data[2] | (data[3] << 8));

			int16_t imu[6] = { int16_t(data[4] | (data[5] << 8)),
				int16_t(data[6] | (data[7] << 8)),
				int16_t(data[8] | (data[9] << 8)),
				int16_t(data[10] | (data[11] << 8)),
				int16_t(data[12] | (data[13] << 8)),
				int16_t(data[14] | (data[15] << 8)) };

			num_IMUs = num_IMUs + 1;

			log_file << "||IMU||" << "time=" << frame->time()
				<< " device=0x" << hex << i->hw_id << "||" << dec
				<< " gyro=" << imu[0] << "," << imu[1] << "," << imu[2]
				<< " accel=" << imu[3] << "," << imu[4] << "," << imu[5];
		}
	}

	std::cout << num_IMUs;
}

stringstream imu_info_v2(OWL::Context &owl, const OWL::Event *frame, ofstream &log_file)
{
	stringstream IMU_string;

	const OWL::Event *inputs = frame->find(OWL::Type::INPUT, "inputs");
	num_IMUs = 0;

	if (!inputs)
	{
		std::cout << num_IMUs;
		IMU_string << "";
		return IMU_string;
	}


	for (const OWL::Input *i = inputs->begin(); i != inputs->end(); i++)
	{
		OWL::DeviceInfo dev = owl.deviceInfo(i->hw_id);
		if (dev.type == "microdriver" && i->data.size() == 16)
		{
			const uint8_t *data = i->data.data();

			int16_t addr = int16_t(data[2] | (data[3] << 8));

			int16_t imu[6] = { int16_t(data[4] | (data[5] << 8)),
				int16_t(data[6] | (data[7] << 8)),
				int16_t(data[8] | (data[9] << 8)),
				int16_t(data[10] | (data[11] << 8)),
				int16_t(data[12] | (data[13] << 8)),
				int16_t(data[14] | (data[15] << 8)) };

			num_IMUs = num_IMUs + 1;

			IMU_string << "||IMU||" << "time=" << frame->time()
				<< " device=0x" << hex << i->hw_id << "||" << dec
				<< " gyro=" << imu[0] << "," << imu[1] << "," << imu[2]
				<< " accel=" << imu[3] << "," << imu[4] << "," << imu[5];

			log_file << "||IMU||" << "time=" << frame->time()
				<< " device=0x" << hex << i->hw_id << "||" << dec
				<< " gyro=" << imu[0] << "," << imu[1] << "," << imu[2]
				<< " accel=" << imu[3] << "," << imu[4] << "," << imu[5];
		}
	}

	std::cout << num_IMUs;
	return IMU_string;
}


int main(int argc, const char **argv)
{

	/* 2024-05-08 : Setting up PhaseSpace UDP to stream out data*/
	myKINOVA_UDP ROBOT_UDP(0, 27109, 27110, "127.0.0.1", "127.0.0.1");
	ROBOT_UDP.setup_UDP();
	const char* mysendbuf3;
	//mysendbuf3 = "hahahaha";
	//while(1)
	//{
		int iResult = 0;
		//string variable_string = "";
	//}
	//wprintf(L"datagrams sent...\n");
	//

	/* Retrieving the experiment details */
	FILE    *textfile;
	char    *exp_folder;
	long    numbytes;
	textfile = fopen("exp_settings.txt", "r");
	if (textfile == NULL)
		return 1;

	fseek(textfile, 0L, SEEK_END);
	numbytes = ftell(textfile);
	fseek(textfile, 0L, SEEK_SET);

	exp_folder = (char*)calloc(numbytes, sizeof(char));
	if (exp_folder == NULL)
		return 1;

	fread(exp_folder, sizeof(char), numbytes, textfile);
	fclose(textfile);

	printf("Exp folder read from exp_settings.txt is : %s\n", exp_folder);

	char* buffer;

	// Get the current working directory:
	if ((buffer = _getcwd(NULL, 0)) == NULL)
		perror("_getcwd error");

	char base_path[256];
	strcpy(base_path, buffer);
	free(buffer);
	strcat(base_path, exp_folder);
	printf("Base path is: %s\n", base_path);

	string file = "\\PS_data.txt";

	ofstream log_file(base_path + file);

	string address = argc > 1 ? argv[1] : "192.180.0.170";
	OWL::Context owl;
	OWL::Markers markers;

	if (owl.open(address) <= 0 || owl.initialize("event.inputs=1") <= 0) return 0;

	// start streaming
	owl.streaming(1);

	// main loop
	while (owl.isOpen() && owl.property<int>("initialized"))
	{
		const OWL::Event *event = owl.nextEvent(1);
		if (!event) continue;

		if (event->type_id() == OWL::Type::ERROR)
		{
			cerr << event->name() << ": " << event->str() << endl;
		}
		else if (event->type_id() == OWL::Type::FRAME)
		{
			
			stringstream variable_string, IMU_string;
			//string_object_name << 50;
			//variable_string = "";
			std::cout << "# IMUs switched on: ";
			//imu_info(owl, event, log_file);
			
			IMU_string = imu_info_v2(owl, event, log_file);
			variable_string << IMU_string.str();

			log_file << "||m_header||" << "PStime:" << event->time() << " " << event->type_name() << " " << event->name() << "=" << event->size<OWL::Event>();
			variable_string << "||m_header||" << "PStime:" << event->time() << " " << event->type_name() << " " << event->name() << "=" << event->size<OWL::Event>();
			

			if (event->find("markers", markers) > 0)
			{

				log_file << "||MARKERS||";
				std::cout << ", # MARKERS visible = ";
				num_visible = 0;
				for (OWL::Markers::iterator m = markers.begin(); m != markers.end(); m++)
					if (m->cond > 0)
					{
						log_file << "|marker_" << m->id << "|X:" << m->x << ",Y:" << m->y << ",Z:" << m->z << "||";
						variable_string << "|marker_" << m->id << "|X:" << m->x << ",Y:" << m->y << ",Z:" << m->z << "||";
						num_visible = num_visible + 1;
					}
				std::cout << num_visible << ".";
				log_file << "t: " << GetTickUs() << endl;
			}
			std::cout << endl;

			string send_string;
			send_string = variable_string.str();
			mysendbuf3 = send_string.c_str();
			std::cout << "SEND BUFFER IS " << variable_string.str() << std::endl;

			send(ROBOT_UDP.ConnectSocket, mysendbuf3, (int)strlen(mysendbuf3), 0);
		}
	} // while

	owl.done();
	owl.close();
	ROBOT_UDP.cleanup();

	return 0;
}