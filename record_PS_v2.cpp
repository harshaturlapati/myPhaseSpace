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
#if defined(_MSC_VER)
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include <time.h>
#include <ctime>
#include <chrono>
using namespace std::chrono;
int num_visible = 0;

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

	if (!inputs) return;


	//std::cout << inputs->end() << endl;
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

			std::cout << " device=0x" << hex << i->hw_id;
			//" addr=0x" << addr << dec << ",";

			/*log_file << "||IMU starts||" << "time=" << frame->time() << " " << dev.type << " " << dev.name
				<< " device=0x" << hex << i->hw_id << " addr=0x" << addr << dec
				<< " gyro=" << imu[0] << "," << imu[1] << "," << imu[2]
				<< " accel=" << imu[3] << "," << imu[4] << "," << imu[5]
				<< "||IMU ends||";*/

			log_file << "||IMU||" << "time=" << frame->time() 
				//<< " " 
				//<< dev.type << " " << dev.name
				<< " device=0x" << hex << i->hw_id << "||" << dec
				<< " gyro=" << imu[0] << "," << imu[1] << "," << imu[2]
				<< " accel=" << imu[3] << "," << imu[4] << "," << imu[5];

			//log_file << "||IMU||" << "time=" << frame->time() << " " 
			//	<< " device=0x" << hex << i->hw_id
			//	<< " gyro=" << imu[0] << "," << imu[1] << "," << imu[2]
			//	<< " accel=" << imu[3] << "," << imu[4] << "," << imu[5];
		}
	}
}

int main(int argc, const char **argv)
{

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
		const OWL::Event *event = owl.nextEvent(1000);
		if (!event) continue;

		if (event->type_id() == OWL::Type::ERROR)
		{
			cerr << event->name() << ": " << event->str() << endl;
		}
		else if (event->type_id() == OWL::Type::FRAME)
		{


			log_file << "||m_header||" << "PStime:" << event->time() << " " << event->type_name() << " " << event->name() << "=" << event->size<OWL::Event>();

			if (event->find("markers", markers) > 0)
			{

				log_file << "||MARKERS||";
				std::cout << "# MARKERS visible = ";
				//<< markers.size() << ". ";
				num_visible = 0;
				for (OWL::Markers::iterator m = markers.begin(); m != markers.end(); m++)
					if (m->cond > 0)
					{
						log_file << "|marker_" << m->id << "|X:" << m->x << ",Y:" << m->y << ",Z:" << m->z << "||";
						num_visible = num_visible + 1;
					}
				std::cout << num_visible << ".";
				log_file << "t: " << GetTickUs() << endl;
				//cout << "  " << m->id << ") " << m->x << "," << m->y << "," << m->z << endl;
			}
			std::cout << "IMUs available:";
			imu_info(owl, event, log_file);
			std::cout << "." << endl;

		}
	} // while

	owl.done();
	owl.close();

	return 0;
}