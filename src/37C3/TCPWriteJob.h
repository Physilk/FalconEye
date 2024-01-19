#pragma once

#include "Threading/ThreadingInterface.h"
#include "Threading/JobBase.h"

#include "gKit/image.h"
#include "gKit/color.h"

#include "Utils/resourceManager.h"

#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iomanip>
#define PORT 1337
#if __has_include(<format>)
    #include <format>
    using std::format;
#else
    #include <fmt/format.h>
    using fmt::format;
#endif

using Threading::TJobBase;

namespace FalconEye {

class TCPWriteJob : public TJobBase {

		protected:
            std::string Address;
            int Port;
            Image_ptr Image;
            int Start_x;
            int Start_y;
            int Size;
            int Socket_fd = 0;
		public:
			TCPWriteJob() = delete;
			TCPWriteJob(const TCPWriteJob&) = default;
			TCPWriteJob(std::string InAddress, int InPort, Image_ptr InImage, int InStart_x, int InStart_y, int InSize)
            : Address(InAddress)
            , Port(InPort)
            , Image(InImage)
            , Start_x(InStart_x)
            , Start_y(InStart_y)
            , Size(InSize)
			{}
			

			virtual ~TCPWriteJob() { /*...*/ }

		private:
			virtual int VirtualExecute() override;
            virtual int Init() override;

            bool IsInImageBound(int x, int y);
	};
		
	using TCPWriteJob_ptr = std::shared_ptr<TCPWriteJob>;

} // End namespace FalconEye