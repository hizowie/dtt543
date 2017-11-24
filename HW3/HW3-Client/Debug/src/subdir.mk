################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Client.cpp \
../src/DataSocket.cpp \
../src/Timer.cpp \
../src/UdpSocket.cpp 

OBJS += \
./src/Client.o \
./src/DataSocket.o \
./src/Timer.o \
./src/UdpSocket.o 

CPP_DEPS += \
./src/Client.d \
./src/DataSocket.d \
./src/Timer.d \
./src/UdpSocket.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


